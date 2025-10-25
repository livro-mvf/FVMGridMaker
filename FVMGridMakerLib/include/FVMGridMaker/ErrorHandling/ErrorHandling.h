// ----------------------------------------------------------------------------
// File: ErrorHandling.h
// Author: FVMGridMaker Team
// Version: 1.0  
// Description: Umbrella header para o módulo de Erros.
// License: GNU GPL v3
// ----------------------------------------------------------------------------

#pragma once

/**
 * @defgroup error Error Handling
 * @brief O sistema de tratamento de erros, log e exceções.
 *
 * Este módulo fornece uma maneira extensível de definir códigos de erro,
 * gerar mensagens localizadas (Português/Inglês), logar erros e
 * opcionalmente lançar exceções.
 */

// ----------------------------------------------------------------------------
// includes FVMGridMaker
// ----------------------------------------------------------------------------
#include <FVMGridMaker/ErrorHandling/Language.h>
// #include <FVMGridMaker/ErrorHandling/Severity.h>       
// #include <FVMGridMaker/ErrorHandling/ErrorRecord.h>     
#include <FVMGridMaker/ErrorHandling/ErrorTraits.h>
#include <FVMGridMaker/ErrorHandling/IErrorLogger.h>    
// #include <FVMGridMaker/ErrorHandling/ErrorConfig.h>
// #include <FVMGridMaker/ErrorHandling/MessageCatalog.hpp>  
// #include <FVMGridMaker/ErrorHandling/Status.h>          
// #include <FVMGridMaker/ErrorHandling/FVMGException.h>  
#include <FVMGridMaker/ErrorHandling/ErrorManager.h>
#include <FVMGridMaker/ErrorHandling/Macros.h>          
// #include <FVMGridMaker/ErrorHandling/ErrorHandling.h>      
#include <FVMGridMaker/ErrorHandling/FileErrors.h>     