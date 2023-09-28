cmake_minimum_required(VERSION 3.26)
project(bootstrap)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)
# fetch `fmt` library
FetchContent_Declare(
        fmtlib
        GIT_REPOSITORY https://github.com/fmtlib/fmt
        GIT_TAG 10.1.1
)
FetchContent_MakeAvailable(fmtlib)

include_directories(include)
add_executable(bootstrap
        src/main.cpp
        src/parser/lexer.cpp
        src/parser/token.cpp
        include/parser/parser.h
        include/parser/ast/ast_node.hpp)

target_link_libraries(bootstrap fmt::fmt)
