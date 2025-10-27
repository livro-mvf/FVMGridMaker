// ----------------------------------------------------------------------------
// File: Grid1DValidation.hpp
// Author: FVMGridMaker Team
// Version: 1.0
// Date: 2025-10-26
// Description: Valida invariantes de malhas 1D (faces/centros crescentes e
//              larguras positivas), com opção serial/paralela (PSTL).
// License: GNU GPL v3
// ----------------------------------------------------------------------------
#pragma once

// ----------------------------------------------------------------------------
// includes FVMGridMaker
// ----------------------------------------------------------------------------
#include <FVMGridMaker/Core/type.h>
#include <FVMGridMaker/ErrorHandling/ErrorHandling.h>
#include <FVMGridMaker/Grid/Grid1D/Errors/GridErrors.h>   // define GridErr
#include <FVMGridMaker/Core/namespace.h>                  // macros de namespace

// ----------------------------------------------------------------------------
// includes C++
// ----------------------------------------------------------------------------
#include <algorithm>
#include <iterator>
#include <span>
#if defined(FVMG_HAVE_PSTL_EXEC)
  #include <execution>
#endif

// ----------------------------------------------------------------------------
// Configuração por macro
//   FVMG_GRID_RUNTIME_CHECKS : liga/desliga validações (1 padrão)
//   FVMG_GRID_PAR_MIN_N      : limiar para usar PSTL (se disponível)
// ----------------------------------------------------------------------------
#ifndef FVMG_GRID_RUNTIME_CHECKS
  #define FVMG_GRID_RUNTIME_CHECKS 1
#endif
#ifndef FVMG_GRID_PAR_MIN_N
  #define FVMG_GRID_PAR_MIN_N 50000
#endif

FVMG_GRID1D_UTILS_OPEN
namespace validation {

// faces estritamente crescentes
inline void strictly_increasing_faces(std::span<const core::Real> xf) {
#if FVMG_GRID_RUNTIME_CHECKS
    using Real = core::Real;
    if (xf.size() < 2) return;

#if defined(FVMG_HAVE_PSTL_EXEC)
    auto exec = (xf.size() >= FVMG_GRID_PAR_MIN_N) ? std::execution::par : std::execution::seq;
    bool ok = std::transform_reduce(
        exec,
        xf.begin(), xf.end() - 1,
        xf.begin() + 1,
        true,
        std::logical_and<bool>{},
        std::less<Real>{}
    );
    if (!ok) {
        auto it = std::adjacent_find(xf.begin(), xf.end(),
                                     [](Real a, Real b){ return !(b > a); });
        const auto i = static_cast<std::ptrdiff_t>(std::distance(xf.begin(), it));
        FVMG_ERROR(error::GridErr::NonIncreasingFaces, {{"i", std::to_string(i)}});
    }
#else
    auto it = std::adjacent_find(xf.begin(), xf.end(),
                                 [](Real a, Real b){ return !(b > a); });
    if (it != xf.end()) {
        const auto i = static_cast<std::ptrdiff_t>(std::distance(xf.begin(), it));
        FVMG_ERROR(error::GridErr::NonIncreasingFaces, {{"i", std::to_string(i)}});
    }
#endif
#else
    (void)xf;
#endif
}

// centros estritamente crescentes
inline void strictly_increasing_centers(std::span<const core::Real> xc) {
#if FVMG_GRID_RUNTIME_CHECKS
    using Real = core::Real;
    if (xc.size() < 2) return;

#if defined(FVMG_HAVE_PSTL_EXEC)
    auto exec = (xc.size() >= FVMG_GRID_PAR_MIN_N) ? std::execution::par : std::execution::seq;
    bool ok = std::transform_reduce(
        exec,
        xc.begin(), xc.end() - 1,
        xc.begin() + 1,
        true,
        std::logical_and<bool>{},
        std::less<Real>{}
    );
    if (!ok) {
        auto it = std::adjacent_find(xc.begin(), xc.end(),
                                     [](Real a, Real b){ return !(b > a); });
        const auto i = static_cast<std::ptrdiff_t>(std::distance(xc.begin(), it));
        FVMG_ERROR(error::GridErr::NonIncreasingCenters, {{"i", std::to_string(i)}});
    }
#else
    auto it = std::adjacent_find(xc.begin(), xc.end(),
                                 [](Real a, Real b){ return !(b > a); });
    if (it != xc.end()) {
        const auto i = static_cast<std::ptrdiff_t>(std::distance(xc.begin(), it));
        FVMG_ERROR(error::GridErr::NonIncreasingCenters, {{"i", std::to_string(i)}});
    }
#endif
#else
    (void)xc;
#endif
}

// larguras entre faces estritamente positivas
inline void positive_face_lengths(std::span<const core::Real> xf) {
#if FVMG_GRID_RUNTIME_CHECKS
    using Real = core::Real;
    if (xf.size() < 2) return;

#if defined(FVMG_HAVE_PSTL_EXEC)
    auto exec = (xf.size() >= FVMG_GRID_PAR_MIN_N) ? std::execution::par : std::execution::seq;
    bool ok = std::transform_reduce(
        exec,
        xf.begin(), xf.end() - 1,
        xf.begin() + 1,
        true,
        std::logical_and<bool>{},
        [](Real a, Real b){ return b - a > Real(0); }
    );
    if (!ok) {
        auto it = std::adjacent_find(xf.begin(), xf.end(),
                                     [](Real a, Real b){ return !(b - a > Real(0)); });
        const auto i = static_cast<std::ptrdiff_t>(std::distance(xf.begin(), it));
        FVMG_ERROR(error::GridErr::NonPositiveDxFace, {{"i", std::to_string(i)}});
    }
#else
    auto it = std::adjacent_find(xf.begin(), xf.end(),
                                 [](Real a, Real b){ return !(b - a > Real(0)); });
    if (it != xf.end()) {
        const auto i = static_cast<std::ptrdiff_t>(std::distance(xf.begin(), it));
        FVMG_ERROR(error::GridErr::NonPositiveDxFace, {{"i", std::to_string(i)}});
    }
#endif
#else
    (void)xf;
#endif
}

} // namespace validation
FVMG_GRID1D_UTILS_CLOSE
