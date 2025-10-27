// ----------------------------------------------------------------------------
// File: ConfigureDistribution.hpp
// Author: FVMGridMaker Team
// Version: 2.0
// Date: 2025-10-26
// Description: Função utilitária para aplicar DistOptions em Grid1DBuilder.
//              Define a DistributionTag e injeta (quando presente) o payload
//              de opções no builder (com std::any_cast para o tipo correto).
// License: GNU GPL v3
// ----------------------------------------------------------------------------

/**
 * @file ConfigureDistribution.hpp
 * @brief Utilitário para configurar distribuição no Grid1DBuilder a partir de DistOptions.
 */

#ifndef FVMGRIDMAKER_GRID_GRID1D_BUILDERS_CONFIGURE_DISTRIBUTION_HPP
#define FVMGRIDMAKER_GRID_GRID1D_BUILDERS_CONFIGURE_DISTRIBUTION_HPP

#include <FVMGridMaker/Core/namespace.h>
#include <FVMGridMaker/Grid/Grid1D/Builders/Grid1DBuilder.hpp>
#include <FVMGridMaker/Grid/Grid1D/Builders/DistOptions.hpp>

// Para o any_cast do payload suportado pelo builder atual:
#include <FVMGridMaker/Grid/Grid1D/Patterns/Distribution/Random1D.hpp>

#include <any>

FVMGRIDMAKER_NAMESPACE_OPEN
GRID_NAMESPACE_OPEN
GRID1D_NAMESPACE_OPEN
BUILDERS_NAMESPACE_OPEN

/**
 * @brief Aplica @ref DistOptions ao @ref Grid1DBuilder.
 *
 * @details
 * - Sempre define o `DistributionTag`.
 * - Se houver payload em `cfg.any`, faz `std::any_cast` para o tipo suportado
 *   pelo builder **para aquele tag** e chama `setOption(...)`.
 * - Implementado de forma mínima para o estado atual do builder (Random1D).
 *   Outras distribuições podem ser adicionadas depois (novo case do switch).
 */
inline void configureDistribution(Grid1DBuilder& builder, const DistOptions& cfg) {
    builder.setDistribution(cfg.tag);

    if (!cfg.any.has_value()) return;

    switch (cfg.tag) {
        case grid::DistributionTag::Random1D: {
            using patterns::distribution::Random1D;
            if (const auto* opt = std::any_cast<Random1D::Options>(&cfg.any)) {
                builder.setOption(*opt); // assinatura existente do builder
            }
            break;
        }

        default:
            // Sem payload conhecido para outros tags no estado atual do builder.
            break;
    }
}

BUILDERS_NAMESPACE_CLOSE
GRID1D_NAMESPACE_CLOSE
GRID_NAMESPACE_CLOSE
FVMGRIDMAKER_NAMESPACE_CLOSE

#endif // FVMGRIDMAKER_GRID_GRID1D_BUILDERS_CONFIGURE_DISTRIBUTION_HPP
