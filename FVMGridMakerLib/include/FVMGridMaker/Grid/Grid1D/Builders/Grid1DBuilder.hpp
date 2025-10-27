// ----------------------------------------------------------------------------
// File: Grid1DBuilder.hpp
// Author: FVMGridMaker Team
// Version: 2.3
// Date: 2025-10-26
// Description: Declaração do construtor de malhas 1D (Grid1DBuilder).
//              - Resolve geradores via registro (faces/centers)
//              - Suporta distribuições Uniform1D e Random1D (e outras via registro)
//              - Permite escolher o centering (Face/Cell)
//              - setOption(Random1D::Options) para injetar parâmetros de Random1D
// License: GNU GPL v3
// ----------------------------------------------------------------------------
#pragma once

// ----------------------------------------------------------------------------
// includes FVMGridMaker (ordem alfabética por caminho)
// ----------------------------------------------------------------------------
#include <FVMGridMaker/Core/namespace.h>
#include <FVMGridMaker/Core/type.h>
#include <FVMGridMaker/Grid/Common/Tags1D.hpp>
#include <FVMGridMaker/Grid/Grid1D/API/Grid1D.h>
#include <FVMGridMaker/Grid/Grid1D/Patterns/Distribution/Random1D.hpp> // Options de Random1D

// ----------------------------------------------------------------------------
// includes C++ (ordem alfabética)
// ----------------------------------------------------------------------------
#include <optional>

FVMG_GRID1D_BUILDERS_OPEN

using FVMGridMaker::core::Index;
using FVMGridMaker::core::Real;
using FVMGridMaker::grid::CenteringTag;
using FVMGridMaker::grid::DistributionTag;
using FVMGridMaker::grid::grid1d::api::Grid1D;
using FVMGridMaker::grid::grid1d::patterns::distribution::Random1D;

/**
 * @brief Builder de malhas 1D (Grid1D).
 *
 * Armazena N, domínio [A,B], distribuição e centering; chama o registro para
 * gerar a sequência base (faces OU centers) e fecha a malha.
 */
class Grid1DBuilder {
public:
    Grid1DBuilder() = default;

    /// Define o número de volumes (N > 0).
    Grid1DBuilder& setN(Index n);

    /// Define o domínio [A,B] (exige B > A).
    Grid1DBuilder& setDomain(Real a, Real b);

    /// Define a distribuição (Uniform1D, Random1D, ... via registro).
    Grid1DBuilder& setDistribution(DistributionTag tag);

    /// Define o centering (FaceCentered ou CellCentered).
    Grid1DBuilder& setCentering(CenteringTag centering);

    /// Injeta opções específicas da distribuição Random1D.
    Grid1DBuilder& setOption(const Random1D::Options& opt);

    /// Constrói e retorna o Grid1D materializado.
    Grid1D build() const;

private:
    Index            n_    {0};
    Real             a_    {0.0};
    Real             b_    {1.0};
    DistributionTag  dist_ {DistributionTag::Uniform1D};
    CenteringTag     cent_ {CenteringTag::FaceCentered};

    // Opções específicas de Random1D (armazenadas se fornecidas)
    std::optional<Random1D::Options> random1d_options_;
};

FVMG_GRID1D_BUILDERS_CLOSE
