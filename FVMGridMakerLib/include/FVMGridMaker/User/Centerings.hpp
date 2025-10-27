// ----------------------------------------------------------------------------
// File: Centerings.hpp  (USER-SIDE EXTENSION POINT)
// Author: You / Your Team
// Version: 1.0
// Date: 2025-10-25
// Description: Registro de centerings customizados do usuário.
//              Inclua aqui os headers dos SEUS functors e liste-os na macro
//              FVMG_CENTERINGS_EXTRA(X). A biblioteca detecta este arquivo
//              via __has_include e integra automaticamente (sem switch).
// License: Your project license
// ----------------------------------------------------------------------------
#pragma once


// ----------------------------------------------------------------------------
// includes FVMGridMaker  
// ----------------------------------------------------------------------------
// (nenhum include obrigatório; a própria biblioteca inclui este arquivo
//  se ele existir, a partir de CenteringList.hpp)

// -----------------------------------------------------------------------------
// 1) INCLUA os headers dos seus functors de centralização (se houver):
// -----------------------------------------------------------------------------

// Exemplo (substitua pelos seus caminhos):
// #include <MyProject/Grids/MyCentered.hpp>

// -----------------------------------------------------------------------------
// 2) LISTE os nomes na macro FVMG_CENTERINGS_EXTRA(X):
//    Cada item deve ser exatamente o nome do functor (classe) que está no
//    namespace FVMGridMaker::grid::grid1d::patterns::centering
//    e que implementa AO MENOS um dos operadores:
//      void operator()(std::span<const Real> xf, std::span<Real> xc,
//                      std::span<Real> dF,  std::span<Real> dC) const noexcept;
//      void operator()(std::span<const Real> xc, std::span<Real> xf,
//                      std::span<Real> dF,  std::span<Real> dC) const noexcept;
// -----------------------------------------------------------------------------

#ifndef FVMG_CENTERINGS_EXTRA
#define FVMG_CENTERINGS_EXTRA(X) \
    /* X(MyCentered) */          \
    /* acrescente os seus aqui... */
#endif
