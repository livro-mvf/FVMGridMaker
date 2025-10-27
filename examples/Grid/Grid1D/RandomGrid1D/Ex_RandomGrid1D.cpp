// ----------------------------------------------------------------------------
// File: Ex_RandomGrid1D.cpp
// Author: FVMGridMaker Team
// Version: 2.3
// Date: 2025-10-26
// Description: Exemplo sem CLI — cria duas malhas 1D com distribuição Random1D:
//              (1) Face-centered e (2) Cell-centered. Usa seed fixa para
//              determinismo e multiplicadores relativos w_lo/w_hi.
// License: GNU GPL v3
// ----------------------------------------------------------------------------

/**
 * @file Ex_RandomGrid1D.cpp
 * @brief Demonstra a construção de malhas Random1D (face/cell) sem leitura de
 *        argumentos, e imprime estatísticas + malha usando utilitários da lib.
 *
 * Notas importantes:
 *  - Este exemplo inclui o cabeçalho do padrão Random1D **somente aqui**;
 *    o core permanece desacoplado de padrões.
 *  - Em Random1D::Options, w_lo e w_hi são **fatores relativos** a dx_ref
 *    (o passo uniforme médio): use valores como 0.6 … 1.4.
 */

#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <numeric>
#include <span>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

// FVMGridMaker
#include <FVMGridMaker/Core/type.h>
#include <FVMGridMaker/Grid/Common/Tags1D.hpp>
#include <FVMGridMaker/Grid/Grid1D/API/Grid1D.h>
#include <FVMGridMaker/Grid/Grid1D/Builders/Grid1DBuilder.hpp>
#include <FVMGridMaker/Grid/Grid1D/Utils/Grid1DStats.hpp>
#include <FVMGridMaker/Grid/Grid1D/Utils/Grid1DStatsExec.hpp>

// Somente no EXEMPLO (não no core):
#include <FVMGridMaker/Grid/Grid1D/Patterns/Distribution/Random1D.hpp>

using Real  = FVMGridMaker::core::Real;
using Index = FVMGridMaker::core::Index;

using FVMGridMaker::grid::CenteringTag;
using FVMGridMaker::grid::DistributionTag;
using FVMGridMaker::grid::grid1d::api::Grid1D;
using FVMGridMaker::grid::grid1d::builders::Grid1DBuilder;
namespace G1U = FVMGridMaker::grid::grid1d::utils;
namespace dist = FVMGridMaker::grid::grid1d::patterns::distribution;

// ----------------------------------------------------------------------------
// precisão padrão para números impressos
// ----------------------------------------------------------------------------
constexpr int FVMG_PRINT_PREC = 6;

// ----------------------------------------------------------------------------
// helpers de impressão (alinhamento limpo)
// ----------------------------------------------------------------------------
static inline void kv_text(const char* key, std::string_view val,
                           int wkey = 24, int wval = 26) {
    std::cout << std::left  << std::setw(wkey) << key
              << std::right << std::setw(wval) << val << "\n";
}

template <class T>
static inline void kv_num(const char* key, T val,
                          int wkey = 24, int wval = 26, int prec = FVMG_PRINT_PREC) {
    std::ostringstream os;
    os.setf(std::ios::fixed);
    os << std::setprecision(prec) << val;
    std::cout << std::left  << std::setw(wkey) << key
              << std::right << std::setw(wval) << os.str() << "\n";
}

static inline void print_section(std::string_view title, int width = 24 + 26) {
    std::cout << "\n[" << title << "]\n";
    const auto n = static_cast<std::size_t>(width < 0 ? 0 : width);
    std::cout << std::string(n, '-') << "\n";
}

