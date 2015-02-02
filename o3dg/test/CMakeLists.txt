project(O3DGC_TEST)
FILE (GLOB PROJECT_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
		${CMAKE_CURRENT_SOURCE_DIR}/inc/*.h
		${CMAKE_CURRENT_SOURCE_DIR}/inc/*.inl
    )

add_executable(test_o3dgc ${PROJECT_FILES})

link_directories( 
	${CMAKE_CURRENT_BINARY_DIR}/o3dgc_common_lib 
	${CMAKE_CURRENT_BINARY_DIR}/o3dgc_enc_lib
	${CMAKE_CURRENT_BINARY_DIR}/o3dgc_dec_lib
)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../o3dgc_decode_lib/inc" "${CMAKE_CURRENT_SOURCE_DIR}/../o3dgc_encode_lib/inc" "${CMAKE_CURRENT_SOURCE_DIR}/../o3dgc_common_lib/inc")

target_link_libraries(test_o3dgc o3dgc_dec_lib o3dgc_enc_lib o3dgc_common_lib)
IF(WIN32)
target_link_libraries(test_o3dgc o3dgc_common_lib)
ELSEIF(APPLE)
target_link_libraries(test_o3dgc o3dgc_common_lib)
ELSE ()
set(CMAKE_CXX_FLAGS "-g -Wall")
target_link_libraries(test_o3dgc o3dgc_common_lib rt)
ENDIF()




