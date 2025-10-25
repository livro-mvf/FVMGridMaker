// ============================================================================
// File: FileErrors.h
// Project: FVMGridMaker
// Version: 1.6 (Remove thread_local from constexpr function)
// Description: Erros de E/S (File) + especialização de ErrorTraits.
// License: GNU GPL v3
// ============================================================================

#pragma once


// ----------------------------------------------------------------------------
// includes c++
// ----------------------------------------------------------------------------
#include <array>
#include <cstdint>
#include <string_view>
#include <stdexcept>

// ----------------------------------------------------------------------------
// includes FVMGridMaker
// ----------------------------------------------------------------------------
#include <FVMGridMaker/Core/namespace.h>  
#include <FVMGridMaker/ErrorHandling/ErrorTraits.h>
#include <FVMGridMaker/ErrorHandling/Severity.h>

/**
 * @file FileErrors.h
 * @brief Erros de E/S (Arquivo) e seus traits (RNF08).
 * @ingroup ErrorHandling
 */
FVMGRIDMAKER_NAMESPACE_OPEN
ERROR_NAMESPACE_OPEN

enum class FileErr : std::uint16_t {
    FileNotFound = 1, AccessDenied = 2, ReadError = 3,
    WriteError = 4, InvalidPath = 5,
    _Min = FileNotFound, _Max = InvalidPath
};

DETAIL_NAMESPACE_OPEN
using sv = std::string_view;

struct FileErrorInfo {
    sv key;
    Severity severity;
    sv enUS;
    sv ptBR;
};

constexpr FileErrorInfo get_file_error_info_data(FileErr e) {
    switch (e) {
        case FileErr::FileNotFound:
            return {sv{"FILE_NOT_FOUND"}, Severity::Error, sv{"File not found: {path}."}, sv{"Arquivo não encontrado: {path}."}};
        case FileErr::AccessDenied:
            return {sv{"FILE_ACCESS_DENIED"}, Severity::Error, sv{"Access denied to file: {path}."}, sv{"Acesso negado ao arquivo: {path}."}};
        case FileErr::ReadError:
            return {sv{"FILE_READ_ERROR"}, Severity::Error, sv{"An error occurred while reading the file: {path}."}, sv{"Ocorreu um erro ao ler o arquivo: {path}."}};
        case FileErr::WriteError:
            return {sv{"FILE_WRITE_ERROR"}, Severity::Error, sv{"An error occurred while writing to the file: {path}."}, sv{"Ocorreu um erro ao escrever no arquivo: {path}."}};
        case FileErr::InvalidPath:
            return {sv{"FILE_INVALID_PATH"}, Severity::Error, sv{"The provided path is invalid: {path}."}, sv{"O caminho fornecido é inválido: {path}."}};
        default:
             return {sv{}, Severity::Trace, sv{}, sv{}}; // Valor padrão seguro
    }
}

constexpr FileErrorInfo get_file_info(FileErr e) noexcept {
     if (static_cast<uint16_t>(e) >= static_cast<uint16_t>(FileErr::_Min) &&
         static_cast<uint16_t>(e) <= static_cast<uint16_t>(FileErr::_Max))
     {
          return get_file_error_info_data(e);
     }
     return {sv{}, Severity::Trace, sv{}, sv{}}; // Retorna valor padrão seguro
}

DETAIL_NAMESPACE_CLOSE

template <> struct ErrorTraits<FileErr> {
    static constexpr std::uint16_t domain_id() noexcept { return 0x0002; }
    static constexpr std::string_view domain_name() noexcept { return detail::sv{"File"}; }
    // As chamadas aqui continuam válidas
    static constexpr Severity default_severity(FileErr e) noexcept { return detail::get_file_info(e).severity; }
    static constexpr std::string_view key(FileErr e) noexcept { return detail::get_file_info(e).key; }
    static constexpr std::string_view enUS(FileErr e) noexcept { return detail::get_file_info(e).enUS; }
    static constexpr std::string_view ptBR(FileErr e) noexcept { return detail::get_file_info(e).ptBR; }
};

ERROR_NAMESPACE_CLOSE
FVMGRIDMAKER_NAMESPACE_CLOSE