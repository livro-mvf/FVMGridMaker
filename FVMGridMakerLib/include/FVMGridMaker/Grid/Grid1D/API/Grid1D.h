// ----------------------------------------------------------------------------
// File: Grid1D.h
// Author: FVMGridMaker Team
// Version: 2.0
// Date: 2025-10-25
// Description: API leve e simples para Grid1D (faces, centros, deltas).
// License: GNU GPL v3
// ----------------------------------------------------------------------------
#pragma once

// ----------------------------------------------------------------------------
// includes C++
// ----------------------------------------------------------------------------
#include <span>
#include <vector>
#include <cstddef>
#include <utility>

// ----------------------------------------------------------------------------
// includes FVMGridMaker
// ----------------------------------------------------------------------------
#include <FVMGridMaker/Core/namespace.h>
#include <FVMGridMaker/Core/type.h>

FVMGRIDMAKER_NAMESPACE_OPEN
GRID_NAMESPACE_OPEN
GRID1D_NAMESPACE_OPEN
API_NAMESPACE_OPEN

class Grid1D {
public:
    // Tipos fundamentais (use "::" para escapar ao escopo atual)
    using Real  = ::FVMGridMaker::core::Real;
    using Index = ::FVMGridMaker::core::Index;

    // ctors básicos
    Grid1D()                              = default;
    Grid1D(const Grid1D&)                 = default;
    Grid1D(Grid1D&&) noexcept             = default;
    Grid1D& operator=(const Grid1D&)      = default;
    Grid1D& operator=(Grid1D&&) noexcept  = default;
    ~Grid1D()                             = default;

    // Construtor usado pelo Builder (precisa ser público para evitar o erro “private within this context”)
    explicit Grid1D(std::vector<Real> faces,
                    std::vector<Real> centers,
                    std::vector<Real> dF,
                    std::vector<Real> dC)
        : m_faces(std::move(faces))
        , m_centers(std::move(centers))
        , m_dF(std::move(dF))
        , m_dC(std::move(dC))
    {}

    // Acesso por span (somente leitura)
    std::span<const Real> faces() const noexcept {
        return std::span<const Real>(m_faces.data(), m_faces.size());
    }
    std::span<const Real> centers() const noexcept {
        return std::span<const Real>(m_centers.data(), m_centers.size());
    }
    std::span<const Real> deltasFaces() const noexcept {
        return std::span<const Real>(m_dF.data(), m_dF.size());
    }
    std::span<const Real> deltasCenters() const noexcept {
        return std::span<const Real>(m_dC.data(), m_dC.size());
    }

    // Info agregada
    Index nVolumes() const noexcept { return static_cast<Index>(m_centers.size()); }
    Index nFaces()   const noexcept { return static_cast<Index>(m_faces.size());   }

    // Acesso escalar (sem checagem de faixa)
    Real face(Index i) const noexcept   { return m_faces[static_cast<std::size_t>(i)]; }
    Real center(Index i) const noexcept { return m_centers[static_cast<std::size_t>(i)]; }
    Real deltaFace(Index i) const noexcept { return m_dF[static_cast<std::size_t>(i)]; }
    Real deltaCenter(Index i) const noexcept { return m_dC[static_cast<std::size_t>(i)]; }

private:
    std::vector<Real> m_faces;   // tamanho N+1
    std::vector<Real> m_centers; // tamanho N
    std::vector<Real> m_dF;      // tamanho N
    std::vector<Real> m_dC;      // tamanho N+1
};

API_NAMESPACE_CLOSE
GRID1D_NAMESPACE_CLOSE
GRID_NAMESPACE_CLOSE
FVMGRIDMAKER_NAMESPACE_CLOSE
