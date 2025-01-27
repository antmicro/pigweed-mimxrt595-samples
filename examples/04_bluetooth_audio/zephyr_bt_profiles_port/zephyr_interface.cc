#include "zephyr_interface.h"
#include "pw_sync/counting_semaphore.h"
#include "pw_sync/binary_semaphore.h"

int k_sem_init(struct k_sem *sem, unsigned int initial_count,
			  unsigned int limit) {
    sem->is_initialized = true;
    sem->is_binary = (limit == 1U) ? true : false;
    if(sem->is_binary) {
        if (initial_count != 0) {
            return -EINVAL;
        }
    } else {
        sem->counting_semaphore.release(initial_count);
    }
    return 0;
}

void k_sem_give(struct k_sem *sem) {
    if(sem->is_binary) {
        sem->binary_semaphore.release();
    } else {
        sem->counting_semaphore.release();
    }
}
