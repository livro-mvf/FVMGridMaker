// ============================================================================
// File: IErrorLogger.h
// Author: FVMGridMaker Team
// Version: 1.0  
// Description: Interface abstrata para "sinks" de log.
// License: GNU GPL v3
// ============================================================================

#pragma once

// ----------------------------------------------------------------------------
// includes C++
// ----------------------------------------------------------------------------
#include <vector>

// ----------------------------------------------------------------------------
// includes FVMGridMaker
// ----------------------------------------------------------------------------
#include <FVMGridMaker/ErrorHandling/ErrorRecord.h> // Dependência adaptada

/**
 * @file IErrorLogger.h
 * @brief Classe base abstrata para destinos de log (sinks).
 * @ingroup error
 */
FVMGRIDMAKER_NAMESPACE_OPEN
ERROR_NAMESPACE_OPEN
/**
 * @brief Interface para destinos de log (sinks).
 */
class IErrorLogger {
public:
    virtual ~IErrorLogger() = default;
    /** @brief Loga um único registro de erro. */
    virtual void log(const ErrorRecord& record) = 0;
    /** @brief Descarrega (flush) quaisquer registros em buffer. */
    virtual std::vector<ErrorRecord> flush() { return {}; }
};

ERROR_NAMESPACE_CLOSE
FVMGRIDMAKER_NAMESPACE_CLOSE