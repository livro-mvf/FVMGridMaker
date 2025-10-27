// ----------------------------------------------------------------------------
// File: DistOptions.hpp
// Author: FVMGridMaker Team
// Version: 2.0
// Date: 2025-10-26
// Description: Contêiner leve para transportar a configuração de distribuição
//              (DistributionTag + payload opcional via std::any) do usuário
//              para o Grid1DBuilder.
// License: GNU GPL v3
// ----------------------------------------------------------------------------

/**
 * @file DistOptions.hpp
 * @brief Declaração de @ref DistOptions para configurar distribuições 1D.
 */

#ifndef FVMGRIDMAKER_GRID_GRID1D_BUILDERS_DISTOPTIONS_HPP
#define FVMGRIDMAKER_GRID_GRID1D_BUILDERS_DISTOPTIONS_HPP

#include <FVMGridMaker/Core/namespace.h>
#include <FVMGridMaker/Core/type.h>
#include <FVMGridMaker/Grid/Common/Tags1D.hpp> // DistributionTag

#include <any>

FVMGRIDMAKER_NAMESPACE_OPEN
GRID_NAMESPACE_OPEN
GRID1D_NAMESPACE_OPEN
BUILDERS_NAMESPACE_OPEN

/**
 * @brief Envelope de configuração de distribuição 1D.
 *
 * @details
 * - `tag` indica qual distribuição será usada pelo @ref Grid1DBuilder.
 * - `any` (opcional) carrega um payload específico da distribuição, típico:
 *   `Random1D::Options`. O consumidor (ex.: `configureDistribution`) faz
 *   `std::any_cast<T>` para o tipo certo e injeta no builder.
 */
class DistOptions {
public:
    /// Distribuição alvo.
    grid::DistributionTag tag;

    /// Payload opcional (ex.: Random1D::Options). Pode estar vazio.
    std::any any;

public:
    /// @name Fábricas convenientes para Random1D
    /// @{

    /**
     * @brief Cria configuração Random1D com semente fixa.
     * @param w_lo fator relativo mínimo para as larguras.
     * @param w_hi fator relativo máximo para as larguras.
     * @param seed semente fixa (determinística).
     */
    static DistOptions Random1D_Fixed(core::Real w_lo,
                                      core::Real w_hi,
                                      std::uint64_t seed);

    /**
     * @brief Cria configuração Random1D com semente baseada no relógio.
     * @param w_lo fator relativo mínimo para as larguras.
     * @param w_hi fator relativo máximo para as larguras.
     */
    static DistOptions Random1D_Clock(core::Real w_lo,
                                      core::Real w_hi);

    /// @}
};

BUILDERS_NAMESPACE_CLOSE
GRID1D_NAMESPACE_CLOSE
GRID_NAMESPACE_CLOSE
FVMGRIDMAKER_NAMESPACE_CLOSE

#endif // FVMGRIDMAKER_GRID_GRID1D_BUILDERS_DISTOPTIONS_HPP
