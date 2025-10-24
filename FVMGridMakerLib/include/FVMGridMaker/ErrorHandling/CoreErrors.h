// ----------------------------------------------------------------------------
// File: CoreErrors.h
// Project: FVMGridMaker
// Version: 1.6 
// Description: Enum de erros do Core + especialização de ErrorTraits.
// License: GNU GPL v3
// ----------------------------------------------------------------------------

#pragma once

// ----------------------------------------------------------------------------
// includes FVMGridMaker
// ----------------------------------------------------------------------------
#include <FVMGridMaker/Core/namespace.h> 
#include <FVMGridMaker/ErrorHandling/ErrorTraits.h>
#include <FVMGridMaker/ErrorHandling/Severity.h>

/**
 * @file CoreErrors.h
 * @brief Erros do Core e seus traits (RNF08).
 * @ingroup ErrorHandling
 */
FVMGRIDMAKER_NAMESPACE_OPEN
ERROR_NAMESPACE_OPEN

enum class CoreErr : std::uint16_t {
    InvalidArgument = 1, OutOfRange = 2, NotImplemented = 3,
    AssertFailed = 4, InconsistentGeometry = 5,
    _Min = InvalidArgument, _Max = InconsistentGeometry
};

DETAIL_NAMESPACE_OPEN
    using sv = std::string_view;

    struct CoreErrorInfo {
        sv key;
        Severity severity;
        sv enUS;
        sv ptBR;
    };

    constexpr CoreErrorInfo get_core_error_info_data(CoreErr e) {
        switch (e) {
            case CoreErr::InvalidArgument:
                return {sv{"CORE_INVALID_ARGUMENT"}, Severity::Error, sv{"Invalid argument: {name}."}, sv{"Argumento inválido: {name}."}};
            case CoreErr::OutOfRange:
                return {sv{"CORE_OUT_OF_RANGE"}, Severity::Error, sv{"Index out of range: {index}."}, sv{"Índice fora do intervalo: {index}."}};
            case CoreErr::NotImplemented:
                return {sv{"CORE_NOT_IMPLEMENTED"}, Severity::Warning, sv{"Feature not implemented."}, sv{"Recurso não implementado."}};
            case CoreErr::AssertFailed:
                return {sv{"CORE_ASSERT_FAILED"}, Severity::Fatal, sv{"Assertion failed."}, sv{"Falha de asserção."}};
            case CoreErr::InconsistentGeometry:
                 return {sv{"CORE_INCONSISTENT_GEOMETRY"}, Severity::Error, sv{"Geometric inconsistency detected: {details}."}, sv{"Inconsistência geométrica detectada: {details}."}};
            default:
                 return {sv{}, Severity::Trace, sv{}, sv{}};
        }
    }

    constexpr CoreErrorInfo get_core_info(CoreErr e) noexcept {
         if (static_cast<uint16_t>(e) >= static_cast<uint16_t>(CoreErr::_Min) &&
             static_cast<uint16_t>(e) <= static_cast<uint16_t>(CoreErr::_Max))
         {
              return get_core_error_info_data(e);
         }
         return {sv{}, Severity::Trace, sv{}, sv{}};
    }

DETAIL_NAMESPACE_CLOSE

template <> struct ErrorTraits<CoreErr> {
    static constexpr std::uint16_t domain_id() noexcept { return 0x0001; }
    static constexpr std::string_view domain_name() noexcept { return detail::sv{"Core"}; }
    static constexpr Severity default_severity(CoreErr e) noexcept { return detail::get_core_info(e).severity; }
    static constexpr std::string_view key(CoreErr e) noexcept { return detail::get_core_info(e).key; }
    static constexpr std::string_view enUS(CoreErr e) noexcept { return detail::get_core_info(e).enUS; }
    static constexpr std::string_view ptBR(CoreErr e) noexcept { return detail::get_core_info(e).ptBR; }
};

ERROR_NAMESPACE_CLOSE
FVMGRIDMAKER_NAMESPACE_CLOSE