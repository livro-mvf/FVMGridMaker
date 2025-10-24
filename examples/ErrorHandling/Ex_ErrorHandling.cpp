#include <FVMGridMaker/ErrorHandling/ErrorHandling.h> // Inclui todo o módulo
#include <iostream>
#include <vector>
#include <string>
#include <iomanip> // Para std::setw, std::left
#include <stdexcept> // Para std::exception

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

    // --- 1. Configuração Inicial (Padrão: PtBR, Throw, Warning) ---
    std::cout << "1. Configuração Padrão:\n";
    auto cfg_padrao = Config::get();
    if (!cfg_padrao) {
        std::cerr << "ERRO FATAL: Não foi possível obter a configuração padrão.\n";
        return 1;
    }
    std::cout << "   Idioma: " << (cfg_padrao->language == Language::PtBR ? "PtBR" : "EnUS") << "\n";
    std::cout << "   Política: " << (cfg_padrao->policy == Policy::Throw ? "Throw" : "Status") << "\n";
    std::cout << "   Severidade Mínima: " << static_cast<int>(cfg_padrao->min_severity) << " (Warning=" << static_cast<int>(Severity::Warning) << ")\n";

    // --- 2. Logando Erros (Acima da Severidade Mínima) ---
    std::cout << "\n2. Logando Erros (Severidade >= Warning):\n";
    FVMG_ERROR(CoreErr::NotImplemented); // Warning
    FVMG_ERROR(CoreErr::InvalidArgument, {{"name", "parametro_X"}}); // Error

    std::cout << "   Verificando o log (espera 2 mensagens em PtBR):\n";
    printErrorLog(ErrorManager::flush());

    // --- 3. Demonstração da Localização (RNF08) ---
    std::cout << "\n3. Mudando Idioma para Inglês (EnUS):\n";
    ErrorConfig cfg_en = *Config::get(); // Copia a config atual
    cfg_en.language = Language::EnUS;
    Config::set(cfg_en);

    std::cout << "   Logando o mesmo erro 'InvalidArgument' em Inglês:\n";
    FVMG_ERROR(CoreErr::InvalidArgument, {{"name", "parameter_Y"}});

    std::cout << "   Verificando o log (espera 1 mensagem em EnUS):\n";
    printErrorLog(ErrorManager::flush());

    // Restaura para Português
    cfg_en.language = Language::PtBR;
    Config::set(cfg_en);
    std::cout << "   (Idioma restaurado para PtBR)\n";

    // --- 4. Demonstração da Política "Throw" ---
    std::cout << "\n4. Testando Política 'Throw' (Padrão):\n";
    std::string arquivo_inexistente = "dados.bin";
    try {
        std::cout << "   Tentando logar um erro 'FileNotFound' (Severidade Error)...\n";
        FVMG_ERROR(FileErr::FileNotFound, {{"path", arquivo_inexistente}});
        std::cerr << "   ERRO: Exceção FVMGException não foi lançada!\n";
    }
    catch (const FVMGException& e) {
        std::cout << "   Exceção FVMGException capturada com sucesso!\n";
        std::cout << "     what(): " << e.what() << "\n";
        std::cout << "     code(): 0x" << std::hex << e.code() << std::dec << "\n";
        std::cout << "     severity(): " << static_cast<int>(e.severity()) << "\n";
        // Verifica se o código é o esperado (usando if em vez de EXPECT_EQ)
        if (e.code() != code(FileErr::FileNotFound)) {
             std::cerr << "     ALERTA: Código da exceção inesperado!\n";
        }
    }
    catch (const std::exception& e) { // Captura genérica para outros erros
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
    cfg_status.policy = Policy::Throw;
    Config::set(cfg_status);

    int valor_ok = 10;
    std::cout << "   Assert OK (valor_ok > 0): ";
    try { // Boa prática envolver assert em try/catch se a política for Throw
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
        // --- CORREÇÃO AQUI: Remove EXPECT_EQ ---
        // Apenas imprime a informação ou usa if para verificar
        std::cout << "     code(): 0x" << std::hex << e.code() << std::dec << " (Esperado: 0x" << code(CoreErr::AssertFailed) << ")\n";
        std::cout << "     severity(): " << static_cast<int>(e.severity()) << " (Esperado: " << static_cast<int>(Severity::Fatal) << ")\n";
        // --- FIM DA CORREÇÃO ---
    } catch (const std::exception& e) { // Captura genérica
         std::cerr << "ERRO: Capturou exceção inesperada no Assert: " << e.what() << "\n";
    }

    std::cout << "   Verificando o log após Assert (deve estar vazio):\n";
    printErrorLog(ErrorManager::flush());

    std::cout << "\n--- Fim do Exemplo ---\n";
    return 0;
}