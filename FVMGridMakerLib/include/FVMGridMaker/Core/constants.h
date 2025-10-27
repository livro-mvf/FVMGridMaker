// ----------------------------------------------------------------------------
// File: constants.h
// Author: Joao Flavio Vieira de Vasconcellos + FVMGridMaker Team
// Version: 2.0
// Date: 2025-10-25
// Description: Constantes numéricas (tolerâncias e padrões) para o Core.
// License: GNU GPL v3
// ----------------------------------------------------------------------------
#pragma once

// ----------------------------------------------------------------------------
// includes C++ 
// ----------------------------------------------------------------------------
#include <limits>
#include <numbers>

// ----------------------------------------------------------------------------
// includes FVMGridMaker  
// ----------------------------------------------------------------------------
#include <FVMGridMaker/Core/namespace.h>
#include <FVMGridMaker/Core/type.h>

FVMGRIDMAKER_NAMESPACE_OPEN
CORE_NAMESPACE_OPEN

namespace constants {

using FVMGridMaker::core::Index;
using FVMGridMaker::core::Real;

inline constexpr Real  kZeroTol      = static_cast<Real>(1e-12);
inline constexpr Real  kLimit        = static_cast<Real>(1e-30);
inline constexpr Real  kEpsilon      = static_cast<Real>(1e-6);
inline constexpr int   kDefaultSize  = 80;
inline constexpr Real  kPi           = static_cast<Real>(std::numbers::pi_v<long double>);
inline constexpr Real  kCoordEps     = static_cast<Real>(1e-12);
inline constexpr Real  kDefaultDxMin = static_cast<Real>(0);
inline constexpr Index kInvalidIndex = static_cast<Index>(-1);

} // namespace constants

inline constexpr auto ZERO    = constants::kZeroTol;
inline constexpr auto LIMIT   = constants::kLimit;
inline constexpr auto EPSILON = constants::kEpsilon;
inline constexpr auto LSIZE   = constants::kDefaultSize;
inline constexpr auto PI      = constants::kPi;

static_assert(
    std::numeric_limits<FVMGridMaker::core::Real>::is_iec559 ||
    std::numeric_limits<FVMGridMaker::core::Real>::is_specialized,
    "FVMGridMaker/Core/constants.h: Real deve ser um tipo de ponto flutuante."
);

CORE_NAMESPACE_CLOSE
FVMGRIDMAKER_NAMESPACE_CLOSE
