#include <types.h>
#include <kern/errno.h>
#include <kern/fcntl.h>
#include <kern/limits.h>
#include <kern/stat.h>
#include <kern/seek.h>
#include <lib.h>
#include <uio.h>
#include <proc.h>
#include <current.h>
#include <synch.h>
#include <vfs.h>
#include <vnode.h>
#include <file.h>
#include <syscall.h>
#include <copyinout.h>
#include <proc.h>

#define NUM_STDIO_FDS 3

/*
 * Add your file-related functions here ...
 */


int sys_open(userptr_t filename, int flags, mode_t mode) {
    
    // Checking arguments
    if (filename == NULL) return EFAULT; // if filename ptr is invalid
    
    // Reserving memory for the filename
    char *fname = kmalloc(NAME_MAX); 
    if (fname == NULL) return ENOMEM; // if malloc fails

    // Extracting filename from user space
    int copy_status = copyinstr(filename, fname, NAME_MAX, NULL);
    if (copy_status) {
        kfree(fname);
        return copy_status;
    }

    // Finding an available file descriptor
    int fd;
    for (fd = 3; fd < OPEN_MAX; fd++) {
        if (curproc->fd_table[fd] == NULL)
            break;
    }

    
    // Verifying whether all file descriptors are currently in use
    if (fd == OPEN_MAX) {
        kfree(fname);
        return EMFILE;
    }

    // Reserving memory for file attributes
    struct file_att *file_attr = kmalloc(sizeof(struct file_att));
    if (file_attr == NULL) {
        kfree(fname);
        return ENOMEM; // no memory
    }

    // Defining file attributes
    file_attr->perm_flags = flags & O_ACCMODE;
    KASSERT(file_attr->perm_flags == O_RDONLY || file_attr->perm_flags == O_WRONLY || file_attr->perm_flags == O_RDWR);
    file_attr->file_refcount = 1;
    file_attr->offset = 0;

    // Opening the file
    int open_status = vfs_open(fname, flags, mode, &(file_attr->vnode));
    if (open_status) {
        kfree(fname);
        kfree(file_attr);
        return open_status;
    }

    // Applying file attributes to the file descriptor
    curproc->fd_table[fd] = file_attr;
    kfree(fname);

    return fd; // return successfully
}

ssize_t sys_read(int fd, void *buf, size_t buflen) {

    // Checking arguments
    int validity_check = rw_validity(fd, buf, buflen);
    if (validity_check != 0)
        return validity_check;

    // Verifying if file has right permission
    if (curproc->fd_table[fd]->perm_flags == O_WRONLY) return EACCES; 

    // Initializing input/output structures for user operations.
    struct uio user_io;
    struct iovec io_vector;
    uio_uinit(&io_vector, &user_io, buf, buflen, curproc->fd_table[fd]->offset, UIO_READ);

    // Executing the read operation.
    int read_status = VOP_READ(curproc->fd_table[fd]->vnode, &user_io);
    
    // if any error occurs during the execution
    if (!read_status) return read_status; 

    // Updating the file offset
    curproc->fd_table[fd]->offset = user_io.uio_offset;

    // Return the number of bytes read
    return buflen - user_io.uio_resid;
}

int sys_dup2(int oldfd, int newfd) {

	 // check arguments
    int check = closelseekdup_validity(oldfd);
    if (check) return check;

	if (newfd < 0 || newfd > OPEN_MAX + 1) return EBADF; // invalid fd

    if (newfd == oldfd) return newfd;

    if (curproc -> fd_table[newfd] != NULL) {
        int dup_status = sys_close(newfd);
        if (dup_status) return dup_status;
    }

    curproc -> fd_table[newfd] = curproc -> fd_table[oldfd];
    curproc -> fd_table[oldfd] -> file_refcount++;

    return newfd;
}

