/*
 * Copyright (c) 2000, 2001, 2002, 2003, 2004, 2005, 2008, 2009
 *	The President and Fellows of Harvard College.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE UNIVERSITY AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE UNIVERSITY OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <spl.h>
#include <spinlock.h>
#include <current.h>
#include <mips/tlb.h>
#include <addrspace.h>
#include <vm.h>
#include <proc.h>

/*
 * Note! If OPT_DUMBVM is set, as is the case until you start the VM
 * assignment, this file is not compiled or linked or in any way
 * used. The cheesy hack versions in dumbvm.c are used instead.
 *
 * UNSW: If you use ASST3 config as required, then this file forms
 * part of the VM subsystem.
 *
 */

struct addrspace *
as_create(void)
{
	struct addrspace *as;

	// Allocate memory for the address space structure
	as = kmalloc(sizeof(struct addrspace));
	if (as == NULL) {
		return NULL;
	}
	
	// Allocate memory for the page table
	as->pagetable = kmalloc(PAGETABLE_SIZE_FIRST_LEVEL * sizeof(paddr_t *));
	if (as->pagetable == NULL) {
			kfree(as);
			return NULL;
	}
	for (int i = 0; i < PAGETABLE_SIZE_FIRST_LEVEL; i++) {
			as->pagetable[i] = NULL;
	}
	as->head = NULL; /* region initialisation */

	return as;
}

int
as_copy(struct addrspace *old, struct addrspace **ret)
{
	// Check if the source address space is valid
    if (old == NULL) {
        return EINVAL;
    }

    // Attempt to create a new address space
    struct addrspace *newas = as_create();
    if (newas == NULL) {
        return ENOMEM; 
    }

    // Copy all regions from the old address space to the new one
    struct as_region *current_region = old->head;
    struct as_region *last_new_region = NULL;
    while (current_region != NULL) {
        // Allocate memory for the new region
        struct as_region *new_region = kmalloc(sizeof(struct as_region));
        if (new_region == NULL) {
            as_destroy(newas); 
            return ENOMEM;
        }

        // Copy data from the old region to the new region
		//  Ensure the next pointer is NULL
        copy_region(current_region, new_region);
        new_region->next = NULL; //

        // Link the new region into the new address space
        if (last_new_region == NULL) {
            newas->head = new_region; 
        } else {
            last_new_region->next = new_region; 
        }
		// Update the last region pointer
        last_new_region = new_region; 

        // Move to the next region in the old address space
        current_region = current_region->next;
    }

    // Duplicate the page table from old to new address space
    if (copy_pt(old->pagetable, newas->pagetable) != 0) {
        as_destroy(newas); // Handle failure in page table copying
        return ENOMEM;
    }

    // Assign the newly created address space to the output parameter
    *ret = newas;
    return 0;
}

void
as_destroy(struct addrspace *as)
{
	/*
	 * Clean up as needed.
	 */
	// Check if the pagetable exists before attempting to free it
	if (as->pagetable != NULL) {
			// Iterate through each entry in the first-level page table
			for (int i = 0; i < PAGETABLE_SIZE_FIRST_LEVEL; i++) {
					// Check if the second-level page table exists
					if (as->pagetable[i] != NULL) {
							// Free all valid pages in the second-level page table
							for (int j = 0; j < PAGETABLE_SIZE_SECOND_LEVEL; j++) {
									if (as->pagetable[i][j] != 0) {
											free_kpages(PADDR_TO_KVADDR(as->pagetable[i][j]) & PAGE_FRAME);
									}
							}
							kfree(as->pagetable[i]);
					}
			}
			kfree(as->pagetable);
	}

	// Clean up the linked list of regions
	cleanup_regions(as);

	// Free the address space structure itself
	kfree(as);
}

void
as_activate(void)
{
	struct addrspace *as;

	as = proc_getas();
	if (as == NULL) {
		/*
		 * Kernel thread without an address space; leave the
		 * prior address space in place.
		 */
		return;
	}
	/**
	 * Taken from kern/arch/mips/vm/dumbvm.c
	 * Disable interrupts on this CPU while frobbing the TLB.
	*/
	int spl = splhigh();

	for (int i = 0; i < NUM_TLB; i++) {
		tlb_write(TLBHI_INVALID(i), TLBLO_INVALID(), i);
	}

	splx(spl);

}

void
as_deactivate(void)
{
	/*
	 * Write this. For many designs it won't need to actually do
	 * anything. See proc.c for an explanation of why it (might)
	 * be needed.
	 */
	as_activate();
}

/*
 * Set up a segment at virtual address VADDR of size MEMSIZE. The
 * segment in memory extends from VADDR up to (but not including)
 * VADDR+MEMSIZE.
 *
 * The READABLE, WRITEABLE, and EXECUTABLE flags are set if read,
 * write, or execute permission should be set on the segment. At the
 * moment, these are ignored. When you write the VM system, you may
 * want to implement them.
 */
