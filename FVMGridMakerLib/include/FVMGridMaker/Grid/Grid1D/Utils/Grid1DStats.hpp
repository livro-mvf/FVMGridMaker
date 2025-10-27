// ----------------------------------------------------------------------------
// File: Grid1DStats.hpp
// Author: FVMGridMaker Team
// Version: 2.1
// Date: 2025-10-25
// Description: Utilitário (SRP) para estatísticas de malhas 1D.
//              Calcula métricas de qualidade sobre spans (sem cópias).
//              • Padrão: STL sequencial (minmax_element/accumulate)
//              • Opcional: paralelismo com transform_reduce (defina FVMG_STATS_PARALLEL)
// License: GNU GPL v3
// ----------------------------------------------------------------------------
#pragma once

// ----------------------------------------------------------------------------
// includes C++ (ordem alfabética)
// ----------------------------------------------------------------------------
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <numeric>
#include <optional>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>

#if defined(FVMG_STATS_PARALLEL)
  #include <execution> 
#endif

// ----------------------------------------------------------------------------
// includes FVMGridMaker
// ----------------------------------------------------------------------------
#include <FVMGridMaker/Core/namespace.h>
#include <FVMGridMaker/Core/type.h>
#include <FVMGridMaker/Grid/Grid1D/API/Grid1D.h>

FVMGRIDMAKER_NAMESPACE_OPEN
GRID_NAMESPACE_OPEN
GRID1D_NAMESPACE_OPEN
UTILS_NAMESPACE_OPEN

/// Classe de estatística (SRP): não gera malha; apenas resume dados.
struct Grid1DStats {
    using Real = FVMGridMaker::core::Real;

    // ------------------------------------------------------------------------
    // Núcleo compatível com a API anterior
    // ------------------------------------------------------------------------
    struct Stats { Real min{0}, max{0}, mean{0}; };

    /**
     * @brief Estatísticas (min, max, mean) sobre um span.
     *        Sem cópias; sequencial por padrão; paralela com FVMG_STATS_PARALLEL.
     */
    static inline Stats from_span(std::span<const Real> v) {
        if (v.empty()) return {};

    #if defined(FVMG_STATS_PARALLEL)
        struct Acc { Real min, max, sum; std::size_t n; };
        const Acc init{
            std::numeric_limits<Real>::infinity(),
            -std::numeric_limits<Real>::infinity(),
            Real(0), 0
        };
        auto make = [](Real x) noexcept -> Acc { return {x, x, x, 1}; };
        auto join = [](const Acc& a, const Acc& b) noexcept -> Acc {
            return { std::min(a.min,b.min), std::max(a.max,b.max), a.sum+b.sum, a.n+b.n };
        };
        const Acc r = std::transform_reduce(std::execution::par, v.begin(), v.end(), init, join, make);
        return { r.min, r.max, r.sum / static_cast<Real>(r.n) };
    #else
        const auto [itMin, itMax] = std::minmax_element(v.begin(), v.end());
        const Real sum = std::accumulate(v.begin(), v.end(), Real(0));
        return { *itMin, *itMax, sum / static_cast<Real>(v.size()) };
    #endif
    }

    /// Estatística dos dX das FACES (usa grid.deltasFaces()).
    template <class GridLike>
    static inline Stats faces(const GridLike& g) { return from_span(g.deltasFaces()); }

    /// Estatística dos dX dos CENTROS (usa grid.deltasCenters()).
    template <class GridLike>
    static inline Stats centers(const GridLike& g) { return from_span(g.deltasCenters()); }

    // ------------------------------------------------------------------------
    // 1) Estatísticas básicas estendidas
    // ------------------------------------------------------------------------
    struct Basic {
        Real min{0}, max{0}, mean{0}, stddev{0};
        Real aspect{0};   // max/min
        Real cv{0};       // stddev/mean
    };

    static inline Basic basic(std::span<const Real> L) {
        Basic out{};
        if (L.empty()) return out;

        const auto s = from_span(L);
        out.min = s.min; out.max = s.max; out.mean = s.mean;

        // stddev (populacional)
        const Real m = out.mean;
        Real acc = 0;
        for (Real x : L) { const Real d = x - m; acc += d*d; }
        out.stddev = std::sqrt(acc / static_cast<Real>(L.size()));

        out.aspect = (out.min > Real(0)) ? (out.max / out.min)
                                         : std::numeric_limits<Real>::infinity();
        out.cv     = (out.mean > Real(0)) ? (out.stddev / out.mean) : Real(0);
        return out;
    }

    template <class GridLike>
    static inline Basic basicFaces(const GridLike& g) { return basic(g.deltasFaces()); }

    // ------------------------------------------------------------------------
    // 2) Uniformidade: coef. variação + uniformidade relativa U
    // ------------------------------------------------------------------------
    static inline Real uniformidade_relativa(std::span<const Real> L) {
        if (L.empty()) return Real(0);
        const Real mean = std::accumulate(L.begin(), L.end(), Real(0)) / static_cast<Real>(L.size());
        if (mean == Real(0)) return Real(0);
        Real num = 0;
        for (Real x : L) num += std::abs(x - mean);
        const Real U = Real(1) - (num / (static_cast<Real>(L.size()) * mean));
        return std::clamp(U, Real(0), Real(1));
    }

