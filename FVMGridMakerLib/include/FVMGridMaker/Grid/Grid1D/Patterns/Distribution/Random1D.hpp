// ----------------------------------------------------------------------------
// File: Random1D.hpp
// Author: FVMGridMaker Team
// Version: 2.1
// Date: 2025-10-26
// Description: Distribuição Random1D para geração de malhas 1D aleatórias,
//              respeitando limites inferiores/superiores de largura por célula.
//              Implementa projeção no "simplex com cotas" para garantir
//              d_i ∈ [w_lo*dx0, w_hi*dx0] e ∑ d_i = (B-A).
// License: GNU GPL v3
// ----------------------------------------------------------------------------
#pragma once

/**
 * @file   Random1D.hpp
 * @brief  Gera malhas 1D aleatórias com limites por célula.
 *
 * @details
 * Política:
 *   - Sorteia pesos brutos r_i ~ U[w_lo, w_hi].
 *   - Projeta vetor r para x (em unidades de dx0) tal que:
 *        lo ≤ x_i ≤ hi  e  ∑ x_i = N,
 *     onde dx0 = (B-A)/N e lo=w_lo, hi=w_hi.
 *   - Define larguras d_i = dx0 * x_i.
 *   - Faces por soma prefixada exclusiva; centros pelas médias das faces.
 *
 * Notas:
 *   - Determinismo quando seed é fixada no Options.
 *   - Pré-condição de viabilidade: w_lo ≤ 1 ≤ w_hi (para N células).
 *     Se violada, ajustamos suavemente para garantir viabilidade:
 *       w_lo' = std::min(w_lo, Real(1)), w_hi' = std::max(w_hi, Real(1)).
 */

#include <FVMGridMaker/Core/namespace.h>
#include <FVMGridMaker/Core/type.h>

#include <any>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <optional>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

FVMGRIDMAKER_NAMESPACE_OPEN
GRID_NAMESPACE_OPEN
GRID1D_NAMESPACE_OPEN
PATTERNS_NAMESPACE_OPEN
DISTRIBUTION_NAMESPACE_OPEN

class Random1D {
public:
    using Real  = FVMGridMaker::core::Real;
    using Index = FVMGridMaker::core::Index;

    struct Options {
        Real    w_lo { Real(0.5) };   ///< limite inferior relativo (≥0)
        Real    w_hi { Real(1.5) };   ///< limite superior relativo (≥ w_lo)
        std::optional<std::uint64_t> seed {}; ///< semente opcional (determinismo)

        // Futuro: políticas alternativas (ex.: distribuição dos pesos).
        enum class Policy : std::uint8_t { BoundedProject = 0 };
        Policy policy { Policy::BoundedProject };
    };

    // ------------------------------------------------------------------------
    // Interface principal
    // ------------------------------------------------------------------------
    static std::vector<Real> faces(Index n, Real A, Real B,
                                   const Options* opt = nullptr)
    {
        ensure_inputs(n, A, B);
        const auto cfg = sanitize_opts(opt);
        const auto widths = make_widths(n, A, B, cfg);
        std::vector<Real> xf(static_cast<std::size_t>(n + 1));
        xf[0] = A;

        // soma prefixada exclusiva para faces internas
        for (Index i = 1; i <= n; ++i) {
            xf[static_cast<std::size_t>(i)] =
                xf[static_cast<std::size_t>(i-1)] + widths[static_cast<std::size_t>(i-1)];
        }
        // Por construção da projeção: soma(widths) = (B-A) ⇒ xf[n] = B
        xf.back() = B;
        return xf;
    }

    static std::vector<Real> centers(Index n, Real A, Real B,
                                     const Options* opt = nullptr)
    {
        auto xf = faces(n, A, B, opt);
        std::vector<Real> xc(static_cast<std::size_t>(n));
        for (Index i = 0; i < n; ++i) {
            const auto i0 = static_cast<std::size_t>(i);
            xc[i0] = Real(0.5) * (xf[i0] + xf[i0 + 1]);
        }
        return xc;
    }

    // ------------------------------------------------------------------------
    // Ponte para o registro (std::any*)
    // ------------------------------------------------------------------------
    static std::vector<Real> faces(Index n, Real A, Real B, const std::any* any_opt)
    {
        Options cfg = options_from_any(any_opt);
        return faces(n, A, B, &cfg);
    }

