cmake_minimum_required(VERSION 3.1 )

project( concat )

add_subdirectory( third_party/clara )

set( CMAKE_CXX_STANDARD 17 )
if( NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message("Setting build type to 'RelWithDebInfo' as none was specified.")
  set( CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)
endif()

set( MainTarget main.cpp )

add_executable( concat ${MainTarget} )
target_include_directories( concat PUBLIC ${PROJECT_SOURCE_DIR} )
target_compile_definitions( concat PUBLIC Wall Wextra Wpedantic )
target_link_libraries( concat Clara )
