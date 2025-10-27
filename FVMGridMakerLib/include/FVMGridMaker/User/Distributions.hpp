// ----------------------------------------------------------------------------
// File: Distributions.hpp  (USER-SIDE EXTENSION POINT)
// Author: You / Your Team
// Version: 1.0
// Date: 2025-10-25
// Description: Registro de distribuições customizadas do usuário.
//              Inclua aqui os headers dos SEUS functors e liste-os na macro
//              FVMG_DISTRIBUTIONS_EXTRA(X). A biblioteca detecta este arquivo
//              via __has_include e integra automaticamente (sem switch).
// License: Your project license
// ----------------------------------------------------------------------------
#pragma once

// ----------------------------------------------------------------------------
// includes C++ 
// ----------------------------------------------------------------------------
// (normalmente desnecessário aqui)

// ----------------------------------------------------------------------------
// includes FVMGridMaker  
// ----------------------------------------------------------------------------
// (nenhum include obrigatório; a própria biblioteca inclui este arquivo
//  se ele existir, a partir de DistributionList.hpp)

// -----------------------------------------------------------------------------
// 1) INCLUA os headers dos seus functors de distribuição (se houver):
// -----------------------------------------------------------------------------

// Exemplo (substitua pelos seus caminhos):
// #include <MyProject/Grids/Geometric1D.hpp>
// #include <MyProject/Grids/Clustered1D.hpp>

// -----------------------------------------------------------------------------
// 2) LISTE os nomes na macro FVMG_DISTRIBUTIONS_EXTRA(X):
//    Cada item deve ser exatamente o nome do functor (classe) que está no
//    namespace FVMGridMaker::grid::grid1d::patterns::distribution
//    e que implementa:
//      void makeFaces(Index, Real, Real, std::span<Real>, std::uint64_t, Real);
//      void makeCenters(Index, Real, Real, std::span<Real>, std::uint64_t, Real);
// -----------------------------------------------------------------------------

#ifndef FVMG_DISTRIBUTIONS_EXTRA
#define FVMG_DISTRIBUTIONS_EXTRA(X) \
    /* X(Geometric1D) */            \
    /* X(Clustered1D) */            \
    /* acrescente os seus aqui... */
#endif
