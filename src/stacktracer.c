#include <stdio.h>
#include <string.h>
#include "stacktracer.h"

static function_call_stack_data stack[4096];
static int stack_count = 0;
static function_call_stack_data data;
extern void* __builtin_frame_address(unsigned int level);


__attribute__((constructor)) 
static void stacktracer_initialize() {


}

__attribute__((destructor)) 
static void stacktracer_destruct() {

}


__attribute__((no_instrument_function))
void __cyg_profile_func_enter(void* func_addr, void* call_site) {

    data.func_addr = func_addr;
    data.call_site = call_site;
    data.frame_addr  = (void*)__builtin_frame_address(0);
    data.call_frame_addr = (void*)__builtin_return_address(1);
    function_stack_push(&data);
}


__attribute__((no_instrument_function)) 
void __cyg_profile_func_exit(void* func_addr, void* call_site) {

    function_stack_pop();
}


/**
 * @brief スタックを空にする
 */
void function_stack_clear() {
    stack_count = 0;
}

/**
 * @brief スタックが空かどうかを調べる
 */
int function_stack_is_empty() {

    return ( !stack_count );
}

/**
 * @brief 関数コールスタックに詰む
 */
void function_stack_push(function_call_stack_data* data) {

    memcpy(stack + stack_count, data, sizeof(function_call_stack_data));
    stack_count ++;
}

/**
 * @brief 関数コールスタックの先頭を参照する
 *
 */
function_call_stack_data* function_stack_peek() {

    if ( function_stack_is_empty() ) {
        return NULL;
    }
    return &stack[stack_count - 1];
}


/**
 * @brief 関数コールスタックの先頭を取得し、スタックから削除する
 */
function_call_stack_data* function_stack_pop() {

    if ( function_stack_is_empty() ) {
        return NULL;
    }

    stack_count --;
    return &stack[stack_count];
}





