/*
 * Declarations for file handle and file table management.
 */

#ifndef _FILE_H_
#define _FILE_H_

/*
 * Contains some file-related maximum length constants
 */
#include <limits.h>

struct file_att {
    struct vnode* vnode; 
    int perm_flags;
    off_t offset;
    int file_refcount;
};
/*
 * Put your function declarations and data types here ...


 */
int sys_open(userptr_t filename, int flags, mode_t mode);
ssize_t sys_read(int fd, void *buf, size_t buflen);
int sys_dup2(int oldfd, int newfd);
ssize_t sys_write(int fd, void *buf, size_t nbytes); 
int sys_close(int fd);
off_t sys_lseek(int fd, off_t pos, int whence);
int init_stdioerr(void);

// helper functions
int open_stdio_file(char *path, int flags, struct file_att **file_ptr);
int rw_validity(int fd, void *buf, size_t buflen);
int closelseekdup_validity(int fd);
#endif /* _FILE_H_ */
