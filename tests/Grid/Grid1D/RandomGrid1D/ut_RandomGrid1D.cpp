// tests/Grid/Grid1D/RandomGrid1D/ut_RandomGrid1D.cpp
#include <gtest/gtest.h>
#include <numeric>
#include <vector>

#include <FVMGridMaker/Core/type.h>
#include <FVMGridMaker/Grid/Common/Tags1D.hpp>
#include <FVMGridMaker/Grid/Grid1D/API/Grid1D.h>
#include <FVMGridMaker/Grid/Grid1D/Builders/Grid1DBuilder.hpp>
#include <FVMGridMaker/Grid/Grid1D/Patterns/Distribution/Random1D.hpp>

using Real  = FVMGridMaker::core::Real;
using Index = FVMGridMaker::core::Index;
using FVMGridMaker::grid::DistributionTag;
using FVMGridMaker::grid::CenteringTag;
using Grid1D = FVMGridMaker::grid::grid1d::api::Grid1D;
using Random1D = FVMGridMaker::grid::grid1d::patterns::distribution::Random1D;

static std::vector<Real> as_vec(auto range) { return {range.begin(), range.end()}; }

TEST(RandomGrid1D, DeterminismWithSeed) {
    constexpr Index N = 200;
    constexpr Real  A = 0.0;
    constexpr Real  B = 1.0;
    Random1D::Options opt{};
    opt.w_lo = 0.7; opt.w_hi = 1.3; opt.seed = 123456789u;

    auto g1 = FVMGridMaker::grid::grid1d::builders::Grid1DBuilder{}
                .setN(N).setDomain(A,B)
                .setDistribution(DistributionTag::Random1D)
                .setCentering(CenteringTag::FaceCentered)
                .setOption(opt).build();

    auto g2 = FVMGridMaker::grid::grid1d::builders::Grid1DBuilder{}
                .setN(N).setDomain(A,B)
                .setDistribution(DistributionTag::Random1D)
                .setCentering(CenteringTag::FaceCentered)
                .setOption(opt).build();

    EXPECT_EQ(as_vec(g1.faces()),   as_vec(g2.faces()));
    EXPECT_EQ(as_vec(g1.centers()), as_vec(g2.centers()));
    EXPECT_EQ(as_vec(g1.deltasFaces()), as_vec(g2.deltasFaces()));
    EXPECT_EQ(as_vec(g1.deltasCenters()), as_vec(g2.deltasCenters()));
}

TEST(RandomGrid1D, WidthsWithinBounds) {
    constexpr Index N = 500;
    constexpr Real  A = 0.0;
    constexpr Real  B = 2.0;
    constexpr Real  wlo = 0.6, whi = 1.4;
    Random1D::Options opt{};
    opt.w_lo = wlo; opt.w_hi = whi; opt.seed = 987654321u;

    auto g = FVMGridMaker::grid::grid1d::builders::Grid1DBuilder{}
                .setN(N).setDomain(A,B)
                .setDistribution(DistributionTag::Random1D)
                .setCentering(CenteringTag::CellCentered)
                .setOption(opt).build();

    const Real dx_ref = (B-A)/static_cast<Real>(N);
    const Real lo = wlo*dx_ref, hi = whi*dx_ref;
    for (Real d : g.deltasFaces()) {
        EXPECT_GE(d, lo * Real(0.999999));  // pequena folga
        EXPECT_LE(d, hi * Real(1.000001));
    }

    // somatório deve bater com B-A
    const Real sum_dF = std::accumulate(g.deltasFaces().begin(), g.deltasFaces().end(), Real(0));
    EXPECT_NEAR(sum_dF, B-A, Real(1e-9));
}
