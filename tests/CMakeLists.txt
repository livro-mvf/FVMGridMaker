# ------------------------------------------------------------
# tests/CMakeLists.txt - Versão Simplificada
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

if(NOT googletest_SOURCE_DIR)
    message(FATAL_ERROR "[tests] googletest_SOURCE_DIR não está definido.")
endif()

foreach(test_source IN LISTS TEST_SOURCES)
    get_filename_component(test_name ${test_source} NAME_WE)
    
    message(STATUS "  - Configurando teste: ${test_name}")

    add_executable(${test_name} ${test_source})

    # Define explicitamente onde o executável será gerado
    set_target_properties(${test_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )

    # Flag -fconcepts para GCC < 10
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 10.0)
        target_compile_options(${test_name} PRIVATE -fconcepts)
    endif()

    # Linkagem
    target_link_libraries(${test_name} PRIVATE
        FVMGridMaker
        gtest_main
    )

    # Includes
    target_include_directories(${test_name} PRIVATE
        SYSTEM "${googletest_SOURCE_DIR}/googletest/include"
        SYSTEM "${googletest_SOURCE_DIR}/googlemock/include"
        "${FVMG_INCLUDE_DIR}"
    )

    # --- CORREÇÃO SIMPLIFICADA ---
    # Usa caminho absoluto explícito
    set(test_executable_path "${CMAKE_CURRENT_BINARY_DIR}/${test_name}")
    
    add_test(
        NAME ${test_name}
        COMMAND "${test_executable_path}"
    )
    # --- FIM DA CORREÇÃO ---

    # Alvo run_... individual
    add_custom_target(run_${test_name}
        DEPENDS ${test_name}
        COMMAND "${test_executable_path}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        COMMENT "Executando teste ${test_name} (via GTest)..."
    )

endforeach()

# Garante que run_all_tests depende de todos os testes
set(TEST_TARGETS)
foreach(test_source IN LISTS TEST_SOURCES)
    get_filename_component(test_name ${test_source} NAME_WE)
    list(APPEND TEST_TARGETS ${test_name})
endforeach()

add_dependencies(run_all_tests ${TEST_TARGETS})