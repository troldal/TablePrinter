#=======================================================================================================================
# Preamble
#=======================================================================================================================
cmake_minimum_required(VERSION 3.03 FATAL_ERROR)
project (TablePrinter)

#=======================================================================================================================
# Set project metadata
#=======================================================================================================================
set(PROJECT_VENDOR "Kenneth Troldal Balslev")
set(PROJECT_CONTACT "kenneth.balslev@gmail.com")
set(PROJECT_URL "https://github.com/troldal/TablePrinter")
set(PROJECT_DESCRIPTION "A header-only C++ library for printing tables in console applications")

#=======================================================================================================================
# Set C/C++ compiler version
#=======================================================================================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#=======================================================================================================================
# Define Example target
#=======================================================================================================================
add_executable(Example example.cpp)
target_include_directories(Example PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)