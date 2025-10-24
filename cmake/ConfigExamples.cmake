# ------------------------------------------------------------
# ConfigExamples.cmake
# - Apenas adiciona o subdiretório 'examples' se a
#   opção BUILD_EXAMPLES estiver ligada.
# ------------------------------------------------------------

if(NOT BUILD_EXAMPLES)
    return()
endif()

message(STATUS "[examples] Configurando subdiretório de exemplos...")
# Esta linha executa o examples/CMakeLists.txt
add_subdirectory(examples)