    template <class GridLike>
    static inline Real uniformidadeFaces(const GridLike& g) { return uniformidade_relativa(g.deltasFaces()); }

    struct Histogram {
        Real bin_min{0}, bin_max{0}, bin_width{0};
        std::vector<std::size_t> counts; // size == bins
    };

    static inline Histogram histogram(std::span<const Real> L,
                                      std::size_t bins,
                                      std::optional<std::pair<Real,Real>> range = std::nullopt)
    {
        Histogram H{};
        if (L.empty() || bins == 0) return H;

        Real lo = range ? range->first  : *std::min_element(L.begin(), L.end());
        Real hi = range ? range->second : *std::max_element(L.begin(), L.end());
        if (!(hi > lo)) { hi = lo + Real(1); } // evita divisão zero

        H.bin_min = lo; H.bin_max = hi;
        H.bin_width = (hi - lo) / static_cast<Real>(bins);
        H.counts.assign(bins, 0);

        for (Real x : L) {
            if (x <= lo) { H.counts[0]++; continue; }
            if (x >= hi) { H.counts[bins-1]++; continue; }
            const Real pos = (x - lo) / (hi - lo);
            std::size_t k = static_cast<std::size_t>(pos * static_cast<Real>(bins)); // evita -Wconversion
            if (k >= bins) k = bins - 1;
            H.counts[k]++;
        }
        return H;
    }

    // ------------------------------------------------------------------------
    // 3) Mudança brusca de refinamento
    // ------------------------------------------------------------------------
    struct AdjacentRatios {
        std::vector<Real> R;      // N-1 razões
        Real max_ratio{1};        // max(R_i)
        Real worst_sym_ratio{1};  // max(max(R_i, 1/R_i))
    };

    static inline AdjacentRatios adjacent_ratios(std::span<const Real> L) {
        AdjacentRatios out{};
        const std::size_t n = L.size();
        if (n < 2) return out;
        out.R.resize(n - 1);
        Real mr = Real(1), w = Real(1);
        for (std::size_t i = 0; i + 1 < n; ++i) {
            const Real Li = L[i], Lj = L[i+1];
            const Real r  = (Li != Real(0)) ? (Lj / Li) : std::numeric_limits<Real>::infinity();
            out.R[i] = r;
            mr = std::max(mr, r);
            const Real sym = (r > Real(0)) ? ((r >= Real(1)) ? r : (Real(1)/r))
                                           : std::numeric_limits<Real>::infinity();
            w = std::max(w, sym);
        }
        out.max_ratio = mr;
        out.worst_sym_ratio = w;
        return out;
    }

    // ------------------------------------------------------------------------
    // 4) Suavidade
    // ------------------------------------------------------------------------
    struct Smoothness {
        Real mean_grad{0}, max_grad{0};
    };

    static inline Smoothness smoothness(std::span<const Real> L) {
        Smoothness s{};
        const std::size_t n = L.size();
        if (n < 2) return s;
        Real sum = 0, mx = 0;
        for (std::size_t i = 0; i + 1 < n; ++i) {
            const Real Li = L[i], Lj = L[i+1];
            const Real denom = std::min(Li, Lj);
            const Real g = (denom > Real(0)) ? (std::abs(Lj - Li)/denom) : Real(0);
            sum += g; mx = std::max(mx, g);
        }
        const Real m = static_cast<Real>(n - 1);
        s.mean_grad = sum / m; s.max_grad = mx;
        return s;
    }

    // ------------------------------------------------------------------------
    // 5) Bordas vs. interior
    // ------------------------------------------------------------------------
    struct EdgeVsInterior {
        Real mean_interior{0};
        Real left_over_interior{0};   // L0 / mean_interior
        Real right_over_interior{0};  // L_{N-1} / mean_interior
    };

    static inline EdgeVsInterior edges_vs_interior(std::span<const Real> L) {
        EdgeVsInterior e{};
        const std::size_t n = L.size();
        if (n == 0) return e;
        if (n <= 2) {
            e.mean_interior = Real(0);
            e.left_over_interior  = Real(0);
            e.right_over_interior = Real(0);
            return e;
        }
        Real sum = 0;
        for (std::size_t i = 1; i + 1 < n; ++i) sum += L[i];
        e.mean_interior = sum / static_cast<Real>(n - 2);
        if (e.mean_interior > Real(0)) {
            e.left_over_interior  = L.front() / e.mean_interior;
            e.right_over_interior = L.back()  / e.mean_interior;
        }
        return e;
    }

    // ------------------------------------------------------------------------
    // 6) Simetria
    // ------------------------------------------------------------------------
    struct Symmetry {
        Real symmetry_score{1};   // 1 → perfeito; 0 → ruim
        Real max_rel_diff{0};     // max |Li - Lrev| / max(Li, Lrev)
    };

