// ============================================================================
// File: Ex_ErrorHandling.cpp
// Project: FVMGridMaker
// Author: FVMGridMaker Team
// Version: 1.1  
// Description: Exemplo de uso do módulo ErrorHandling.
// License: GNU GPL v3
// ============================================================================


// ----------------------------------------------------------------------------
// includes c++
// ----------------------------------------------------------------------------
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>  
#include <stdexcept>  

// ----------------------------------------------------------------------------
// includes FVMGridMaker
// ----------------------------------------------------------------------------
#include <FVMGridMaker/ErrorHandling/ErrorHandling.h> 

// Define o namespace para facilitar
using namespace FVMGridMaker::error;

// Função auxiliar para imprimir os erros logados
void printErrorLog(const std::vector<ErrorRecord>& errors) {
    if (errors.empty()) {
        std::cout << "  (Log vazio)\n";
        return;
    }
    std::cout << "  --- Log de Erros ---\n";
    for (const auto& rec : errors) {
        std::cout << "  [Code: 0x" << std::hex << std::setw(8) << std::setfill('0') << rec.code << std::dec << "]";
        std::cout << " [Sev: " << static_cast<int>(rec.severity) << "]";
        std::cout << " Msg: " << rec.message << "\n";
    }
    std::cout << "  --------------------\n";
}

