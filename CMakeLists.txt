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
# Add build options
#=======================================================================================================================

# NOTE: Algorithms is header-only, so without examples or the tests, there are not targets to be built.
option(CREATE_DOCS "Build library documentation (requires Doxygen and Graphviz/Dot to be installed)" ON)
option(BUILD_SAMPLES "Build sample programs" ON)
option(BUILD_TESTS "Build and run library tests" ON)

#=======================================================================================================================
# Add project subdirectories
#=======================================================================================================================
add_subdirectory(library)

if (${CREATE_DOCS})
    add_subdirectory(doxy)
endif ()

if (${BUILD_TESTS})
    #add_subdirectory(tests)
endif ()

if (${BUILD_SAMPLES})
    add_subdirectory(examples)
endif ()