# ------------------------------------------------------------
# Compiler settings and optimizations
# ------------------------------------------------------------

# NOTA: include_directories() global removido.
#       Usamos target_include_directories() por alvo.

# Adiciona flags específicas para C++20 Concepts em GCC < 10
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 10.0)
    message(STATUS "Adding -fconcepts flag for GCC ${CMAKE_CXX_COMPILER_VERSION}")
    add_compile_options(-fconcepts)
endif()

# Function to set compiler optimizations
function(set_target_optimizations target)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE -O3 -march=native -flto)
    elseif(MSVC)
        target_compile_options(${target} PRIVATE /O2 /GL)
    endif()
endfunction()