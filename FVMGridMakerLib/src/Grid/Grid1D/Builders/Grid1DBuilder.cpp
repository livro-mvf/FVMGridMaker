// ----------------------------------------------------------------------------
/* File: Grid1DBuilder.cpp
 * Author: FVMGridMaker Team
 * Version: 2.4
 * Date: 2025-10-26
 * Description: Implementação do Grid1DBuilder.
 *   - Obtém geradores via Grid1DDistributionRegistry (faces/centers)
 *   - Fecha a malha conforme o centering (Face/Cell)
 *   - Validações integram com ErrorHandling (FVMGException)
 * License: GNU GPL v3
 */
// ----------------------------------------------------------------------------

#include <FVMGridMaker/Grid/Grid1D/Builders/Grid1DBuilder.hpp>

// Registro de distribuições
#include <FVMGridMaker/Grid/Grid1D/Builders/Grid1DDistributionRegistry.hpp>

// API/Tags
#include <FVMGridMaker/Grid/Common/Tags1D.hpp>
#include <FVMGridMaker/Grid/Grid1D/API/Grid1D.h>

// *** Error handling (umbrella) ***
#include <FVMGridMaker/ErrorHandling/ErrorHandling.h>

// C++
#include <algorithm>
#include <any>
#include <iterator>
#include <numeric>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

FVMG_GRID1D_BUILDERS_OPEN

using core::Index;
using core::Real;
using grid::DistributionTag;
using grid::CenteringTag;
using api::Grid1D;

// ----------------------------------------------------------------------------
// Setters (fluent API)
// ----------------------------------------------------------------------------
Grid1DBuilder& Grid1DBuilder::setN(Index n) {
    this->n_ = n;
    return *this;
}

Grid1DBuilder& Grid1DBuilder::setDomain(Real a, Real b) {
    this->a_ = a;
    this->b_ = b;
    return *this;
}

Grid1DBuilder& Grid1DBuilder::setDistribution(DistributionTag tag) {
    this->dist_ = tag;
    return *this;
}

Grid1DBuilder& Grid1DBuilder::setCentering(CenteringTag centering) {
    this->cent_ = centering;
    return *this;
}

Grid1DBuilder& Grid1DBuilder::setOption(
    const FVMGridMaker::grid::grid1d::patterns::distribution::Random1D::Options& opt) {
    this->random1d_options_ = opt;
    return *this;
}

// ----------------------------------------------------------------------------
// build()
// ----------------------------------------------------------------------------
Grid1D Grid1DBuilder::build() const {
    // Validações que o teste espera lançar FVMGException:
    if (this->n_ == 0) {
        FVMG_ERROR(error::CoreErr::InvalidArgument, {
            {"where", "Grid1DBuilder::build"},
            {"what",  "N must be > 0"}
        });
    }
    if (!(this->b_ > this->a_)) {
        FVMG_ERROR(error::CoreErr::InvalidArgument, {
            {"where", "Grid1DBuilder::build"},
            {"what",  "requires B > A"}
        });
    }

    // Resolve geradores no registro
    auto& reg = Grid1DDistributionRegistry::instance();

    auto nameOpt = reg.nameForTag(this->dist_);
    if (!nameOpt.has_value()) {
        // não é verificado em teste; manter std::runtime_error está OK
        throw std::runtime_error("Grid1DBuilder::build(): distribuição não registrada para o tag.");
    }

    auto entryOpt = reg.find(*nameOpt);
    if (!entryOpt.has_value()) {
        throw std::runtime_error("Grid1DBuilder::build(): gerador ausente no registro.");
    }
    const auto& entry = *entryOpt;

    // Empacota Options específicas (quando Random1D)
    const std::any* options_any = nullptr;
    std::any holder;
    if (this->dist_ == DistributionTag::Random1D && this->random1d_options_.has_value()) {
        holder      = *this->random1d_options_;
        options_any = &holder;
    }

    std::vector<Real> xf; // faces (N+1)
    std::vector<Real> xc; // centros (N)

    // 1) Gera sequência base
    if (this->cent_ == CenteringTag::FaceCentered) {
        // Base: faces
        xf = entry.faces_fn(this->n_, this->a_, this->b_, options_any);
        if (xf.size() != static_cast<std::size_t>(this->n_) + 1u) {
            throw std::runtime_error("Distribuição gerou faces com tamanho inválido.");
        }

        // Deriva centros: média entre faces adjacentes
        xc.resize(static_cast<std::size_t>(this->n_));
        auto itR = std::next(xf.begin());
        std::transform(
            xf.begin(), std::prev(xf.end()), xc.begin(),
            [&](Real xL) { Real xR = *itR++; return Real(0.5) * (xL + xR); }
        );
    } else {
        // Base: centros
        xc = entry.centers_fn(this->n_, this->a_, this->b_, options_any);
        if (xc.size() != static_cast<std::size_t>(this->n_)) {
            throw std::runtime_error("Distribuição gerou centros com tamanho inválido.");
        }

        // Fecha faces
        xf.resize(static_cast<std::size_t>(this->n_) + 1u);
        xf.front() = this->a_;
        xf.back()  = this->b_;
        if (this->n_ >= 2) {
            for (std::size_t i = 1; i < static_cast<std::size_t>(this->n_); ++i) {
                xf[i] = Real(0.5) * (xc[i - 1] + xc[i]);
            }
        }
    }

    // 2) dF (N) = dif entre faces adjacentes
    std::vector<Real> dF(static_cast<std::size_t>(this->n_));
    std::transform(
        std::next(xf.begin()), xf.end(), xf.begin(), dF.begin(),
        [](Real xr, Real xl){ return xr - xl; }
    );

    // 3) dC (N+1): convenção do projeto
    std::vector<Real> dC(static_cast<std::size_t>(this->n_) + 1u);
    dC.front() = xc.front() - xf.front();
    if (this->n_ >= 2) {
        std::transform(
            std::next(xc.begin()), xc.end(), xc.begin(), std::next(dC.begin()),
            [](Real c, Real p){ return c - p; }
        );
    }
    dC.back() = xf.back() - xc.back();

    return Grid1D{std::move(xf), std::move(xc), std::move(dF), std::move(dC)};
}

FVMG_GRID1D_BUILDERS_CLOSE
