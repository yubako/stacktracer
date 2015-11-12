#include <stdio.h>
#include "module.h"

int submodule2(int a, int b) {

    int c = 0xb;
    int d = 0xa;

    return a + b + c + d;

}
int submodule1(void) {

    int a;
    int b;

    a = 0x3;
    b = 0x4;


    return a + b;
}

