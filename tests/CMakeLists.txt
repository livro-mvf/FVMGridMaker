# ------------------------------------------------------------
# tests/CMakeLists.txt - Descoberta e montagem dos testes
# ------------------------------------------------------------
include(CTest)

add_custom_target(run_all_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    COMMENT "Executando todos os testes (via ctest)..."
)

# Todos os testes principais começam com ut_*.cpp
file(GLOB_RECURSE TEST_MAIN_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/**/ut_*.cpp"
)

message(STATUS "[tests] Encontrando testes...")

if(NOT googletest_SOURCE_DIR)
    message(FATAL_ERROR "[tests] googletest_SOURCE_DIR não está definido.")
endif()

set(ALL_TEST_TARGETS)

foreach(test_main IN LISTS TEST_MAIN_SOURCES)
    get_filename_component(test_name ${test_main} NAME_WE)
    get_filename_component(test_dir  ${test_main} DIRECTORY)

    message(STATUS "  - Configurando teste: ${test_name} (dir: ${test_dir})")

    # Colete todos os .cpp auxiliares no mesmo diretório do teste principal
    file(GLOB helper_srcs CONFIGURE_DEPENDS "${test_dir}/*.cpp")

    # Remova o próprio arquivo ut_*.cpp da lista de helpers
    list(REMOVE_ITEM helper_srcs "${test_main}")

    # Remova quaisquer outros ut_*.cpp para evitar múltiplos mains
    foreach(s IN LISTS helper_srcs)
        if(s MATCHES ".*/ut_.*\\.cpp$")
            list(REMOVE_ITEM helper_srcs "${s}")
        endif()
    endforeach()

    # Monte o executável do teste com o main + helpers do diretório
    add_executable(${test_name} ${test_main} ${helper_srcs})

    # Saída e padrão de compilação
    set_target_properties(${test_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )

    # -fconcepts para GCC antigos
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

    # Caminho absoluto do executável para ctest e alvos run_*
    set(test_exe "${CMAKE_CURRENT_BINARY_DIR}/${test_name}")

    add_test(NAME ${test_name} COMMAND "${test_exe}")

    add_custom_target(run_${test_name}
        DEPENDS ${test_name}
        COMMAND "${test_exe}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        COMMENT "Executando teste ${test_name} (via GTest)..."
    )

    list(APPEND ALL_TEST_TARGETS ${test_name})
endforeach()

add_dependencies(run_all_tests ${ALL_TEST_TARGETS})
