cmake_minimum_required(VERSION 2.8)
set(PROJECT_NAME ghLib)
project(${PROJECT_NAME})
set(CMAKE_BUILD_TYPE Debug)
option(ROBOT "Build library for Robot" ON)

set(EXT_PROJECTS_DIR ${PROJECT_SOURCE_DIR}/ext)
set(NWT_INCLUDE_DIR ${EXT_PROJECTS_DIR}/wpilib/networktables/cpp/include)
set(COMMON_INCLUDES ${PROJECT_SOURCE_DIR}/include ${NWT_INCLUDE_DIR})

add_subdirectory(${EXT_PROJECTS_DIR}/gtest gtest)
add_subdirectory(${EXT_PROJECTS_DIR}/wpilib/networktables/cpp networktables)

set(CMAKE_C_FLAGS
    "${CMAKE_C_FLAGS} -pthread -Wpointer-arith -Wtype-limits -Wwrite-strings -Wuninitialized -Werror=redundant-decls -Wsign-compare -Wconversion -g -fno-ident")
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS} -std=c++11")

file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_library(${PROJECT_NAME} STATIC ${SRC_FILES})

enable_testing()
set(PROJECT_TEST_NAME ${PROJECT_NAME}_test)
include_directories(${GTEST_INCLUDE_DIRS} ${COMMON_INCLUDES})

file(GLOB TEST_SRC_FILES ${PROJECT_SOURCE_DIR}/test/*.cpp)

add_executable(${PROJECT_TEST_NAME} ${TEST_SRC_FILES})
add_dependencies(${PROJECT_TEST_NAME} googletest)

target_link_libraries(${PROJECT_TEST_NAME}
    ${CMAKE_BINARY_DIR}/lib${PROJECT_NAME}.a
    ${GTEST_LIBS_DIR}/libgtest.a
    ${GTEST_LIBS_DIR}/libgtest_main.a
)

#target_link_libraries( ghLib.a ${CMAKE_BINARY_DIR}/networktables/libNetworkTables.a )
