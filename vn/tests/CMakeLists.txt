cmake_minimum_required(VERSION 3.0)

project(vn_tests)

set(VN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../)

IF (NOT DEFINED BOOST_DIR)
MESSAGE(FATAL_ERROR "Boost include path must be defined; use -DBOOST_DIR=... to specify")
ENDIF()

include_directories(BEFORE ${VN_DIR})
include_directories(${BOOST_DIR})

file(GLOB SOURCES *.cpp)

add_executable(vn_test ${SOURCES})