#include <assert.h>
#include "event_loop.h"

void el_init(event_loop* el){
    printf("hello from init\n");
    for(int i = 0; i< TASK_PRIORITY_MAX; i++){
        el->task_array[i] = NULL;
    };
    pthread_mutex_init(&el->el_mutex,NULL);
    pthread_cond_init(&el->thread_cond, NULL);
    el->current_task = NULL;
    el->stop_requested = false;
}

task* get_next_task(event_loop* el){
    task* t = NULL;
    for(int i=0;i<TASK_PRIORITY_MAX;i++){
        if(el->task_array[i]){
            t = el->task_array[i];
            el->task_array[i] = t->right;
            if(el->task_array[i]){
                el->task_array[i]->left = NULL;
            }
            t->right = NULL;
            t->left = NULL;
            return t; 
        }
    }
    return NULL;
}

task * create_task(event_cbk_t cbk, void* args, task_priority_t priority ){
    task *t = (task*)calloc(1,sizeof(task));
    t->cbk = cbk;
    t->args = args;
    t->priority = priority;
    t->right = NULL;
    t->left = NULL;   
    return t;
}

void el_add_task(event_loop* el,task *new_task){
    pthread_mutex_lock(&el->el_mutex);
    task *temp = NULL;
    task *t = NULL;
    t = el->task_array[new_task->priority];
    while(t){
        temp = t;
        t = t->right;
    }
    if(temp){
        temp->right = new_task;
        new_task->left = temp;
    }
    else{
        el->task_array[new_task->priority] = new_task;
    }
    if(el->state == EVENT_LOOP_RUNNING){
        pthread_mutex_unlock(&el->el_mutex);
        return;
    }
    else{
        pthread_cond_signal(&el->thread_cond);
        pthread_mutex_unlock(&el->el_mutex);
    }
}
void * el_thread_routine(void* arg){
    task* t;
    event_loop* el = (event_loop*) arg;
    while(!el->stop_requested){
        pthread_mutex_lock(&el->el_mutex);
        //if there are task execute else wait
        while((t=get_next_task(el)) == NULL){
            el->state = EVENT_LOOP_IDLE;
            pthread_cond_wait(&el->thread_cond, &el->el_mutex);
        }
        el->state = EVENT_LOOP_RUNNING;
        pthread_mutex_unlock(&el->el_mutex);

        el->state = EVENT_LOOP_RUNNING;
        el->current_task = t;
        t->cbk(t->args);
        el->current_task = NULL;
        //task must be heap allocated
        free(t);
    }
    printf("el thread routing stoped on request\n");
    return NULL;
}





void el_run(event_loop* el){
    printf("Executing el run\n");

    pthread_attr_t attr;
    el->thread = (pthread_t*)calloc(1,sizeof(pthread_t));
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);

    //create thread 
    pthread_create(el->thread, &attr, el_thread_routine, (void *)el);


}



void el_remove_task(event_loop* el,task *task){
    if(!el || !task) return;

    if (el->task_array[task->priority] == task) {
        el->task_array[task->priority] = task->right;
    }

	if(!task->left){
        if(task->right){
            task->right->left = NULL;
            task->right = 0;
            return;
        }
        return;
    }
    if(!task->right){
        task->left->right = NULL;
        task->left = NULL;
        return;
    }

    task->left->right = task->right;
    task->right->left = task->left;
    task->left = 0;
    task->right = 0;
}

