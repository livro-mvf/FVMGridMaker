// ----------------------------------------------------------------------------
// File: ut_UniformGrid1D.cpp
// Author: FVMGridMaker Team
// Version: 2.5
// Date: 2025-10-26
// Description: Testes de unidade para Grid1D (Uniform × Face/Cell Centered)
//              usando gtest + gmock (matchers com tolerância).
// License: GNU GPL v3
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// includes FVMGridMaker
// ----------------------------------------------------------------------------
#include <FVMGridMaker/ErrorHandling/FVMGException.h>
#include <FVMGridMaker/Grid/Grid1D/API/Grid1D.h>
#include <FVMGridMaker/Grid/Grid1D/Builders/Grid1DBuilder.hpp>

// ----------------------------------------------------------------------------
// includes C++
// ----------------------------------------------------------------------------
#include <algorithm>
#include <numeric>
#include <type_traits>
#include <vector>

// gtest/gmock (silencia -Wsign-conversion apenas nesses headers)
#if defined(__GNUC__) || defined(__clang__)
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wsign-conversion"
#endif
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#if defined(__GNUC__) || defined(__clang__)
  #pragma GCC diagnostic pop
#endif

// ----------------------------------------------------------------------------
// aliases úteis
// ----------------------------------------------------------------------------
using Real  = FVMGridMaker::core::Real;
using Index = FVMGridMaker::core::Index;
using FVMGridMaker::grid::DistributionTag;
using FVMGridMaker::grid::CenteringTag;
using Grid1D = FVMGridMaker::grid::grid1d::api::Grid1D;
using FVMGridMaker::grid::grid1d::builders::Grid1DBuilder;

using ::testing::Pointwise;

// sanity de tipo (evita surpresas com tolerâncias)
static_assert(std::is_same_v<Real, double>, "Real deve ser double");

// -----------------------------------------------------------------------------
// Matcher gmock: compara elemento a elemento com tolerância absoluta
// -----------------------------------------------------------------------------
MATCHER_P(WithinAbs, eps, "diferença absoluta <= eps") {
    using std::abs;
    return abs(std::get<0>(arg) - std::get<1>(arg)) <= eps;
}

// -----------------------------------------------------------------------------
// constantes de teste
// -----------------------------------------------------------------------------
namespace {
    constexpr Index NVol   = 10;
    constexpr Real  A      = Real(0);
    constexpr Real  Length = Real(1);
    constexpr Real  B      = A + Length;
    constexpr Real  eps    = Real(1e-12);
}

// -----------------------------------------------------------------------------
// helpers para construir sequências esperadas
// -----------------------------------------------------------------------------
static std::vector<Real> make_uniform_faces(Index N, Real a, Real b) {
    std::vector<Real> xf;
    xf.resize(static_cast<std::size_t>(N + 1));
    const Real dx = (b - a) / static_cast<Real>(N);
    for (Index i = 0; i <= N; ++i) {
        xf[static_cast<std::size_t>(i)] = a + static_cast<Real>(i) * dx;
    }
    return xf;
}

static std::vector<Real> make_uniform_centers(Index N, Real a, Real b) {
    std::vector<Real> xc;
    xc.resize(static_cast<std::size_t>(N));
    const Real dx = (b - a) / static_cast<Real>(N);
    for (Index i = 0; i < N; ++i) {
        xc[static_cast<std::size_t>(i)] = a + (static_cast<Real>(i) + Real(0.5)) * dx;
    }
    return xc;
}

static std::vector<Real> make_uniform_dF(Index N, Real a, Real b) {
    std::vector<Real> dF;
    dF.resize(static_cast<std::size_t>(N));
    const Real dx = (b - a) / static_cast<Real>(N);
    std::fill(dF.begin(), dF.end(), dx);
    return dF;
}