// ----------------------------------------------------------------------------
// imprime malha (como você forneceu)
// ----------------------------------------------------------------------------
static void print_malha(std::span<const Real> xf,
                        std::span<const Real> xc,
                        std::span<const Real> dF,
                        std::span<const Real> dC) {
    std::cout << "\n\n";
    print_section("Legenda", 24 + 26);
    kv_text("xFace",    "coordenadas das faces (N+1)");
    kv_text("xCentro",  "coordenadas dos centros (N)");
    kv_text("dXFace",   "larguras entre faces (N)  -> fornecido pela malha");
    kv_text("dXCentro", "larguras centradas (N+1)  -> fornecido pela malha");

    const Index N = static_cast<Index>(xc.size());
    const int   W = 24;

    // Cabeçalho alinhado aos números
    std::cout << std::right
              << "\n\n"
              << std::setw(W) << "xFace"
              << std::setw(W) << "xCentro"
              << std::setw(W) << "dXFace"
              << std::setw(W) << "dXCentro" << "\n";

    std::cout << std::string(static_cast<std::size_t>(W) * 4u, '-') << "\n";
    std::cout << std::right << std::fixed << std::setprecision(FVMG_PRINT_PREC);

    // Formatter de uma célula (largura W, precisão FVMG_PRINT_PREC)
    auto fmt = [W](Real v) {
        std::ostringstream os;
        os.setf(std::ios::fixed);
        os << std::setw(W) << std::setprecision(FVMG_PRINT_PREC) << v;
        return os.str();
    };
    const std::string blank(static_cast<std::size_t>(W), ' ');

    // Índices 0..N (para imprimir N+1 linhas)
    std::vector<std::size_t> ids(static_cast<std::size_t>(N) + 1u);
    std::iota(ids.begin(), ids.end(), std::size_t{0});

    // Constrói cada linha completa (4 colunas concatenadas) via transform
    std::vector<std::string> linhas(ids.size());
    std::transform(ids.begin(), ids.end(), linhas.begin(),
                   [&](std::size_t i) {
                       const std::string c0 = fmt(xf[i]);
                       const std::string c1 = (i < static_cast<std::size_t>(N)) ? fmt(xc[i]) : blank;
                       const std::string c2 = (i < static_cast<std::size_t>(N)) ? fmt(dF[i]) : blank;
                       const std::string c3 = fmt(dC[i]);
                       return c0 + c1 + c2 + c3;
                   });

    // Imprime todas as linhas
    std::copy(linhas.begin(), linhas.end(),
              std::ostream_iterator<std::string>(std::cout, "\n"));
}

// ----------------------------------------------------------------------------
// estatísticas (como você forneceu)
// ----------------------------------------------------------------------------
static void mostra_estatisticas(const Grid1D& grid,
                                std::string_view stats_mode) {
    using G1U::ExecPolicy;

    ExecPolicy pol = ExecPolicy::Auto;
    if (stats_mode == "ser" || stats_mode == "serial")        pol = ExecPolicy::Serial;
    else if (stats_mode == "par" || stats_mode == "parallel") pol = ExecPolicy::Parallel;

    bool used_par = false;
    const auto basicF = G1U::basic_exec(grid, pol, &used_par);

    print_section("Estatísticas");
    kv_text("Modo", used_par ? "paralelo (PSTL)" : "serial");

    print_section(used_par ? "Básicas — dXFace(par)" : "Básicas — dXFace");
    kv_num("min",    basicF.min);
    kv_num("max",    basicF.max);
    kv_num("mean",   basicF.mean);
    kv_num("std",    basicF.stddev);
    kv_num("aspect", basicF.aspect);
    kv_num("CV",     basicF.cv);

    const auto U    = G1U::Grid1DStats::uniformidadeFaces(grid);
    const auto adj  = G1U::Grid1DStats::adjacent(grid);
    const auto sm   = G1U::Grid1DStats::smooth(grid);
    const auto edge = G1U::Grid1DStats::edgeBalance(grid);
    const auto sym  = G1U::Grid1DStats::symmetry(grid);
    const auto gp   = G1U::Grid1DStats::geom(grid);

    print_section("Uniformidade");
    kv_num("U", U);

    print_section("Adjacente");
    kv_num("maxRazao",           adj.max_ratio);
    kv_num("piorSaltoSimetrico", adj.worst_sym_ratio);

    print_section("Suavidade");
    kv_num("meanG", sm.mean_grad);
    kv_num("maxG",  sm.max_grad);

    print_section("Bordas/Interior");
    kv_num("meanInt",    edge.mean_interior);
    kv_num("L0/meanInt", edge.left_over_interior);
    kv_num("Ln/meanInt", edge.right_over_interior);

    print_section("Simetria");
    kv_num("score",      sym.symmetry_score);
    kv_num("maxRelDiff", sym.max_rel_diff);

    print_section("Progr. Geométrica");
    kv_num("r_est",            gp.r_est);
    kv_num("std_log_r",        gp.std_log_r);
    kv_num("max|r_i - r_est|", gp.max_dev_abs);
    kv_text("dentroTol",       gp.within_tolerance ? "sim" : "nao");
}

