cmake_minimum_required(VERSION 4.1.1)
project(HelloWorldPass LANGUAGES C CXX)

find_package(LLVM REQUIRED CONFIG)

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

add_llvm_pass_plugin(HelloWorldPass src/main.cpp)