// Definição do projeto para dC (N+1):
//   dC[0]   = xCentro[0]     - xFace[0]
//   dC[i]   = xCentro[i]     - xCentro[i-1],  i = 1..N-1
//   dC[N]   = xFace[N]       - xCentro[N-1]
static std::vector<Real> make_uniform_dC(Index N, Real a, Real b) {
    std::vector<Real> dC;
    dC.resize(static_cast<std::size_t>(N + 1));
    const Real dx = (b - a) / static_cast<Real>(N);
    dC.front() = Real(0.5) * dx;
    for (Index i = 1; i < N; ++i) {
        dC[static_cast<std::size_t>(i)] = dx;
    }
    dC.back() = Real(0.5) * dx;
    return dC;
}

// -----------------------------------------------------------------------------
// CENÁRIO 1: Uniform + FaceCentered (gera faces → centers/deltas)
// -----------------------------------------------------------------------------
TEST(Grid1D_Uniform, FaceCentered) {
    auto grid = Grid1DBuilder{}
        .setN(NVol)
        .setDomain(A, B)
        .setDistribution(DistributionTag::Uniform1D)
        .setCentering(CenteringTag::FaceCentered)
        .build();

    const auto xf_exp = make_uniform_faces(NVol, A, B);
    const auto xc_exp = make_uniform_centers(NVol, A, B);
    const auto dF_exp = make_uniform_dF(NVol, A, B);
    const auto dC_exp = make_uniform_dC(NVol, A, B);

    const auto xf = grid.faces();         std::vector<Real> xf_act(xf.begin(), xf.end());
    const auto xc = grid.centers();       std::vector<Real> xc_act(xc.begin(), xc.end());
    const auto dF = grid.deltasFaces();   std::vector<Real> dF_act(dF.begin(), dF.end());
    const auto dC = grid.deltasCenters(); std::vector<Real> dC_act(dC.begin(), dC.end());

    EXPECT_THAT(xf_act, Pointwise(WithinAbs(eps), xf_exp));
    EXPECT_THAT(xc_act, Pointwise(WithinAbs(eps), xc_exp));
    EXPECT_THAT(dF_act, Pointwise(WithinAbs(eps), dF_exp));
    EXPECT_THAT(dC_act, Pointwise(WithinAbs(eps), dC_exp));

    const Real sum_dF = std::accumulate(dF_act.begin(), dF_act.end(), Real(0));
    EXPECT_NEAR(sum_dF, B - A, eps);
    const Real sum_dC = std::accumulate(dC_act.begin(), dC_act.end(), Real(0));
    EXPECT_NEAR(sum_dC, B - A, eps);

    EXPECT_EQ(grid.nVolumes(), NVol);
    EXPECT_EQ(grid.nFaces(), NVol + 1);
    EXPECT_NEAR(grid.face(0), A, eps);
    EXPECT_NEAR(grid.face(NVol), B, eps);
}

// -----------------------------------------------------------------------------
// CENÁRIO 2: Uniform + CellCentered (gera centers → faces/deltas)
// -----------------------------------------------------------------------------
TEST(Grid1D_Uniform, CellCentered) {
    auto grid = Grid1DBuilder{}
        .setN(NVol)
        .setDomain(A, B)
        .setDistribution(DistributionTag::Uniform1D)
        .setCentering(CenteringTag::CellCentered)
        .build();

    const auto xf_exp = make_uniform_faces(NVol, A, B);
    const auto xc_exp = make_uniform_centers(NVol, A, B);
    const auto dF_exp = make_uniform_dF(NVol, A, B);
    const auto dC_exp = make_uniform_dC(NVol, A, B);

    const auto xf = grid.faces();         std::vector<Real> xf_act(xf.begin(), xf.end());
    const auto xc = grid.centers();       std::vector<Real> xc_act(xc.begin(), xc.end());
    const auto dF = grid.deltasFaces();   std::vector<Real> dF_act(dF.begin(), dF.end());
    const auto dC = grid.deltasCenters(); std::vector<Real> dC_act(dC.begin(), dC.end());

    EXPECT_THAT(xf_act, Pointwise(WithinAbs(eps), xf_exp));
    EXPECT_THAT(xc_act, Pointwise(WithinAbs(eps), xc_exp));
    EXPECT_THAT(dF_act, Pointwise(WithinAbs(eps), dF_exp));
    EXPECT_THAT(dC_act, Pointwise(WithinAbs(eps), dC_exp));

    const Real sum_dF = std::accumulate(dF_act.begin(), dF_act.end(), Real(0));
    EXPECT_NEAR(sum_dF, B - A, eps);
    const Real sum_dC = std::accumulate(dC_act.begin(), dC_act.end(), Real(0));
    EXPECT_NEAR(sum_dC, B - A, eps);

    EXPECT_EQ(grid.nVolumes(), NVol);
    EXPECT_EQ(grid.nFaces(), NVol + 1);
    EXPECT_NEAR(grid.face(0), A, eps);
    EXPECT_NEAR(grid.face(NVol), B, eps);
}

