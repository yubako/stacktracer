cmake_minimum_required(VERSION 3.0)

# コンパイルオプション
#set (CMAKE_CXX_FLAGS "-g -Wall -O2 -finstrument-functions -rdynamic")
#set (CMAKE_C_FLAGS   "-g -Wall -O2 -finstrument-functions -rdynamic")
set (CMAKE_C_FLAGS   "-g -Wall -include wrap.h")
set (CMAKE_CXX_FLAGS   "-g -Wall -include wrap.h")

# プロジェクト名
project(stacktracer)

# バージョン
set(serial "1.0.0")

# 共有ライブラリバージョン
set(soserial "1")

# インクルードヘッダファイル指定
include_directories(src)
include_directories(test)
include_directories(/usr/local/include)

# 共有ライブラリとしてビルド
add_library(stacktracer SHARED src/stacktracer.c)

# バージョン番号の適用
set_target_properties(stacktracer PROPERTIES version ${serial} soversion ${soserial})

# リンクディレクトリ
link_directories(/usr/local/lib)

# 実行ファイル
#add_executable(myapp  src/module.cpp src/main.cpp)
add_executable(utest  test/runner.cpp test/test_tracer.cpp src/stacktracer.c)

# cpputestリンケージ
target_link_libraries(utest CppUTest CppUTestExt)