int
as_define_region(struct addrspace *as, vaddr_t vaddr, size_t memsize,
		 int readable, int writeable, int executable)
{
	
	/* Align the addresses to page boundaries */
	/* Align the region. First, the base... */
	vaddr_t base_aligned = vaddr & PAGE_FRAME;

	/* Calculate the offset within the page */
	size_t offset_within_page = vaddr - base_aligned;

	/* Calculate the end address of the region */
	vaddr_t end_addr = base_aligned + memsize;

	/* Calculate the number of pages needed */
	size_t num_pages = ((memsize + offset_within_page - 1) / PAGE_SIZE) + 1;

	/* Check if the region is defined within kuseg */
	if (end_addr > MIPS_KSEG0) {
		return EFAULT;
	}

	/* Adjust the memsize to cover whole pages */
	memsize = num_pages * PAGE_SIZE;

    /* Check if the region overlaps with other regions */
    struct as_region *curr = as->head;
    while (curr != NULL) {
        if ((curr->init_addr < vaddr + memsize) &&
            (curr->init_addr + curr->region_size > vaddr)) {
            return EINVAL; // Overlapping region
        }
        curr = curr->next;
    }

    /* Create a new region */
    struct as_region *new = kmalloc(sizeof(struct as_region));
    if (new == NULL) {
        return ENOMEM; // Memory allocation failed
    }

    /* Initialize the new region */
    new->init_addr = vaddr;
    new->region_size = memsize;
    new->r = readable;
    new->w = writeable;
    new->e = executable;
    new->next = NULL;

    /* Insert the new region into the address space */
    if (as->head == NULL) {
        as->head = new;
    } else {
        struct as_region *last = as->head;
        while (last->next != NULL) {
            last = last->next;
        }
        last->next = new;
    }

    return 0; // Success
}

int
as_prepare_load(struct addrspace *as)
{
	for ( struct as_region *curr = as->head; curr != NULL; curr = curr->next) {
			curr->prev_write_status = curr->w;
			curr->w = 1;
	}

	return 0;

}

int
as_complete_load(struct addrspace *as)
{
 as_activate(); // Flush TLB after loading

	// Get the page table from the address space
	paddr_t **pt = as->pagetable;

	// Iterate through the page table entries
	for (int i = 0; i < PAGETABLE_SIZE_FIRST_LEVEL; i++) {
			if (pt[i] != NULL) {
					for (int j = 0; j < PAGETABLE_SIZE_SECOND_LEVEL; j++) {
							if (pt[i][j] != 0) {
									vaddr_t pd_bits = get_11_msb_bits(i);// Use 11 most significant bits of the page number (32 - 21)
									vaddr_t pt_bits = get_9_lsb_bits(j); // Use 9 least significant bits of the page number (20 - 12)
									vaddr_t vaddr = pd_bits | pt_bits; 

									// Find the memory region corresponding to the virtual address
									struct as_region *curr = as->head;
									while (curr != NULL) {
											if (vaddr >= curr->init_addr && vaddr < (curr->init_addr + curr->region_size)) {
													// Found the memory region, reset write permissions
													curr->w = curr->prev_write_status;

													// If the region was not writable before loading, update the page table entry
													if (curr->prev_write_status == 0) {
															pt[i][j] = (pt[i][j] & PAGE_FRAME) | TLBLO_VALID;
													}
													break;
											}
											curr = curr->next;
									}
							}
					}
			}
	}

	// Return success
	return 0;
}

int
as_define_stack(struct addrspace *as, vaddr_t *stackptr)
{
	/* Initial user-level stack pointer */
	*stackptr = USERSTACK;

	as_define_region(as, *stackptr - USERSTACK_SIZE, USERSTACK_SIZE, 1, 1, 0);

	return 0;
}

/**
 * HELPER FUNCTIONS
*/

void cleanup_regions(struct addrspace *as) {
    // Start with the first region in the address space
    struct as_region *current = as->head;
    
    // Loop through all regions and free them
    while (current != NULL) {
        struct as_region *next = current->next;  
        kfree(current);
        current = next; 
    }

    as->head = NULL;
}

void copy_region(struct as_region *old, struct as_region *new) {

	// copy attributes
	new->init_addr = old->init_addr;
	new->region_size = old->region_size;
	new->r = old->r;
	new->w = old->w;
	new->e = old->e;
	new->prev_write_status = old->prev_write_status;
	new->next = NULL;
}

/* PAGE TABLE HELPER FUNCTION */
int copy_pt(paddr_t **old_pt, paddr_t **new_pt)
{
	// Iterate over all entries in the first-level page table
	for(int i = 0; i < PAGETABLE_SIZE_FIRST_LEVEL; i++) {
		if(old_pt[i] != NULL) {
			new_pt[i] = kmalloc(PAGETABLE_SIZE_FIRST_LEVEL * sizeof(paddr_t *));
			if(new_pt[i] == NULL) {
				return ENOMEM;
			}
			// Copy each entry from the old page table to the new page
			for(int j = 0; j < PAGETABLE_SIZE_SECOND_LEVEL; j++)
			{
				if(old_pt[i][j] != 0) {
					// Allocate a new frame for the page
					vaddr_t frame_vaddr = frame_allocate(PADDR_TO_KVADDR(old_pt[i][j]));
					if(frame_vaddr == 0) return ENOMEM;
					// Copy data from the old frame to the new frame
					memmove((void *) frame_vaddr, (void *) PADDR_TO_KVADDR(old_pt[i][j] & PAGE_FRAME), PAGE_SIZE);

					// Set up the new page table entry with the copied frame address and preserve flags
					new_pt[i][j] = KVADDR_TO_PADDR(frame_vaddr) & PAGE_FRAME;
					new_pt[i][j] = new_pt[i][j] | (TLBLO_DIRTY & old_pt[i][j]) | (TLBLO_VALID & old_pt[i][j]);
				} else {
					new_pt[i][j] = 0;
				}
			}
		} else {
			new_pt[i] = NULL;
		}
		
	}
	return 0;
}