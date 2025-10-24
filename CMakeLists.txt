# ------------------------------------------------------------
# Root CMakeLists.txt for FVMGridMaker
# ------------------------------------------------------------
cmake_minimum_required(VERSION 3.15)

# Definição do projeto
project(FVMGridMaker VERSION 1.0 LANGUAGES CXX)

# Incluir todos os módulos
include(cmake/ConfigUtils.cmake)          # Utilitários (bloqueio de in-source build)
include(cmake/ConfigOptions.cmake)        # Opções (C++20, BUILD_TESTS, etc)
include(cmake/ConfigCompiler.cmake)       # Configurações do compilador
include(cmake/ConfigTargets.cmake)        # Alvo principal da biblioteca
include(cmake/ConfigExamples.cmake)       # Exemplos (chama add_subdirectory)
include(cmake/ConfigTests.cmake)          # Testes (chama add_subdirectory)
include(cmake/ConfigDocs.cmake)           # Documentação (Sphinx)

# --- CORREÇÃO AQUI ---
# A chamada de add_subdirectory(examples) FOI REMOVIDA DAQUI
# porque ela já está DENTRO de cmake/ConfigExamples.cmake.
#
# if(BUILD_EXAMPLES)
#     add_subdirectory(examples) # <- LINHA REMOVIDA
# endif()
#
# O mesmo vale para 'tests', que já é chamado por ConfigTests.cmake
#
# if(BUILD_TESTS)
#     # ConfigTests.cmake chama add_subdirectory(tests) internamente
# endif()
# --- FIM DA CORREÇÃO ---


# Mensagem final
message(STATUS "FVMGridMaker configuration complete!")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")