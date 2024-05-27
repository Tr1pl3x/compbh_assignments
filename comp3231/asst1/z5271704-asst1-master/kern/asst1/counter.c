#include "opt-synchprobs.h"
#include "counter.h"
#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <test.h>
#include <thread.h>
#include <synch.h>


/*
 * ********************************************************************
 * INSERT ANY GLOBAL VARIABLES THAT YOU MAY REQUIRE HERE
 * ********************************************************************
 */


/*
 * counter_initialise() allocates a synchronised counter and initialises
 * it with the value passed in.
 * 
 * Multiple counters an be initialised concurrently. You can assume
 * kmalloc() is thread safe (i.e. it is itself synchronised).
 */

struct sync_counter * counter_initialise(int val)
{
        struct sync_counter *sc_ptr;

        sc_ptr = (struct sync_counter *) kmalloc(sizeof(struct sync_counter));

        if (sc_ptr == NULL) {
                /* Allocation failed */
                return NULL;
        }

        sc_ptr->counter = val;
        sc_ptr->counter_lock = lock_create("counter_lock"); // Initialize lock
        if (sc_ptr->counter_lock == NULL) {
                /* Lock creation failed */
                kfree(sc_ptr);
                return NULL;
        }
        
        return sc_ptr;
}

/*
 * counter_read_and_destroy() takes a pointer to a specific
 * synchronised counter and de-allocates any resources associated with
 * it and returns the final value of the counter.
 *
 * This function can be called concurrently. You can assume kfree() is thread safe.
 */

int counter_read_and_destroy(struct sync_counter *sc_ptr)
{
        int count;

        lock_acquire(sc_ptr->counter_lock); // Acquire lock before accessing counter
        count = sc_ptr->counter;
        lock_release(sc_ptr->counter_lock); // Release lock

        lock_destroy(sc_ptr->counter_lock); // Destroy the lock
        kfree(sc_ptr);
        return count;
}

/*
 * Increment the specific synchronised counter by 1. This function can
 * be called concurrently by multiple threads.
 */

void counter_increment(struct sync_counter *sc_ptr)
{
        lock_acquire(sc_ptr->counter_lock); // Acquire lock before modifying counter
        sc_ptr->counter = sc_ptr->counter + 1;
        lock_release(sc_ptr->counter_lock); // Release lock
}

/*
 * Decrement the specific synchronised counter by 1.This function can
 * be called concurrently by multiple threads. 
 */

void counter_decrement(struct sync_counter *sc_ptr)
{
        lock_acquire(sc_ptr->counter_lock); // Acquire lock before modifying counter
        sc_ptr->counter = sc_ptr->counter - 1;
        lock_release(sc_ptr->counter_lock); // Release lock
}

