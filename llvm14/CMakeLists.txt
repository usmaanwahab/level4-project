cmake_minimum_required(VERSION 3.28.3)
project(HelloWorldPass LANGUAGES C CXX)

find_package(LLVM 14 REQUIRED CONFIG)

message(STATUS "LLVM include dirs: ${LLVM_INCLUDE_DIRS}")
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

add_llvm_pass_plugin(HelloWorldPass src/main.cpp)

file(GLOB TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp")

foreach(src ${TEST_SOURCES})
    get_filename_component(name ${src} NAME_WE)
    get_filename_component(dir ${src} DIRECTORY)

    set(OUTPUT_LL "${dir}/${name}.ll")

    add_custom_command(
        OUTPUT ${OUTPUT_LL}
        COMMAND clang++ -fopenmp -S -emit-llvm -o ${OUTPUT_LL} ${src}
        DEPENDS ${src}
        COMMENT "Compiling ${src} -> ${OUTPUT_LL}"
        VERBATIM
    )

    add_custom_target(${name}_ll ALL
        DEPENDS ${OUTPUT_LL}
    )
endforeach()
