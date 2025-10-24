# ------------------------------------------------------------
# tests/CMakeLists.txt
# - Versão 2.5: Corrige CTest - Usa COMMAND ${test_name}
#   e WORKING_DIRECTORY na definição do teste.
# ------------------------------------------------------------
include(CTest)

add_custom_target(run_all_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    COMMENT "Executando todos os testes (via ctest)..."
)

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/ut_*.cpp"
)

message(STATUS "[tests] Encontrando testes...")

foreach(test_source IN LISTS TEST_SOURCES)
    get_filename_component(test_name ${test_source} NAME_WE)
    get_filename_component(test_dir_abs "${test_source}" DIRECTORY)
    file(RELATIVE_PATH test_dir_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${test_dir_abs}")
    # Caminho absoluto para o diretório de saída do executável
    set(test_output_dir "${CMAKE_BINARY_DIR}/tests/${test_dir_rel}")

    message(STATUS "  - Configurando teste: ${test_name}")

    add_executable(${test_name} ${test_source})

    # Propriedades C++20
    set_target_properties(${test_name} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )

    # Flag -fconcepts para GCC < 10
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 10.0)
        target_compile_options(${test_name} PRIVATE -fconcepts)
        message(STATUS " -- Adding -fconcepts flag for ${test_name} (GCC < 10 detected)")
    endif()

    # Linkagem
    target_link_libraries(${test_name} PRIVATE
        FVMGridMaker
        gtest_main
    )

    # Includes
    target_include_directories(${test_name} PRIVATE
        ${FVMG_INCLUDE_DIR}
    )

    # Propriedades de RPATH e diretório de saída do executável
    set_target_properties(${test_name} PROPERTIES INSTALL_RPATH "$ORIGIN")
    set_target_properties(${test_name} PROPERTIES BUILD_RPATH "${FVMG_OUTPUT_BIN_DIR}")
    # Define onde o *make* coloca o executável
    set_target_properties(${test_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${test_output_dir}")

    # --- CORREÇÃO AQUI ---
    # Adiciona ao CTest:
    # COMMAND é apenas o nome do executável/alvo.
    # WORKING_DIRECTORY diz ao CTest para entrar nessa pasta antes de rodar COMMAND.
    add_test(NAME ${test_name} COMMAND ${test_name})
    set_tests_properties(${test_name} PROPERTIES WORKING_DIRECTORY "${test_output_dir}")
    # --- FIM DA CORREÇÃO ---

    # Alvo run_... (executa diretamente, não muda)
    add_custom_target(run_${test_name}
        DEPENDS ${test_name}
        COMMAND ${test_name} # 'make' sabe onde encontrar o alvo ${test_name}
        WORKING_DIRECTORY "${test_output_dir}"
        COMMENT "Executando teste ${test_name} (via GTest)..."
    )

endforeach()