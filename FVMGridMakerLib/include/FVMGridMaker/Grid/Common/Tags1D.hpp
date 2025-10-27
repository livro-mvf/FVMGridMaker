// ----------------------------------------------------------------------------
// File: Tags1D.hpp
// Author: FVMGridMaker Team
// Version: 1.4
// Date: 2025-10-26
// Description: Tags (1D) para seleção de centralização e distribuição,
//              geradas via X-Macro (suporta extensão do usuário).
// License: GNU GPL v3
// ----------------------------------------------------------------------------
#pragma once

/**
 * @file   Tags1D.hpp
 * @brief  Enumerações 1D para Centering e Distribution com suporte a extensão
 *         via X-Macro, sem depender de headers de registro.
 *
 * @details
 * Define:
 *   - `CenteringTag`     (FaceCentered, CellCentered)
 *   - `DistributionTag`  (Uniform1D, Random1D)
 *
 * As listas são geradas por X-Macros. O usuário pode definir, antes de incluir:
 *   - FVMG_CENTERINGS(X)
 *   - FVMG_DISTRIBUTIONS(X)
 *
 * Caso nada seja definido, usamos os defaults abaixo.
 */

// ----------------------------------------------------------------------------
// includes C++ 
// ----------------------------------------------------------------------------
#include <string_view>

// ----------------------------------------------------------------------------
// includes FVMGridMaker  
// ----------------------------------------------------------------------------
#include <FVMGridMaker/Core/namespace.h>

FVMGRIDMAKER_NAMESPACE_OPEN
GRID_NAMESPACE_OPEN  
// ----------------------------------------------------------------------------
// Defaults das X-Macros (se o usuário não fornecer listas próprias)
// ----------------------------------------------------------------------------
#ifndef FVMG_CENTERINGS
  #define FVMG_CENTERINGS(X) \
      X(FaceCentered)        \
      X(CellCentered)
#endif

#ifndef FVMG_DISTRIBUTIONS
  #define FVMG_DISTRIBUTIONS(X) \
      X(Uniform1D)              \
      X(Random1D)
#endif

// -----------------------------------------------------------------------------
// CenteringTag (X-Macro; extensível)
// -----------------------------------------------------------------------------
enum class CenteringTag : std::uint8_t {
  #define FVMG_ENUM_ELEM(name) name,
    FVMG_CENTERINGS(FVMG_ENUM_ELEM)
  #undef  FVMG_ENUM_ELEM
    Count
};

#define FVMG_NAME_ELEM(name) #name,
inline constexpr const char* kCenteringNames[] = {
    FVMG_CENTERINGS(FVMG_NAME_ELEM)
};
#undef FVMG_NAME_ELEM

inline constexpr std::string_view to_string(CenteringTag t) noexcept {
    const auto idx = static_cast<std::uint8_t>(t);
    return (idx < static_cast<std::uint8_t>(CenteringTag::Count))
         ? std::string_view{kCenteringNames[idx]}
         : std::string_view{"Unknown"};
}

static_assert(
    static_cast<std::size_t>(CenteringTag::Count)
    == (sizeof(kCenteringNames) / sizeof(kCenteringNames[0])),
    "kCenteringNames size must match CenteringTag::Count"
);

// -----------------------------------------------------------------------------
// DistributionTag (X-Macro; extensível)
// -----------------------------------------------------------------------------
enum class DistributionTag : std::uint8_t {
  #define FVMG_ENUM_ELEM(name) name,
    FVMG_DISTRIBUTIONS(FVMG_ENUM_ELEM)
  #undef  FVMG_ENUM_ELEM
    Count
};

#define FVMG_NAME_ELEM(name) #name,
inline constexpr const char* kDistributionNames[] = {
    FVMG_DISTRIBUTIONS(FVMG_NAME_ELEM)
};
#undef FVMG_NAME_ELEM

inline constexpr std::string_view to_string(DistributionTag t) noexcept {
    const auto idx = static_cast<std::uint8_t>(t);
    return (idx < static_cast<std::uint8_t>(DistributionTag::Count))
         ? std::string_view{kDistributionNames[idx]}
         : std::string_view{"Unknown"};
}

static_assert(
    static_cast<std::size_t>(DistributionTag::Count)
    == (sizeof(kDistributionNames) / sizeof(kDistributionNames[0])),
    "kDistributionNames size must match DistributionTag::Count"
);

GRID_NAMESPACE_CLOSE
FVMGRIDMAKER_NAMESPACE_CLOSE
