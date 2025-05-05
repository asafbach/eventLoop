#ifndef __event_loop_h__
#define __event_loop_h__

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>

typedef enum {
    EVENT_LOOP_RUNNING,
    EVENT_LOOP_IDLE,        

} event_loop_status_t;

typedef enum {
    TASK_PRIORITY_HIGH,
    TASK_PRIORITY_MEDIUM,
    TASK_PRIORITY_LOW,
    TASK_PRIORITY_MAX
} task_priority_t;


//callback 
typedef void (*event_cbk_t)(void *ctx);


typedef struct task{
    event_cbk_t cbk;
    void* args;
    struct task *left,*right;
    task_priority_t priority;
} task;

typedef struct {
    //thread 
    pthread_t *thread;
    //mutex
    pthread_mutex_t el_mutex;
    //thread condition 
    pthread_cond_t thread_cond;
    //state
    event_loop_status_t state;
    //current task
    task *current_task;
    //task array
    task *task_array[TASK_PRIORITY_MAX];
    bool stop_requested;

} event_loop;


void el_init(event_loop* el);
void el_add_task(event_loop* el, task *add_task);
void el_remove_task(event_loop* el, task *task);
void el_run(event_loop* el);
task * create_task(event_cbk_t cbk, void* args, task_priority_t priority );

#endif