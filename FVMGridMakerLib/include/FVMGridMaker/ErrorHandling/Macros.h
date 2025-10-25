// ============================================================================
// File: Macros.h
// Author: FVMGridMaker Team
// Version: 1.0 (Criado e Adaptado)
// Description: Macros de conveniência (FVMG_ERROR, FVMG_ASSERT).
// License: GNU GPL v3
// ============================================================================

#pragma once

// ----------------------------------------------------------------------------
// includes FVMGridMaker
// ----------------------------------------------------------------------------
#include <FVMGridMaker/ErrorHandling/Detail.h>       // Para detail::log_error
#include <FVMGridMaker/ErrorHandling/FVMGException.h>// Para FVMGException
#include <FVMGridMaker/ErrorHandling/CoreErrors.h>   // Para CoreErr::AssertFailed
// #include <FVMGridMaker/ErrorHandling/ErrorConfig.h>  // Para Policy::Throw
// #include <FVMGridMaker/ErrorHandling/ErrorManager.h> // Para ErrorManager::flush
/**
 * @file Macros.h
 * @brief Macros de conveniência FVMG_ERROR, FVMG_ASSERT.
 * @ingroup ErrorHandling
 */

/**
 * @brief Macro principal para registrar e/ou lançar um erro (RNF08).
 *
 * Loga o erro se a severidade for >= min_severity.
 * Lança FVMGException se policy == Throw E a severidade do erro for >= Error.
 */
#define FVMG_ERROR(ERR_CODE, ...)                                           \
    do {                                                                    \
        /* 1. Loga o erro (formata a mensagem, respeita min_severity) */    \
        FVMGridMaker::error::detail::log_error(ERR_CODE, ##__VA_ARGS__);    \
                                                                            \
        /* 2. Verifica a política global */                                 \
        auto fvmg_err_cfg_ = FVMGridMaker::error::Config::get();            \
        if (fvmg_err_cfg_ && fvmg_err_cfg_->policy == FVMGridMaker::error::Policy::Throw) { \
            /* --- CORREÇÃO AQUI --- */                                     \
            /* 3. Verifica a severidade DESTE erro específico */            \
            using ErrType_ = decltype(ERR_CODE);                            \
            const FVMGridMaker::error::Severity fvmg_err_sev_ =             \
                FVMGridMaker::error::ErrorTraits<ErrType_>::default_severity(ERR_CODE); \
            /* 4. Lança APENAS se for Error ou Fatal */                     \
            if (fvmg_err_sev_ >= FVMGridMaker::error::Severity::Error) {     \
            /* --- FIM DA CORREÇÃO --- */                                   \
                auto fvmg_err_errors_ = FVMGridMaker::error::ErrorManager::flush(); \
                if (!fvmg_err_errors_.empty()) {                            \
                    /* Lança com o registro completo do log */              \
                    throw FVMGridMaker::error::FVMGException(std::move(fvmg_err_errors_.back())); \
                } else {                                                    \
                    /* Fallback: Lança com info mínima se o log falhou */   \
                    throw FVMGridMaker::error::FVMGException(               \
                       {FVMGridMaker::error::code(ERR_CODE),              \
                        fvmg_err_sev_,                                     \
                        "Erro grave lançado (verifique log/severidade)"}); \
                }                                                           \
            }                                                               \
        }                                                                   \
    } while (0)


/**
 * @brief Macro para asserções internas. Lança um erro Fatal se a condição for falsa.
 */
#define FVMG_ASSERT(condition, ...)                                         \
    do {                                                                    \
        if (!(condition)) {                                                 \
            /* Chama FVMG_ERROR com AssertFailed (que é Fatal e vai lançar se policy=Throw) */ \
            FVMG_ERROR(FVMGridMaker::error::CoreErr::AssertFailed, ##__VA_ARGS__); \
        }                                                                   \
    } while (0)


    