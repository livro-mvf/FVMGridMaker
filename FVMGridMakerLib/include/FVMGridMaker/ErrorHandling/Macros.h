// ============================================================================
// File: Macros.h
// Author: FVMGridMaker Team
// Version: 1.0 (Criado e Adaptado)
// Description: Macros de conveniência (FVMG_ERROR, FVMG_ASSERT).
// License: GNU GPL v3
// ============================================================================
#pragma once
#include <FVMGridMaker/ErrorHandling/Detail.h>       // Para detail::log_error
#include <FVMGridMaker/ErrorHandling/FVMGException.h>// Para FVMGException
#include <FVMGridMaker/ErrorHandling/CoreErrors.h>   // Para CoreErr::AssertFailed
#include <FVMGridMaker/ErrorHandling/ErrorConfig.h>  // Para Policy::Throw
#include <FVMGridMaker/ErrorHandling/ErrorManager.h> // Para ErrorManager::flush

/**
 * @file Macros.h
 * @brief Macros de conveniência FVMG_ERROR, FVMG_ASSERT.
 * @ingroup error
 */

/**
 * @brief Macro principal para registrar e/ou lançar um erro (RNF08).
 *
 * Esta macro sempre chama `detail::log_error` para formatar e registrar
 * a mensagem. Se a política global estiver configurada para `Policy::Throw`,
 * ela também lança uma `FVMGException` contendo o último `ErrorRecord`
 * registrado por esta thread.
 *
 * Uso:
 * @code
 * FVMG_ERROR(CoreErr::InvalidArgument, {{"name", "arg1"}});
 * FVMG_ERROR(FileErr::FileNotFound, {{"path", "/tmp/file.txt"}});
 * FVMG_ERROR(CoreErr::NotImplemented); // Sem argumentos
 * @endcode
 *
 * @param ERR_CODE O valor do enum de erro (ex: `CoreErr::InvalidArgument`).
 * @param ... (Opcional) Uma lista inicializadora de pares chave/valor
 * para substituição na mensagem. Ex: `{{"key1", val1}, {"key2", val2}}`
 */
#define FVMG_ERROR(ERR_CODE, ...)                                           \
    do {                                                                    \
        /* 1. Loga o erro (formata a mensagem e envia para o logger) */     \
        FVMGridMaker::error::detail::log_error(ERR_CODE, ##__VA_ARGS__);    \
                                                                            \
        /* 2. Verifica a política configurada */                            \
        auto fvmg_err_cfg_ = FVMGridMaker::error::Config::get();            \
        if (fvmg_err_cfg_ && fvmg_err_cfg_->policy == FVMGridMaker::error::Policy::Throw) { \
            /* Tenta pegar o último erro logado nesta thread */             \
            auto fvmg_err_errors_ = FVMGridMaker::error::ErrorManager::flush(); \
            if (!fvmg_err_errors_.empty()) {                                \
                /* Lança a exceção com o registro completo */               \
                throw FVMGridMaker::error::FVMGException(std::move(fvmg_err_errors_.back())); \
            } else {                                                        \
                 /* Fallback: Lança exceção com info mínima */              \
                 /* (Pode acontecer se o log foi suprimido pela severidade) */ \
                 using ErrType = decltype(ERR_CODE);                        \
                 throw FVMGridMaker::error::FVMGException(                   \
                    {FVMGridMaker::error::code(ERR_CODE),                  \
                     FVMGridMaker::error::ErrorTraits<ErrType>::default_severity(ERR_CODE), \
                     "Erro lançado (verifique log/severidade)"});          \
            }                                                               \
        }                                                                   \
    } while (0)


/**
 * @brief Macro para asserções internas. Lança um erro Fatal se a condição for falsa.
 *
 * @param condition A condição a ser verificada.
 * @param ... (Opcional) Argumentos adicionais para a mensagem de erro.
 */
#define FVMG_ASSERT(condition, ...)                                         \
    do {                                                                    \
        if (!(condition)) {                                                 \
            /* Usa FVMG_ERROR com código AssertFailed (que é Fatal) */      \
            FVMG_ERROR(FVMGridMaker::error::CoreErr::AssertFailed, ##__VA_ARGS__); \
        }                                                                   \
    } while (0)


    