// ----------------------------------------------------------------------------
// File: Random1D_DistOptions.hpp
// Author: FVMGridMaker Team
// Version: 2.1
// Date: 2025-10-26
// Description: Helpers *somente para o exemplo* que montam DistOptions para
//              a distribuição Random1D (semente por relógio ou fixa).
//              Evita depender de nomes/campos específicos do Options.
// License: GNU GPL v3
// ----------------------------------------------------------------------------
/**
 * @file Random1D_DistOptions.hpp
 * @brief Helpers locais (apenas do exemplo) para configurar Random1D.
 *
 * @details
 * Estes helpers preenchem um `DistOptions` contendo:
 *  - `tag = DistributionTag::Random1D`;
 *  - `any = Random1D::Options{ ... }` com atribuições tolerantes a variações
 *    de nomes de campos (dx_min/w_lo/w_min/min_w, etc.) e sem depender de
 *    um enum SeedPolicy (se existir usamos, senão caímos em bools comuns).
 *
 * Importante: arquivo vive em `examples/` e **não** altera o core.
 */

#pragma once

#include <cstdint>
#include <any>
#include <type_traits>

#include <FVMGridMaker/Core/type.h>
#include <FVMGridMaker/Grid/Common/Tags1D.hpp>
#include <FVMGridMaker/Grid/Grid1D/Builders/DistOptions.hpp>
#include <FVMGridMaker/Grid/Grid1D/Patterns/Distribution/Random1D.hpp>

namespace FVMGridMaker::grid::grid1d::builders::example_helpers {

namespace dist = FVMGridMaker::grid::grid1d::patterns::distribution;

// ------------------------------------------------------------------
// Pequenos setters tolerantes a diferenças de nomes em Options
// ------------------------------------------------------------------
template <class Opt>
inline void set_min_width(Opt& o, core::Real v) {
    if constexpr (requires { o.dx_min = v; })      { o.dx_min = v; }
    else if constexpr (requires { o.w_min  = v; }) { o.w_min  = v; }
    else if constexpr (requires { o.w_lo   = v; }) { o.w_lo   = v; }
    else if constexpr (requires { o.min_w  = v; }) { o.min_w  = v; }
    // se nada existir, silenciosamente não faz nada
}

template <class Opt>
inline void set_max_width(Opt& o, core::Real v) {
    if constexpr (requires { o.dx_max = v; })      { o.dx_max = v; }
    else if constexpr (requires { o.w_max  = v; }) { o.w_max  = v; }
    else if constexpr (requires { o.w_hi   = v; }) { o.w_hi   = v; }
    else if constexpr (requires { o.max_w  = v; }) { o.max_w  = v; }
}

template <class Opt>
inline void set_seed_value(Opt& o, std::uint64_t s) {
    if constexpr (requires { o.seed = s; }) { o.seed = s; }
}

// Semente fixa: tenta policy, senão usa flags/bools comuns
template <class Opt>
inline void set_seed_fixed(Opt& o, std::uint64_t s) {
    set_seed_value(o, s);

    // Se houver flag de "usar relógio", desligamos
    if constexpr (requires { o.use_clock = false; }) { o.use_clock = false; }
    else if constexpr (requires { o.use_time = false; }) { o.use_time = false; }

    // Se houver campo 'policy' (enum ou integral), forçamos o "Fixed"
    if constexpr (requires { o.policy = static_cast<decltype(o.policy)>(0); }) {
        o.policy = static_cast<decltype(o.policy)>(0); // assumimos 0 = Fixed
    }
}

// Semente por relógio: tenta policy, senão usa flags/bools comuns
template <class Opt>
inline void set_seed_time(Opt& o) {
    // Ativa "usar relógio" se existir
    if constexpr (requires { o.use_clock = true; }) { o.use_clock = true; }
    else if constexpr (requires { o.use_time = true; }) { o.use_time = true; }

    // Zera semente se existir (não faz mal)
    set_seed_value(o, 0);

    // Se houver campo 'policy' (enum ou integral), forçamos o "Time"
    if constexpr (requires { o.policy = static_cast<decltype(o.policy)>(1); }) {
        o.policy = static_cast<decltype(o.policy)>(1); // assumimos 1 = Time
    }
}

// ------------------------------------------------------------------
// Helpers públicos usados pelo exemplo
// ------------------------------------------------------------------
/**
 * @brief Cria DistOptions para Random1D com semente fixa.
 * @param w_lo largura mínima
 * @param w_hi largura máxima
 * @param seed semente fixa
 */
inline DistOptions make_random1d_fixed(core::Real w_lo,
                                       core::Real w_hi,
                                       std::uint64_t seed)
{
    using Opt = dist::Random1D::Options;

    DistOptions cfg{};
    cfg.tag = DistributionTag::Random1D;

    Opt opt{};
    set_min_width(opt, w_lo);
    set_max_width(opt, w_hi);
    set_seed_fixed(opt, seed);

    cfg.any = opt; // guarda Options dentro do std::any
    return cfg;
}

/**
 * @brief Cria DistOptions para Random1D com semente baseada no relógio.
 * @param w_lo largura mínima
 * @param w_hi largura máxima
 */
inline DistOptions make_random1d_clock(core::Real w_lo,
                                       core::Real w_hi)
{
    using Opt = dist::Random1D::Options;

    DistOptions cfg{};
    cfg.tag = DistributionTag::Random1D;

    Opt opt{};
    set_min_width(opt, w_lo);
    set_max_width(opt, w_hi);
    set_seed_time(opt);

    cfg.any = opt;
    return cfg;
}

} // namespace FVMGridMaker::grid::grid1d::builders::example_helpers
