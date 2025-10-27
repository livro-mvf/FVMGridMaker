// ----------------------------------------------------------------------------
// File: Grid1DDistributionRegistry.hpp
// Author: FVMGridMaker Team
// Version: 2.0
// Date: 2025-10-26
// Description: Interface do registro extensível de geradores de distribuições
//              1D (faces/centros) para o Grid1D. Não realiza auto-registro.
//              Cada padrão (Uniform1D, Random1D, etc.) deve se registrar em
//              seu próprio TU de implementação.
// License: GNU GPL v3
// ----------------------------------------------------------------------------

/**
 * @file Grid1DDistributionRegistry.hpp
 * @brief Registro extensível para geradores de distribuições 1D.
 *
 * Este registro mapeia um **nome de distribuição** para um par de *functors*
 * (geradores de faces e de centros). Opcionalmente mantém um mapeamento entre
 * `DistributionTag` e o nome correspondente, permitindo consulta por *tag*.
 *
 * Não há *auto-registration* aqui: cada implementação de padrão deve, no seu
 * `.cpp`, chamar `Grid1DDistributionRegistry::instance().registerDistribution(...)`.
 */

#pragma once

#include <FVMGridMaker/Core/namespace.h>
#include <FVMGridMaker/Core/type.h>               // FVMGridMaker::core::Real, Index
#include <FVMGridMaker/Grid/Common/Tags1D.hpp>    // FVMGridMaker::grid::DistributionTag

#include <any>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

FVMGRIDMAKER_NAMESPACE_OPEN
GRID_NAMESPACE_OPEN
GRID1D_NAMESPACE_OPEN
BUILDERS_NAMESPACE_OPEN

/**
 * @class Grid1DDistributionRegistry
 * @brief Registro singleton de geradores de distribuições 1D.
 *
 * Cada entrada contém dois *functors*:
 * - `faces_fn(n, A, B, any_opt)`  → retorna coordenadas das **faces**
 * - `centers_fn(n, A, B, any_opt)`→ retorna coordenadas dos **centros**
 *
 * Onde:
 * - `n`   : número de células (ou pontos, conforme a convenção do padrão)
 * - `A,B` : limites do domínio 1D (A < B)
 * - `any_opt` : ponteiro opcional para configurações em `std::any` (pode ser `nullptr`)
 */
class Grid1DDistributionRegistry {
public:
    /// Assinatura de um gerador de coordenadas (faces ou centros).
    using GenFn = std::function<
        std::vector<FVMGridMaker::core::Real>(
            FVMGridMaker::core::Index n,
            FVMGridMaker::core::Real  A,
            FVMGridMaker::core::Real  B,
            const std::any*           any_opt)>;

    /// Par de geradores (faces, centros) para uma distribuição.
    struct Entry {
        GenFn faces_fn;    ///< Gerador de faces.
        GenFn centers_fn;  ///< Gerador de centros.
    };

    /**
     * @brief Acessa a instância singleton do registro.
     * @return Referência ao registro global.
     */
    static Grid1DDistributionRegistry& instance();

    /**
     * @brief Registra (ou sobrescreve) uma distribuição por **nome**.
     * @param name  Nome único da distribuição.
     * @param entry Conjunto de *functors* (faces/centros).
     */
    void registerDistribution(std::string name, Entry entry);

    /**
     * @brief Registra (ou sobrescreve) por **nome** e associa um `DistributionTag`.
     * @param name  Nome único da distribuição.
     * @param entry Conjunto de *functors* (faces/centros).
     * @param tag   Enum `DistributionTag` associado.
     *
     * @details Além de salvar por nome, mantém o mapeamento `(int)tag -> name`.
     */
    void registerDistribution(std::string name,
                              Entry entry,
                              FVMGridMaker::grid::DistributionTag tag);

    /**
     * @brief Procura uma distribuição pelo **nome**.
     * @param name Nome da distribuição.
     * @return `Entry` correspondente, ou `std::nullopt` se não existir.
     */
    [[nodiscard]] std::optional<Entry> find(std::string_view name) const;

    /**
     * @brief Obtém o **nome** associado a um `DistributionTag`.
     * @param tag Enum `DistributionTag`.
     * @return Nome mapeado, ou `std::nullopt` se não existir.
     */
    [[nodiscard]] std::optional<std::string>
    nameForTag(FVMGridMaker::grid::DistributionTag tag) const;

    /**
     * @brief Procura uma distribuição por `DistributionTag`.
     * @param tag Enum `DistributionTag`.
     * @return `Entry` correspondente, ou `std::nullopt` se não existir.
     */
    [[nodiscard]] std::optional<Entry>
    findByTag(FVMGridMaker::grid::DistributionTag tag) const;

private:
    Grid1DDistributionRegistry() = default;

    // Mapa nome -> Entry
    std::unordered_map<std::string, Entry> names_{};

    // Mapa (int)tag -> nome (usa int para evitar necessidade de hash de enum)
    std::unordered_map<int, std::string> tag_to_name_{};
};

BUILDERS_NAMESPACE_CLOSE
GRID1D_NAMESPACE_CLOSE
GRID_NAMESPACE_CLOSE
FVMGRIDMAKER_NAMESPACE_CLOSE
