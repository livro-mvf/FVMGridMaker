#include <gtest/gtest.h>
#include <FVMGridMaker/ErrorHandling/ErrorHandling.h> // Caminho correto para o módulo
#include <string>

// Namespace para facilitar
using namespace FVMGridMaker::error;

// --- Test Suite para o Módulo de Erros ---

// Testa a configuração global (idioma, política, severidade)
TEST(ErrorHandlingTest, Configuracao) {
    // Guarda a config original para restaurar depois
    auto original_cfg_ptr = Config::get();
    // Cria uma cópia da configuração padrão para modificar
    ErrorConfig cfg = *original_cfg_ptr;

    // Modifica a cópia
    cfg.language = Language::PtBR;
    cfg.policy = Policy::Status;
    cfg.min_severity = Severity::Debug;
    // O logger padrão (ThreadLocalBufferLogger) é criado no construtor

    // Define a nova configuração globalmente
    Config::set(cfg); // Passa a cópia modificada

    // Verifica se a configuração foi aplicada corretamente
    auto current_cfg = Config::get();
    ASSERT_TRUE(current_cfg); // Garante que não é nullptr
    EXPECT_EQ(current_cfg->language, Language::PtBR);
    EXPECT_EQ(current_cfg->policy, Policy::Status);
    EXPECT_EQ(current_cfg->min_severity, Severity::Debug);
    EXPECT_TRUE(current_cfg->logger); // Verifica se o logger padrão foi criado

    // Restaura a configuração original
    Config::set(*original_cfg_ptr);
    ASSERT_EQ(Config::get()->language, original_cfg_ptr->language);
    ASSERT_EQ(Config::get()->policy, original_cfg_ptr->policy);
}

// Testa o fluxo básico de log e flush usando o logger padrão
TEST(ErrorHandlingTest, LogEFlush) {
    auto original_cfg_ptr = Config::get();
    // Usa configuração padrão (PtBR, Throw, Warning)
    ErrorConfig cfg_default; // Cria uma nova config padrão
    Config::set(cfg_default);

    // Usa a função interna detail::log_error diretamente para testar o log
    detail::log_error(CoreErr::NotImplemented);

    // Recupera os erros logados nesta thread
    auto errors = ErrorManager::flush();

    // Verifica se UM erro foi logado corretamente
    ASSERT_EQ(errors.size(), 1);
    const auto& rec = errors[0];

    EXPECT_EQ(rec.code, code(CoreErr::NotImplemented));
    EXPECT_EQ(rec.severity, Severity::Warning); // Severidade padrão de NotImplemented
    EXPECT_EQ(rec.message, "Recurso não implementado."); // Mensagem em PtBR (RNF08)

    // Verifica se o flush limpou o buffer
    errors = ErrorManager::flush();
    EXPECT_TRUE(errors.empty());

    Config::set(*original_cfg_ptr); // Restaura
}

// Testa a formatação da mensagem com placeholders e a localização (RNF08)
TEST(ErrorHandlingTest, FormatacaoELocalizacao) {
    auto original_cfg_ptr = Config::get();

    // 1. Testa em Português (Padrão)
    Config::set(ErrorConfig{}); // Padrão é PtBR
    std::string arg_pt = "NomeDoArgumentoPT";
    detail::log_error(CoreErr::InvalidArgument, {{"name", arg_pt}});
    auto errors_pt = ErrorManager::flush();

    ASSERT_EQ(errors_pt.size(), 1);
    EXPECT_EQ(errors_pt[0].message, "Argumento inválido: " + arg_pt + ".");

    // 2. Testa em Inglês
    ErrorConfig cfg_en;
    cfg_en.language = Language::EnUS;
    Config::set(cfg_en); // Define globalmente para EnUS

    std::string arg_en = "ArgumentNameEN";
    detail::log_error(CoreErr::InvalidArgument, {{"name", arg_en}});
    auto errors_en = ErrorManager::flush();

    ASSERT_EQ(errors_en.size(), 1);
    EXPECT_EQ(errors_en[0].message, "Invalid argument: " + arg_en + ".");

    Config::set(*original_cfg_ptr); // Restaura
}

