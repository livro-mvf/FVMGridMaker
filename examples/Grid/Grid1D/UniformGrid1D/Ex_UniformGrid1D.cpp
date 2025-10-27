#ifndef FVMG_EX_ENABLE_PAR
#define FVMG_EX_ENABLE_PAR 1
#endif

#if !FVMG_EX_ENABLE_PAR
  #undef FVMG_HAVE_PSTL_EXEC
#endif


// ----------------------------------------------------------------------------
// includes C++ (ordem alfabética)
// ----------------------------------------------------------------------------
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <numeric>
#include <span>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>

// ----------------------------------------------------------------------------
// includes FVMGridMaker (ordem alfabética por caminho)
// ----------------------------------------------------------------------------
// #include <FVMGridMaker/Core/type.h>
#include <FVMGridMaker/Grid/Common/Tags1D.hpp>
#include <FVMGridMaker/Grid/Grid1D/API/Grid1D.h>
#include <FVMGridMaker/Grid/Grid1D/Builders/Grid1DBuilder.hpp>
#include <FVMGridMaker/Grid/Grid1D/Utils/Grid1DStats.hpp>
#include <FVMGridMaker/Grid/Grid1D/Utils/Grid1DStatsExec.hpp>



// ----------------------------------------------------------------------------
// aliases úteis
// ----------------------------------------------------------------------------
using Real  = FVMGridMaker::core::Real;
using Index = FVMGridMaker::core::Index;

using FVMGridMaker::grid::CenteringTag;
using FVMGridMaker::grid::DistributionTag;
using FVMGridMaker::grid::grid1d::api::Grid1D;
using FVMGridMaker::grid::grid1d::builders::Grid1DBuilder;
namespace G1U = FVMGridMaker::grid::grid1d::utils;

// ----------------------------------------------------------------------------
// precisão padrão para números impressos
// ----------------------------------------------------------------------------
constexpr int FVMG_PRINT_PREC = 6;

// ----------------------------------------------------------------------------

static void print_malha     (   std::span<const Real> xf,
                                  std::span<const Real> xc,
                                  std::span<const Real> dF,
                                  std::span<const Real> dC
                            );

static void mostra_estatisticas (   const Grid1D& grid,
                                    std::string_view stats_mode
                                );

// helpers de impressão (alinhamento limpo)
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
// main — usa apenas a API da biblioteca (sem cópias nem contas próprias)
// CLI: Ex_UniformGrid1D [N] [A] [B] [modo_centering] [stats_mode]
// ----------------------------------------------------------------------------
int main() {
    Index N   = 10;
    Real  A   = 0.0;
    Real  B   = 1.0;
    std::string modo_centering = "auto";
    std::string stats_mode     = "auto";

    if (!(N > 0 && B > A)) {
        std::cerr << "Parâmetros inválidos.\nUso:\n"
                  << "  Ex_UniformGrid1D [N] [A] [B] [modo_centering] [stats_mode]\n"
                  << "  modo_centering: face | cell | auto\n"
                  << "  stats_mode    : ser  | par  | auto\n";
        return 1;
    }

    CenteringTag centering = CenteringTag::FaceCentered;
    if      (modo_centering == "face") centering = CenteringTag::FaceCentered;
    else if (modo_centering == "cell") centering = CenteringTag::CellCentered;
    else if (modo_centering != "auto") {
        std::cerr << "Modo de centering desconhecido: " << modo_centering
                  << " (use: face | cell | auto)\n";
        return 1;
    }

    Grid1D grid = Grid1DBuilder{}
                    .setN(N)
                    .setDomain(A, B)
                    .setDistribution(DistributionTag::Uniform1D)
                    .setCentering(centering)
                    .build();

    auto xf = grid.faces();          // N+1
    auto xc = grid.centers();        // N
    auto dF = grid.deltasFaces();    // N
    auto dC = grid.deltasCenters();  // N+1

    std::cout << "[Malha 1D Uniforme]  "
              << (G1U::has_parallel()
                  ? "(binário com estatística paralela disponível)\n"
                  : "(binário SERIAL — estatística paralela não compilada)\n");

    std::cout << "N=" << N
              << "  A=" << A
              << "  B=" << B
              << "  modo_centering="
              << (centering == CenteringTag::FaceCentered ? "face" : "cell")
              << "  stats_mode=" << stats_mode << "\n";

    mostra_estatisticas(grid, stats_mode);
    print_malha(xf, xc, dF, dC);

    return 0;
}


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
    std::vector<std::string> linhas;
    linhas.resize(ids.size());

    std::transform(ids.begin(), ids.end(), linhas.begin(),
                   [&](std::size_t i) {
                       const std::string c0 = fmt(xf[i]);
                       const std::string c1 = (i < static_cast<std::size_t>(N)) ? fmt(xc[i]) : blank;
                       const std::string c2 = (i < static_cast<std::size_t>(N)) ? fmt(dF[i]) : blank;
                       const std::string c3 = fmt(dC[i]);
                       return c0 + c1 + c2 + c3;
                   });

    // Imprime todas as linhas com ostream_iterator (sem laço explícito)
    std::copy(linhas.begin(), linhas.end(),
              std::ostream_iterator<std::string>(std::cout, "\n"));
}

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
