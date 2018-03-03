cmake_minimum_required( VERSION 3.1 )

set( Project Sort )
project( ${Project} )


###############################################################################
# Prepare external dependencies
###############################################################################
# If using any external libraries create a 'external'
# subdir with an appropriate CMakeLists and add the following
# for each library
# add_subdirectory( external/some_lib )
add_subdirectory( external/clara )
add_subdirectory( external/catch )


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
set( MainSources
    ${PROJECT_SOURCE_DIR}/src/main.cpp
   )

# Add all of the remaining source files to be build as a static library
# linked against the main executable
# set( LibrarySources
#      ${PROJECT_SOURCE_DIR}/src/small_test.cpp
#    )
# Or let all of the source files be globed, minus the one given in MainSources
file( GLOB LibrarySources
      "${PROJECT_SOURCE_DIR}/src/*.cpp"
    )
list( REMOVE_ITEM LibrarySources ${MainSources} )


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
# Internal library first
set( InternalLibrary "${Project}_internallib" ) # cmake will add a "lib" prefix to the name

add_library( ${InternalLibrary}
    ${LibrarySources}
    )
add_library( Lib::InternalLibrary
    ALIAS ${InternalLibrary}
    )
target_include_directories( ${InternalLibrary}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
    )
if( CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU" )
    target_compile_options( ${InternalLibrary} PUBLIC -Wall -Wextra -pedantic -Werror )
endif()
if( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
	target_compile_options( ${InternalLibrary} PRIVATE /W4 /WX )
endif()
# target_link_libraries( ${InternalLibrary}
#     # ${Boost_LIBRARIES}
#     )

# Then the executable
# set( Executable !!_ENTER_EXECUTABLE_NAME_HERE!! )
add_executable( ${PROJECT_NAME}
    ${MainSources}
    )
target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
    )
if( CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU" )
    target_compile_options( ${PROJECT_NAME} PUBLIC -Wall -Wextra -pedantic -Werror )
endif()
if( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
	target_compile_options( ${PROJECT_NAME} PRIVATE /W4 /WX )
endif()

# If using any external libraries include them here after the Internal Library.
# They need to be properly found first. See find_package section
target_link_libraries( ${PROJECT_NAME}
    Lib::InternalLibrary
    Clara::Clara
    # ${Boost_LIBRARIES}
    )


###############################################################################
# Unit Tests
###############################################################################
add_subdirectory( tests )


###############################################################################
# Install
###############################################################################
# for make install
# Set installation root directory
# set( CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" )
# install( TARGETS ${PROJECT_NAME}
#          DESTINATION .
#        )

# # On Windows
# # install( TARGETS ${PROJECT_NAME}
# #          RUNTIME DESTINATION .
# #        )

# # install any additional files
# # install( FILES enter-file-dir-here 
# #          DESTINATION .
# #        )

# # Create a cpack-based installer
# include( InstallRequiredSystemLibraries )
# # set( CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt" )
# # set( CPACK_PACKAGE_VERSION_MAJOR "${${Project}_VERSION_MAJOR}" )
# # set( CPACK_PACKAGE_VERSION_MINOR "${${Project}_VERSION_MINOR}" )
# include( CPack )
