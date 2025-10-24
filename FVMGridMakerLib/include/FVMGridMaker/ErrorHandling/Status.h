// ============================================================================
// File: Status.h
// Author: FVMGridMaker Team
// Version: 1.0 (Criado)
// Description: Objeto Status para política de não-exceção.
// License: GNU GPL v3
// ============================================================================
#pragma once
#include <FVMGridMaker/ErrorHandling/ErrorRecord.h> // Inclui ErrorRecord
#include <FVMGridMaker/Core/namespace.h>       // Inclui macros de namespace

/**
 * @file Status.h
 * @brief Classe Status para funções que retornam erros (Policy::Status).
 * @ingroup error
 */
FVMGRIDMAKER_NAMESPACE_OPEN
namespace error {

/**
 * @brief Objeto que armazena um status (OK ou um erro).
 *
 * Usado por funções quando a política global é `Policy::Status`.
 * Permite verificar `if (status.ok())` ou acessar `status.record()`.
 */
class Status {
public:
    /** @brief Constrói um Status OK. */
    Status() : ok_(true) {}
    /** @brief Constrói um Status de Erro a partir de um ErrorRecord. */
    explicit Status(ErrorRecord record) : ok_(false), record_(std::move(record)) {}

    /** @brief Verifica se o status é OK (sem erro). */
    bool ok() const noexcept { return ok_; }
    /** @brief Retorna o registro de erro (válido apenas se !ok()). */
    const ErrorRecord& record() const {
        // Poderia adicionar um assert(!ok_) aqui em modo Debug
        return record_;
    }

    /** @brief Factory method para criar um Status OK. */
    static Status OK() noexcept { return Status(); }

private:
    bool ok_;
    ErrorRecord record_; // Contém o erro se ok_ == false
};

// TODO: Adicionar classe StatusOr<T> se necessário

} // namespace error
FVMGRIDMAKER_NAMESPACE_CLOSE