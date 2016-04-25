cmake_minimum_required(VERSION 3.0)

project(vn_tests)

set(VN_ROOT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(BOOST_VERSION 1.60.0)
set(BOOST_COMPONENTS )


set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
set(Boost_DEBUG 1)

find_package(Boost ${BOOST_VERSION} COMPONENTS ${BOOST_COMPONENTS}) 

if(Boost_FOUND)
    file(GLOB TEST_SOURCES *.cpp)

    add_executable(vn_test ${TEST_SOURCES})
 
    target_include_directories(vn_test BEFORE PUBLIC "${VN_ROOT_SRC}/include")
    target_include_directories(vn_test PUBLIC ${Boost_INCLUDE_DIRS}) 
    target_link_libraries(vn_test ${Boost_LIBRARIES})
endif()




