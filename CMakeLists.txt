cmake_minimum_required(VERSION 2.8)
set(PROJECT_NAME ghLib)

option(ROBOT "Build library for Robot" ON)

#set(CMAKE_BUILD_TYPE Debug)

set(EXT_PROJECTS_DIR ${CMAKE_SOURCE_DIR}/ext)
set(WPI_PATH ${CMAKE_SOURCE_DIR}/ext/wpilib)

if(ROBOT)
set(CMAKE_TOOLCHAIN_FILE ${WPI_PATH}/arm-toolchain.cmake)
endif(ROBOT)

project(${PROJECT_NAME})


set(NWT_INCLUDE_DIR ${EXT_PROJECTS_DIR}/wpilib/networktables/cpp/include)
set(COMMON_INCLUDES ${PROJECT_SOURCE_DIR}/include ${NWT_INCLUDE_DIR})


file(GLOB_RECURSE NI_LIBS ${WPI_PATH}/ni-libraries/*.so*)
list(REMOVE_ITEM NI_LIBS ${WPI_PATH}/ni-libraries/libwpi.so ${WPI_PATH}/ni-libraries/libwpi_2015.so)
set(WPI_LD_LIBS ${WPI_PATH}/ni-libraries/libwpi.so ${WPI_PATH}/ni-libraries/libwpi_2015.so)


get_filename_component(WPILIB_INCLUDES ${WPI_PATH}/wpilibc/wpilibC++/include REALPATH)
get_filename_component(HAL_API_INCLUDES ${WPI_PATH}/hal/include REALPATH)
#get_filename_component(NWT_API_INCLUDES ${WPI_PATH}/networktables/cpp/include REALPATH)
get_filename_component(WPILIB_DEVICES_INCLUDES ${WPI_PATH}/wpilibc/wpilibC++Devices/include REALPATH)

if(ROBOT)
	add_definitions(-DROBOT=1)
else(ROBOT)
	add_definitions(-DGHLIB_PREF_FILENAME="${PROJECT_SOURCE_DIR}/test/prefs.ini")
	add_subdirectory(${EXT_PROJECTS_DIR}/gtest gtest)
	add_subdirectory(${EXT_PROJECTS_DIR}/wpilib/networktables/cpp networktables)
endif(ROBOT)

add_subdirectory(${EXT_PROJECTS_DIR}/libwebsockets libwebsockets)
#add_subdirectory(${EXT_PROJECTS_DIR}/jsoncpp jsoncpp)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS} -std=c++11")

file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_library(${PROJECT_NAME} STATIC ${SRC_FILES})

include_directories(${COMMON_INCLUDES} ${NWT_INCLUDE_DIR} ${LIBWEBSOCKETS_INCLUDE_DIRS})

if(ROBOT)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter -fPIC -L.  -march=armv7-a -mtune=cortex-a9 ")
	set(CMAKE_EXE_LINKER_FLAGS "-Wl,-rpath,/opt/GenICam_v2_3/bin/Linux_armv7-a,-L.")

	include_directories(${WPILIB_DEVICES_INCLUDES} ${WPILIB_INCLUDES} ${HAL_API_INCLUDES})

	set(wpilib_nonshared ${LIBRARY_PATH}/libwpilib_nonshared.a)
	set(HALAthena ${LIBRARY_PATH}/libHALAthena.a)
	set(NetworkTables ${LIBRARY_PATH}/libNetworkTables.a)

else(ROBOT)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wpointer-arith -Wtype-limits -Wwrite-strings -Wuninitialized -Werror=redundant-decls -Wsign-compare -Wconversion -g -fno-ident")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS} -std=c++11")
	include_directories(${GTEST_INCLUDE_DIRS})
	enable_testing()
	set(PROJECT_TEST_NAME ${PROJECT_NAME}_test)

	file(GLOB TEST_SRC_FILES ${PROJECT_SOURCE_DIR}/test/*.cpp)

	add_executable(${PROJECT_TEST_NAME} ${TEST_SRC_FILES})
	add_dependencies(${PROJECT_TEST_NAME} googletest NetworkTables)

	target_link_libraries(${PROJECT_TEST_NAME}
			${CMAKE_BINARY_DIR}/lib${PROJECT_NAME}.a
			${GTEST_LIBS_DIR}/libgtest.a
			${GTEST_LIBS_DIR}/libgtest_main.a
			${LIBWEBSOCKETS_LIBS_DIR}/libwebsockets.a
			${CMAKE_BINARY_DIR}/networktables/libNetworkTables.a
	)
endif(ROBOT)

#target_link_libraries( ghLib.a ${CMAKE_BINARY_DIR}/networktables/libNetworkTables.a )
