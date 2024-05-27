#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <thread.h>
#include <addrspace.h>
#include <vm.h>
#include <machine/tlb.h>
#include <current.h>
#include <spl.h>
#include <current.h>
#include <synch.h>
#include <proc.h>

/* Place your page table functions here */
// insert
// lookup
// update
int insert_pt_entry(struct addrspace *as, vaddr_t vaddr, paddr_t paddr)
{
    uint32_t directory_index = get_11_msb_bits(vaddr);
    uint32_t table_index = get_9_lsb_bits(vaddr);

    if (directory_index >= PAGETABLE_SIZE_FIRST_LEVEL || table_index >= PAGETABLE_SIZE_SECOND_LEVEL) {
        return EFAULT; 
    }

    if (as->pagetable[directory_index] == NULL) {
        as->pagetable[directory_index] = (paddr_t *)kmalloc(PAGE_SIZE);
        if (!as->pagetable[directory_index]) {
            return ENOMEM;
        }
        memset(as->pagetable[directory_index], 0, PAGE_SIZE);

    } else {
        // verifying that the entry is free
        if (as->pagetable[directory_index][table_index] != 0 ){
            return EFAULT; 
        }
    }

    as->pagetable[directory_index][table_index] = paddr;
    return 0;
}

paddr_t lookup_pt_entry(struct addrspace *as, vaddr_t vaddr)
{
    uint32_t directory_index = get_11_msb_bits(vaddr);
    uint32_t table_index = get_9_lsb_bits(vaddr);

    if (validate_pt_entry(as, vaddr) != 0) {
        return 0;
    }

    return as->pagetable[directory_index][table_index];
}

int validate_pt_entry(struct addrspace *as, vaddr_t vaddr) {
    uint32_t directory_index = get_11_msb_bits(vaddr);
    uint32_t table_index = get_9_lsb_bits(vaddr);

    if (directory_index >= PAGETABLE_SIZE_FIRST_LEVEL || table_index >= PAGETABLE_SIZE_SECOND_LEVEL) {
        return -1; 
    }

    if (as->pagetable == NULL || 
        as->pagetable[directory_index] == NULL || 
        as->pagetable[directory_index][table_index] == 0) {
        return -1;
    }

    return 0;
}

int update_pt_entry(struct addrspace *as, vaddr_t vaddr, paddr_t paddr)
{
    uint32_t valid = validate_pt_entry(as, vaddr);
    if (valid) return valid;

    uint32_t directory_index = get_11_msb_bits(vaddr);
    uint32_t table_index = get_9_lsb_bits(vaddr);

    as->pagetable[directory_index][table_index] = paddr;

    return 0;
}

void vm_bootstrap(void)
{
    /* Initialise any global components of your VM sub-system here.  
     *  
     * You may or may not need to add anything here depending what's
     * provided or required by the assignment spec.
     */
}

int
vm_fault(int faulttype, vaddr_t faultaddress)
{
    if (faulttype == VM_FAULT_READONLY) {
        return EFAULT; // Error: Attempt to write to read-only page
    } else if (faulttype == VM_FAULT_WRITE || faulttype == VM_FAULT_READ) {
        // Continue processing
    } else {
        return EINVAL; // Error: Invalid fault type
    }

    if (curproc == NULL) {
        // Handle kernel fault early in boot
        return EFAULT;
     }


    // Access process's address space 
    struct addrspace *as = proc_getas(); 
    if (!as || !as->pagetable) return EFAULT;
    uint32_t directory_index = get_11_msb_bits(faultaddress);
    uint32_t table_index = get_9_lsb_bits(faultaddress);

    // checking if the page table entry exists
    if (validate_pt_entry(as, faultaddress) == 0)
    {
        // validating memeory region access
        if (check_region_validity(as, faultaddress, faulttype) == 0) {
            load_tlb(faultaddress & PAGE_FRAME, as->pagetable[directory_index][table_index]);
            return 0;
        }
        return EFAULT;
    }


    // Validate and potentially allocate new frame 
    int validate = check_region_validity(as, faultaddress, faulttype); 
    if(validate) {
        return validate;
    }

    // Allocate and zero-fill a new frame, then insert new page table entry
    vaddr_t new_frame_addr = frame_allocate(faultaddress);
    if (new_frame_addr == 0) return ENOMEM;
    paddr_t frame_paddr= KVADDR_TO_PADDR(new_frame_addr) & PAGE_FRAME;
    struct as_region *region = find_region(as, faultaddress);
    /* inserting into the page table entry */
    if (region->w != 0) {
        frame_paddr |= TLBLO_DIRTY;
    }

    validate = insert_pt_entry(as, faultaddress, frame_paddr | TLBLO_VALID);
    if (validate != 0) {
        return ENOMEM;
    }

    load_tlb(faultaddress & PAGE_FRAME, as->pagetable[directory_index][table_index]);
    return 0;       
}
/*
 * SMP-specific functions.  Unused in our UNSW configuration.
 */

void
vm_tlbshootdown(const struct tlbshootdown *ts)
{
	(void)ts;
	panic("vm tried to do tlb shootdown?!\n");
}

///////////////////////////////////////////////////////////
/////////////////////////HELPER FUNCTIONS//////////////////
///////////////////////////////////////////////////////////
/* Save an entry into TLB */
void load_tlb(uint32_t entryhi, uint32_t entrylo)
{
    int sql = splhigh();
    tlb_random(entryhi, entrylo);
    splx(sql);
}

int check_region_validity(struct addrspace *as, vaddr_t vaddr, int faulttype)
{
    struct as_region *region = find_region(as, vaddr);
    if (!region) return EFAULT; /* Region not found, error out */

     if (faulttype == VM_FAULT_WRITE) {
        if (region->w == 0) return EPERM; // Error: Write permission denied
    } else if (faulttype == VM_FAULT_READ) {
        if (region->r == 0) return EPERM; // Error: Read permission denied
    } else {
        return EINVAL; // Error: Invalid argument
    }
    return 0;
}

struct as_region *find_region(struct addrspace *as, vaddr_t vaddr) {
    struct as_region *curr = as->head;
    while (curr != NULL) {
        if (vaddr >= curr->init_addr &&
            (vaddr < (curr->init_addr + curr->region_size))) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

vaddr_t get_11_msb_bits(vaddr_t addr) {
    return (addr >> 21) & 0x7FF;
}

vaddr_t get_9_lsb_bits(vaddr_t addr) {
    return (addr >> 12) & 0x1FF;
}


vaddr_t frame_allocate(vaddr_t vaddr) {
	/*  Allocate Frame for this region  */
    vaddr = vaddr;
    vaddr_t new_vaddr= alloc_kpages(1);
	if (new_vaddr== 0) return 0;
    bzero((void *) new_vaddr, PAGE_SIZE);
	return new_vaddr;
}