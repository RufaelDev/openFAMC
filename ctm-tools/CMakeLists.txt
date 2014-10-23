cmake_minimum_required (VERSION 2.8)

# CWI IMTP CODEC: OPTIMZED MESH CODING WITH CERTH MESHES
SET (MODULE_NAME CTM-TOOLS)

project( ${MODULE_NAME})

add_subdirectory(glew)
add_subdirectory(pnglite)
add_subdirectory(rply)
add_subdirectory(tinyxml)
add_subdirectory(zlib)

add_definitions(-DDLL_IMPORT)

FILE (GLOB sources
		${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/*.cc
		${CMAKE_CURRENT_SOURCE_DIR}/*.c
		${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

FILE (GLOB headers
		${CMAKE_CURRENT_SOURCE_DIR}/*.h
		${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
    )

include_directories(
${CMAKE_CURRENT_SOURCE_DIR}/glew 
${CMAKE_CURRENT_SOURCE_DIR}/pnglite 
${CMAKE_CURRENT_SOURCE_DIR}/rply
${CMAKE_CURRENT_SOURCE_DIR}/tinyxml
${CMAKE_CURRENT_SOURCE_DIR}/zlib 
${CMAKE_CURRENT_SOURCE_DIR}/../openctmlib
${CMAKE_CURRENT_SOURCE_DIR}/../FAMCCodec
${CMAKE_CURRENT_SOURCE_DIR}/../FAMCDecoder
${CMAKE_CURRENT_SOURCE_DIR}/../FAMCEncoder
${CMAKE_CURRENT_SOURCE_DIR}/../ld3dmc
${CMAKE_CURRENT_SOURCE_DIR}/../Commun
${CMAKE_CURRENT_SOURCE_DIR}/../BaseFAMC
)
link_directories( ${CMAKE_CURRENT_BINARY_DIR}/glew 
${CMAKE_CURRENT_BINARY_DIR}/pnglite 
${CMAKE_CURRENT_BINARY_DIR}/rply
${CMAKE_CURRENT_BINARY_DIR}/tinyxml
${CMAKE_CURRENT_BINARY_DIR}/zlib
${CMAKE_CURRENT_BINARY_DIR}/../openctmlib
${CMAKE_CURRENT_BINARY_DIR}/../FAMCCodec
${CMAKE_CURRENT_BINARY_DIR}/../FAMCDecoder
${CMAKE_CURRENT_BINARY_DIR}/../FAMCEncoder
${CMAKE_CURRENT_BINARY_DIR}/../ld3dmc
${CMAKE_CURRENT_BINARY_DIR}/../BaseFAMC
)

ADD_EXECUTABLE(${MODULE_NAME} ${sources} ${headers})

target_link_libraries( ${MODULE_NAME} pnglite rply tinyxml zlib glew openctmlib FAMCCodec ld3dmc BaseFAMC )

add_custom_command(TARGET ${MODULE_NAME} POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${CMAKE_BINARY_DIR}/openctmlib/$<CONFIGURATION>/openctmlib.dll"      # <--this is in-file
        $<TARGET_FILE_DIR:CTM-TOOLS>)                 # <--this is out-file path
add_custom_command(TARGET ${MODULE_NAME} POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${CMAKE_SOURCE_DIR}/test_data/famcParamFile.txt"      # <--this is in-file
        $<TARGET_FILE_DIR:CTM-TOOLS>)                 # <--this is out-file path	

	file(COPY "${CMAKE_SOURCE_DIR}/test_data/" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Debug" )

	file(COPY "${CMAKE_SOURCE_DIR}/test_data/" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Release" )
