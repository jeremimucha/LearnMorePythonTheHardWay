cmake_minimum_required(VERSION 3.0)

project(cxxopts)

# Prepare "Catch2" library for other executables
add_library(cxxopts INTERFACE)
add_library(cxxopts::cxxopts ALIAS cxxopts)
target_include_directories(cxxopts INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