    static std::vector<Real> centers(Index n, Real A, Real B, const std::any* any_opt)
    {
        Options cfg = options_from_any(any_opt);
        return centers(n, A, B, &cfg);
    }

private:
    // ------------------------------------------------------------------------
    // Utilitários
    // ------------------------------------------------------------------------
    static void ensure_inputs(Index n, Real A, Real B) {
        if (n == 0) {
            throw std::invalid_argument("Random1D::faces/centers(): N deve ser > 0.");
        }
        if (!(B > A)) {
            throw std::invalid_argument("Random1D::faces/centers(): exige dominio com B > A.");
        }
    }

    static Options sanitize_opts(const Options* opt_in) {
        Options cfg = opt_in ? *opt_in : Options{};
        if (cfg.w_lo < Real(0)) cfg.w_lo = Real(0);
        if (cfg.w_hi < cfg.w_lo) cfg.w_hi = cfg.w_lo;

        // Viabilidade: é necessário w_lo ≤ 1 ≤ w_hi para ∑ x_i = N com x_i∈[lo,hi].
        if (cfg.w_lo > Real(1)) cfg.w_lo = Real(1);
        if (cfg.w_hi < Real(1)) cfg.w_hi = Real(1);
        return cfg;
    }

    static Options options_from_any(const std::any* any_opt) {
        if (any_opt && any_opt->has_value()) {
            if (any_opt->type() == typeid(Options)) {
                return std::any_cast<Options>(*any_opt);
            }
        }
        return Options{};
    }

    // Gera larguras d_i, garantindo:
    //   lo*dx0 ≤ d_i ≤ hi*dx0 e ∑ d_i = (B-A)
    static std::vector<Real> make_widths(Index n, Real A, Real B, const Options& cfg)
    {
        const Real N   = static_cast<Real>(n);
        const Real len = (B - A);
        const Real dx0 = len / N;
        const Real lo  = cfg.w_lo;  // limites em unidades relativas a dx0
        const Real hi  = cfg.w_hi;

        // 1) Pesos brutos r_i ~ U[lo, hi]
        std::mt19937_64 rng;
        if (cfg.seed) rng.seed(*cfg.seed);
        else {
            // fallback determinístico por padrão (semente fixa) p/ reprodutibilidade
            rng.seed(0x9E3779B97F4A7C15ULL);
        }
        std::uniform_real_distribution<Real> dist(lo, hi);

        std::vector<Real> r(static_cast<std::size_t>(n));
        for (auto& v : r) v = dist(rng);

        // 2) Projeção em { x : lo ≤ x_i ≤ hi, ∑ x_i = N }
        std::vector<Real> x = bounded_simplex_project(r, lo, hi, N);

        // 3) Converte para larguras reais
        std::vector<Real> d(static_cast<std::size_t>(n));
        for (std::size_t i = 0; i < d.size(); ++i) d[i] = dx0 * x[i];
        return d;
    }

