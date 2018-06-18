cmake_minimum_required(  VERSION 3.1 )

set( Project FsmTable )
project( ${Project} )


###############################################################################
# Prepare external dependencies
###############################################################################
# If using any external libraries create a 'external'
# subdir with an appropriate CMakeLists and add the following
# for each library
# add_subdirectory( external/clara )

# Find any external libraries via find_backage
# see cmake --help-module-list and cmake --help-module ModuleName
# for details on a specific module
# If using boost
# find_package( Boost 1.60.0
#   REQUIRED COMPONENTS
#   filesystem system iostreams
#   )
#
# if( Boost_FOUND )
#   include_directories( ${Boost_INCLUDE_DIRS} )
# else()
#   message( FATAL_ERROR "Cannon find Boost" )
# endif()


###############################################################################
# Prepare source files for build
###############################################################################
# Create a Sources variable to all the cpp files necessary
# to build the target
# set( Sources
#     ${PROJECT_SOURCE_DIR}/src/file1.cpp
#     ${PROJECT_SOURCE_DIR}/src/file2.cpp
#    )


###############################################################################
# Configure build
###############################################################################
# Set required C++ standard
set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED TRUE )
# Set build type
if( NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message("Setting build type to 'Debug' as none was specified.")
  set( CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

# Export compile_commands.json for use with cppcheck
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )


###############################################################################
# Build target
###############################################################################

add_executable( basic src/basic.cpp )

if( CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU" )
    target_compile_options( basic PUBLIC
    -Wall -Wextra -pedantic 
    -fconcepts
    )
endif()
if( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
	target_compile_options( basic PRIVATE /W4 /WX )
endif()

add_executable( extended src/extended.cpp )

if( CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU" )
    target_compile_options( extended PUBLIC
    -Wall -Wextra -pedantic 
    -fconcepts
    )
endif()
if( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
	target_compile_options( extended PRIVATE /W4 /WX )
endif()

# if using any external libraries include them here
# they need to be properly found first. See find_package section
# target_link_libraries( basic
#     # Clara::Clara
#     # ${Boost_LIBRARIES}
#     )
