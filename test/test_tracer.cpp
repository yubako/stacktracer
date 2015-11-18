#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
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

extern "C" int open(const char *pathname, int flags, ... ) {

    return mock_c()->actualCall("open")
            ->withStringParameters("pathname", pathname)
            ->withIntParameters("flags", flags)
            ->returnValue().value.intValue;
}

extern "C" ssize_t write(int fd, const void *buf, size_t count) {

    return 0;
}

static void output_dumpaddr(const char* buff, size_t size) {

    mock_c()->actualCall("output_dumpaddr")->withStringParameters("buff", buff)->withIntParameters("size", size);
}


static char   output_buffer[4096];
static size_t output_buffer_size = 0;
static void output_dumpaddr_combined(const char* buff, size_t size) {
    
    memcpy(output_buffer + output_buffer_size, buff, size);
    output_buffer_size += size;
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

TEST( tracer, set_call_frame_address ) {

    char data[10];
    function_call_stack_data* stack;

    mock_c()->enable();

    mock_c()->expectOneCall("mock__builtin_frame_address")
        ->withIntParameters("level", 0)
        ->andReturnPointerValue( &data[0]);

    mock_c()->expectOneCall("mock__builtin_frame_address")
        ->withIntParameters("level", 1)
        ->andReturnPointerValue( &data[10]);

    __cyg_profile_func_enter( (void*)0x1, (void*)0x2);
    stack = function_stack_peek();

    POINTERS_EQUAL( &data[0], stack->frame_addr);
    POINTERS_EQUAL( &data[10], stack->call_frame_addr );
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

TEST( tracer, create_dump_file ) {

    mock_c()->enable();

    mock_c()->expectOneCall("open")
        ->withStringParameters("pathname", "stack_dump.log")
        ->withIntParameters("flags", O_CREAT | O_WRONLY )
        ->andReturnIntValue(10);

    LONGS_EQUAL(10, create_dump_file());
    mock_c()->checkExpectations();
    mock_c()->clear();

}

TEST( tracer, check_address_dump ) {

    LONGS_EQUAL( 1, is_dump_address((unsigned char*)0x20) );
    LONGS_EQUAL( 0, is_dump_address((unsigned char*)0x21) );
    LONGS_EQUAL( 0, is_dump_address((unsigned char*)0x22) );
    LONGS_EQUAL( 0, is_dump_address((unsigned char*)0x23) );
    LONGS_EQUAL( 0, is_dump_address((unsigned char*)0x24) );
    LONGS_EQUAL( 0, is_dump_address((unsigned char*)0x25) );
    LONGS_EQUAL( 0, is_dump_address((unsigned char*)0x26) );
    LONGS_EQUAL( 0, is_dump_address((unsigned char*)0x27) );
    LONGS_EQUAL( 0, is_dump_address((unsigned char*)0x28) );
    LONGS_EQUAL( 0, is_dump_address((unsigned char*)0x29) );
    LONGS_EQUAL( 0, is_dump_address((unsigned char*)0x30) );
    LONGS_EQUAL( 1, is_dump_address((unsigned char*)0x40) );
}


TEST( tracer, skip_to_dumpaddr ) {

    uint32_t buff[64];
    char     expect_string[256];
    size_t   size;
    int      i;

    mock_c()->enable();
    UT_PTR_SET( output, output_dumpaddr );

    for ( i = sizeof(buff)/sizeof(buff[0]) - 1; i >= 0; i-- ) {
        if ( is_dump_address(&buff[i]) ) break;
    }

    /* 7つ目から表示 */
    size  = sprintf(expect_string, "\n%p: ", &buff[i]);
    size += sprintf(expect_string + size, "%8s ", " ");
    size += sprintf(expect_string + size, "%8s ", " ");
    size += sprintf(expect_string + size, "%8s ", " ");
    size += sprintf(expect_string + size, "%8s ", " ");
    size += sprintf(expect_string + size, "%8s ", " ");
    size += sprintf(expect_string + size, "%8s ", " ");
    size += sprintf(expect_string + size, "%8s ", " ");

    mock_c()->expectOneCall("output_dumpaddr")->withStringParameters("buff", expect_string)->withIntParameters("size", size);

    skip_to_dumpaddr( &buff[i + 7] );
    mock_c()->checkExpectations();
    mock_c()->clear();
}

TEST( tracer, skip_to_dumpaddr_middle ) {

    uint32_t buff[64];
    char     expect_string[256];
    size_t   size;
    int      i;

    mock_c()->enable();

    for ( i = sizeof(buff)/sizeof(buff[0]) - 1; i >= 0; i-- ) {
        if ( is_dump_address(&buff[i]) ) break;
    }

    /* 4つ目から表示 */
    size  = sprintf(expect_string, "\n%p: ", &buff[i]);
    size += sprintf(expect_string + size, "%8s ", " ");
    size += sprintf(expect_string + size, "%8s ", " ");
    size += sprintf(expect_string + size, "%8s ", " ");
    size += sprintf(expect_string + size, "%8s ", " ");

    UT_PTR_SET( output, output_dumpaddr );
    mock_c()->expectOneCall("output_dumpaddr")->withStringParameters("buff", expect_string)->withIntParameters("size", size);

    skip_to_dumpaddr( &buff[i + 4] );
    mock_c()->checkExpectations();
    mock_c()->clear();
}

TEST( tracer, skip_to_dumpaddr_fit ) {

    uint32_t buff[64];
    int      i;

    mock_c()->enable();
    for ( i = sizeof(buff)/sizeof(buff[0]) - 1; i >= 0; i-- ) {
        if ( is_dump_address(&buff[i]) ) break;
    }

    /* 1つ目から表示 */
    UT_PTR_SET( output, output_dumpaddr );
    skip_to_dumpaddr( &buff[i] );
    mock_c()->checkExpectations();
    mock_c()->clear();
}

TEST( tracer, dump_stack_binary ) {

    uint32_t buff[64];
    char     expect_string[512];
    size_t   size;
    size_t   i, j;

    mock_c()->enable();
    for ( i = 0; i < sizeof(buff)/sizeof(buff[0]); i++ ) {
        if ( is_dump_address(&buff[i]) ) break;
    }

    /* 1つ目から表示 */
    size  = sprintf(expect_string, "\n%p: ", &buff[i]);
    for ( j = 0; j < 8; j++ ) {
        buff[i + j] = j + 1;
        size += sprintf(expect_string + size, "%08x ", buff[i +  j]);
    }

    UT_PTR_SET( output, output_dumpaddr_combined );

    dump_stack_binary( &buff[i], &buff[i + 7]);

    STRCMP_EQUAL( expect_string, output_buffer );
    mock_c()->checkExpectations();
    mock_c()->clear();

}

TEST( tracer, get_function_name ) {

    STRCMP_EQUAL( "get_function_name", get_function_name( (const void*)get_function_name ));
}