// Testa se a macro FVMG_ERROR lança exceção com Policy::Throw (RNF08)
TEST(ErrorHandlingTest, PoliticaDeExcecao) {
    auto original_cfg_ptr = Config::get();
    ErrorConfig cfg;
    cfg.policy = Policy::Throw; // Garante que vai lançar
    cfg.language = Language::PtBR; // Testa mensagem em PT na exceção
    Config::set(cfg);

    std::string path_teste = "/caminho/falso.txt";

    try {
        // Usa a macro FVMG_ERROR (que deve logar E lançar)
        FVMG_ERROR(FileErr::FileNotFound, {{"path", path_teste}});
        // Se chegou aqui, falhou (não lançou)
        FAIL() << "Deveria ter lançado FVMGException";
    }
    catch (const FVMGException& e) { // Captura por referência constante
        // Verifica se a exceção capturada está correta
        SUCCEED(); // Capturou a exceção correta
        EXPECT_EQ(e.code(), code(FileErr::FileNotFound));
        EXPECT_EQ(e.severity(), Severity::Error); // Severidade padrão de FileNotFound
        // Verifica a mensagem em português (RNF08)
        std::string expected_msg = "Arquivo não encontrado: " + path_teste + ".";
        EXPECT_STREQ(e.what(), expected_msg.c_str());
        // Verifica o record interno
        EXPECT_EQ(e.record().message, expected_msg);
    }
    catch (...) {
        FAIL() << "Lançou um tipo de exceção inesperado";
    }

    // Garante que o buffer foi esvaziado ao lançar a exceção
    EXPECT_TRUE(ErrorManager::flush().empty());

    Config::set(*original_cfg_ptr); // Restaura
}

// Testa se a macro FVMG_ERROR NÃO lança exceção com Policy::Status
TEST(ErrorHandlingTest, PoliticaDeStatus) {
    auto original_cfg_ptr = Config::get();
    ErrorConfig cfg;
    cfg.policy = Policy::Status; // Garante que NÃO vai lançar
    Config::set(cfg);

    // A macro FVMG_ERROR deve apenas logar, sem lançar
    EXPECT_NO_THROW({
        FVMG_ERROR(CoreErr::NotImplemented);
    });

    // O erro deve ter sido logado e estar no buffer
    auto errors = ErrorManager::flush();
    ASSERT_EQ(errors.size(), 1);
    EXPECT_EQ(errors[0].code, code(CoreErr::NotImplemented));
    EXPECT_EQ(errors[0].message, "Recurso não implementado.");

    Config::set(*original_cfg_ptr); // Restaura
}

// Testa a macro FVMG_ASSERT
TEST(ErrorHandlingTest, Assert) {
    auto original_cfg_ptr = Config::get();
    ErrorConfig cfg;
    cfg.policy = Policy::Throw; // Configura para lançar exceção
    cfg.language = Language::PtBR; // Garante mensagens em PT
    Config::set(cfg);

    // --- Parte 1: Asserção que passa ---
    EXPECT_NO_THROW({
        int x = 5;
        // Condição verdadeira, não deve fazer nada (nem logar, nem lançar)
        FVMG_ASSERT(x == 5, {{"value", std::to_string(x)}});
    });
    // Verifica que nenhum erro foi logado (buffer deve estar vazio)
    EXPECT_TRUE(ErrorManager::flush().empty()) << "Buffer não deveria conter erros após asserção verdadeira";


    // --- Parte 2: Asserção que falha ---
    try {
        int y = 10;
        std::string y_str = std::to_string(y);
        // Esta chamada deve logar o erro Fatal e lançar FVMGException
        FVMG_ASSERT(y < 5, {{"value", y_str}});
        // Se chegar aqui, o teste falhou (não lançou)
        FAIL() << "FVMG_ASSERT deveria ter lançado uma exceção";
    }
    catch (const FVMGException& e) { // Captura a exceção
        // Verifica o conteúdo DENTRO da exceção
        SUCCEED(); // Confirma que a exceção correta foi capturada
        EXPECT_EQ(e.code(), code(CoreErr::AssertFailed));
        EXPECT_EQ(e.severity(), Severity::Fatal);
        // Verifica a mensagem (pode incluir o valor de 'y' dependendo da implementação exata)
        EXPECT_NE(std::string(e.what()).find("Falha de asserção"), std::string::npos);
        // Exemplo de como verificar o argumento na mensagem (se foi implementado assim):
        // EXPECT_NE(std::string(e.what()).find("value=" + std::to_string(10)), std::string::npos);
    }
    catch (...) {
        FAIL() << "Lançou um tipo de exceção inesperado";
    }

    // --- Verificação Final ---
    // Após a exceção ser capturada e tratada, o buffer
    // thread-local deve estar vazio (pois o erro foi consumido pela exceção).
    EXPECT_TRUE(ErrorManager::flush().empty()) << "Buffer deveria estar vazio após exceção de Assert";

    Config::set(*original_cfg_ptr); // Restaura a configuração original
}