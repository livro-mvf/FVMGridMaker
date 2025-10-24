# ------------------------------------------------------------
# examples/CMakeLists.txt
# - Um executável por pasta que contenha [Ee]x_*.cpp
# - Adaptado para FVMGridMaker (STL-Only)
# - Lógica de 'run_...' REVERTIDA (sem CTest)
# - CORRIGIDO o erro de 'libFVMGridMaker.so not found'
# ------------------------------------------------------------
cmake_minimum_required(VERSION 3.16)

# NÃO inclua o CTest aqui
# include(CTest) 

# Políticas úteis
if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()
if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

# Standalone (caso rode cmake direto nesta pasta)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(FVMGridMakerExamples LANGUAGES CXX)
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(FVMG_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/../FVMGridMakerLib/include")
  # Se standalone, FVMG_OUTPUT_BIN_DIR não está definido, precisamos de um fallback
  set(FVMG_OUTPUT_BIN_DIR "${CMAKE_BINARY_DIR}/../bin") 
endif()

set(PROJECT_ROOT    "${CMAKE_SOURCE_DIR}")
set(EXAMPLES_ROOT   "${CMAKE_CURRENT_SOURCE_DIR}")
if(NOT DEFINED FVMG_INCLUDE_DIR)
    set(FVMG_INCLUDE_DIR "${PROJECT_ROOT}/FVMGridMakerLib/include")
endif()

# Alvos agregados
add_custom_target(examples         COMMENT "Compilando todos os exemplos")

# --- REVERSÃO AQUI: 'run_all_examples' volta a ser um alvo agregador simples ---
add_custom_target(run_all_examples COMMENT "Executando todos os exemplos...")
# --- FIM DA REVERSÃO ---


# 1) Varre recursivamente todos os [Ee]x_*.cpp
file(GLOB_RECURSE EXAMPLE_CPP_ABS CONFIGURE_DEPENDS "${EXAMPLES_ROOT}/[Ee]x_*.cpp")
if(EXAMPLE_CPP_ABS STREQUAL "")
  message(STATUS "[examples] Nenhum arquivo [Ee]x_*.cpp encontrado em ${EXAMPLES_ROOT}")
  return()
endif()

# (A lógica de agrupar por diretório e nomear alvos permanece a mesma...)
set(EXAMPLE_DIRS_ABS "")
foreach(f_abs IN LISTS EXAMPLE_CPP_ABS)
  get_filename_component(dir_abs "${f_abs}" DIRECTORY)
  list(APPEND EXAMPLE_DIRS_ABS "${dir_abs}")
endforeach()
list(REMOVE_DUPLICATES EXAMPLE_DIRS_ABS)
message(STATUS "[examples] Pastas de exemplos detectadas:")

foreach(dir_abs IN LISTS EXAMPLE_DIRS_ABS)
  file(GLOB EX_SOURCES_THIS_DIR "${dir_abs}/[Ee]x_*.cpp")
  if(EX_SOURCES_THIS_DIR STREQUAL "")
    continue()
  endif()
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

  # Cria alvo
  add_executable(${target_name} ${EX_SOURCES_THIS_DIR})
  target_link_libraries(${target_name} PRIVATE FVMGridMaker)
  target_include_directories(${target_name} PRIVATE "${FVMG_INCLUDE_DIR}")

  # --- CORREÇÃO AQUI: Corrige o erro "libFVMGridMaker.so not found" ---
  # Define o RPATH (runtime search path) para o executável
  # $ORIGIN é um token especial que significa "a pasta do executável"
  # FVMG_OUTPUT_BIN_DIR é a pasta build/bin/ onde a .so está
  set_target_properties(${target_name} PROPERTIES
      INSTALL_RPATH "$ORIGIN" 
      BUILD_RPATH "${FVMG_OUTPUT_BIN_DIR}"
  )
  # --- FIM DA CORREÇÃO ---

  # Define o nome/local de saída
  if(_n_src EQUAL 1)
    set(out_name "${stem}")
  else()
    set(out_name "${target_name}")
  endif()
  set_target_properties(${target_name} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${dir_abs}"
    OUTPUT_NAME "${out_name}"
  )

  # Otimizações
  if(COMMAND set_target_optimizations)
    set_target_optimizations(${target_name})
  endif()

  # --- REVERSÃO AQUI: Alvos auxiliares voltam a rodar o comando direto ---
  
  # 1. Dependência de compilação
  add_dependencies(examples ${target_name})

  # 2. Cria o alvo 'run_Ex_Calculator'
  add_custom_target(run_${target_name}
    DEPENDS ${target_name}
    COMMAND "${dir_abs}/${out_name}" # Comando direto
    WORKING_DIRECTORY "${dir_abs}"
    COMMENT "Executando exemplo ${out_name} (${dir_rel})"
  )
  
  # 3. Adiciona dependência ao alvo agregador
  add_dependencies(run_all_examples run_${target_name})
  
  # --- FIM DA REVERSÃO ---
endforeach()