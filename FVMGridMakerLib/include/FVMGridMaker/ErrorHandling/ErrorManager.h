// ============================================================================
// File: ErrorManager.h
// Author: FVMGridMaker Team
// Version: 1.0 (Adaptado)
// Description: Fachada de gerenciamento de erros e logger padrão.
// License: GNU GPL v3
// ============================================================================
#pragma once
#include <vector> // Incluído para std::vector
#include <memory> // Incluído para std::shared_ptr
#include <FVMGridMaker/ErrorHandling/IErrorLogger.h>
#include <FVMGridMaker/ErrorHandling/ErrorConfig.h>
#include <FVMGridMaker/ErrorHandling/ErrorRecord.h> // Incluído para ErrorRecord

/**
 * @file ErrorManager.h
 * @brief Fornece um logger padrão (thread-local) e ponto de acesso global.
 * @ingroup error
 */
FVMGRIDMAKER_NAMESPACE_OPEN
namespace error {

/**
 * @brief Logger padrão que armazena erros em um buffer thread-local.
 */
class ThreadLocalBufferLogger : public IErrorLogger {
public:
    void log(const ErrorRecord& record) override {
        auto cfg = Config::get(); // Lê config para pegar o tamanho do buffer
        if (!cfg) return; // Segurança
        auto cap = cfg->thread_buffer_cap;
        auto& buf = buffer();
        if (buf.size() < cap) {
            buf.push_back(record);
        }
        // else: descarta o erro (implementação simples)
    }
    std::vector<ErrorRecord> flush() override {
        auto& buf = buffer();
        std::vector<ErrorRecord> out;
        out.swap(buf); // Move eficientemente o conteúdo
        return out;
    }
private:
    // Retorna uma referência para o buffer desta thread
    static std::vector<ErrorRecord>& buffer() {
        thread_local std::vector<ErrorRecord> tl_buf;
        return tl_buf;
    }
};

/**
 * @brief Implementação inline do construtor de ErrorConfig.
 * Cria o logger padrão ao inicializar a configuração.
 */
inline ErrorConfig::ErrorConfig()
    : logger(std::make_shared<ThreadLocalBufferLogger>()) {}

/**
 * @brief Fachada global para o sistema de tratamento de erros.
 */
class ErrorManager {
public:
    /** @brief Loga um registro usando o logger global configurado. */
    static void log(ErrorRecord rec) {
        if (auto cfg = Config::get()) {
            if (cfg->logger) {
                cfg->logger->log(std::move(rec));
            }
        }
    }
    /** @brief Descarrega (flush) o logger global configurado. */
    static std::vector<ErrorRecord> flush() {
        if (auto cfg = Config::get()) {
            if (cfg->logger) {
                return cfg->logger->flush();
            }
        }
        return {};
    }
};
} // namespace error
FVMGRIDMAKER_NAMESPACE_CLOSE