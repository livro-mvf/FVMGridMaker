// ----------------------------------------------------------------------------
// File: Random1D_DistOptions.hpp
// Author: FVMGridMaker Team
// Version: 2.0
// Date: 2025-10-26
// Description: Helpers de exemplo para criar DistOptions de Random1D sem
//              depender dos detalhes internos de Random1D::Options.
// License: GNU GPL v3
// ----------------------------------------------------------------------------

/**
 * @file Random1D_DistOptions.hpp
 * @brief Helpers para fabricar @ref DistOptions para a distribuição Random1D.
 */

#ifndef EXAMPLES_GRID_GRID1D_RANDOMGRID1D_RANDOM1D_DISTOPTIONS_HPP
#define EXAMPLES_GRID_GRID1D_RANDOMGRID1D_RANDOM1D_DISTOPTIONS_HPP

#include <FVMGridMaker/Core/namespace.h>
#include <FVMGridMaker/Core/type.h>
#include <FVMGridMaker/Grid/Grid1D/Builders/DistOptions.hpp>

FVMGRIDMAKER_NAMESPACE_OPEN
GRID_NAMESPACE_OPEN
GRID1D_NAMESPACE_OPEN
BUILDERS_NAMESPACE_OPEN

namespace example_helpers {

inline DistOptions make_random1d_fixed(core::Real w_lo,
                                       core::Real w_hi,
                                       std::uint64_t seed)
{
    return DistOptions::Random1D_Fixed(w_lo, w_hi, seed);
}

inline DistOptions make_random1d_clock(core::Real w_lo,
                                       core::Real w_hi)
{
    return DistOptions::Random1D_Clock(w_lo, w_hi);
}

} // namespace example_helpers

BUILDERS_NAMESPACE_CLOSE
GRID1D_NAMESPACE_CLOSE
GRID_NAMESPACE_CLOSE
FVMGRIDMAKER_NAMESPACE_CLOSE

#endif // EXAMPLES_GRID_GRID1D_RANDOMGRID1D_RANDOM1D_DISTOPTIONS_HPP
