cmake_minimum_required(VERSION 3.10)
project(bign_tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(..)
include_directories(../..)

# Source files
set(BIGN_SOURCES
  IBigNumber.h
  BigNumberNaive.h
  BigNumberNaive.cpp
  BigNumberCore.h
  BigNumberCore.cpp
  BigDigit.h
)

# Test executable
add_executable(bign_tests tests.cc
  ${BIGN_SOURCES}
)

# Link libraries if needed
# target_link_libraries(bign_tests ...)

