// ============================================================================
// File: FVMGException.h
// Author: FVMGridMaker Team
// Version: 1.0 (Criado e Adaptado)
// Description: Classe de exceção base para a biblioteca (RNF08).
// License: GNU GPL v3
// ============================================================================
#pragma once
#include <stdexcept>
#include <string>
#include <FVMGridMaker/ErrorHandling/ErrorRecord.h> // Inclui ErrorRecord

/**
 * @file FVMGException.h
 * @brief Classe de exceção base (RNF08).
 * @ingroup error
 */
FVMGRIDMAKER_NAMESPACE_OPEN
namespace error {

/**
 * @brief Classe de exceção base para FVMGridMaker.
 *
 * Carrega um ErrorRecord contendo a mensagem formatada e localizada (RNF08).
 */
class FVMGException : public std::exception {
public:
    /** @brief Constrói a exceção a partir de um registro de erro. */
    explicit FVMGException(ErrorRecord record)
        : record_(std::move(record)) {}

    /** @brief Retorna a mensagem de erro formatada. */
    const char* what() const noexcept override {
        // Garante que a string interna não seja invalidada
        // (embora std::string::c_str() geralmente seja estável)
        try {
             // Atualiza uma cópia interna caso a string original seja movida
             // (improvável aqui, mas mais seguro em geral)
             what_cache_ = record_.message;
             return what_cache_.c_str();
        } catch (...) {
            return "Falha ao obter mensagem da exceção.";
        }
    }
    /** @brief Retorna o registro de erro completo associado à exceção. */
    const ErrorRecord& record() const noexcept {
        return record_;
    }
    /** @brief Retorna o código de erro numérico de 32 bits. */
    std::uint32_t code() const noexcept {
        return record_.code;
    }
    /** @brief Retorna a severidade do erro. */
    Severity severity() const noexcept {
         return record_.severity;
    }

private:
    ErrorRecord record_;
    // Cache para garantir que what() retorne um ponteiro válido
    mutable std::string what_cache_;
};
} // namespace error
FVMGRIDMAKER_NAMESPACE_CLOSE