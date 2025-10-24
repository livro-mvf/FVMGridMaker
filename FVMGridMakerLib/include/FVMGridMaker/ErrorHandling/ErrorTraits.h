// ============================================================================
// File: ErrorTraits.h
// Project: FVMGridMaker
// Version: 1.3 (Concept Simplificado para GCC 13)
// Description: Traits para descrever enums de erro (extensível).
// License: GNU GPL v3
// ============================================================================
#pragma once
#include <cstdint>
#include <string_view>
#include <concepts>
#include <type_traits>
#include <FVMGridMaker/ErrorHandling/Severity.h>
#include <FVMGridMaker/Core/namespace.h>

/**
 * @file ErrorTraits.h
 * @brief Traits extensíveis para enums de erro definidos pelo usuário.
 * @ingroup ErrorHandling
 */
FVMGRIDMAKER_NAMESPACE_OPEN
namespace error {

template <class T> struct ErrorTraits; // Declaração primária

constexpr std::uint32_t make_code(std::uint16_t domain,
                                  std::uint16_t value) noexcept {
    return (static_cast<std::uint32_t>(domain) << 16) |
           static_cast<std::uint32_t>(value);
}

/** @brief C++20 concept para enums suportados pelo ErrorTraits (Simplificado). */
template <class T>
concept ErrorEnum = std::is_enum_v<T> &&
    requires(T e) {
        // --- CORREÇÃO AQUI: Sintaxe simplificada ---
        // Apenas verificamos se as funções existem e são noexcept.
        // A verificação de tipo de retorno é feita implicitamente pelo compilador
        // ao usar a especialização de ErrorTraits.
        { ErrorTraits<T>::domain_id() } noexcept;
        { ErrorTraits<T>::domain_name() } noexcept;
        { ErrorTraits<T>::key(e) } noexcept;
        { ErrorTraits<T>::enUS(e) } noexcept;
        { ErrorTraits<T>::ptBR(e) } noexcept;
        { ErrorTraits<T>::default_severity(e) } noexcept;
        // --- FIM DA CORREÇÃO ---
    };

/** @brief Retorna o código de erro de 32 bits composto. */
template <ErrorEnum E> // Agora usa o concept corrigido
constexpr std::uint32_t code(E err_enum) noexcept {
    // Verifica se ErrorTraits<E> está definido (proteção contra erro de compilação)
    static_assert(requires { ErrorTraits<E>::domain_id(); },
                  "ErrorTraits<E> não está especializado ou incompleto.");
    return make_code(ErrorTraits<E>::domain_id(),
                     static_cast<std::uint16_t>(err_enum));
}

} // namespace error
FVMGRIDMAKER_NAMESPACE_CLOSE