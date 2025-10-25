// ----------------------------------------------------------------------------
// File: ErrorConfig.h
// Author: FVMGridMaker Team
// Version: 1.1 
// Description: Configuração de runtime para o tratamento de erros.
// License: GNU GPL v3
// ----------------------------------------------------------------------------
#pragma once

// ----------------------------------------------------------------------------
// includes c++
// ----------------------------------------------------------------------------
// #include <atomic> 
// #include <memory>
// #include <mutex>  

// ----------------------------------------------------------------------------
// includes FVMGridMaker
// ----------------------------------------------------------------------------
// #include <FVMGridMaker/Core/namespace.h> 
// #include <FVMGridMaker/ErrorHandling/Language.h>
// #include <FVMGridMaker/ErrorHandling/Severity.h>
// #include <FVMGridMaker/ErrorHandling/IErrorLogger.h>

/**
 * @file ErrorConfig.h
 * @brief Configuração de runtime (idioma, política, logger).
 * @ingroup error
 */
FVMGRIDMAKER_NAMESPACE_OPEN
ERROR_NAMESPACE_OPEN

enum class Policy { Throw, Status };

struct ErrorConfig {
    Language language{Language::PtBR};
    Policy policy{Policy::Throw};
    Severity min_severity{Severity::Warning};
    std::size_t thread_buffer_cap{256};
    std::shared_ptr<IErrorLogger> logger;
    ErrorConfig(); // Definido em ErrorManager.h
};

/** @brief Handle de configuração global (protegido por Mutex). */
class Config {
public:
    /** @brief Obtém a configuração atual (thread-safe). */
    static std::shared_ptr<const ErrorConfig> get() noexcept {
        std::lock_guard<std::mutex> lock(instance().mtx_); // Bloqueia o mutex
        return instance().ptr_; // Retorna a cópia do shared_ptr
    } 

    /** @brief Define uma nova configuração global (thread-safe). */
    static void set(ErrorConfig cfg) {
        auto sp = std::make_shared<const ErrorConfig>(std::move(cfg));
        std::lock_guard<std::mutex> lock(instance().mtx_); // Bloqueia o mutex
        instance().ptr_ = sp; // Atribui o novo shared_ptr
    } 

private:
    Config() : ptr_(std::make_shared<const ErrorConfig>()) {} // Inicializa no construtor

    static Config& instance() {
        static Config inst;
        return inst;
    }

    std::shared_ptr<const ErrorConfig> ptr_;
    std::mutex mtx_; // Mutex para proteger o acesso a ptr_
};

ERROR_NAMESPACE_CLOSE
FVMGRIDMAKER_NAMESPACE_CLOSE