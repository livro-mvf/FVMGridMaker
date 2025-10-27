// ----------------------------------------------------------------------------
// File: GridErrors.h
// Project: FVMGridMaker
// Author: FVMGridMaker Team
// Version: 1.0
// Date: 2025-10-26
// Description: Erros relacionados a malhas (Grid) + especialização de
//              ErrorTraits. Destinado a validações de Builder/Patterns
//              (parâmetros inválidos, malha degenerada, etc.).
// License: GNU GPL v3
// ----------------------------------------------------------------------------
#pragma once

// ----------------------------------------------------------------------------
// includes C++
// ----------------------------------------------------------------------------
#include <cstdint>
#include <string_view>

// ----------------------------------------------------------------------------
// includes FVMGridMaker
// ----------------------------------------------------------------------------
#include <FVMGridMaker/Core/namespace.h>
#include <FVMGridMaker/ErrorHandling/ErrorTraits.h>
#include <FVMGridMaker/ErrorHandling/Severity.h>

/**
 * @file GridErrors.h
 * @brief Erros de GRID (parâmetros/consistência de malhas) e seus traits.
 * @ingroup ErrorHandling
 */

FVMGRIDMAKER_NAMESPACE_OPEN
ERROR_NAMESPACE_OPEN

// ----------------------------------------------------------------------------
// Enum de erros de GRID
// ----------------------------------------------------------------------------
enum class GridErr : std::uint16_t {
    InvalidN                 = 1,   // N <= 0
    InvalidDomain            = 2,   // B <= A
    InvalidCentering         = 3,   // centering desconhecido/não suportado
    InvalidDistribution      = 4,   // distribution tag desconhecida/não suportada
    MissingOptions           = 5,   // opções da distribuição ausentes quando requeridas
    OptionsOutOfRange        = 6,   // w_lo/w_hi inválidos, etc.
    DegenerateMesh           = 7,   // tamanhos nulos/negativos
    NonIncreasingFaces       = 8,   // faces não estritamente crescentes
    NonIncreasingCenters     = 9,   // centros não estritamente crescentes
    NaNCoordinate            = 10,  // coordenadas com NaN
    InfCoordinate            = 11,  // coordenadas com +/-inf
    ExecPolicyUnsupported    = 12,  // política de execução indisponível
    ParallelBackendMissing   = 13,  // PSTL pedida, backend ausente (ex.: TBB)
    BuilderStateInvalid      = 14,  // Builder usado em estado inconsistente
    _Min = InvalidN,
    _Max = BuilderStateInvalid
};

// ----------------------------------------------------------------------------
// Mapeamento constexpr -> (key, severity, enUS, ptBR)
// ----------------------------------------------------------------------------
DETAIL_NAMESPACE_OPEN
using sv = std::string_view;

struct GridErrorInfo {
    sv key;
    Severity severity;
    sv enUS;
    sv ptBR;
};

