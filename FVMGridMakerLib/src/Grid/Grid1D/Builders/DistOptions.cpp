// ----------------------------------------------------------------------------
// File: DistOptions.cpp
// Author: FVMGridMaker Team
// Version: 2.0
// Date: 2025-10-26
// Description: Implementação das fábricas de DistOptions para Random1D.
// License: GNU GPL v3
// ----------------------------------------------------------------------------

/**
 * @file DistOptions.cpp
 * @brief Implementação de @ref DistOptions (fábricas para Random1D).
 */

#include <FVMGridMaker/Core/namespace.h>
#include <FVMGridMaker/Grid/Grid1D/Builders/DistOptions.hpp>

// Precisamos conhecer o tipo do payload ao construir o std::any:
#include <FVMGridMaker/Grid/Grid1D/Patterns/Distribution/Random1D.hpp>

FVMGRIDMAKER_NAMESPACE_OPEN
GRID_NAMESPACE_OPEN
GRID1D_NAMESPACE_OPEN
BUILDERS_NAMESPACE_OPEN

using patterns::distribution::Random1D;

DistOptions DistOptions::Random1D_Fixed(core::Real w_lo,
                                        core::Real w_hi,
                                        std::uint64_t seed)
{
    DistOptions out;
    out.tag = grid::DistributionTag::Random1D;

    Random1D::Options opt{};
    opt.w_lo = w_lo;
    opt.w_hi = w_hi;
    opt.seed = seed;        // determinístico
    out.any = opt;          // guarda no std::any

    return out;
}

DistOptions DistOptions::Random1D_Clock(core::Real w_lo,
                                        core::Real w_hi)
{
    DistOptions out;
    out.tag = grid::DistributionTag::Random1D;

    Random1D::Options opt{};
    opt.w_lo = w_lo;
    opt.w_hi = w_hi;
    opt.seed = std::nullopt; // sem semente fixa → usa relógio no padrão
    out.any = opt;

    return out;
}

BUILDERS_NAMESPACE_CLOSE
GRID1D_NAMESPACE_CLOSE
GRID_NAMESPACE_CLOSE
FVMGRIDMAKER_NAMESPACE_CLOSE
