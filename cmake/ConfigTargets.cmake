# ------------------------------------------------------------
# Main library target
# ------------------------------------------------------------

# Find sources
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
     "${FVMG_SRC_DIR}/*.cpp")

# Create library
add_library(FVMGridMaker SHARED ${SOURCES})

# Adiciona diretórios de include (Modo Moderno)
target_include_directories(FVMGridMaker
    PUBLIC
        ${FVMG_INCLUDE_DIR}
)

# Link dependencies (RNF06: Nenhuma)
target_link_libraries(FVMGridMaker
    PUBLIC
        # Vazio (CGAL Removido)
)

# Set optimizations
set_target_optimizations(FVMGridMaker)

# Output directories (usa variável adaptada)
set_target_properties(FVMGridMaker PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${FVMG_OUTPUT_BIN_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${FVMG_OUTPUT_BIN_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${FVMG_OUTPUT_BIN_DIR}
)

# Install rules
install(TARGETS FVMGridMaker
    EXPORT FVMGridMakerTargets
    LIBRARY DESTINATION ${FVMG_OUTPUT_BIN_DIR}
    ARCHIVE DESTINATION ${FVMG_OUTPUT_BIN_DIR}
    RUNTIME DESTINATION ${FVMG_OUTPUT_BIN_DIR})

# Instala o header namespaced (FVMGridMakerLib/include/FVMGridMaker)
install(DIRECTORY ${FVMG_INCLUDE_DIR}/FVMGridMaker
    DESTINATION include
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" PATTERN "*.tpp")

# Instala arquivos .tpp (template implementations) do src/
install(DIRECTORY ${FVMG_SRC_DIR}
    DESTINATION include
    FILES_MATCHING PATTERN "*.tpp")