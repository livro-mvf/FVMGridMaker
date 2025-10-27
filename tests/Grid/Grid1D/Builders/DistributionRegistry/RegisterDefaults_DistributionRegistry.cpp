// ----------------------------------------------------------------------------
// File: RegisterDefaults_DistributionRegistry.cpp
// Author: FVMGridMaker Team
// Description: Registro de padrões (Uniform1D e Random1D) exclusivo do alvo
//              de teste ut_DistributionRegistry. Não altera o core.
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

// Mensagem de compilação para confirmar que este TU entrou no alvo
#if defined(__GNUC__) || defined(__clang__)
#  pragma message("[FVMGridMaker][build] Compilando RegisterDefaults_DistributionRegistry.cpp para ut_DistributionRegistry.")
#endif

// Função “isca” para evitar eliminação do TU pelo linker se for parar numa lib.
extern "C" void FVMGM_force_link_registry_defaults_test_plugin() {}

// ----------------------------------------------------------------------------
// Lambdas mínimas (corretas) para os geradores de Uniform1D
// ----------------------------------------------------------------------------
static std::vector<Real> uniform_faces(Index n, Real A, Real B) {
    if (n == 0) return {};
    const Real dx = (B - A) / static_cast<Real>(n);
    std::vector<Real> xf(n + 1);
    for (Index i = 0; i <= n; ++i) xf[i] = A + static_cast<Real>(i) * dx;
    return xf;
}

static std::vector<Real> uniform_centers(Index n, Real A, Real B) {
    if (n == 0) return {};
    const Real dx = (B - A) / static_cast<Real>(n);
    std::vector<Real> xc(n);
    for (Index i = 0; i < n; ++i) xc[i] = A + (static_cast<Real>(i) + Real(0.5)) * dx;
    return xc;
}

// ----------------------------------------------------------------------------
// Registro (executado uma única vez por processo)
// ----------------------------------------------------------------------------
static void register_defaults_for_distribution_registry_tests_once() {
    static bool done = false;
    if (done) return;

    std::fprintf(stderr,
        "[FVMGridMaker][runtime] RegisterDefaults_DistributionRegistry.cpp ativo: "
        "registrando Uniform1D e Random1D para ut_DistributionRegistry...\n");

    auto& reg = Grid1DDistributionRegistry::instance();

    // Uniform1D
    {
        Grid1DDistributionRegistry::Entry e{};
        e.faces_fn   = [](Index n, Real A, Real B, const std::any*) { return uniform_faces(n, A, B); };
        e.centers_fn = [](Index n, Real A, Real B, const std::any*) { return uniform_centers(n, A, B); };
        reg.registerDistribution("Uniform1D", std::move(e), DistributionTag::Uniform1D);
    }

    // Random1D (para este teste, só precisamos garantir a presença no registry;
    // as funções não são chamadas neste suite específico, então um stub simples basta)
    {
        Grid1DDistributionRegistry::Entry e{};
        e.faces_fn   = [](Index n, Real A, Real B, const std::any*) { return uniform_faces(n, A, B); };
        e.centers_fn = [](Index n, Real A, Real B, const std::any*) { return uniform_centers(n, A, B); };
        reg.registerDistribution("Random1D", std::move(e), DistributionTag::Random1D);
    }

    done = true;
}

// ----------------------------------------------------------------------------
// Ambiente global do GTest: garante registro antes dos testes
// ----------------------------------------------------------------------------
struct RegistryDefaultsEnv : ::testing::Environment {
    void SetUp() override { register_defaults_for_distribution_registry_tests_once(); }
};

::testing::Environment* const kRegistryDefaultsEnv =
    ::testing::AddGlobalTestEnvironment(new RegistryDefaultsEnv{});
