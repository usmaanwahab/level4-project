cmake_minimum_required(VERSION 3.28.3)
project(HelloWorldPass LANGUAGES C CXX)

set(NOELLE_DIR "/home/usmaan/Workspace/noelle")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(OPENMP_LIB_DIR "/home/usmaan/Workspace/llvm-14/openmp/build/runtime/src")
link_directories(${OPENMP_LIB_DIR})

set(OPENMP_INCLUDE_DIR "/home/usmaan/Workspace/llvm-14/openmp/runtime/src/include")
include_directories(${OPENMP_INCLUDE_DIR})

find_package(LLVM 14 REQUIRED CONFIG)

message(STATUS "LLVM include dirs: ${LLVM_INCLUDE_DIRS}")
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_library(Noelle SHARED IMPORTED)
set_target_properties(Noelle PROPERTIES IMPORTED_LOCATION
    "${NOELLE_DIR}/install/lib/Noelle.so"
)

include_directories(${NOELLE_DIR}/include)
link_directories(${NOELLE_DIR}/install/lib)

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

add_llvm_pass_plugin(HelloWorldPass src/main.cpp)

target_link_libraries(HelloWorldPass PRIVATE Noelle)

file(GLOB TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp")

target_include_directories(HelloWorldPass PRIVATE ${OPENMP_INCLUDE_DIR})
target_link_libraries(HelloWorldPass PRIVATE ${OPENMP_LIB_DIR}/libomp.so)

foreach(src ${TEST_SOURCES})
    get_filename_component(name ${src} NAME_WE)
    get_filename_component(dir ${src} DIRECTORY)

    set(OUTPUT_LL "${dir}/${name}.ll")

    add_custom_command(
        OUTPUT ${OUTPUT_LL}
        COMMAND clang-14
                -x c++
                -fopenmp
                -I${OPENMP_INCLUDE_DIR}
                -S -emit-llvm -o ${OUTPUT_LL} ${src}
        DEPENDS ${src}
        COMMENT "Compiling ${src} -> ${OUTPUT_LL} with custom OpenMP"
        VERBATIM
    )

    add_custom_target(${name}_ll ALL
        DEPENDS ${OUTPUT_LL}
    )
endforeach()
