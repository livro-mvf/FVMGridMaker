#pragma once
#include <span>
#include <cstdint>
#include <cassert>
#include <FVMGridMaker/Core/namespace.h>
#include <FVMGridMaker/Core/type.h>

FVMGRIDMAKER_NAMESPACE_OPEN
GRID_NAMESPACE_OPEN
GRID1D_NAMESPACE_OPEN
PATTERNS_NAMESPACE_OPEN
DISTRIBUTION_NAMESPACE_OPEN

struct Uniform1D {
    using Real = FVMGridMaker::core::Real;
    using Size = std::size_t;

    // Faces: xf[i] = A + i*dx, i=0..N
    void makeFaces(Size N, Real A, Real B,
                   std::span<Real> xf,
                   std::uint64_t /*seed*/ = 0,
                   Real /*dx_min*/ = Real(0)) const
    {
        assert(xf.size() == N + Size(1));
        const Real dx = (B - A) / static_cast<Real>(N);
        for (Size i = 0; i <= N; ++i) {
            xf[i] = A + static_cast<Real>(i) * dx;
        }
    }

    // Centros: xc[i] = A + (i+0.5)*dx, i=0..N-1
    void makeCenters(Size N, Real A, Real B,
                     std::span<Real> xc,
                     std::uint64_t /*seed*/ = 0,
                     Real /*dx_min*/ = Real(0)) const
    {
        assert(xc.size() == N);
        const Real dx = (B - A) / static_cast<Real>(N);
        for (Size i = 0; i < N; ++i) {
            xc[i] = A + (static_cast<Real>(i) + Real(0.5)) * dx;
        }
    }
};

DISTRIBUTION_NAMESPACE_CLOSE
PATTERNS_NAMESPACE_CLOSE
GRID1D_NAMESPACE_CLOSE
GRID_NAMESPACE_CLOSE
FVMGRIDMAKER_NAMESPACE_CLOSE
