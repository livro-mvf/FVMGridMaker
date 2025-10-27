// ----------------------------------------------------------------------------
// File: ut_DistributionRegistry.cpp
// Author: FVMGridMaker Team
// Version: 1.1
// Date: 2025-10-26
// Description: Testes de unidade do Grid1DDistributionRegistry.
//              Verifica presença de padrões e registro de distribuições
//              definidas pelo usuário (faces/centers).
// License: GNU GPL v3
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// includes FVMGridMaker
// ----------------------------------------------------------------------------
#include <FVMGridMaker/Core/type.h>
#include <FVMGridMaker/Grid/Common/Tags1D.hpp>
#include <FVMGridMaker/Grid/Grid1D/Builders/Grid1DDistributionRegistry.hpp>

// ----------------------------------------------------------------------------
// includes C++
// ----------------------------------------------------------------------------
#include <algorithm>
#include <any>
#include <cmath>
#include <numeric>
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

using FVMGridMaker::core::Index;
using FVMGridMaker::core::Real;
using FVMGridMaker::grid::DistributionTag;
using FVMGridMaker::grid::grid1d::builders::Grid1DDistributionRegistry;

// ----------------------------------------------------------------------------
// Helpers de verificação (comparação com tolerância)
// ----------------------------------------------------------------------------
static ::testing::AssertionResult near_vec(const std::vector<Real>& a,
                                           const std::vector<Real>& b,
                                           Real eps)
{
    if (a.size() != b.size()) {
        return ::testing::AssertionFailure()
            << "size mismatch: " << a.size() << " vs " << b.size();
    }
    for (std::size_t i = 0; i < a.size(); ++i) {
        if (std::abs(a[i] - b[i]) > eps) {
            return ::testing::AssertionFailure()
                << "at i=" << i << ": |" << a[i] << " - " << b[i]
                << "| = " << std::abs(a[i]-b[i]) << " > " << eps;
        }
    }
    return ::testing::AssertionSuccess();
}

// ----------------------------------------------------------------------------
// Construção de sequências de referência (uniformes) para os testes
// ----------------------------------------------------------------------------
static std::vector<Real> make_uniform_faces(Index n, Real A, Real B) {
    std::vector<Real> xf(static_cast<std::size_t>(n + 1));
    const Real dx = (B - A) / static_cast<Real>(n);
    for (Index i = 0; i <= n; ++i) {
        xf[static_cast<std::size_t>(i)] = A + static_cast<Real>(i) * dx;
    }
    return xf;
}

static std::vector<Real> make_uniform_centers(Index n, Real A, Real B) {
    std::vector<Real> xc(static_cast<std::size_t>(n));
    const Real dx = (B - A) / static_cast<Real>(n);
    for (Index i = 0; i < n; ++i) {
        xc[static_cast<std::size_t>(i)] =
            A + (static_cast<Real>(i) + Real(0.5)) * dx;
    }
    return xc;
}

// ----------------------------------------------------------------------------
// TESTES
// ----------------------------------------------------------------------------

TEST(DistributionRegistry, DefaultsPresent) {
    auto& reg = Grid1DDistributionRegistry::instance();

    // Os nomes abaixo vêm da lista padrão de Tags1D.hpp (X-Macro default).
    // O teste não assume geração exata, apenas a presença dos registros
    // e do mapeamento tag->nome.
    auto e_uniform  = reg.find("Uniform1D");
    auto e_random   = reg.find("Random1D");
    EXPECT_TRUE(e_uniform.has_value());
    EXPECT_TRUE(e_random.has_value());

    auto n_uniform  = reg.nameForTag(DistributionTag::Uniform1D);
    auto n_random   = reg.nameForTag(DistributionTag::Random1D);
    EXPECT_TRUE(n_uniform.has_value());
    EXPECT_TRUE(n_random.has_value());
}

TEST(DistributionRegistry, RegisterUserDefinedUniform) {
    constexpr Index n  = 8;
    constexpr Real  A  = Real(0.0);
    constexpr Real  B  = Real(1.0);
    constexpr Real  eps = Real(1e-12);

    auto& reg = Grid1DDistributionRegistry::instance();

    Grid1DDistributionRegistry::Entry e{};

    // faces_fn: malha uniforme (sem warnings de conversão)
    e.faces_fn = [](Index n_, Real A_, Real B_, const std::any*) {
        std::vector<Real> xf(static_cast<std::size_t>(n_ + 1));
        const Real dx = (B_ - A_) / static_cast<Real>(n_);
        for (Index i = 0; i <= n_; ++i) {
            xf[static_cast<std::size_t>(i)] = A_ + static_cast<Real>(i) * dx;
        }
        return xf;
    };

    // centers_fn: centros uniformes
    e.centers_fn = [](Index n_, Real A_, Real B_, const std::any*) {
        std::vector<Real> xc(static_cast<std::size_t>(n_));
        const Real dx = (B_ - A_) / static_cast<Real>(n_);
        for (Index i = 0; i < n_; ++i) {
            xc[static_cast<std::size_t>(i)] =
                A_ + (static_cast<Real>(i) + Real(0.5)) * dx;
        }
        return xc;
    };

    // registra com um nome específico do teste
    const std::string name = "UserUniform_TestDist";
    reg.registerDistribution(name, e);

    // recupera e valida
    auto got = reg.find(name);
    ASSERT_TRUE(got.has_value());

    auto xf = got->faces_fn(n, A, B, nullptr);
    auto xc = got->centers_fn(n, A, B, nullptr);

    auto xf_ref = make_uniform_faces(n, A, B);
    auto xc_ref = make_uniform_centers(n, A, B);

    EXPECT_TRUE(near_vec(xf, xf_ref, eps));
    EXPECT_TRUE(near_vec(xc, xc_ref, eps));

    // checagens rápidas adicionais
    const Real len_faces = std::accumulate(xf.begin(), xf.end(), Real(0));
    const Real len_cent  = std::accumulate(xc.begin(), xc.end(), Real(0));
    EXPECT_TRUE(std::isfinite(len_faces));
    EXPECT_TRUE(std::isfinite(len_cent));
}
