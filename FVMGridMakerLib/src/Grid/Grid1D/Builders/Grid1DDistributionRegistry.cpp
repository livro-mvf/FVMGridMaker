// ----------------------------------------------------------------------------
// File: Grid1DDistributionRegistry.cpp
// Author: FVMGridMaker Team
// Version: 2.0
// Date: 2025-10-26
// Description: Implementação do registro de geradores de distribuições 1D para
//              o Grid1D. Não realiza auto-registro de padrões — cada padrão
//              (Uniform1D, Random1D, etc.) deve registrar-se em seu próprio TU.
// License: GNU GPL v3
// ----------------------------------------------------------------------------

/**
 * @file Grid1DDistributionRegistry.cpp
 * @brief Implementação do registro extensível para distribuições 1D.
 */

#include <FVMGridMaker/Core/namespace.h>
#include <FVMGridMaker/Grid/Grid1D/Builders/Grid1DDistributionRegistry.hpp>

#include <utility>      // std::move
#include <string>
#include <string_view>
#include <optional>

FVMGRIDMAKER_NAMESPACE_OPEN
GRID_NAMESPACE_OPEN
GRID1D_NAMESPACE_OPEN
BUILDERS_NAMESPACE_OPEN

// -----------------------------------------------------------------------------
// Singleton
// -----------------------------------------------------------------------------
/**
 * @brief Acessa a instância singleton do registro.
 * @return Referência ao registro global.
 *
 * @details Não há auto-registro de distribuições aqui. Padrões devem se
 *          registrar em seus próprios TUs chamando os métodos `registerDistribution`.
 */
Grid1DDistributionRegistry& Grid1DDistributionRegistry::instance() {
    static Grid1DDistributionRegistry reg;
    return reg;
}

// -----------------------------------------------------------------------------
// Registro
// -----------------------------------------------------------------------------
/**
 * @brief Registra (ou sobrescreve) uma distribuição por nome.
 * @param name Nome único da distribuição.
 * @param entry Estrutura com os geradores *faces_fn* e *centers_fn*.
 */
void Grid1DDistributionRegistry::registerDistribution(std::string name, Entry entry) {
    names_[std::move(name)] = std::move(entry);
}

/**
 * @brief Registra (ou sobrescreve) uma distribuição por nome e `DistributionTag`.
 * @param name Nome único da distribuição.
 * @param entry Estrutura com os geradores *faces_fn* e *centers_fn*.
 * @param tag Valor do enum `DistributionTag` associado à distribuição.
 *
 * @details Além de salvar por nome, mantém o mapeamento `(int)tag -> name`.
 */
void Grid1DDistributionRegistry::registerDistribution(
    std::string name,
    Entry entry,
    FVMGridMaker::grid::DistributionTag tag)
{
    // Associa o tag ao nome (guarda a cópia do nome antes de movê-lo)
    tag_to_name_[static_cast<int>(tag)] = name;
    // Registra a entry por nome
    names_[std::move(name)] = std::move(entry);
}

// -----------------------------------------------------------------------------
// Consulta
// -----------------------------------------------------------------------------
/**
 * @brief Procura uma distribuição pelo nome.
 * @param name Nome da distribuição.
 * @return `Entry` encontrado, ou `std::nullopt` caso não exista.
 */
std::optional<Grid1DDistributionRegistry::Entry>
Grid1DDistributionRegistry::find(std::string_view name) const {
    auto it = names_.find(std::string{name});
    if (it == names_.end()) {
        return std::nullopt;
    }
    return it->second; // cópia do Entry
}

/**
 * @brief Obtém o nome associado a um `DistributionTag`.
 * @param tag Enum `DistributionTag`.
 * @return Nome da distribuição ou `std::nullopt` caso não exista mapeamento.
 */
std::optional<std::string>
Grid1DDistributionRegistry::nameForTag(FVMGridMaker::grid::DistributionTag tag) const {
    auto it = tag_to_name_.find(static_cast<int>(tag));
    if (it == tag_to_name_.end()) {
        return std::nullopt;
    }
    return it->second; // cópia do nome
}

/**
 * @brief Procura uma distribuição por `DistributionTag`.
 * @param tag Enum `DistributionTag`.
 * @return `Entry` correspondente, ou `std::nullopt` se não existir.
 */
std::optional<Grid1DDistributionRegistry::Entry>
Grid1DDistributionRegistry::findByTag(FVMGridMaker::grid::DistributionTag tag) const {
    auto name_opt = nameForTag(tag);
    if (!name_opt) {
        return std::nullopt;
    }
    return find(*name_opt);
}

BUILDERS_NAMESPACE_CLOSE
GRID1D_NAMESPACE_CLOSE
GRID_NAMESPACE_CLOSE
FVMGRIDMAKER_NAMESPACE_CLOSE
