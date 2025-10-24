# ------------------------------------------------------------
# Project options and policies
# ------------------------------------------------------------

# Set minimum CMake version
cmake_minimum_required(VERSION 3.15)

# Set policies
if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)  # Improved MSVC_RUNTIME_LIBRARY handling
endif()
if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)  # FILE_SET for install()
endif()

# C++ standard (RNF01)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Project options (RNF07)
option(BUILD_EXAMPLES "Build examples" ON)
option(BUILD_TESTS "Build tests" ON)
option(BUILD_DOCS "Build docs (Doxygen + Sphinx)" OFF)

# Default build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

# Set build type options
set(CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING "Choose the type of build: Debug Release RelWithDebInfo MinSizeRel")

# Project layout (Adaptado para FVMGridMaker)
set(FVMG_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/FVMGridMakerLib)
set(FVMG_INCLUDE_DIR ${FVMG_LIB_DIR}/include)
set(FVMG_SRC_DIR ${FVMG_LIB_DIR}/src)
# Saída para DENTRO do diretório de build
set(FVMG_OUTPUT_BIN_DIR ${CMAKE_BINARY_DIR}/bin)

# Compiler-specific settings
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # GCC-specific settings
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 13.0)
        add_compile_options(-Wno-psabi)  # Silence ABI warnings if needed
    endif()
endif()

# Common compiler warnings
add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wnon-virtual-dtor
    -Wold-style-cast
    -Wcast-align
    -Wunused
    -Woverloaded-virtual
    -Wconversion
    -Wsign-conversion
)

# Release mode optimizations
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3 -DNDEBUG)
endif()

# Debug mode options
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -O0 -DDEBUG)
endif()