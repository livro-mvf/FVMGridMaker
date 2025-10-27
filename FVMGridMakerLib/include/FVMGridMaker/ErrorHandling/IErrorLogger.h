// ----------------------------------------------------------------------------
// File: IErrorLogger.h
// Author: FVMGridMaker Team
// Version: 1.1 (Corrigido com include de ErrorRecord)
// Description: Interface para loggers de erro.
// License: GNU GPL v3
// ----------------------------------------------------------------------------
#pragma once


// ----------------------------------------------------------------------------
// includes c++
// ----------------------------------------------------------------------------
#include <memory>
#include <vector>
#include <string>


// ----------------------------------------------------------------------------
// includes FVMGridMaker
// ----------------------------------------------------------------------------
#include <FVMGridMaker/Core/namespace.h>

// --- ESTA É A CORREÇÃO ---
// Esta interface depende de ErrorRecord, então deve incluí-lo.
#include <FVMGridMaker/ErrorHandling/ErrorRecord.h>
// --- FIM DA CORREÇÃO ---


/**
 * @file IErrorLogger.h
 * @brief Define a interface IErrorLogger.
 * @ingroup ErrorHandling
 */
FVMGRIDMAKER_NAMESPACE_OPEN
ERROR_NAMESPACE_OPEN

/**
 * @class IErrorLogger
 * @brief Interface (ABC) para todas as classes de logger de erro.
 * @ingroup ErrorHandling
 */
class IErrorLogger {
public:
    virtual ~IErrorLogger() = default;

    /**
     * @brief Registra um registro de erro.
     */
    virtual void log(const ErrorRecord& record) = 0;

    /**
     * @brief Limpa o buffer do logger (se houver) e retorna os erros.
     */
    virtual std::vector<ErrorRecord> flush() { return {}; }
};

ERROR_NAMESPACE_CLOSE
FVMGRIDMAKER_NAMESPACE_CLOSE