ssize_t sys_write(int fd, void *buf, size_t nbytes) {

    int validity_check = rw_validity(fd, buf, nbytes);
    if (validity_check != 0)
        return validity_check;
    //  if file is READ_ONLY
    if (curproc->fd_table[fd] == NULL) return EBADF;
    if (curproc->fd_table[fd]->perm_flags == O_RDONLY) return EACCES; 

    struct uio user_io;
    struct iovec io_vector;

    // helper function to set up a uio and io for a userspace transfer.
    uio_uinit(&io_vector, &user_io, (userptr_t)buf, nbytes, curproc->fd_table[fd]->offset, UIO_WRITE);

    int write_status = VOP_WRITE(curproc->fd_table[fd]->vnode, &user_io);
    if (write_status) return write_status;

    // updating the file offset
    curproc->fd_table[fd]->offset = user_io.uio_offset;

    return nbytes - user_io.uio_resid;
}

int sys_close(int fd)
{
	// check args
    int check = closelseekdup_validity(fd);
    if (check) return check;

    // after sys_open(), files get a refcount of atleast 1
    KASSERT( curproc->fd_table[fd]->file_refcount > 0); 
    if (--curproc->fd_table[fd]->file_refcount == 0) {
        // if reference count is zero - close the file
        vfs_close(curproc->fd_table[fd]->vnode);
        kfree(curproc->fd_table[fd]);
        curproc->fd_table[fd] = NULL;
    }

    return 0;   
}

off_t sys_lseek(int fd, off_t pos, int whence) {
	// check arguments
    int check = closelseekdup_validity(fd);
    if (check) return check;

    off_t new_offset;
    struct stat fStat;
    int lseek_status = VOP_STAT(curproc->fd_table[fd]->vnode, &fStat);
    if(lseek_status) return lseek_status;

    switch(whence) {
        /* seeking in reference to beginning of the file */
        case SEEK_SET: // 0
            if (pos < 0) return EINVAL;  
            new_offset = pos;
            break;

        /* seeking in reference to current position in file */
        case SEEK_CUR: // 1
            new_offset = curproc->fd_table[fd]->offset + pos;
            if (new_offset < 0) return EINVAL;
            break;

        /* seek in reference to EOF of the file */
        case SEEK_END:
            new_offset = pos + fStat.st_size; 
            break;

        default:
            return EINVAL;  		
    }
    if (!VOP_ISSEEKABLE(curproc->fd_table[fd]->vnode)) {
        return ESPIPE; 				
    }

    curproc->fd_table[fd]->offset = new_offset;
    return new_offset;
}

int init_stdioerr() {
    char c0[] = "con:";
    char c1[] = "con:";
    char c2[] = "con:";

    int std_status;

    // open Standard Input
    std_status = open_stdio_file(c0, O_RDONLY, &curproc->fd_table[0]);
    if (std_status) return std_status;

    // open Standard Output
    std_status = open_stdio_file(c1, O_WRONLY, &curproc->fd_table[1]);
    if (std_status) return std_status;

    // open Standard Error
    std_status = open_stdio_file(c2, O_WRONLY, &curproc->fd_table[2]);
    if (std_status) return std_status;

    // close the original file descriptors
    sys_close(0);

    return 0;
}

int open_stdio_file(char *path, int flags, struct file_att **file_ptr) {
    *file_ptr = kmalloc(sizeof(struct file_att));
    if (*file_ptr == NULL) return ENOMEM;

    (*file_ptr)->perm_flags = flags;
    (*file_ptr)->file_refcount = 1;
    (*file_ptr)->offset = 0;

    int std_status = vfs_open(path, flags, 0664, &(*file_ptr)->vnode);
    if (std_status) {
        kfree(*file_ptr);
        return std_status;
    }

    return 0;
}



/**
 * HELPER FUNCTIONS FOR VALIDITY CHECKING
 * 
*/

// write read check
int rw_validity(int fd, void *buf, size_t buflen) {
    // checking file descriptor validity
    if (fd < 0 || fd >= OPEN_MAX || curproc->fd_table[fd] == NULL)
        return EBADF;

    // checking buffer address validity
    if (buf == NULL)
        return EFAULT;

    // checking byte count validity
    if (buflen <= 0)
        return EINVAL;

    return 0; 
}

int closelseekdup_validity(int fd) {
    // checking if fd is within valid range
    if (fd < 0 || fd > OPEN_MAX + 1) return EBADF;

    // checking if fd is associated with a file descriptor
    if (curproc->fd_table[fd] == NULL) return EBADF;

    return 0;
}