int main() {
    std::cout << "--- Exemplo do Módulo de ErrorHandling ---\n\n";

    // Guarda a config original para restaurar no final
    auto original_cfg_ptr = Config::get();
    if (!original_cfg_ptr) {
         std::cerr << "ERRO FATAL: Não foi possível obter a configuração inicial.\n";
         return 1;
    }

    // --- 1. Configuração Inicial (Padrão: PtBR, Throw, Warning) ---
    std::cout << "1. Configuração Padrão:\n";
    auto cfg_padrao = Config::get(); // Re-obtém (embora seja o mesmo que original_cfg_ptr)
    std::cout << "   Idioma: " << (cfg_padrao->language == Language::PtBR ? "PtBR" : "EnUS") << "\n";
    std::cout << "   Política: " << (cfg_padrao->policy == Policy::Throw ? "Throw" : "Status") << "\n";
    std::cout << "   Severidade Mínima: " << static_cast<int>(cfg_padrao->min_severity) << " (Warning=" << static_cast<int>(Severity::Warning) << ")\n";

    // --- 2. Logando Erros (Acima da Severidade Mínima) ---
    std::cout << "\n2. Logando Erros (Severidade >= Warning):\n";

    // Muda temporariamente para Policy::Status para esta seção
    std::cout << "   (Mudando temporariamente para Policy::Status para evitar throws)\n";
    ErrorConfig cfg_temp_log = *Config::get();
    cfg_temp_log.policy = Policy::Status;
    Config::set(cfg_temp_log);

    // Loga um Warning (deve aparecer)
    FVMG_ERROR(CoreErr::NotImplemented);
    // Loga um Error com argumento (deve aparecer)
    FVMG_ERROR(CoreErr::InvalidArgument, {{"name", "parametro_X"}});

    std::cout << "   Verificando o log (espera 2 mensagens em PtBR):\n";
    printErrorLog(ErrorManager::flush()); // Esvazia e imprime o log

    // Restaura a política original (Throw)
    std::cout << "   (Restaurando política original: Throw)\n";
    Config::set(*original_cfg_ptr);


    // --- 3. Demonstração da Localização (RNF08) ---
    std::cout << "\n3. Mudando Idioma para Inglês (EnUS):\n";
    ErrorConfig cfg_en = *Config::get(); // Copia a config atual (Throw)
    cfg_en.language = Language::EnUS;
    // Muda temporariamente para Policy::Status para logar sem throw
    cfg_en.policy = Policy::Status;
    Config::set(cfg_en);
    std::cout << "   (Política mudada para Status para logar em Inglês sem throw)\n";


    std::cout << "   Logando o mesmo erro 'InvalidArgument' em Inglês:\n";
    FVMG_ERROR(CoreErr::InvalidArgument, {{"name", "parameter_Y"}}); // Não vai lançar

    std::cout << "   Verificando o log (espera 1 mensagem em EnUS):\n";
    printErrorLog(ErrorManager::flush());

    // Restaura configuração original completa (PtBR, Throw)
    std::cout << "   (Restaurando configuração original: PtBR, Throw)\n";
    Config::set(*original_cfg_ptr);


    // --- 4. Demonstração da Política "Throw" ---
    // A configuração já foi restaurada para Throw
    std::cout << "\n4. Testando Política 'Throw' (Restaurada):\n";
    std::string arquivo_inexistente = "dados.bin";
    try {
        std::cout << "   Tentando logar um erro 'FileNotFound' (Severidade Error)...\n";
        FVMG_ERROR(FileErr::FileNotFound, {{"path", arquivo_inexistente}}); // Deve lançar
        std::cerr << "   ERRO: Exceção FVMGException não foi lançada!\n";
    }
    catch (const FVMGException& e) {
        std::cout << "   Exceção FVMGException capturada com sucesso!\n";
        std::cout << "     what(): " << e.what() << "\n";
        std::cout << "     code(): 0x" << std::hex << e.code() << std::dec << "\n";
        std::cout << "     severity(): " << static_cast<int>(e.severity()) << "\n";
        if (e.code() != code(FileErr::FileNotFound)) {
             std::cerr << "     ALERTA: Código da exceção inesperado!\n";
        }
    }
    catch (const std::exception& e) {
        std::cerr << "   ERRO: Capturou exceção inesperada: " << e.what() << "\n";
    }

    std::cout << "   Verificando o log após exceção (deve estar vazio):\n";
    printErrorLog(ErrorManager::flush());


    // --- 5. Demonstração da Política "Status" ---
    std::cout << "\n5. Mudando Política para 'Status':\n";
    ErrorConfig cfg_status = *Config::get();
    cfg_status.policy = Policy::Status;
    Config::set(cfg_status);

    std::cout << "   Tentando logar 'FileNotFound' novamente (NÃO deve lançar):\n";
    FVMG_ERROR(FileErr::FileNotFound, {{"path", "outro_arquivo.txt"}});
    std::cout << "   (Nenhuma exceção lançada, como esperado)\n";

    std::cout << "   Verificando o log (deve conter o erro 'FileNotFound'):\n";
    printErrorLog(ErrorManager::flush());


    // --- 6. Demonstração do FVMG_ASSERT ---
    std::cout << "\n6. Testando FVMG_ASSERT:\n";
    // Volta para Policy::Throw para que o Assert lance exceção
    cfg_status.policy = Policy::Throw; // Modifica a cópia local
    Config::set(cfg_status); // Define globalmente

    int valor_ok = 10;
    std::cout << "   Assert OK (valor_ok > 0): ";
    try {
        FVMG_ASSERT(valor_ok > 0);
        std::cout << "Passou.\n";
    } catch (const FVMGException& e) {
         std::cerr << "ERRO: Assert OK lançou exceção inesperadamente: " << e.what() << "\n";
    }


    int valor_ruim = -5;
    try {
        std::cout << "   Assert Falha (valor_ruim > 0): ";
        FVMG_ASSERT(valor_ruim > 0, {{"valor", std::to_string(valor_ruim)}});
        std::cerr << "ERRO: Assert não lançou exceção!\n";
    } catch (const FVMGException& e) {
        std::cout << "Exceção de Assert capturada!\n";
        std::cout << "     what(): " << e.what() << "\n";
        std::cout << "     code(): 0x" << std::hex << e.code() << std::dec << " (Esperado: 0x" << code(CoreErr::AssertFailed) << ")\n";
        std::cout << "     severity(): " << static_cast<int>(e.severity()) << " (Esperado: " << static_cast<int>(Severity::Fatal) << ")\n";
    } catch (const std::exception& e) {
         std::cerr << "ERRO: Capturou exceção inesperada no Assert: " << e.what() << "\n";
    }

    std::cout << "   Verificando o log após Assert (deve estar vazio):\n";
    printErrorLog(ErrorManager::flush());


    // Restaura a configuração original salva no início
    std::cout << "\nRestaurando configuração original...\n";
    Config::set(*original_cfg_ptr);


    std::cout << "\n--- Fim do Exemplo ---\n";
    return 0;
}


