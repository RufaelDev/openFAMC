project(O3DGC_ENC_LIB CXX C)

set(LIB_TYPE "STATIC" CACHE STRING "STATIC, SHARED or MODULE?")
#set(CMAKE_CXX_FLAGS "-g -Wall")
FILE (GLOB PROJECT_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
		${CMAKE_CURRENT_SOURCE_DIR}/inc/*.h
		${CMAKE_CURRENT_SOURCE_DIR}/inc/*.inl
    )

add_library(o3dgc_enc_lib ${PROJECT_FILES})

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../o3dgc_decode_lib/inc" "${CMAKE_CURRENT_SOURCE_DIR}/inc" "${CMAKE_CURRENT_SOURCE_DIR}/../o3dgc_common_lib/inc")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../o3dgc_common_lib/inc")