constexpr GridErrorInfo get_grid_error_info_data(GridErr e) {
    switch (e) {
        case GridErr::InvalidN:
            return {sv{"GRID_INVALID_N"}, Severity::Error,
                    sv{"Invalid number of volumes N: {N} (must be > 0)."},
                    sv{"Número de volumes N inválido: {N} (deve ser > 0)."}};

        case GridErr::InvalidDomain:
            return {sv{"GRID_INVALID_DOMAIN"}, Severity::Error,
                    sv{"Invalid domain: B <= A (A={A}, B={B})."},
                    sv{"Domínio inválido: B <= A (A={A}, B={B})."}};

        case GridErr::InvalidCentering:
            return {sv{"GRID_INVALID_CENTERING"}, Severity::Error,
                    sv{"Unsupported or unknown centering: {center}."},
                    sv{"Centering desconhecido ou não suportado: {center}."}};

        case GridErr::InvalidDistribution:
            return {sv{"GRID_INVALID_DISTRIBUTION"}, Severity::Error,
                    sv{"Unsupported or unknown distribution: {dist}."},
                    sv{"Distribuição desconhecida ou não suportada: {dist}."}};

        case GridErr::MissingOptions:
            return {sv{"GRID_MISSING_OPTIONS"}, Severity::Error,
                    sv{"Required distribution options are missing for {dist}."},
                    sv{"Opções obrigatórias da distribuição ausentes para {dist}."}};

        case GridErr::OptionsOutOfRange:
            return {sv{"GRID_OPTIONS_OUT_OF_RANGE"}, Severity::Error,
                    sv{"Distribution options out of valid range (e.g., w_lo={w_lo}, w_hi={w_hi})."},
                    sv{"Opções da distribuição fora da faixa válida (ex.: w_lo={w_lo}, w_hi={w_hi})."}};

        case GridErr::DegenerateMesh:
            return {sv{"GRID_DEGENERATE_MESH"}, Severity::Error,
                    sv{"Degenerate mesh: at least one cell size is non-positive."},
                    sv{"Malha degenerada: ao menos um tamanho de célula é não-positivo."}};

        case GridErr::NonIncreasingFaces:
            return {sv{"GRID_NON_INCREASING_FACES"}, Severity::Error,
                    sv{"Faces must be strictly increasing; violation at index {i}."},
                    sv{"Faces devem ser estritamente crescentes; violação no índice {i}."}};

        case GridErr::NonIncreasingCenters:
            return {sv{"GRID_NON_INCREASING_CENTERS"}, Severity::Error,
                    sv{"Centers must be strictly increasing; violation at index {i}."},
                    sv{"Centros devem ser estritamente crescentes; violação no índice {i}."}};

        case GridErr::NaNCoordinate:
            return {sv{"GRID_NAN_COORDINATE"}, Severity::Error,
                    sv{"Coordinate has NaN at index {i}."},
                    sv{"Coordenada com NaN no índice {i}."}};

        case GridErr::InfCoordinate:
            return {sv{"GRID_INF_COORDINATE"}, Severity::Error,
                    sv{"Coordinate has +/-inf at index {i}."},
                    sv{"Coordenada com +/-inf no índice {i}."}};

        case GridErr::ExecPolicyUnsupported:
            return {sv{"GRID_EXEC_POLICY_UNSUPPORTED"}, Severity::Warning,
                    sv{"Requested execution policy is unsupported; falling back to serial."},
                    sv{"Política de execução solicitada não suportada; retornando ao modo serial."}};

        case GridErr::ParallelBackendMissing:
            return {sv{"GRID_PAR_BACKEND_MISSING"}, Severity::Warning,
                    sv{"Parallel execution requested but backend is missing (e.g., TBB)."},
                    sv{"Execução paralela solicitada, mas o backend está ausente (ex.: TBB)."}};

        case GridErr::BuilderStateInvalid:
            return {sv{"GRID_BUILDER_STATE_INVALID"}, Severity::Error,
                    sv{"Grid1DBuilder used in an invalid or incomplete state."},
                    sv{"Grid1DBuilder usado em estado inválido ou incompleto."}};
        default:
            return {sv{}, Severity::Trace, sv{}, sv{}};
    }
}

constexpr GridErrorInfo get_grid_info(GridErr e) noexcept {
    if (static_cast<std::uint16_t>(e) >= static_cast<std::uint16_t>(GridErr::_Min) &&
        static_cast<std::uint16_t>(e) <= static_cast<std::uint16_t>(GridErr::_Max))
    {
        return get_grid_error_info_data(e);
    }
    return {sv{}, Severity::Trace, sv{}, sv{}};
}

DETAIL_NAMESPACE_CLOSE

// ----------------------------------------------------------------------------
// Traits para GridErr
// ----------------------------------------------------------------------------
template <> struct ErrorTraits<GridErr> {
    static constexpr std::uint16_t   domain_id()   noexcept { return 0x0003; }
    static constexpr std::string_view domain_name() noexcept { return detail::sv{"Grid"}; }

    static constexpr Severity        default_severity(GridErr e) noexcept { return detail::get_grid_info(e).severity; }
    static constexpr std::string_view key(GridErr e)              noexcept { return detail::get_grid_info(e).key; }
    static constexpr std::string_view enUS(GridErr e)             noexcept { return detail::get_grid_info(e).enUS; }
    static constexpr std::string_view ptBR(GridErr e)             noexcept { return detail::get_grid_info(e).ptBR; }
};

ERROR_NAMESPACE_CLOSE
FVMGRIDMAKER_NAMESPACE_CLOSE
