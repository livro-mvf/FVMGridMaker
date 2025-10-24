//==============================================================================
// Name         : type.h
// Author       : Joao Flavio Vieira de Vasconcellos
// Version      : 1.0
// Description  : Define os tipos numéricos centrais (Real, Index) e aliases
//              : de contêineres STL para a biblioteca FVMGridMaker.
// License      : GNU GPL v3
//==============================================================================

#pragma once

/**
 * @file type.h
 * @brief Define os tipos numéricos centrais e aliases da STL.
 *
 * Este header centraliza os tipos escalares usados para cálculos (Real)
 * e para indexação (Index), conforme RF13 e RNF05.
 *
 * @ingroup core
 */

//==============================================================================
//  C++ includes
//==============================================================================
#include <cstdint>
#include <memory>
#include <list>
#include <vector>

//==============================================================================
//  FVMGridMaker includes
//==============================================================================
#include <FVMGridMaker/namespace.h>

FVMGRIDMAKER_NAMESPACE_OPEN
CORE_NAMESPACE_OPEN

//------------------------------------------------------------------------------
// Tipos Numéricos Fundamentais
//------------------------------------------------------------------------------

/**
 * @brief Alias para o tipo de ponto flutuante usado em cálculos.
 * @details Pode ser reconfigurado (ex: para float) via flags de compilação,
 * mas o padrão é double 
 */
using Real = double;

/**
 * @brief Alias para o tipo de inteiro usado para contagens e índices.
 * @details std::size_t é o tipo nativo do C++ para indexação de arrays,
 * garantindo portabilidade e performance. 
using Index = std::size_t;

//------------------------------------------------------------------------------
// Contêineres STL (baseados nos tipos acima)
//------------------------------------------------------------------------------

using VecReal  = std::vector<Real>;
using LstReal  = std::list<Real>;
using VecIndex = std::vector<Index>;
using LstIndex = std::list<Index>;

CORE_NAMESPACE_CLOSE
FVMGRIDMAKER_NAMESPACE_CLOSE