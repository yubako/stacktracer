#include <stdio.h>
#include "module.h"

int submodule2(int a, int b) {

    int c = 0xb;
    int d = 0xa;

    return a + b + c + d;

}
int submodule1(void) {

    int a = 0x3;
    int b = 0x4;

    int i;
    char buff[256];
    for ( i = 0; i < 256; i++ ) {
        buff[i] = i;
    }
    return a + b;
}