    static inline Symmetry symmetry(std::span<const Real> L) {
        Symmetry s{};
        const std::size_t n = L.size();
        if (n == 0) return s;

        Real num = 0, den = 0, mx = 0;
        for (std::size_t i = 0; i < n/2; ++i) {
            const Real a = L[i];
            const Real b = L[n-1-i];
            num += std::abs(a - b);
            den += std::max(a, b);
            const Real denom = std::max(a,b);
            const Real rel = (denom > Real(0)) ? (std::abs(a-b)/denom) : Real(0);
            mx = std::max(mx, rel);
        }
        s.max_rel_diff = mx;
        s.symmetry_score = (den > Real(0)) ? (Real(1) - num/den) : Real(1);
        s.symmetry_score = std::clamp(s.symmetry_score, Real(0), Real(1));
        return s;
    }

    // ------------------------------------------------------------------------
    // 7) Conformidade com progressão geométrica
    // ------------------------------------------------------------------------
    struct GeomProgression {
        Real r_est{1};           // razão média estimada (exp média de log r_i)
        Real std_log_r{0};       // desvio padrão de log(r_i)
        Real max_dev_abs{0};     // max |r_i - r_est|
        bool within_tolerance{true};
    };

    static inline GeomProgression geometric_progression(std::span<const Real> L, Real tol = Real(1e-6)) {
        GeomProgression g{};
        const std::size_t n = L.size();
        if (n < 2) return g;

        std::vector<Real> log_r; log_r.reserve(n-1);
        Real max_dev = 0;
        for (std::size_t i = 0; i + 1 < n; ++i) {
            const Real Li = L[i], Lj = L[i+1];
            const Real r  = (Li > Real(0)) ? (Lj / Li) : Real(1);
            const Real r_pos = std::max(r, Real(1e-300));
            log_r.push_back(std::log(r_pos));
        }
        const Real mean_log = std::accumulate(log_r.begin(), log_r.end(), Real(0))
                            / static_cast<Real>(log_r.size());
        Real acc = 0;
        for (Real t : log_r) { const Real d = t - mean_log; acc += d*d; }
        g.std_log_r = std::sqrt(acc / static_cast<Real>(log_r.size()));
        g.r_est = std::exp(mean_log);

        for (std::size_t i = 0; i + 1 < n; ++i) {
            const Real Li = L[i], Lj = L[i+1];
            const Real r  = (Li > Real(0)) ? (Lj / Li) : Real(1);
            max_dev = std::max(max_dev, std::abs(r - g.r_est));
        }
        g.max_dev_abs = max_dev;
        g.within_tolerance = (max_dev <= tol);
        return g;
    }

    // ------------------------------------------------------------------------
    // 8) Regiões de interesse (ROI)
    // ------------------------------------------------------------------------
    struct RegionStats {
        std::size_t count{0};
        Real sum_lengths{0};
        Real mean_length{0};
    };

    /// ROI por predicado sobre xCentro
    template <class Pred>
    static inline RegionStats region_by_predicate(std::span<const Real> xcenters,
                                                  std::span<const Real> lengths,
                                                  Pred pred) {
        RegionStats r{};
        const std::size_t n = xcenters.size();
        if (lengths.size() != n) return r;
        for (std::size_t i = 0; i < n; ++i) {
            if (pred(xcenters[i])) {
                r.count++;
                r.sum_lengths += lengths[i];
            }
        }
        r.mean_length = (r.count > 0) ? (r.sum_lengths / static_cast<Real>(r.count)) : Real(0);
        return r;
    }

    /// ROI por intervalo [x0, x1] em xCentro
    static inline RegionStats region_interval(std::span<const Real> xcenters,
                                              std::span<const Real> lengths,
                                              Real x0, Real x1) {
        if (x1 < x0) std::swap(x0, x1);
        return region_by_predicate(xcenters, lengths, [&](Real x){ return x >= x0 && x <= x1; });
    }

    // ------------------------------------------------------------------------
    // Atalhos baseados em GridLike (usa deltasFaces como L_i)
    // ------------------------------------------------------------------------
    template <class GridLike>
    static inline Basic basic(const GridLike& g) { return basic(g.deltasFaces()); }

    template <class GridLike>
    static inline AdjacentRatios adjacent(const GridLike& g) { return adjacent_ratios(g.deltasFaces()); }

    template <class GridLike>
    static inline Smoothness smooth(const GridLike& g) { return smoothness(g.deltasFaces()); }

    template <class GridLike>
    static inline EdgeVsInterior edgeBalance(const GridLike& g) { return edges_vs_interior(g.deltasFaces()); }

    template <class GridLike>
    static inline Symmetry symmetry(const GridLike& g) { return symmetry(g.deltasFaces()); }

    template <class GridLike>
    static inline GeomProgression geom(const GridLike& g, Real tol = Real(1e-6)) {
        return geometric_progression(g.deltasFaces(), tol);
    }

    template <class GridLike>
    static inline RegionStats region(const GridLike& g, Real x0, Real x1) {
        return region_interval(g.centers(), g.deltasFaces(), x0, x1);
    }
};

UTILS_NAMESPACE_CLOSE
GRID1D_NAMESPACE_CLOSE
GRID_NAMESPACE_CLOSE
FVMGRIDMAKER_NAMESPACE_CLOSE
