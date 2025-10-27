// ----------------------------------------------------------------------------
// File: RegisterRandom1D.cpp
// Author: FVMGridMaker Team
// Description: Registro do padrão Random1D exclusivo para o executável
//              de exemplo. Não altera o core. Injeta geradores no registry.
// ----------------------------------------------------------------------------

#include <any>
#include <cstdio>
#include <vector>

#include <FVMGridMaker/Core/type.h>
#include <FVMGridMaker/Grid/Common/Tags1D.hpp>
#include <FVMGridMaker/Grid/Grid1D/Builders/Grid1DDistributionRegistry.hpp>
#include <FVMGridMaker/Grid/Grid1D/Patterns/Distribution/Random1D.hpp>

using FVMGridMaker::core::Index;
using FVMGridMaker::core::Real;
using FVMGridMaker::grid::DistributionTag;
using FVMGridMaker::grid::grid1d::builders::Grid1DDistributionRegistry;
namespace dist = FVMGridMaker::grid::grid1d::patterns::distribution;

#if defined(__GNUC__) || defined(__clang__)
#  pragma message("[FVMGridMaker][build] Compilando RegisterRandom1D.cpp para este alvo.")
#endif

namespace {

// Registrador estático: injeta “Random1D” quando este TU é linkado.
struct Random1D_Registrar {
    Random1D_Registrar() {
        auto& reg = Grid1DDistributionRegistry::instance();

        Grid1DDistributionRegistry::Entry e{};
        // Assinatura exigida: std::vector<Real>(Index, Real, Real, const std::any*)
        e.faces_fn = [](Index n, Real A, Real B, const std::any* any_opt) {
            return dist::Random1D::faces(n, A, B, any_opt);
        };
        e.centers_fn = [](Index n, Real A, Real B, const std::any* any_opt) {
            return dist::Random1D::centers(n, A, B, any_opt);
        };

        reg.registerDistribution("Random1D", std::move(e), DistributionTag::Random1D);

        // Log em runtime para facilitar diagnóstico
        std::fprintf(stderr,
            "[FVMGridMaker][runtime] RegisterRandom1D.cpp ativo: registrando Random1D...\n");
    }
};

// Instância global: garante execução do construtor antes do main().
static Random1D_Registrar g_random_registrar;

// Isca opcional (útil se este TU for parar dentro de uma lib)
extern "C" void FVMGM_force_link_random1d_example_plugin() {}

} // namespace
