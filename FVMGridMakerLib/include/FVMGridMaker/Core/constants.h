//------------------------------------------------------------------------------
// Name         : constants.h
// Author       : Joao Flavio Vieira de Vasconcellos
// Version      : 1.0 (Adaptado para FVMGridMaker)
// Description  : Constantes numéricas (tolerâncias e padrões) para o core
//              : da biblioteca FVMGridMaker.
// License      : GNU GPL v3
//------------------------------------------------------------------------------

#pragma once

/**
 * @file constants.h
 * @brief Constantes numéricas usadas em toda a biblioteca.
 *
 * Este header define tolerâncias numéricas e padrões a serem usados
 * pelos módulos de geometria e da malha.
 *
 * @ingroup core
 */

//------------------------------------------------------------------------------
//  C++ includes
//------------------------------------------------------------------------------
#include <limits>   // std::numeric_limits
#include <numbers>  // std::numbers::pi_v (C++20)

//------------------------------------------------------------------------------
//  FVMGridMaker includes
//------------------------------------------------------------------------------
#include <FVMGridMaker/core/type.h>      // para FVMGridMaker::core::Real
#include <FVMGridMaker/namespace.h>  // para as macros de namespace

// Abre o namespace raiz: FVMGridMaker
FVMGRIDMAKER_NAMESPACE_OPEN

/**
 * @namespace FVMGridMaker::constants
 * @brief Namespace interno para constantes numéricas.
 *
 * Manter as constantes em um namespace dedicado evita poluir a API principal
 * e permite nomes simples para uso frequente.
 */
CONSTANTS_NAMESPACE_OPEN

//------------------------------------------------------------------------------
// Constantes tipadas (preferir estas em código novo)
//------------------------------------------------------------------------------

// Usa o 'Real' definido em type.h
using FVMGridMaker::core::Real;

/**
 * @brief Tolerância numérica zero (limiar muito pequeno).
 *
 * Usado para comparações de ponto flutuante contra zero.
 */
inline constexpr Real kZeroTol = static_cast<Real>(1e-12);

/**
 * @brief Limiar pequeno para comparações conservadoras.
 * Exemplo: "travar" áreas/volumes negativos muito pequenos em zero.
 */
inline constexpr Real kLimit = static_cast<Real>(1e-30);

/**
 * @brief Epsilon para checagens de igualdade em predicados não críticos.
 */
inline constexpr Real kEpsilon = static_cast<Real>(1e-6);

/**
 * @brief Tamanho padrão para buffers/contêineres usados em utilitários.
 */
inline constexpr int kDefaultSize = 0x50; // 80

/**
 * @brief Constante em tempo de compilação para π (pi).
 */
inline constexpr Real kPi =
    static_cast<Real>(std::numbers::pi_v<long double>);

CONSTANTS_NAMESPACE_CLOSE

//------------------------------------------------------------------------------
// Aliases para compatibilidade (manter para evitar quebrar código existente)
// Preferir FVMGridMaker::constants::k* em código novo.
//------------------------------------------------------------------------------

inline constexpr Real ZERO    = constants::kZeroTol;
inline constexpr Real LIMIT   = constants::kLimit;
inline constexpr Real EPSILON = constants::kEpsilon;
inline constexpr int  LSIZE   = constants::kDefaultSize;

/// Pi como uma constante (preferir esta forma em contextos de compilação).
inline constexpr Real PI = constants::kPi;

//------------------------------------------------------------------------------
// Verificações de sanidade (em tempo de compilação): garante que Real se
// comporte como um tipo de ponto flutuante.
//------------------------------------------------------------------------------
static_assert   (   std::numeric_limits<FVMGridMaker::core::Real>::is_iec559 ||
                    std::numeric_limits<FVMGridMaker::core::Real>::is_specialized,
                    "Real deve ser um tipo similar a ponto flutuante."
                );

// Fecha o namespace raiz: FVMGridMaker
FVMGRIDMAKER_NAMESPACE_CLOSE