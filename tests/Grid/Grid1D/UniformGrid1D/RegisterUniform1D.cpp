// ----------------------------------------------------------------------------
// File: RegisterUniform1D.cpp
// Author: FVMGridMaker Team
// Description: Registro do padrão Uniform1D exclusivo para o binário de testes.
//              Não altera o core. Injeta o gerador no registry antes dos testes.
// ----------------------------------------------------------------------------

#include <gtest/gtest.h>
#include <any>
#include <cstdio>   // std::fprintf, stderr
#include <vector>

#include <FVMGridMaker/Core/type.h>
#include <FVMGridMaker/Grid/Common/Tags1D.hpp>
#include <FVMGridMaker/Grid/Grid1D/Builders/Grid1DDistributionRegistry.hpp>

using FVMGridMaker::core::Index;
using FVMGridMaker::core::Real;
using FVMGridMaker::grid::DistributionTag;
using FVMGridMaker::grid::grid1d::builders::Grid1DDistributionRegistry;

// Força mensagem de compilação em TU (ajuda a diagnosticar se o arquivo entrou no alvo)
#if defined(__GNUC__) || defined(__clang__)
#  pragma message("[FVMGridMaker][build] Compilando RegisterUniform1D.cpp para este alvo.")
#endif

// Função “isca” para forçar o linker a manter este TU se ele for parar numa lib.
extern "C" void FVMGM_force_link_uniform1d_test_plugin() {}

// ----------------------------------------------------------------------------
// Registro (executado uma única vez por processo)
// ----------------------------------------------------------------------------
static void register_uniform1d_once() {
    static bool done = false;
    if (done) return;

    // Mensagem de runtime para confirmar que o registrador rodou
    std::fprintf(stderr, "[FVMGridMaker][runtime] RegisterUniform1D.cpp ativo: registrando Uniform1D...\n");

    Grid1DDistributionRegistry::Entry e{};

    // Gerador de faces: n+1 pontos uniformemente espaçados entre [A, B]
    e.faces_fn = [](Index n, Real A, Real B, const std::any* /*any_opt*/) -> std::vector<Real> {
        if (n == 0) return {}; // builder geralmente valida antes; aqui devolvemos vazio.
        const Real dx = (B - A) / static_cast<Real>(n);
        std::vector<Real> xf(n + 1);
        for (Index i = 0; i <= n; ++i) {
            xf[i] = A + static_cast<Real>(i) * dx;
        }
        return xf;
    };

    // Gerador de centros: n pontos em (A + (i+0.5) * dx)
    e.centers_fn = [](Index n, Real A, Real B, const std::any* /*any_opt*/) -> std::vector<Real> {
        if (n == 0) return {};
        const Real dx = (B - A) / static_cast<Real>(n);
        std::vector<Real> xc(n);
        for (Index i = 0; i < n; ++i) {
            xc[i] = A + (static_cast<Real>(i) + Real(0.5)) * dx;
        }
        return xc;
    };

    auto& reg = Grid1DDistributionRegistry::instance();
    reg.registerDistribution("Uniform1D", std::move(e), DistributionTag::Uniform1D);

    done = true;
}

// ----------------------------------------------------------------------------
// Ambiente global do GTest: garante registro antes dos testes
// ----------------------------------------------------------------------------
struct Uniform1DRegisterEnv : ::testing::Environment {
    void SetUp() override { register_uniform1d_once(); }
};

::testing::Environment* const kUniformRegEnv =
    ::testing::AddGlobalTestEnvironment(new Uniform1DRegisterEnv{});
