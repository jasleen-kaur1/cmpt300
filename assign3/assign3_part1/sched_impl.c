#include "scheduler.h"
#include "sched_impl.h"

int current_worker_position;

/* Fill in your scheduler implementation code below: */

/************************ THREAD FUNCTIONS *********************/

// Create thread and it's info
static void init_thread_info(thread_info_t *info, sched_queue_t *queue)
{
	/*...Code goes here...*/

	// Thread needs to know which queue it belongs to and needs to interact with
	info->queue = queue;

	//Initialize the thread (element in the list)
	info->queue_element = (list_elem_t*) malloc(sizeof(list_elem_t));
  	list_elem_init(info->queue_element, info);


	/* Initializing a semaphore for thread. Value is 0, meaning has to wait until 
	someone else does a sem_post - initially blocked */
	sem_init(&(info->thread_sem), 0, 0);

}

// Destroy a particular thread info
static void destroy_thread_info(thread_info_t *info)
{
	/*...Code goes here...*/

	// Set thread info queue pointer to NULL. Thread not associated with the 
	// queue anymore 
	info->queue = NULL;

	// Free the element for efficiency as it was malloced before
	free(info->queue_element);

	// Set that queue element to NULL
	info->queue_element = NULL;

	// Destroy thread semaphore when destroying particular thread
	sem_destroy(&(info->thread_sem));

}

// Insert a thread in the scheduler queue
static void enter_sched_queue(thread_info_t *info)
{
	/*...Code goes here...*/

  	// If space available in queue, threads will be added to queue, blocked if there is no space in queue
  	sem_wait(&(info->queue->admission_sem));

  	// could add mutex lock to insert at tail

  	// Insert the thread at the tail end of the queue
  	list_insert_tail(info->queue->lst, info->queue_element);

  	// could add mutex unlock

}

// Remove thread from queue when it has finished executing
static void leave_sched_queue(thread_info_t *info)
{
	/*...Code goes here...*/

	// Remove thread(element) from the queue. Decrement a thread from queue
	list_remove_elem(info->queue->lst, info->queue_element);
	// free(info->queue_element); --> gets rid of double free error

	/* After removing a thread, a space is available to add next thread.
	Remove a process from the blocked queue and place in the ready list
	Places a thread that was waiting to be added into the queue*/
	sem_post(&(info->queue->admission_sem));

	// a worker leaves, so decrement current worker position for rr
	current_worker_position--;


}

// blocking thread in queue until scheduler schedules the thread to run in CPU
static void wait_for_cpu(thread_info_t *info)
{
	/*...Code goes here...*/
	sem_wait(&(info->thread_sem));

}

// Thread finishes an iteration at CPU and wants to release CPU for another thread to go in CPU
// A thread releases CPU and another thread is able to take over CPU
static void release_cpu(thread_info_t *info)
{
	/*...Code goes here...*/
	sem_post(&(info->queue->cpu_sem));
}

/*...More functions go here.. PLACED THEM HERE FROM scheduler.h.*/

/************************ SCHEDULER FUNCTIONS *********************/

// Create a scheduler queue
static void init_sched_queue(sched_queue_t *queue, int queue_size)
{
	/*...Code goes here...*/

	// Initialize queue with size 
	queue->lst = (list_t *) malloc(sizeof(list_t));

	// Initialize the q list with NULL head and tail.
	list_init(queue->lst);
	current_worker_position = 0;

	// Initialize admsision_sem
	sem_init(&(queue->admission_sem), 0, queue_size);

	// Initialize cpu_sem
	sem_init(&(queue->cpu_sem), 0, 0);

}

// Destroy the scheduler queue, freeing resources
static void destroy_sched_queue(sched_queue_t *queue)
{
	/*...Code goes here...*/

	// When queue is not empty destroy the queue and it's elements
	if (queue != NULL ){

		// Go through the q list that's left and free elements 
		list_foreach(queue->lst, (void *) free);

		// Free queue lst for efficiency as it was malloced before
		free(queue->lst);
	}

	// Destroy queue relating semaphores on destroy
	sem_destroy(&(queue->admission_sem));

	sem_destroy(&(queue->cpu_sem));

}

// Scheduler wakes up the worker thread so it can run in CPU
static void wake_up_worker(thread_info_t *info)
{
	/*...Code goes here...*/
	//increment thread_sem value. Next thread able to run
	sem_post(&(info->thread_sem));

}

// Queue waits for worker. Worker uses CPU, and blocking until the thread in CPU releases CPU
static void wait_for_worker(sched_queue_t *queue)
{
	/*...Code goes here...*/
	//blocking thread until another thread releases CPU
	sem_wait(&(queue->cpu_sem));

}

// static thread_info_t *next_worker(sched_queue_t *queue)
// {
// 	/*...Code goes here...*/

// }

// Next worker to run would always be from the head of the list --> FIFO
static thread_info_t * next_worker_fifo(sched_queue_t *queue)
{
	thread_info_t *next_worker = NULL;

	list_elem_t *queue_head = list_get_head(queue->lst);
	if(queue_head){
		next_worker = (thread_info_t*) (queue_head->datum);
	}
	
	return next_worker;
}

// Separate implemenation for round robin
static thread_info_t * next_worker_round_robin(sched_queue_t *queue)
{

	thread_info_t *next_worker;	
	list_elem_t * elt = list_get_head(queue->lst);

	// try mutex locks here, this implementation passes 2 tests and fails 3 sometimes
	// if(elt == NULL){
	// 	return NULL;
	// }else{
	// 	next_worker = (thread_info_t*) (elt -> datum);
	// 	list_remove_elem(queue->q, elt);
	// 	list_insert_tail(queue->q, elt);
	// 	return next_worker;
	// }

	// return NULL if queue is empty
	if(elt == NULL){
		return NULL;
	}

	//current worker position reaches the end of the list, go back to the head
	if (current_worker_position == list_size(queue->lst))
	{
		current_worker_position = 0;
	}

	for (int head = 0;  head < current_worker_position; head++)
	{
		elt = elt->next;
	}
	next_worker = (thread_info_t*) (elt->datum);
	current_worker_position++;

	return next_worker;
}

// check size of queue to enter more threads
static void wait_for_queue(sched_queue_t *queue)
{
	/*...Code goes here...*/

	while(!list_size(queue->lst)){
		sched_yield();

	}

}

/*...More functions go here...*/

/* You need to statically initialize these structures: */
sched_impl_t sched_fifo = {
	{ init_thread_info, destroy_thread_info, enter_sched_queue, leave_sched_queue, wait_for_cpu, release_cpu}, 
	{ init_sched_queue, destroy_sched_queue, wake_up_worker, wait_for_worker,/* next_worker, */ next_worker_fifo, wait_for_queue} 
},
sched_rr = {
	{ init_thread_info, destroy_thread_info, enter_sched_queue, leave_sched_queue, wait_for_cpu, release_cpu}, 
	{ init_sched_queue, destroy_sched_queue, wake_up_worker, wait_for_worker, /* next_worker, */ next_worker_round_robin, wait_for_queue} 
};
