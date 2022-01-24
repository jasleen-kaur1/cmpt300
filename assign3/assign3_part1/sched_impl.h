#ifndef	__SCHED_IMPL__H__
#define	__SCHED_IMPL__H__

#include <stdlib.h>
#include <stdio.h>

#include "list.h"
#include "scheduler.h"
#include <semaphore.h>

#include <pthread.h>

struct thread_info {
	/*...Fill this in...*/

	// Scheduler queue
	sched_queue_t *queue;

	// Elements in the queue list. The threads themselves
	list_elem_t *queue_element;

	// Thread semaphore
	sem_t thread_sem;


};

struct sched_queue {
	/*...Fill this in...*/

	// Queue capacity semaphore
	sem_t admission_sem;

	// CPU state semaphore
	sem_t cpu_sem;

	// Initialize q as a pointer to a list. This will be our queue 
	list_t * lst;

};

#endif /* __SCHED_IMPL__H__ */
