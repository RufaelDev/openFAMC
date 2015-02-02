# CMAKE BUILD SCRIPT FOR LOADERS AND FAMC BY CWI
cmake_minimum_required (VERSION 2.6) 
project (FAMC) 

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(BaseFAMC) 
add_subdirectory(FAMCCodec)
add_subdirectory(FAMCDecoder)
add_subdirectory(FAMCEncoder)
add_subdirectory(ld3dmc)
add_subdirectory(openctmlib)
add_subdirectory(ctm-tools)
add_subdirectory(o3dg)