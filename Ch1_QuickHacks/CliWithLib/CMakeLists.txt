cmake_minimum_required(VERSION 3.1)

project(CliWithLib)

add_subdirectory( third_party/cxxopts )

# find_package( Boost 1.60.0 REQUIRED COMPONENTS filesystem system iostreams )

# if( Boost_FOUND )
#   message( "boost found" )
# else()
#   message( FATAL_ERROR "Cannot find Boost" )
# endif()

set( CMAKE_CXX_STANDARD 17 )
if( NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message("Setting build type to 'RelWithDebInfo' as none was specified.")
  set( CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)
endif()

set( MainTarget main.cpp )

add_executable( CliWithLib ${MainTarget} )
target_include_directories( CliWithLib PUBLIC ${PROJECT_SOURCE_DIR} )
target_compile_definitions( CliWithLib PUBLIC Wall Wextra Wpedantic )
target_link_libraries( CliWithLib
                       cxxopts::cxxopts
                     )