// -----------------------------------------------------------------------------
// CENÁRIO 3: Falhas de configuração (exceções via Builder)
// -----------------------------------------------------------------------------
TEST(Grid1D_Uniform, InvalidConfig_Throws) {
    // N == 0
    EXPECT_THROW(
        (void)Grid1DBuilder{}.setN(0)
                             .setDomain(A, B)
                             .setDistribution(DistributionTag::Uniform1D)
                             .setCentering(CenteringTag::FaceCentered)
                             .build(),
        FVMGridMaker::error::FVMGException
    );

    // A >= B
    EXPECT_THROW(
        (void)Grid1DBuilder{}.setN(10)
                             .setDomain(1.0, 0.0)
                             .setDistribution(DistributionTag::Uniform1D)
                             .setCentering(CenteringTag::FaceCentered)
                             .build(),
        FVMGridMaker::error::FVMGException
    );
}

// -----------------------------------------------------------------------------
// CENÁRIO 4: Bordas de CellCentered (N=1 e N=2)
// -----------------------------------------------------------------------------
TEST(Grid1D_Uniform, CellCentered_N1_and_N2) {
    constexpr Real AA = Real(0.0), BB = Real(1.0), e = Real(1e-12);

    // N = 1
    {
        auto g1 = Grid1DBuilder{}.setN(1).setDomain(AA, BB)
                   .setDistribution(DistributionTag::Uniform1D)
                   .setCentering(CenteringTag::CellCentered).build();

        std::vector<Real> xf(g1.faces().begin(), g1.faces().end());
        std::vector<Real> xc(g1.centers().begin(), g1.centers().end());
        std::vector<Real> dF(g1.deltasFaces().begin(), g1.deltasFaces().end());
        std::vector<Real> dC(g1.deltasCenters().begin(), g1.deltasCenters().end());

        EXPECT_NEAR(xf.front(), AA, e);
        EXPECT_NEAR(xf.back(),  BB, e);
        EXPECT_NEAR(xc[0], 0.5*(AA+BB), e);
        EXPECT_NEAR(dF[0], BB-AA, e);
        EXPECT_NEAR(dC.front(), 0.5*(BB-AA), e);
        EXPECT_NEAR(dC.back(),  0.5*(BB-AA), e);
        EXPECT_NEAR(std::accumulate(dF.begin(), dF.end(), Real(0)), BB-AA, e);
        EXPECT_NEAR(std::accumulate(dC.begin(), dC.end(), Real(0)), BB-AA, e);
    }

    // N = 2
    {
        auto g2 = Grid1DBuilder{}.setN(2).setDomain(AA, BB)
                   .setDistribution(DistributionTag::Uniform1D)
                   .setCentering(CenteringTag::CellCentered).build();

        std::vector<Real> xf(g2.faces().begin(), g2.faces().end());
        std::vector<Real> xc(g2.centers().begin(), g2.centers().end());
        std::vector<Real> dF(g2.deltasFaces().begin(), g2.deltasFaces().end());
        std::vector<Real> dC(g2.deltasCenters().begin(), g2.deltasCenters().end());

        const Real dx = (BB-AA)/Real(2.0);
        EXPECT_NEAR(xf[0], AA, e);
        EXPECT_NEAR(xf[1], AA+dx, e);
        EXPECT_NEAR(xf[2], BB, e);
        EXPECT_NEAR(xc[0], AA+Real(0.5)*dx, e);
        EXPECT_NEAR(xc[1], AA+Real(1.5)*dx, e);
        EXPECT_THAT(dF, ::testing::Each(::testing::DoubleNear(dx, e)));
        EXPECT_NEAR(std::accumulate(dF.begin(), dF.end(), Real(0)), BB-AA, e);
        EXPECT_NEAR(std::accumulate(dC.begin(), dC.end(), Real(0)), BB-AA, e);
    }
}

