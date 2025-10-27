// ----------------------------------------------------------------------------
// File: FaceCentered.hpp
// Author: FVMGridMaker Team
// Version: 1.3
// Date: 2025-10-25
// Description: faces → centers, dF, dC (sem checks; STL com policy configurável).
// License: GNU GPL v3
// ----------------------------------------------------------------------------
#pragma once

// ----------------------------------------------------------------------------
// includes C++ 
// ----------------------------------------------------------------------------
#include <algorithm>
#include <cstddef>    // std::ptrdiff_t
#include <execution>
#include <span>

// ----------------------------------------------------------------------------
// includes FVMGridMaker  
// ----------------------------------------------------------------------------
#include <FVMGridMaker/Core/ExecPolicy.hpp>
#include <FVMGridMaker/Core/namespace.h>
#include <FVMGridMaker/Core/type.h>

FVMGRIDMAKER_NAMESPACE_OPEN
GRID_NAMESPACE_OPEN
GRID1D_NAMESPACE_OPEN
PATTERNS_NAMESPACE_OPEN
CENTERING_NAMESPACE_OPEN

struct FaceCentered {
    using Real = FVMGridMaker::core::Real;
    using Size = FVMGridMaker::core::Index;

    void operator()(std::span<const Real> xf,
                    std::span<Real>       xc,
                    std::span<Real>       dF,
                    std::span<Real>       dC) const noexcept
    {
        const Size N  = static_cast<Size>(xc.size());
        const auto pN = static_cast<std::ptrdiff_t>(N);

        auto midpoint = [](Real a, Real b) noexcept { return (a + b) * Real(0.5); };
        auto diff_ba  = [](Real a, Real b) noexcept { return b - a; };
        auto diff_ab  = [](Real a, Real b) noexcept { return a - b; };

        // centers
        std::transform(FVMG_EXEC_POLICY,
                       xf.begin(), xf.begin() + pN,
                       xf.begin() + 1,
                       xc.begin(),
                       midpoint);

        // dF
        std::transform(FVMG_EXEC_POLICY,
                       xf.begin(), xf.begin() + pN,
                       xf.begin() + 1,
                       dF.begin(),
                       diff_ba);

        // dC bordas + miolo
        dC[0] = xc[0] - xf[0];
        std::transform(FVMG_EXEC_POLICY,
                       xc.begin() + 1, xc.begin() + pN,
                       xc.begin(),
                       dC.begin() + 1,
                       diff_ab);
        dC[N] = xf[N] - xc[N - 1];
    }
};

CENTERING_NAMESPACE_CLOSE
PATTERNS_NAMESPACE_CLOSE
GRID1D_NAMESPACE_CLOSE
GRID_NAMESPACE_CLOSE
FVMGRIDMAKER_NAMESPACE_CLOSE
