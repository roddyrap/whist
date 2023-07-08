cmake_minimum_required(VERSION 3.15)

# In-source builds clutter the source and are considered bad practice.
if ( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
    message( FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there. You may need to remove CMake cache files." )
endif()

project(Whist VERSION 0.3)

set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")

# Only GNU like compilers are supported.
if(NOT gcc_like_cxx)
    message(FATAL_ERROR "Only GNU-like compilers are supported.")
endif()

# Make sure that CMAKE_BUILD_TYPE has a value, and set a default value if it's not available.
if(NOT DEFINED CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE "Not Specified")
endif()

configure_file(WhistConfig.h.in WhistConfig.h)

add_library(whist_compiler_flags INTERFACE)
target_compile_features(whist_compiler_flags INTERFACE cxx_std_20)
target_compile_options(whist_compiler_flags INTERFACE
  "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Werror>>"
)

option(USE_TERMINAL "Compile the project for a terminal" ON)

add_subdirectory(Logic)

if(USE_TERMINAL)
    add_subdirectory(Terminal)
endif()
