// ----------------------------------------------------------------------------
// File: ConceptsDistribution.hpp
// Author: FVMGridMaker Team
// Version: 1.0
// Date: 2025-10-25
// Description: Concept para validar functors de distribuição 1D.
// License: GNU GPL v3
// ----------------------------------------------------------------------------
#pragma once

// ----------------------------------------------------------------------------
// includes C++ 
// ----------------------------------------------------------------------------
#include <concepts>
#include <cstdint>
#include <span>

// ----------------------------------------------------------------------------
// includes FVMGridMaker  
// ----------------------------------------------------------------------------
#include <FVMGridMaker/Core/namespace.h>
#include <FVMGridMaker/Core/type.h>

FVMGRIDMAKER_NAMESPACE_OPEN
GRID_NAMESPACE_OPEN
GRID1D_NAMESPACE_OPEN
PATTERNS_NAMESPACE_OPEN
DISTRIBUTION_NAMESPACE_OPEN

using Real  = FVMGridMaker::core::Real;
using Index = FVMGridMaker::core::Index;

/**
 * @brief Um functor de distribuição 1D deve prover:
 *  void makeFaces(Index, Real, Real, std::span<Real>, std::uint64_t, Real);
 *  void makeCenters(Index, Real, Real, std::span<Real>, std::uint64_t, Real);
 * Assinaturas exatas; não há verificação de índice no hot-path.
 */
template<class D>
concept Distribution1D =
    requires(D d, Index N, Real A, Real B,
             std::span<Real> v, std::uint64_t seed, Real dx)
{
    { d.makeFaces(N, A, B, v, seed, dx) }   -> std::same_as<void>;
    { d.makeCenters(N, A, B, v, seed, dx) } -> std::same_as<void>;
};

DISTRIBUTION_NAMESPACE_CLOSE
PATTERNS_NAMESPACE_CLOSE
GRID1D_NAMESPACE_CLOSE
GRID_NAMESPACE_CLOSE
FVMGRIDMAKER_NAMESPACE_CLOSE
