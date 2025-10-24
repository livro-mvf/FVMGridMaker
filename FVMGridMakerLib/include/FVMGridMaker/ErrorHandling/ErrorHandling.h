// ============================================================================
// File: ErrorHandling.h
// Author: FVMGridMaker Team
// Version: 1.0  
// Description: Umbrella header para o módulo de Erros.
// License: GNU GPL v3
// ============================================================================
#pragma once

/**
 * @defgroup error Error Handling
 * @brief O sistema de tratamento de erros, log e exceções.
 *
 * Este módulo fornece uma maneira extensível de definir códigos de erro,
 * gerar mensagens localizadas (Português/Inglês), logar erros e
 * opcionalmente lançar exceções.
 */

//------------------------------------------------------------------------------
// Raiz: FVMGridMaker
//------------------------------------------------------------------------------
#include <FVMGridMaker/ErrorHandling/Language.h>
#include <FVMGridMaker/ErrorHandling/Severity.h>       // (Criado)
#include <FVMGridMaker/ErrorHandling/ErrorRecord.h>    // Adicionado (precisa de Severity)
#include <FVMGridMaker/ErrorHandling/ErrorTraits.h>
#include <FVMGridMaker/ErrorHandling/IErrorLogger.h>   // Adicionado (precisa de ErrorHandlngRecord)
#include <FVMGridMaker/ErrorHandling/ErrorConfig.h>
#include <FVMGridMaker/ErrorHandling/MessageCatalog.hpp> // (Criado - Placeholder)
#include <FVMGridMaker/ErrorHandling/Status.h>         // (Criado)
#include <FVMGridMaker/ErrorHandling/FVMGException.h>  // (Criado)
#include <FVMGridMaker/ErrorHandling/ErrorManager.h>
#include <FVMGridMaker/ErrorHandling/Macros.h>         // (Criado)
#include <FVMGridMaker/ErrorHandling/ErrorHandling.h>     // Exemplo de domínio
#include <FVMGridMaker/ErrorHandling/FileErrors.h>     // Exemplo de domínio