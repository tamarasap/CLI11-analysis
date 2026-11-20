# Prevodi moje testove za CLI11.
cmake_minimum_required(VERSION 3.14)
project(MojiTestovi CXX)

set(CLI11_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../CLI11")

# Catch2 v2 (jedan header fajl), ista verzija koju koriste testovi CLI11
set(CATCH_HEADER "${CMAKE_BINARY_DIR}/catch2/catch.hpp")
if(NOT EXISTS "${CATCH_HEADER}")
  file(DOWNLOAD
    https://github.com/philsquared/Catch/releases/download/v2.13.10/catch.hpp
    "${CATCH_HEADER}"
    EXPECTED_HASH SHA256=3725c0f0a75f376a5005dde31ead0feb8f7da7507644c201b814443de8355170)
endif()

file(GLOB MOJI_TEST_FAJLOVI "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp")

add_executable(MojiTestovi "${CLI11_DIR}/tests/main.cpp" ${MOJI_TEST_FAJLOVI})
target_include_directories(MojiTestovi PRIVATE
  "${CLI11_DIR}/include"
  "${CLI11_DIR}/tests"
  "${CMAKE_BINARY_DIR}")

enable_testing()
add_test(NAME MojiTestovi COMMAND MojiTestovi)
