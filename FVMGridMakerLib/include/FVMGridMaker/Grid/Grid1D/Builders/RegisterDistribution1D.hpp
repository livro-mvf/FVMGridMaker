// ============================================================================
// File: RegisterDistribution1D.hpp
// Author: FVMGridMaker Team
// Version: 2.5
// Date: 2025-10-26
// Description:
//   Utilitários para registro estático de distribuições 1D no runtime.
//   Permite que distribuições se auto-registrem de fora da biblioteca.
// License: GNU GPL v3
// ============================================================================

#pragma once
#include <FVMGridMaker/Grid/Grid1D/Builders/Grid1DDistributionRegistry.hpp>
#include <FVMGridMaker/Grid/Common/Tags1D.hpp>

namespace FVMGridMaker::grid::grid1d::builders {
namespace detail {
struct DistRegistrar1D {
    DistRegistrar1D(const char* name,
                    Grid1DDistributionRegistry::Entry entry,
                    std::optional<DistributionTag> tag)
    {
        auto& reg = Grid1DDistributionRegistry::instance();
        if (tag.has_value()) {
            reg.registerDistribution(std::string{name}, std::move(entry), *tag);
        } else {
            reg.registerDistribution(std::string{name}, std::move(entry));
        }
    }
};
} // namespace detail
} // namespace FVMGridMaker::grid::grid1d::builders

// Macro de registro (gera um objeto estático com ctor que registra a dist.)
#define FVMG_REGISTER_GRID1D_DISTRIBUTION(NAME_STR, TAG_OPT, FACES_LAMBDA, CENTERS_LAMBDA) \
    namespace {                                                                            \
    struct FVMG_Reg1D_##TAG_OPT##__##__LINE__ {                                            \
        FVMG_Reg1D_##TAG_OPT##__##__LINE__() {                                             \
            using namespace FVMGridMaker::grid::grid1d::builders;                          \
            Grid1DDistributionRegistry::Entry e{};                                         \
            e.faces_fn   = (FACES_LAMBDA);                                                 \
            e.centers_fn = (CENTERS_LAMBDA);                                               \
            std::optional<FVMGridMaker::grid::DistributionTag> tag_;                       \
            /* TAG_OPT pode ser um DistributionTag ou nada */                              \
            if constexpr (!std::is_same_v<decltype(TAG_OPT), void>) tag_ = TAG_OPT;        \
            static detail::DistRegistrar1D _r(NAME_STR, std::move(e), tag_);               \
        }                                                                                  \
    } FVMG_Reg1D_instance_##TAG_OPT##__##__LINE__;                                         \
    }
