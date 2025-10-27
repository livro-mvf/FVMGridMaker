// ----------------------------------------------------------------------------
// File: ConceptsCentering.hpp
// Author: FVMGridMaker Team
// Version: 1.0
// Date: 2025-10-25
// Description: Concepts e detecções para functors de centralização 1D.
// License: GNU GPL v3
// ----------------------------------------------------------------------------
#pragma once

// ----------------------------------------------------------------------------
// includes C++ 
// ----------------------------------------------------------------------------
#include <concepts>
#include <span>
#include <type_traits>

// ----------------------------------------------------------------------------
// includes FVMGridMaker  
// ----------------------------------------------------------------------------
#include <FVMGridMaker/Core/namespace.h>
#include <FVMGridMaker/Core/type.h>

FVMGRIDMAKER_NAMESPACE_OPEN
GRID_NAMESPACE_OPEN
GRID1D_NAMESPACE_OPEN
PATTERNS_NAMESPACE_OPEN
CENTERING_NAMESPACE_OPEN

using Real  = FVMGridMaker::core::Real;
using Index = FVMGridMaker::core::Index;

// Assinaturas esperadas dos functors de centralização:
//
//  // a partir de faces:
//  void C::operator()(std::span<const Real> xf,
//                     std::span<Real>       xc,
//                     std::span<Real>       dF,
//                     std::span<Real>       dC) const noexcept;
//
//  // a partir de centros:
//  void C::operator()(std::span<const Real> xc,
//                     std::span<Real>       xf,
//                     std::span<Real>       dF,
//                     std::span<Real>       dC) const noexcept;

// Concept: possui operador a partir de FACES
template<class C>
concept CenteringFromFaces =
    requires(const C c,
             std::span<const Real> xf,
             std::span<Real>       xc,
             std::span<Real>       dF,
             std::span<Real>       dC)
{
    { c(xf, xc, dF, dC) } -> std::same_as<void>;
};

// Concept: possui operador a partir de CENTROS
template<class C>
concept CenteringFromCenters =
    requires(const C c,
             std::span<const Real> xc,
             std::span<Real>       xf,
             std::span<Real>       dF,
             std::span<Real>       dC)
{
    { c(xc, xf, dF, dC) } -> std::same_as<void>;
};

// Variáveis auxiliares (usadas pela CenteringLut.hpp)
template<class C>
inline constexpr bool kHasFromFaces   = CenteringFromFaces<C>;

template<class C>
inline constexpr bool kHasFromCenters = CenteringFromCenters<C>;

CENTERING_NAMESPACE_CLOSE
PATTERNS_NAMESPACE_CLOSE
GRID1D_NAMESPACE_CLOSE
GRID_NAMESPACE_CLOSE
FVMGRIDMAKER_NAMESPACE_CLOSE
