// ----------------------------------------------------------------------------
// File: RegisterUniform1D.cpp
// Author: FVMGridMaker Team
// Description: Registro do padrão Uniform1D exclusivo para este executável.
//              Não altera o core. Injeta o gerador no registry antes do main().
// ----------------------------------------------------------------------------

#include <any>
#include <vector>
#include <iostream>

#include <FVMGridMaker/Core/type.h>
#include <FVMGridMaker/Grid/Common/Tags1D.hpp>
#include <FVMGridMaker/Grid/Grid1D/Builders/Grid1DDistributionRegistry.hpp>

// Não incluímos nenhum header de "Uniform1D.hpp" aqui para evitar acoplamento.
// O gerador de malha uniforme é implementado localmente neste TU.

using FVMGridMaker::core::Index;
using FVMGridMaker::core::Real;
using FVMGridMaker::grid::DistributionTag;
using FVMGridMaker::grid::grid1d::builders::Grid1DDistributionRegistry;

#if defined(__GNUC__) || defined(__clang__)
#  pragma message("[FVMGridMaker][build] Compilando RegisterUniform1D.cpp para este alvo.")
#endif

// Defina FVMG_EX_VERBOSE_REG=1 no alvo para logar o registro em runtime.
#ifndef FVMG_EX_VERBOSE_REG
#  define FVMG_EX_VERBOSE_REG 1
#endif

namespace {

// --- Geradores locais (sem depender de Uniform1D.hpp) -----------------------
static std::vector<Real> make_uniform_faces(Index n, Real A, Real B) {
    std::vector<Real> xf;
    xf.reserve(static_cast<std::size_t>(n) + 1u);

    const Real L  = B - A;
    const Real dx = (n > 0) ? (L / static_cast<Real>(n)) : Real(0);

    for (Index i = 0; i <= n; ++i) {
        xf.push_back(A + static_cast<Real>(i) * dx);
    }
    return xf;
}

static std::vector<Real> make_uniform_centers(Index n, Real A, Real B) {
    std::vector<Real> xc;
    xc.reserve(static_cast<std::size_t>(n));

    const Real L  = B - A;
    const Real dx = (n > 0) ? (L / static_cast<Real>(n)) : Real(0);

    for (Index i = 0; i < n; ++i) {
        xc.push_back(A + (static_cast<Real>(i) + Real(0.5)) * dx);
    }
    return xc;
}

// --- Registrador estático ----------------------------------------------------
struct Uniform1D_Registrar {
    Uniform1D_Registrar() {
#if FVMG_EX_VERBOSE_REG
        std::clog << "[FVMGridMaker][runtime] RegisterUniform1D.cpp ativo: registrando Uniform1D...\n";
#endif
        Grid1DDistributionRegistry::Entry e{};

        // Assinaturas devem bater com Grid1DDistributionRegistry::GenFn:
        // std::vector<Real>(Index, Real, Real, const std::any*)
        e.faces_fn = [](Index n, Real A, Real B, const std::any* /*unused*/) {
            return make_uniform_faces(n, A, B);
        };
        e.centers_fn = [](Index n, Real A, Real B, const std::any* /*unused*/) {
            return make_uniform_centers(n, A, B);
        };

        auto& reg = Grid1DDistributionRegistry::instance();
        reg.registerDistribution("Uniform1D", std::move(e), DistributionTag::Uniform1D);
    }
};

// Instância global: garante o registro antes de main()
static const Uniform1D_Registrar FVMG_UNIFORM1D_PLUGIN_;

} // namespace
