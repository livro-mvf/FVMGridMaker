// ----------------------------------------------------------------------------
// File: ErrorRecord.h
// Author: FVMGridMaker Team
// Version: 1.0 
// Description: Estrutura para um registro de erro único.
// License: GNU GPL v3
// ----------------------------------------------------------------------------

#pragma once


// ----------------------------------------------------------------------------
// includes c++
// ----------------------------------------------------------------------------
// #include <chrono>
// #include <cstdint>
// #include <string>
#include <thread>


// ----------------------------------------------------------------------------
// includes FVMGridMaker
// ----------------------------------------------------------------------------
#include <FVMGridMaker/ErrorHandling/Severity.h> 

/**
 * @file ErrorRecord.h
 * @brief Estrutura para guardar informação detalhada sobre um erro.
 * @ingroup error
 */
FVMGRIDMAKER_NAMESPACE_OPEN
ERROR_NAMESPACE_OPEN
/** @brief Guarda toda informação sobre um evento de erro logado. */
struct ErrorRecord {
    std::uint32_t code{0}; ///< Código de erro único (domain << 16 | value)
    Severity severity{Severity::Error}; ///< Nível de severidade
    std::string message; ///< Mensagem formatada e localizada
    std::chrono::system_clock::time_point ts{ ///< Timestamp
        std::chrono::system_clock::now()};
    std::thread::id tid{std::this_thread::get_id()}; ///< ID da thread
};

ERROR_NAMESPACE_CLOSE
FVMGRIDMAKER_NAMESPACE_CLOSE