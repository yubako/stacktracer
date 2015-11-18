#include <stdio.h>
#include "module.h"

int main(void) {


    int ret = submodule1() + submodule2(0x20, 0x30);
    printf("hoge = %d\n" , ret);

    return 0;
}


