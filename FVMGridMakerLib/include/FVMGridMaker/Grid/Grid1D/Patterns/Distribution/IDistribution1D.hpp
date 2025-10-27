// ----------------------------------------------------------------------------
/* File: IDistribution1D.hpp
 * Author: FVMGridMaker Team
 * Version: 1.0
 * Date: 2025-10-26
 * Description: Interface (Strategy) para gerar sequências 1D de faces/centros.
 *              Permite plugar novas distribuições sem alterar o núcleo.
 * License: GNU GPL v3
 */
// ----------------------------------------------------------------------------
#pragma once

// ----------------------------------------------------------------------------
// includes C++
// ----------------------------------------------------------------------------
#include <cstddef>
#include <string>
#include <vector>

// ----------------------------------------------------------------------------
// includes FVMGridMaker
// ----------------------------------------------------------------------------
#include <FVMGridMaker/Core/type.h>

namespace FVMGridMaker::grid::grid1d::patterns::distribution {

/**
 * @brief Interface para distribuições 1D (Strategy).
 *
 * Contrato:
 *  - faces(N,A,B)  → retorna N+1 pontos estritamente crescentes em [A,B]
 *  - centers(N,A,B)→ retorna N   pontos estritamente crescentes em (A,B)
 *  - N > 0 e B > A devem ser respeitados pelo chamador; a implementação pode
 *    validar e lançar exceções (GridErr) se desejar.
 */
struct IDistribution1D {
    using Real  = FVMGridMaker::core::Real;
    using Index = FVMGridMaker::core::Index;

    virtual ~IDistribution1D() = default;

    /// Nome amigável para logs/diagnósticos (ex.: "Uniform1D", "Random1D").
    virtual std::string name() const = 0;

    /// Gera faces (N+1) estritamente crescentes em [A,B].
    virtual std::vector<Real> faces(Index N, Real A, Real B) const = 0;

    /// Gera centros (N) estritamente crescentes em (A,B).
    virtual std::vector<Real> centers(Index N, Real A, Real B) const = 0;
};

} // namespace FVMGridMaker::grid::grid1d::patterns::distribution
