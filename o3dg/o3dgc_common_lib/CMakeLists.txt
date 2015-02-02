project(O3DGC_COMMON_LIB CXX C)

set(LIB_TYPE "STATIC" CACHE STRING "STATIC, SHARED or MODULE?")
message("[o3dgc] \t LIB_TYPE " ${LIB_TYPE})
#set(CMAKE_CXX_FLAGS "-g -Wall")

FILE (GLOB sources
		${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
		${CMAKE_CURRENT_SOURCE_DIR}/inc/*.h
		${CMAKE_CURRENT_SOURCE_DIR}/inc/*.inl
    )

	include_directories("${CMAKE_CURRENT_SOURCE_DIR}/inc")
	
#SET_TARGET_PROPERTIES(o3dgc_common_lib PROPERTIES LINKER_LANGUAGE CXX)
	
add_library(o3dgc_common_lib STATIC ${sources})

