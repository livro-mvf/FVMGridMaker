// ----------------------------------------------------------------------------
// File: RegisterRandom1D.cpp
// Author: FVMGridMaker Team
// Description: Registro do padrão Random1D exclusivo para o binário de testes.
//              Não altera o core. Injeta o gerador no registry antes dos testes.
// ----------------------------------------------------------------------------

#include <gtest/gtest.h>
#include <any>
#include <cstdio> // fprintf

#include <FVMGridMaker/Core/type.h>
#include <FVMGridMaker/Grid/Common/Tags1D.hpp>
#include <FVMGridMaker/Grid/Grid1D/Builders/Grid1DDistributionRegistry.hpp>
#include <FVMGridMaker/Grid/Grid1D/Patterns/Distribution/Random1D.hpp>

// -----------------------------------------------------------------------------
// Banner de COMPILAÇÃO: aparece no output do build quando este TU é compilado.
// (não gera warning)
// -----------------------------------------------------------------------------
#ifndef FVMGM_SILENT_TU_BANNERS
#  if defined(__clang__) || defined(__GNUC__)
#    pragma message ("[FVMGridMaker][build] Compilando RegisterRandom1D.cpp para este alvo.")
#  endif
#endif
// Se preferir forçar como *warning* (chama mais atenção), troque por:
//
// #if defined(__GNUC__) || defined(__clang__)
// #  warning [FVMGridMaker] Compilando RegisterRandom1D.cpp para este alvo
// #endif

using FVMGridMaker::core::Index;
using FVMGridMaker::core::Real;
using FVMGridMaker::grid::DistributionTag;
using FVMGridMaker::grid::grid1d::builders::Grid1DDistributionRegistry;
namespace dist = FVMGridMaker::grid::grid1d::patterns::distribution;

// Função “isca” para forçar o linker a manter este TU se ele for parar numa lib.
extern "C" void FVMGM_force_link_random1d_test_plugin() {}

static void register_random1d_once() {
    static bool done = false;
    if (done) return;

    Grid1DDistributionRegistry::Entry e{};
    e.faces_fn = [](Index n, Real A, Real B, const std::any* any_opt) {
        return dist::Random1D::faces(n, A, B, any_opt);
    };
    e.centers_fn = [](Index n, Real A, Real B, const std::any* any_opt) {
        return dist::Random1D::centers(n, A, B, any_opt);
    };

    auto& reg = Grid1DDistributionRegistry::instance();
    reg.registerDistribution("Random1D", std::move(e), DistributionTag::Random1D);

    done = true;
}

// Ambiente global do GTest que faz o registro antes dos testes.
// Também imprime um banner em RUNTIME para confirmação visual.
struct Random1DRegisterEnv : ::testing::Environment {
    void SetUp() override {
        std::fprintf(stderr,
            "[FVMGridMaker][runtime] RegisterRandom1D.cpp ativo: registrando Random1D...\n");
        register_random1d_once();
    }
};

// A simples existência desse objeto garante que SetUp() roda antes dos testes.
::testing::Environment* const kRegEnv =
    ::testing::AddGlobalTestEnvironment(new Random1DRegisterEnv{});
