#ifndef __STACK_CALL_STACER_H
#define __STACK_CALL_STACER_H

typedef struct {

    void* func_addr;
    void* call_site;
    void* frame_addr;
    void* call_frame_addr;
}function_call_stack_data ;

function_call_stack_data* function_stack_peek();
function_call_stack_data* function_stack_pop();
void                      function_stack_clear();
int                       function_stack_is_empty();
void                      function_stack_push(function_call_stack_data*);


#endif

