cmake_minimum_required(  VERSION 3.1 )

set( Project Uniq )
project( ${Project} )


###############################################################################
# Prepare external dependencies
###############################################################################
# If using any external libraries create a 'external'
# subdir with an appropriate CMakeLists and add the following
# for each library
add_subdirectory( external/clara )

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

# Or glob all the files
file( GLOB Sources 
      "${PROJECT_SOURCE_DIR}/src/*.cpp"
    )


###############################################################################
# Configure build
###############################################################################
# Set required C++ standard
set( CMAKE_CXX_STANDARD 17 )
# Set build type
if( NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message("Setting build type to 'RelWithDebInfo' as none was specified.")
  set( CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)
endif()

# Export compile_commands.json for use with cppcheck
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )


###############################################################################
# Build target
###############################################################################
# set( Executable !!_ENTER_EXECUTABLE_NAME_HERE_!! )

add_executable( ${PROJECT_NAME} ${Sources} )

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
    )
if( CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU" )
    target_compile_options( ${PROJECT_NAME} PUBLIC -Wall -Wextra -pedantic -Werror )
endif()
if( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
	target_compile_options( ${PROJECT_NAME} PRIVATE /W4 /WX )
endif()

# if using any external libraries include them here
# they need to be properly found first. See find_package section
target_link_libraries( ${PROJECT_NAME}
    Clara::Clara
    # ${Boost_LIBRARIES}
    )
