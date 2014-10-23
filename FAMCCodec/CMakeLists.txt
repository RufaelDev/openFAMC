# CMAKE BUILD SCRIPT FOR LOADERS AND FAMC BY CWI
cmake_minimum_required (VERSION 2.6) 

SET(MODULE_NAME FAMCCodec)

add_definitions(-DNO_FAMC_STANDALONE_BUILD)

FILE (GLOB sources
		${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../Commun/*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/*.c
		${CMAKE_CURRENT_SOURCE_DIR}/*.h
		${CMAKE_CURRENT_SOURCE_DIR}/*.cc
		${CMAKE_CURRENT_SOURCE_DIR}/../Commun/*.h
    )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../Commun/)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../ld3dmc/)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../FAMCEncoder/)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../FAMCDecoder/)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../BaseFAMC/)

add_library (${MODULE_NAME}  ${sources} ${CMAKE_CURRENT_SOURCE_DIR}/../FAMCEncoder/FAMCEncoder.cpp ${CMAKE_CURRENT_SOURCE_DIR}/../FAMCDecoder/FAMCDecoder.cpp) 

link_directories (${CMAKE_CURRENT_BINARY_DIR}/../BaseFAMC  ${CMAKE_CURRENT_BINARY_DIR}/../ld3dmc ${CMAKE_CURRENT_BINARY_DIR}/../FAMCEncoder ${CMAKE_CURRENT_BINARY_DIR}/../FAMCDecoder ) 

# Link the executable to the Hello library. 
target_link_libraries (${MODULE_NAME}  BaseFAMC ld3dmc) 

add_custom_command(TARGET ${MODULE_NAME} POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${PROJECT_BINARY_DIR}/openctmlib/$<CONFIGURATION>/openctmlib.dll"      # <--this is in-file
        $<TARGET_FILE_DIR:${MODULE_NAME}>)                 # <--this is out-file path