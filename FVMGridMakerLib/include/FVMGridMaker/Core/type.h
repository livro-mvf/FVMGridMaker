// ----------------------------------------------------------------------------
/* File: type.h
 * Author: FVMGridMaker Team
 * Version: 1.1
 * Date: 2025-10-25
 * Description: Tipos básicos do projeto (Real, Index) com seleção por macro.
 * License: GNU GPL v3
 */
// ----------------------------------------------------------------------------
#pragma once

// ----------------------------------------------------------------------------
// includes C++ 
// ----------------------------------------------------------------------------
#include <cstddef>    // std::size_t
#include <cstdint>    // inteiros fixos (se precisar em outros headers)

// ----------------------------------------------------------------------------
// includes FVMGridMaker  
// ----------------------------------------------------------------------------
#include <FVMGridMaker/Core/namespace.h>

FVMGRIDMAKER_NAMESPACE_OPEN
CORE_NAMESPACE_OPEN

// ----------------------------------------------------------------------------
// Seleção do tipo de ponto flutuante (Real)
//  - Padrão: double
//  - Para alterar, defina UMA das macros antes de incluir este header:
//      * FVMG_REAL_IS_FLOAT
//      * FVMG_REAL_IS_LONG_DOUBLE
// ----------------------------------------------------------------------------
#if defined(FVMG_REAL_IS_FLOAT)
    using Real = float;
#elif defined(FVMG_REAL_IS_LONG_DOUBLE)
    using Real = long double;
#else
    using Real = double;
#endif

// ----------------------------------------------------------------------------
// Indexador inteiro para percorrer malhas/vetores (DOD-friendly).
//  - Padrão: std::size_t (sem sinal, ilimitado na arquitetura).
//  - Mantenha consistente em todo o código para evitar casts desnecessários.
// ----------------------------------------------------------------------------
using Index = std::size_t;

CORE_NAMESPACE_CLOSE
FVMGRIDMAKER_NAMESPACE_CLOSE
