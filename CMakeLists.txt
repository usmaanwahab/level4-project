cmake_minimum_required(VERSION 3.28.3)
project(MyPass LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(NOELLE_DIR "/workspace/noelle")

find_package(LLVM 14 REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

add_library(Noelle SHARED IMPORTED)
set_target_properties(Noelle PROPERTIES IMPORTED_LOCATION
    "${NOELLE_DIR}/install/lib/Noelle.so"
)

add_llvm_pass_plugin(MyPass src/main.cpp)

target_include_directories(MyPass PRIVATE
    ${NOELLE_DIR}/install/include
    ${NOELLE_DIR}/install/include/svf
)

target_link_libraries(MyPass PRIVATE Noelle)

file(GLOB TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp")

foreach(src ${TEST_SOURCES})
    get_filename_component(name ${src} NAME_WE)
    get_filename_component(dir ${src} DIRECTORY)

    set(OUTPUT_LL "${dir}/${name}.ll")
    set(OUTPUT_BC "${dir}/${name}.bc")

    add_custom_command(
        OUTPUT ${OUTPUT_LL}
        COMMAND clang++-14 -fopenmp -S -emit-llvm -o ${OUTPUT_LL} ${src}
        DEPENDS ${src}
        VERBATIM
    )

    add_custom_command(
        OUTPUT ${OUTPUT_BC}
        COMMAND clang++-14 -fopenmp -emit-llvm -c -o ${OUTPUT_BC} ${src}
        DEPENDS ${src}
        VERBATIM
    )

    add_custom_target(${name}_ir ALL
        DEPENDS ${OUTPUT_LL} ${OUTPUT_BC}
    )
endforeach()
