// ----------------------------------------------------------------------------
// File: Status.h
// Author: FVMGridMaker Team
// Version: 1.3 (Corrige erro de digitação em std::optional)
// Description: Classe Status e StatusOr para retorno de erros.
// License: GNU GPL v3
// ----------------------------------------------------------------------------
#pragma once


// ----------------------------------------------------------------------------
// includes c++
// ----------------------------------------------------------------------------
#include <memory>
#include <string>
#include <utility> // Para std::move
#include <optional> // Para std::optional


// ----------------------------------------------------------------------------
// includes FVMGridMaker
// ----------------------------------------------------------------------------
#include <FVMGridMaker/Core/namespace.h>
#include <FVMGridMaker/ErrorHandling/ErrorRecord.h>


/**
 * @file Status.h
 * @brief Define as classes Status e StatusOr.
 * @ingroup ErrorHandling
 */
FVMGRIDMAKER_NAMESPACE_OPEN
ERROR_NAMESPACE_OPEN

/**
 * @class Status
 * @brief Encapsula o resultado de uma operação (sucesso ou erro).
 * @ingroup ErrorHandling
 */
class Status {
public:
    // --- Construtores ---
    Status() : ok_(true), record_() {}
    explicit Status(ErrorRecord record) : ok_(false), record_(std::move(record)) {}

    // --- Métodos de Acesso ---
    bool ok() const { return ok_; }
    uint32_t code() const { return record_.code; } // Corrigido p/ uint32_t
    const std::string& message() const { return record_.message; }
    const ErrorRecord& record() const { return record_; }

    // --- Helpers ---
    static Status OK() { return Status(); }

private:
    bool ok_;
    ErrorRecord record_;
};


// --- CLASSE 'StatusOr' ---

/**
 * @class StatusOr
 * @brief Encapsula um valor de retorno OU um Status de erro.
 * @ingroup ErrorHandling
 */
template <typename T>
class StatusOr {
public:
    // Construtor de sucesso (move o valor)
    StatusOr(T&& value) : value_(std::move(value)) {}
    // Construtor de sucesso (copia o valor)
    StatusOr(const T& value) : value_(value) {}

    // Construtor de falha (move o Status)
    StatusOr(Status status) : status_(std::move(status)) {}

    // Verifica se a operação foi bem-sucedida
    bool ok() const { return status_.ok(); }

    // Retorna o Status (OK() se sucesso, ou o erro se falha)
    const Status& status() const { return status_; }

    // Acessa o valor
    const T& value() const { return value_.value(); }
    T& value() { return value_.value(); }

private:
    // --- CORREÇÃO AQUI ---
    // Era 'std.optional' (erro de digitação)
    std::optional<T> value_;
    // --- FIM DA CORREÇÃO ---
    
    Status status_{Status::OK()};
};

ERROR_NAMESPACE_CLOSE
FVMGRIDMAKER_NAMESPACE_CLOSE