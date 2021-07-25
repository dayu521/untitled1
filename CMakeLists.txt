cmake_minimum_required(VERSION 3.16)

project(untitled1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#if(GNU)
#    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-pthread")
#endif()
#message(${CMAKE_CXX_COMPILER_ID})
#if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
#add_compile_options(-pthread)#给预处理器使用
#add_link_options(-pthread)#给链接器使用
#add_compile_options(${PROJECT_NAME} -fcoroutines)
#endif()
#add_compile_definitions(ATEXIT_FIXED)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/cista/include)
# ...

add_executable(untitled1
#    xxbb.cpp
    main.cpp   
    )
add_executable(test_yield test_yield.cpp)

target_compile_features(untitled1 PRIVATE cxx_std_20)
target_compile_options(untitled1 PRIVATE -fcoroutines)
target_compile_options(untitled1 PRIVATE -pthread)
target_link_options(untitled1 PRIVATE -pthread)

enable_testing()
add_test(NAME test_untitled1 COMMAND untitled1)
add_test(NAME test_yield COMMAND test_yield)
#add_subdirectory(doctest)

#add_library(smallobj SmallObj.cpp)
#target_include_directories(smallobj PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

#add_executable(mytest
#    test2.cpp
#    test_allocator.cpp
#    test_threadlocal.cpp
#    test_bst.cpp)

#target_compile_options(mytest PRIVATE -pthread)
#target_link_options(mytest PRIVATE -pthread)

#target_link_libraries(mytest PRIVATE doctest_with_main)
#target_link_libraries(mytest PRIVATE smallobj)

#target_link_libraries(${PROJECT_NAME} pthread)
#add_custom_target(clangformat
#  COMMAND clang-format -i json_struct.h
#  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
#  SOURCES json_struct.h)
#target_link_libraries(${PROJECT_NAME} jsoncpp)

#target_link_libraries(${PRIJECT_NAME} libgcc)

#cmake_minimum_required(VERSION 3.0)
#project(cmake_test VERSION 0.0.1 LANGUAGES CXX)

# Prepare doctest for other targets to use
#find_package(doctest REQUIRED)

## Make test executable
#add_executable(tests main.cpp)
#target_compile_features(test PRIVATE cxx_std_17)
#target_link_libraries(test PRIVATE doctest::doctest)
