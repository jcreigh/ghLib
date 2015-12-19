cmake_minimum_required(VERSION 2.8)
set(PROJECT_NAME ghLib)

option(ROBOT "Build library for Robot" ON)
option(WPILIB_DIR "Path to WPI libraries" ~/Robotics/lib)

#set(CMAKE_BUILD_TYPE Debug)

set(EXT_PROJECTS_DIR ${CMAKE_SOURCE_DIR}/ext)

if(ROBOT)
	set(CMAKE_TOOLCHAIN_FILE ${EXT_PROJECTS_DIR}/arm-toolchain.cmake)
endif(ROBOT)

project(${PROJECT_NAME})

add_subdirectory(${EXT_PROJECTS_DIR}/narflib narflib)
add_subdirectory(${EXT_PROJECTS_DIR}/narflog narflog)
add_subdirectory(${EXT_PROJECTS_DIR}/ntcore ntcore)
add_subdirectory(${EXT_PROJECTS_DIR}/wpilib)

include_directories(${PROJECT_SOURCE_DIR}/include ${NWT_INCLUDE_DIR}
	${NARFLOG_INCLUDE_DIR} ${NARFLIB_INCLUDE_DIR} ${LIBWEBSOCKETS_INCLUDE_DIRS})

if(ROBOT)
	add_definitions(-DROBOT=1)

	file(GLOB_RECURSE NI_LIBS ${WPILIB_DIR}/ni-libraries/*.so*)
	list(REMOVE_ITEM NI_LIBS ${WPILIB_DIR}/ni-libraries/libwpi.so ${WPILIB_DIR}/ni-libraries/libwpi_2015.so)
	set(WPILIB_LD_LIBS ${WPILIB_DIR}/ni-libraries/libwpi.so ${WPILIB_DIR}/ni-libraries/libwpi_2015.so)

else(ROBOT)
	add_definitions(-DNARF_PREF_FILENAME="${PROJECT_SOURCE_DIR}/test/prefs.ini")
	add_definitions(-DNARF_LOGGER_BASEPATH="${PROJECT_SOURCE_DIR}/test/")
	add_subdirectory(${EXT_PROJECTS_DIR}/gtest gtest)
endif(ROBOT)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS} -std=c++1y")

file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_library(${PROJECT_NAME} STATIC ${SRC_FILES})
add_dependencies(${PROJECT_NAME} ntcore narflib narflog)

if(ROBOT)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter -fPIC -L.  -march=armv7-a -mtune=cortex-a9 ")
	set(CMAKE_EXE_LINKER_FLAGS "-Wl,-rpath,/opt/GenICam_v2_3/bin/Linux_armv7-a,-L.")

	add_dependencies(${PROJECT_NAME} wpilib)
	include_directories(${WPILIB_SRC_DIR}/wpilibc/Athena/include)
	include_directories(${WPILIB_SRC_DIR}/hal/include)
	include_directories(${WPILIB_SRC_DIR}/hal/lib/Athena/)
	include_directories(${WPILIB_SRC_DIR}/wpilibc/shared/include)

	set(wpilib_nonshared ${WPILIB_DIR}/libwpilib_nonshared.a)
	set(HALAthena ${WPILIB_DIR}/libHALAthena.a)

else(ROBOT)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wpointer-arith -Wtype-limits -Wwrite-strings -Wuninitialized -Werror=redundant-decls -Wsign-compare -Wconversion -g -fno-ident")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS} -std=c++11")
	include_directories(${GTEST_INCLUDE_DIRS})
	enable_testing()
	set(PROJECT_TEST_NAME ${PROJECT_NAME}_test)

	file(GLOB TEST_SRC_FILES ${PROJECT_SOURCE_DIR}/test/*.cpp)

	add_executable(${PROJECT_TEST_NAME} ${TEST_SRC_FILES})
	add_dependencies(${PROJECT_TEST_NAME} googletest)

	target_link_libraries(${PROJECT_TEST_NAME}
			${CMAKE_BINARY_DIR}/lib${PROJECT_NAME}.a
			${GTEST_LIBS_DIR}/libgtest.a
			${GTEST_LIBS_DIR}/libgtest_main.a
			${NARFLOG_LIBS_DIR}/libnarflog.a
			${NARFLIB_LIBS_DIR}/libnarflib.a
			${NWT_LIBS_DIR}/libntcore.so
	)
endif(ROBOT)

