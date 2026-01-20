cmake_minimum_required(VERSION 3.28.3)
project(MyPass LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)

set(NOELLE_DIR "/opt/noelle")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(LLVM 14 REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_library(Noelle SHARED IMPORTED)
set_target_properties(Noelle PROPERTIES IMPORTED_LOCATION
    "${NOELLE_DIR}/install/lib/Noelle.so"
)

include_directories("${NOELLE_DIR}/install/include")

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

add_llvm_pass_plugin(MyPass src/main.cpp)

target_include_directories(MyPass PRIVATE
    ${NOELLE_DIR}/install/include
    ${NOELLE_DIR}/install/include/svf
)

target_link_libraries(MyPass PRIVATE Noelle)