// -----------------------------------------------------------------------------
// CENÁRIO 5: Consistência interna (faces vs. centros)
// -----------------------------------------------------------------------------
TEST(Grid1D_Uniform, InternalConsistency_FaceCenterRelation) {
    constexpr Index N = 10;
    constexpr Real A0 = Real(0.0), B0 = Real(1.0), e = Real(1e-12);

    auto g = Grid1DBuilder{}.setN(N).setDomain(A0, B0)
              .setDistribution(DistributionTag::Uniform1D)
              .setCentering(CenteringTag::CellCentered).build();

    const auto xf = g.faces();
    const auto xc = g.centers();

    for (Index i = 1; i < N; ++i) {
        EXPECT_NEAR(xf[static_cast<std::size_t>(i)],
                    Real(0.5) * (xc[static_cast<std::size_t>(i-1)] +
                                 xc[static_cast<std::size_t>(i)]),
                    e);
    }
    EXPECT_NEAR(std::accumulate(g.deltasFaces().begin(), g.deltasFaces().end(), Real(0)), B0-A0, e);
    EXPECT_NEAR(std::accumulate(g.deltasCenters().begin(), g.deltasCenters().end(), Real(0)), B0-A0, e);
}

// -----------------------------------------------------------------------------
// CENÁRIO 6: Fórmulas de borda para dxCentro (ambos os centerings)
//            dxC[0] = xC[0] - xF[0]
//            dxC[N] = xF[N] - xC[N-1]
//            (e, para uniforme, ambos = 0.5*dx)
// -----------------------------------------------------------------------------
TEST(Grid1D_Uniform, DxCentro_BorderFormulas_BothCenterings) {
    auto check = [&](CenteringTag centering) {
        auto g = Grid1DBuilder{}
                    .setN(NVol)
                    .setDomain(A, B)
                    .setDistribution(DistributionTag::Uniform1D)
                    .setCentering(centering)
                    .build();

        std::vector<Real> xf(g.faces().begin(), g.faces().end());
        std::vector<Real> xc(g.centers().begin(), g.centers().end());
        std::vector<Real> dC(g.deltasCenters().begin(), g.deltasCenters().end());

        ASSERT_EQ(xf.size(), static_cast<std::size_t>(NVol + 1));
        ASSERT_EQ(xc.size(), static_cast<std::size_t>(NVol));
        ASSERT_EQ(dC.size(), static_cast<std::size_t>(NVol + 1));

        EXPECT_NEAR(dC.front(), xc.front() - xf.front(), eps);
        EXPECT_NEAR(dC.back(),  xf.back()  - xc.back(),  eps);

        const Real dx = (B - A) / static_cast<Real>(NVol);
        EXPECT_NEAR(dC.front(), Real(0.5) * dx, eps);
        EXPECT_NEAR(dC.back(),  Real(0.5) * dx, eps);
    };

    check(CenteringTag::FaceCentered);
    check(CenteringTag::CellCentered);
}
