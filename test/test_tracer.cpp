#include <stdio.h>
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport_c.h"

extern "C" {
    #include "stacktracer.h"
}

extern "C" void __cyg_profile_func_enter(void* func_addr, void* call_site);
extern "C" void __cyg_profile_func_exit(void* func_addr, void* call_site);


/* mock用関数 */
extern "C" void* mock__builtin_frame_address(unsigned int level) {

    return mock_c()->actualCall("mock__builtin_frame_address")
            ->withIntParameters("level", level)
            ->returnValue().value.pointerValue;
}


TEST_GROUP( tracer ) {

    TEST_SETUP() {
        mock_c()->disable();
        function_stack_clear();
    }


    TEST_TEARDOWN() { 
        mock_c()->clear();
    }

};

TEST( tracer, peek_a_callstack_then_no_push ) {

    POINTERS_EQUAL(NULL, function_stack_peek());
}

TEST( tracer, peek_a_callstack ) {

    mock_c()->disable();
    function_call_stack_data* stack;

    __cyg_profile_func_enter( (void*)0x10, (void*)0x11);
    stack = function_stack_peek();
    POINTERS_EQUAL(0x10, stack->func_addr);
    POINTERS_EQUAL(0x11, stack->call_site);
}


TEST( tracer, pop_a_callstack )  {

    function_call_stack_data* stack;

    __cyg_profile_func_enter( (void*)0x10, (void*)0x11);
    stack = function_stack_pop();
    POINTERS_EQUAL(0x10, stack->func_addr);
    POINTERS_EQUAL(0x11, stack->call_site);
    POINTERS_EQUAL(NULL, function_stack_pop());
}

TEST( tracer, check_empty ) {

    function_stack_clear();
    CHECK( function_stack_is_empty() );
}

TEST( tracer, check_not_empty ) {

    __cyg_profile_func_enter( (void*)0x10, (void*)0x11);
    CHECK( !function_stack_is_empty() );
}


TEST( tracer, push_some_callstack_peek ) {

    function_call_stack_data *stack;
    __cyg_profile_func_enter( (void*)0x10, (void*)0x11);
    __cyg_profile_func_enter( (void*)0x12, (void*)0x13);

    stack = function_stack_peek();
    POINTERS_EQUAL(0x12, stack->func_addr);
    POINTERS_EQUAL(0x13, stack->call_site);

    stack = function_stack_peek();
    POINTERS_EQUAL(0x12, stack->func_addr);
    POINTERS_EQUAL(0x13, stack->call_site);
}

TEST( tracer, push_some_callstack_pop ) {

    function_call_stack_data *stack;
    __cyg_profile_func_enter( (void*)0x10, (void*)0x11);
    __cyg_profile_func_enter( (void*)0x12, (void*)0x13);

    stack = function_stack_pop();
    POINTERS_EQUAL(0x12, stack->func_addr);
    POINTERS_EQUAL(0x13, stack->call_site);

    stack = function_stack_pop();
    POINTERS_EQUAL(0x10, stack->func_addr);
    POINTERS_EQUAL(0x11, stack->call_site);
}

TEST( tracer, set_builtin_frame_address ) {

    function_call_stack_data *stack;

    mock_c()->enable();
    mock_c()->expectOneCall("mock__builtin_frame_address")
        ->withIntParameters("level", 0)
        ->andReturnPointerValue((void*)0x99);

    __cyg_profile_func_enter( (void*)0x1, (void*)0x2);
    stack = function_stack_peek();

    POINTERS_EQUAL(0x99, stack->frame_addr);
    mock_c()->checkExpectations();
    mock_c()->clear();
}

TEST( tracer, pop_callstack_at_func_exit) {

    __cyg_profile_func_enter((void*)0x1, (void*)0x2);
    __cyg_profile_func_exit( (void*)0x1, (void*)0x2);
    CHECK(function_stack_is_empty());
    POINTERS_EQUAL(NULL, function_stack_peek());
    POINTERS_EQUAL(NULL, function_stack_pop());

}


