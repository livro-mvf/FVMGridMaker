// ----------------------------------------------------------------------------
// File: CellCentered.hpp
// Author: FVMGridMaker Team
// Version: 1.1
// Date: 2025-10-25
// Description: Centralização a partir de centros (calcula faces/deltas).
// License: GNU GPL v3
// ----------------------------------------------------------------------------
#pragma once

// ----------------------------------------------------------------------------
// includes C++ 
// ----------------------------------------------------------------------------
#include <algorithm>
#include <cassert>
#include <span>

// ----------------------------------------------------------------------------
// includes FVMGridMaker  
// ----------------------------------------------------------------------------
#include <FVMGridMaker/Core/type.h>

FVMGRIDMAKER_NAMESPACE_OPEN
GRID_NAMESPACE_OPEN
GRID1D_NAMESPACE_OPEN
PATTERNS_NAMESPACE_OPEN
CENTERING_NAMESPACE_OPEN

/**
 * A partir de xc (N), calcula:
 *  - xf (N+1): faces
 *  - dF (N):   larguras de célula = xf[i+1] - xf[i]
 *  - dC (N+1): gaps centrados = { xc0-xf0, xc1-xc0, ..., xcN-1-xcN-2, xfN-xcN-1 }
 */
struct CellCentered {
    using Real = FVMGridMaker::core::Real;
    using Size = std::size_t;

    void operator()(std::span<const Real> xc,
                    std::span<Real>       xf,
                    std::span<Real>       dF,
                    std::span<Real>       dC) const
    {
        const Size N = xc.size();
        assert(xf.size() == N + Size(1) && "xf deve ter N+1 elementos");
        assert(dF.size() == N           && "dF deve ter N elementos");
        assert(dC.size() == N + Size(1) && "dC deve ter N+1 elementos");

        if (N == 0) return;

        if (N == 1) {
            xf[0] = xc[0];
            xf[1] = xc[0];
        } else {
            // faces internas i = 1..N-1: média de centros adjacentes
            std::transform(
                xc.begin(), xc.begin() + static_cast<std::ptrdiff_t>(N - 1),
                xc.begin() + 1,
                xf.begin() + 1,
                [](Real a, Real b) { return Real(0.5) * (a + b); }
            );

            // bordas por meia-largura
            const Real dxL = xc[1]     - xc[0];
            const Real dxR = xc[N - 1] - xc[N - 2];
            xf[0] = xc[0]         - Real(0.5) * dxL;
            xf[N] = xc[N - 1]     + Real(0.5) * dxR;
        }

        // dF[i] = xf[i+1] - xf[i], i = 0..N-1
        std::transform(
            xf.begin() + 1, xf.end(),
            xf.begin(),
            dF.begin(),
            [](Real right, Real left) { return right - left; }
        );

        // dC: extremidades com meias-larguras + diferenças entre centros no miolo
        dC[0] = xc[0] - xf[0];

        if (N > 1) {
            // dC[i] = xc[i] - xc[i-1], i = 1..N-1
            std::transform(
                xc.begin() + 1, xc.end(),
                xc.begin(),
                dC.begin() + 1,
                [](Real cur, Real prev) { return cur - prev; }
            );
        }

        dC[N] = xf[N] - xc[N - 1];
    }
};

CENTERING_NAMESPACE_CLOSE
PATTERNS_NAMESPACE_CLOSE
GRID1D_NAMESPACE_CLOSE
GRID_NAMESPACE_CLOSE
FVMGRIDMAKER_NAMESPACE_CLOSE
