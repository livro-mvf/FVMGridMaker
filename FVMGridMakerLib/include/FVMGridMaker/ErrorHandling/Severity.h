// ============================================================================
// File: Severity.h
// Author: FVMGridMaker Team
// Version: 1.1 (Corrigido)
// Description: Níveis de severidade para logs.
// License: GNU GPL v3
// ============================================================================
#pragma once

// ----------------------------------------------------------------------------
// includes FVMGridMaker
// ----------------------------------------------------------------------------
#include <FVMGridMaker/Core/namespace.h>

/**
 * @file Severity.h
 * @brief Define os níveis de severidade para logging.
 * @ingroup error
 */

FVMGRIDMAKER_NAMESPACE_OPEN
ERROR_NAMESPACE_OPEN
/** @brief Nível de severidade do log. */
enum class Severity {
    Trace   = 0, ///< Detalhes finos de depuração.
    Debug   = 1, ///< Informação útil para depuração.
    Info    = 2, ///< Mensagens informativas gerais.
    Warning = 3, ///< Condições potencialmente problemáticas.
    Error   = 4, ///< Erros que podem impedir a operação normal.
    Fatal   = 5  ///< Erros críticos que devem abortar a operação.
};

ERROR_NAMESPACE_CLOSE
FVMGRIDMAKER_NAMESPACE_CLOSE
