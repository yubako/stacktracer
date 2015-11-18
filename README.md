libstacktracer
========

##概要##
関数内で使用されるスタックについて、関数コール前/コール後それぞれでスタックの  
中身をダンプしてみる。  
動いたり動かなかったりしたときにスタックの使用状況とかを見たりするとき用？  

## 使用例##
下記の関数の場合だと

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

下記な感じでログに出してくれる

     -- _Z10submodule1v (0x400a7a) using stack 304(0x130) byte - before
    0x7ffcb1442a80:                                     004008a0 00000000 00400a97 00000000
    0x7ffcb1442aa0: 02c19430 00007f12 02e1ac00 00007f12 b1442bf0 00007ffc 02e1a918 00007f12
    0x7ffcb1442ac0: b1442cd0 00007ffc 0209ef17 00007f12 00000000 00000000 00000018 00000030
    0x7ffcb1442ae0: b1442bb0 00007ffc b1442af0 00007ffc b1442bf0 00007ffc 02e50e07 00007f12
    0x7ffcb1442b00: 00000000 00000000 02127620 00007f12 00000001 00000000 020999fa 00007f12
    0x7ffcb1442b20: b1442bc0 00007ffc b1442bf0 00007ffc 004008a0 00000000 02e57325 00007f12
    0x7ffcb1442b40: 00000039 00000000 02127620 00007f12 00000039 00000000 b1442bf0 00007ffc
    0x7ffcb1442b60: b1442bc0 00007ffc 00000002 00000000 020999fa 00007f12 030624b8 00007f12
    0x7ffcb1442b80: 0000000b 00000000 02c19e55 00007f12 020999fa 00007f12 b1442bc0 00007ffc
    0x7ffcb1442ba0: b1442bf0 00007ffc 02c19fbd 00007f12 00000000 00000000 00000000 00000000
    0x7ffcb1442bc0: b1442bf0

     -- _Z10submodule1v (0x400a7a) - after
    0x7ffcb1442a80:                                     004008a0 00000000 00400ae6 00000000
    0x7ffcb1442aa0: 03020100 07060504 0b0a0908 0f0e0d0c 13121110 17161514 1b1a1918 1f1e1d1c
    0x7ffcb1442ac0: 23222120 27262524 2b2a2928 2f2e2d2c 33323130 37363534 3b3a3938 3f3e3d3c
    0x7ffcb1442ae0: 43424140 47464544 4b4a4948 4f4e4d4c 53525150 57565554 5b5a5958 5f5e5d5c
    0x7ffcb1442b00: 63626160 67666564 6b6a6968 6f6e6d6c 73727170 77767574 7b7a7978 7f7e7d7c
    0x7ffcb1442b20: 83828180 87868584 8b8a8988 8f8e8d8c 93929190 97969594 9b9a9998 9f9e9d9c
    0x7ffcb1442b40: a3a2a1a0 a7a6a5a4 abaaa9a8 afaeadac b3b2b1b0 b7b6b5b4 bbbab9b8 bfbebdbc
    0x7ffcb1442b60: c3c2c1c0 c7c6c5c4 cbcac9c8 cfcecdcc d3d2d1d0 d7d6d5d4 dbdad9d8 dfdedddc
    0x7ffcb1442b80: e3e2e1e0 e7e6e5e4 ebeae9e8 efeeedec f3f2f1f0 f7f6f5f4 fbfaf9f8 fffefdfc
    0x7ffcb1442ba0: b1442bf0 00000004 00000003 00000100 00000000 00000000 00000000 00000000
    0x7ffcb1442bc0: b1442bf0


## 使い方##
makeしてlibstacktracer.soを作る。

    $ cmake .
    $ make

トレースしたい関数を -finstrument-functions -rdynamicをつけて再コンパイル

    $ g++ -finstrument-functions -rdynamic main.cpp module.cpp -o a.out

LD_PRELOAD を指定して libstacktrace.soをリンクしながらa.outを実行  
カレントディレクトリにstack_dump.logができる。

    $ LD_PRELOAD=./libstacktracer.so  ./a.out
    $ ls stack_dump.log
 
おわり


