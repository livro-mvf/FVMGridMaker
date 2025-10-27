# ------------------------------------------------------------
# examples/CMakeLists.txt
# - Um executável por pasta que contenha [Ee]x_*.cpp
# - Além dos [Ee]x_*.cpp, inclui automaticamente TODOS os
#   outros .cpp da mesma pasta (registradores, helpers, etc.),
#   reproduzindo a estratégia usada em tests/CMakeLists.txt.
# - Mantém alvos 'examples', 'run_all_examples' e 'run_<alvo>'
# - NÃO define modo paralelo; apenas detecta e linka TBB se existir.
#   O programa decide (via #if no .cpp).
# ------------------------------------------------------------
cmake_minimum_required(VERSION 3.16)

# Políticas úteis
if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()
if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

# Standalone (se rodar cmake direto nesta pasta)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(FVMGridMakerExamples LANGUAGES CXX)
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(FVMG_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/../FVMGridMakerLib/include")
  # Fallback para onde a .so fica quando standalone
  set(FVMG_OUTPUT_BIN_DIR "${CMAKE_BINARY_DIR}/../bin")
endif()

set(PROJECT_ROOT  "${CMAKE_SOURCE_DIR}")
set(EXAMPLES_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
if(NOT DEFINED FVMG_INCLUDE_DIR)
  set(FVMG_INCLUDE_DIR "${PROJECT_ROOT}/FVMGridMakerLib/include")
endif()

# ------------------------------------------------------------
# Descoberta de capacidades (NÃO força modo; só informa disponibilidade)
# ------------------------------------------------------------
include(CheckCXXSourceCompiles)
set(CMAKE_REQUIRED_QUIET TRUE)

check_cxx_source_compiles("
  #include <execution>
  int main(){ auto p = std::execution::par; (void)p; return 0; }
" FVMG_HAVE_PSTL_EXEC)

find_package(TBB QUIET) # não falha se não houver TBB

# ------------------------------------------------------------
# Alvos agregados
# ------------------------------------------------------------
add_custom_target(examples         COMMENT "Compilando todos os exemplos")
add_custom_target(run_all_examples COMMENT "Executando todos os exemplos...")

# ------------------------------------------------------------
# Varre recursivamente todos os [Ee]x_*.cpp
# ------------------------------------------------------------
file(GLOB_RECURSE EXAMPLE_CPP_ABS CONFIGURE_DEPENDS
  "${EXAMPLES_ROOT}/[Ee]x_*.cpp"
)
if(EXAMPLE_CPP_ABS STREQUAL "")
  message(STATUS "[examples] Nenhum arquivo [Ee]x_*.cpp encontrado em ${EXAMPLES_ROOT}")
  return()
endif()

# Lista de diretórios únicos
set(EXAMPLE_DIRS_ABS "")
foreach(f_abs IN LISTS EXAMPLE_CPP_ABS)
  get_filename_component(dir_abs "${f_abs}" DIRECTORY)
  list(APPEND EXAMPLE_DIRS_ABS "${dir_abs}")
endforeach()
list(REMOVE_DUPLICATES EXAMPLE_DIRS_ABS)
message(STATUS "[examples] Pastas de exemplos detectadas:")

# ------------------------------------------------------------
# Cria um alvo por pasta e o respectivo run_<alvo>
# - Além dos [Ee]x_*.cpp, inclui todos os outros .cpp daquele dir
#   (registradores, helpers, etc.), assim como fazemos em tests/.
# ------------------------------------------------------------
foreach(dir_abs IN LISTS EXAMPLE_DIRS_ABS)
  # Fontes principais (exemplos)
  file(GLOB EX_SOURCES_THIS_DIR CONFIGURE_DEPENDS "${dir_abs}/[Ee]x_*.cpp")
  if(EX_SOURCES_THIS_DIR STREQUAL "")
    continue()
  endif()

  # Demais .cpp do diretório (extras)
  file(GLOB ALL_CPP_THIS_DIR CONFIGURE_DEPENDS "${dir_abs}/*.cpp")
  # Remove os [Ee]x_*.cpp da lista total para ficar só os extras
  set(EXTRA_CPP_THIS_DIR "${ALL_CPP_THIS_DIR}")
  list(REMOVE_ITEM EXTRA_CPP_THIS_DIR ${EX_SOURCES_THIS_DIR})

  # Nome do alvo
  file(RELATIVE_PATH dir_rel "${EXAMPLES_ROOT}" "${dir_abs}")
  if(dir_rel STREQUAL "")
    set(dir_rel ".")
  endif()

  list(LENGTH EX_SOURCES_THIS_DIR _n_src)
  if(_n_src EQUAL 1)
    list(GET EX_SOURCES_THIS_DIR 0 _only_src)
    get_filename_component(stem "${_only_src}" NAME_WE)
    set(target_name "${stem}")
  else()
    string(REPLACE "/" "_" target_name "${dir_rel}")
    set(target_name "ex_${target_name}")
    message(WARNING "[examples] Pasta '${dir_rel}' contém ${_n_src} exemplos; alvo único '${target_name}'.")
  endif()

  if(TARGET "${target_name}")
    string(MD5 _hash "${dir_rel}")
    string(SUBSTRING "${_hash}" 0 8 _short)
    set(target_name "${target_name}__${_short}")
  endif()

  message(STATUS "  - ${dir_rel}  -> alvo: ${target_name}")
  if(NOT EXTRA_CPP_THIS_DIR STREQUAL "")
    list(LENGTH EXTRA_CPP_THIS_DIR _n_extra)
    message(STATUS "      + ${_n_extra} arquivo(s) extra(s) .cpp adicionados ao alvo (registradores/helpers/etc.).")
  endif()

  # Junta fontes principais + extras
  set(SRCS_FOR_TARGET ${EX_SOURCES_THIS_DIR} ${EXTRA_CPP_THIS_DIR})

  add_executable(${target_name} ${SRCS_FOR_TARGET})
  target_link_libraries(${target_name} PRIVATE FVMGridMaker)
  target_include_directories(${target_name} PRIVATE "${FVMG_INCLUDE_DIR}")

  # Corrige 'libFVMGridMaker.so not found' ao executar direto
  set_target_properties(${target_name} PROPERTIES
    INSTALL_RPATH "$ORIGIN"
    BUILD_RPATH   "${FVMG_OUTPUT_BIN_DIR}"
  )

  # Nome/local de saída
  if(_n_src EQUAL 1)
    set(out_name "${stem}")
  else()
    set(out_name "${target_name}")
  endif()
  set_target_properties(${target_name} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${dir_abs}"
    OUTPUT_NAME              "${out_name}"
  )

  # Otimizações (se a função util existir no projeto)
  if(COMMAND set_target_optimizations)
    set_target_optimizations(${target_name})
  endif()

  # ----------------------------------------------------------
  # Apenas sinaliza disponibilidade e faz link SE existir TBB.
  # (O programa decide usar ou não a via paralela.)
  # ----------------------------------------------------------
  if(FVMG_HAVE_PSTL_EXEC)
    target_compile_definitions(${target_name} PRIVATE FVMG_HAVE_PSTL_EXEC=1)
  else()
    target_compile_definitions(${target_name} PRIVATE FVMG_HAVE_PSTL_EXEC=0)
  endif()

  if(TBB_FOUND)
    target_compile_definitions(${target_name} PRIVATE FVMG_HAVE_TBB=1)
    target_link_libraries(${target_name} PRIVATE TBB::tbb)
  else()
    target_compile_definitions(${target_name} PRIVATE FVMG_HAVE_TBB=0)
  endif()

  # Agrega em 'examples'
  add_dependencies(examples ${target_name})

  # run_<alvo>
  add_custom_target(run_${target_name}
    DEPENDS ${target_name}
    COMMAND "${dir_abs}/${out_name}"
    WORKING_DIRECTORY "${dir_abs}"
    COMMENT "Executando exemplo ${out_name} (${dir_rel})"
  )
  add_dependencies(run_all_examples run_${target_name})
endforeach()

# ------------------------------------------------------------
# Resumo (só para log)
# ------------------------------------------------------------
message(STATUS "Resumo (detecção para TODOS os exemplos):")
message(STATUS "  TBB_FOUND           = ${TBB_FOUND}")
message(STATUS "  FVMG_HAVE_PSTL_EXEC = ${FVMG_HAVE_PSTL_EXEC}")
