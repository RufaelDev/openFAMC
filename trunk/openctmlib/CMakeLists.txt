cmake_minimum_required (VERSION 2.8)

SET (LIB_NAME openctmlib)

add_definitions(-DDLL_EXPORT)

FILE (GLOB sources
		${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/*.cc
		${CMAKE_CURRENT_SOURCE_DIR}/liblzma/*.c
		${CMAKE_CURRENT_SOURCE_DIR}/liblzma/*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/liblzma/*.cc
		${CMAKE_CURRENT_SOURCE_DIR}/*.c
    )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/liblzma)
	
FILE (GLOB headers
		${CMAKE_CURRENT_SOURCE_DIR}/*.h
		${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
    )
	
ADD_LIBRARY(${LIB_NAME} SHARED ${sources} ${headers})