// ----------------------------------------------------------------------------
// helper: constrói uma malha Random1D com o centering desejado
// ----------------------------------------------------------------------------
static Grid1D build_random_grid(Index n, Real A, Real B,
                                CenteringTag centering,
                                const dist::Random1D::Options& opt)
{
    Grid1DBuilder b;
    b.setN(n)
     .setDomain(A, B)
     .setDistribution(DistributionTag::Random1D)
     .setCentering(centering)
     .setOption(opt); // overload específico de Random1D::Options
    return b.build();
}

// ----------------------------------------------------------------------------
// main — cria DUAS malhas (face-centered e cell-centered) e imprime tudo
// ----------------------------------------------------------------------------
int main() {
#if defined(__GNUC__) || defined(__clang__)
#  pragma message("[FVMGridMaker][build] Compilando Ex_RandomGrid1D.cpp (sem CLI)")
#endif
    try {
        // Parâmetros ajustáveis AQUI
        constexpr Index N    = 32;     // número de intervalos
        constexpr Real  A    = 0.0;
        constexpr Real  B    = 1.0;
        constexpr Real  W_LO = 0.6;    // fatores relativos a dx_ref
        constexpr Real  W_HI = 1.4;
        constexpr std::uint64_t SEED = 202501; // determinismo

        // dx_ref apenas para log informativo
        const Real dx_ref = (B - A) / static_cast<Real>(N);

        // Opções do padrão: w_lo/w_hi são multiplicadores relativos
        dist::Random1D::Options opt{};
        opt.w_lo = W_LO;
        opt.w_hi = W_HI;
        opt.seed = SEED;

        std::cout << "[Ex_RandomGrid1D] N=" << N
                  << "  A=" << A << "  B=" << B
                  << "  dx_ref=" << std::fixed << std::setprecision(FVMG_PRINT_PREC) << dx_ref
                  << "  (w_lo=" << W_LO << ", w_hi=" << W_HI << ", seed=" << SEED << ")\n";

        // Malha face-centered
        Grid1D grid_face = build_random_grid(N, A, B, CenteringTag::FaceCentered, opt);

        // Malha cell-centered
        Grid1D grid_cell = build_random_grid(N, A, B, CenteringTag::CellCentered, opt);

        // ---- Imprime FACE-CENTERED -------------------------------------------------
        std::cout << "\n==================== Random1D — FACE-CENTERED ====================\n";
        mostra_estatisticas(grid_face, "auto");
        print_malha(grid_face.faces(), grid_face.centers(),
                    grid_face.deltasFaces(), grid_face.deltasCenters());

        // ---- Imprime CELL-CENTERED -------------------------------------------------
        std::cout << "\n==================== Random1D — CELL-CENTERED =====================\n";
        mostra_estatisticas(grid_cell, "auto");
        print_malha(grid_cell.faces(), grid_cell.centers(),
                    grid_cell.deltasFaces(), grid_cell.deltasCenters());

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "[Ex_RandomGrid1D] Erro: " << e.what() << "\n";
        return 1;
    }
}