    // Projeção proporcional com cotas, iterativa:
    // - Parte dos "pesos" r_i (positivos).
    // - Em cada passo, escala o subconjunto livre por um fator s para atingir a soma
    //   restante; quem violar cotas é "fixado" na cota e removido do conjunto livre.
    // - Termina quando ninguém viola.
    static std::vector<Real>
    bounded_simplex_project(const std::vector<Real>& r, Real lo, Real hi, Real target_sum)
    {
        const std::size_t n = r.size();
        std::vector<Real> x(n, Real(0));
        if (n == 0) return x;

        // Estados
        std::vector<char> fixed(n, 0);  // 0=livre, 1=fixed
        std::vector<Real> cur = r;      // pesos correntes dos livres

        // Sanidade dos pesos (evita divisão por zero)
        Real sum_pos = Real(0);
        for (Real v : cur) sum_pos += (v > Real(0) ? v : Real(0));
        if (!(sum_pos > Real(0))) {
            // Se todos não-positivos, distribui igualmente no interior das cotas
            const Real mid = std::clamp(target_sum / static_cast<Real>(n), lo, hi);
            std::fill(x.begin(), x.end(), mid);
            // Ajuste final pequeno para somar exatamente (distribui o resíduo)
            adjust_residual(x, target_sum, lo, hi);
            return x;
        }
        for (auto& v : cur) if (v <= Real(0)) v = std::numeric_limits<Real>::min();

        Real sum_fixed = Real(0);
        std::size_t n_free = n;

        // Itera no máximo n vezes (cada passo fixa >= 1 elemento)
        for (std::size_t iter = 0; iter < n; ++iter) {
            // Soma dos pesos livres
            Real sum_r_free = Real(0);
            for (std::size_t i = 0; i < n; ++i) if (!fixed[i]) sum_r_free += cur[i];

            // Se não há livres, encerra
            if (n_free == 0 || !(sum_r_free > Real(0))) break;

            const Real remain = target_sum - sum_fixed;
            const Real s = remain / sum_r_free;

            bool clamped_any = false;

            // Primeiro passe: calcula valores escalados e marca violações
            for (std::size_t i = 0; i < n; ++i) {
                if (fixed[i]) continue;
                const Real v = cur[i] * s;
                if (v < lo) {
                    x[i] = lo;
                    fixed[i] = 1;
                    sum_fixed += lo;
                    --n_free;
                    clamped_any = true;
                } else if (v > hi) {
                    x[i] = hi;
                    fixed[i] = 1;
                    sum_fixed += hi;
                    --n_free;
                    clamped_any = true;
                } else {
                    x[i] = v; // candidato válido (ainda livre, mas já no intervalo)
                }
            }

            if (!clamped_any) {
                // Todos dentro das cotas — terminamos com soma correta
                // Pequeno ajuste numérico (se necessário)
                adjust_residual(x, target_sum, lo, hi);
                break;
            }

            // Atualiza pesos dos livres para próxima iteração:
            // Mantém cur[i] proporcional ao x[i] atual (somente livres).
            for (std::size_t i = 0; i < n; ++i) {
                if (!fixed[i]) {
                    // usar x[i] como novo "peso" para próxima redistribuição
                    cur[i] = x[i];
                }
            }
        }

        return x;
    }

    // Corrige minúsculo resíduo numérico para fechar a soma exatamente,
    // respeitando as cotas. Distribui o resíduo pelos elementos com margem.
    static void adjust_residual(std::vector<Real>& x, Real target_sum, Real lo, Real hi)
    {
        Real sum_x = std::accumulate(x.begin(), x.end(), Real(0));
        Real resid = target_sum - sum_x;

        if (std::abs(resid) <= Real(1e-14) * std::max(Real(1), std::abs(target_sum))) {
            return; // nada a ajustar
        }

        // Tenta distribuir o resíduo positivamente em quem tem espaço até hi
        if (resid > Real(0)) {
            for (std::size_t i = 0; i < x.size() && resid > Real(0); ++i) {
                const Real room = hi - x[i];
                if (room > Real(0)) {
                    const Real delta = std::min(room, resid);
                    x[i] += delta;
                    resid -= delta;
                }
            }
        } else { // resid < 0 — remove de quem tem acima de lo
            resid = -resid;
            for (std::size_t i = 0; i < x.size() && resid > Real(0); ++i) {
                const Real room = x[i] - lo;
                if (room > Real(0)) {
                    const Real delta = std::min(room, resid);
                    x[i] -= delta;
                    resid -= delta;
                }
            }
        }
        // Se ainda restar resíduo por erros extremos, faz um ajuste final no primeiro índice com margem:
        if (std::abs(resid) > Real(1e-14)) {
            for (std::size_t i = 0; i < x.size(); ++i) {
                if (resid > Real(0)) {
                    const Real room = hi - x[i];
                    if (room > Real(0)) { x[i] += std::min(room, resid); break; }
                } else {
                    const Real room = x[i] - lo;
                    if (room > Real(0)) { x[i] -= std::min(room, -resid); break; }
                }
            }
        }
    }
};

DISTRIBUTION_NAMESPACE_CLOSE
PATTERNS_NAMESPACE_CLOSE
GRID1D_NAMESPACE_CLOSE
GRID_NAMESPACE_CLOSE
FVMGRIDMAKER_NAMESPACE_CLOSE
