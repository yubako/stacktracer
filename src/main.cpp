#include <stdio.h>
#include "module.h"

int main(void) {


    int ret = submodule1() + submodule2(20, 30);
    printf("hoge = %d\n" , ret);

    return 0;
}


