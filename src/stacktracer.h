#ifndef __STACK_CALL_STACER_H
#define __STACK_CALL_STACER_H

typedef struct {

    void* func_addr;
    void* call_site;
    void* frame_addr;
    void* call_frame_addr;
    unsigned int stack_size;
}function_call_stack_data ;

function_call_stack_data* function_stack_peek();
function_call_stack_data* function_stack_pop();
void                      function_stack_clear();
int                       function_stack_is_empty();
void                      function_stack_push(function_call_stack_data*);
int                       create_dump_file();
void                      dump_stack_binary(const void* from, const void* to);
int                       is_dump_address( const void* addr );
extern void               (*output)(const char* buff, size_t size);
void                      skip_to_dumpaddr(const void* addr);
const char*               get_function_name(const void* funcaddr);

#endif

