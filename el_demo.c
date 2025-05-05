#include "event_loop.h"

typedef struct {
    int p1,p2;
}arg_obj;

void add(void* arg){
    arg_obj* obj = (arg_obj*)arg;
    int s = obj->p1 + obj->p2;
    printf("adding numbers %d + %d = %d\n",obj->p1, obj->p2, s);
}
void sub(void* arg){
    arg_obj* obj = (arg_obj*)arg;
    int s = obj->p1 - obj->p2;
    printf("subracting numbers %d - %d = %d\n",obj->p1, obj->p2, s);
}

int main(int argc, char** argv)
{
    event_loop el;
    el_init(&el);
    arg_obj* args_add = calloc(1, sizeof(arg_obj));
    args_add->p1 = 1;
    args_add->p2 = 2;
    arg_obj* args_sub = calloc(1, sizeof(arg_obj));
    args_sub->p1 = 3;
    args_sub->p2 = 2;
    arg_obj* args_sub1 = calloc(1, sizeof(arg_obj));
    args_sub1->p1 = 5;
    args_sub1->p2 = 6;
    task* t_add = create_task(add,(void*)args_add, TASK_PRIORITY_MEDIUM);
    task* t_sub = create_task(sub,(void*)args_sub,TASK_PRIORITY_HIGH);
    task* t_sub1 = create_task(sub,(void*)args_sub1,TASK_PRIORITY_MEDIUM);
    el_add_task(&el,t_add);
    el_add_task(&el,t_sub);
    el_add_task(&el,t_sub1);
    el_remove_task(&el,t_sub);
    el_run(&el);
    printf("end of main\n");
    scanf("\n");
    free(args_add);
    free(args_sub);
    free(args_sub1);
    return 0;
    //arg_obj obj = { .param1 = 1, .param2 = 3 };

}