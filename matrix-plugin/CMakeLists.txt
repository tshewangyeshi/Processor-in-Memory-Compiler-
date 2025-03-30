cmake_minimum_required(VERSION 3.13)
project(MatrixPass LANGUAGES C CXX)


find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION} in ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
set(CMAKE_CXX_STANDARD 17)

add_library(MatrixPass MODULE src/MatrixPass.cpp)
set_target_properties(MatrixPass PROPERTIES
  COMPILE_FLAGS "-fno-rtti"
)
