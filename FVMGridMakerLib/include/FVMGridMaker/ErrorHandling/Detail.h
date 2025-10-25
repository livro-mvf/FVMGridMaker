// ============================================================================
// File: Detail.h
// Author: FVMGridMaker Team
// Version: 1.0 (Adaptado)
// Description: Helpers internos para log e renderização (RNF08).
// License: GNU GPL v3
// ============================================================================
#pragma once


// ----------------------------------------------------------------------------
// includes FVMGridMaker
// ----------------------------------------------------------------------------
//#include <FVMGridMaker/ErrorHandling/ErrorConfig.h>
// #include <FVMGridMaker/ErrorHandling/ErrorManager.h>
// #include <FVMGridMaker/ErrorHandling/ErrorRecord.h>
// #include <FVMGridMaker/ErrorHandling/ErrorTraits.h>
// #include <FVMGridMaker/ErrorHandling/Language.h>
// #include <FVMGridMaker/ErrorHandling/Severity.h>

/**
 * @file Detail.h
 * @brief Funções auxiliares internas (não devem ser usadas diretamente).
 * @ingroup error
 */
FVMGRIDMAKER_NAMESPACE_OPEN
ERROR_NAMESPACE_OPEN
DETAIL_NAMESPACE_OPEN

/**
 * @brief Função interna para logar um erro com placeholders.
 * @tparam E O tipo do enum de erro (deve satisfazer ErrorEnum).
 * @param err_enum O valor do enum de erro.
 * @param kv Lista de inicialização de pares {chave, valor} para substituição.
 */
template <ErrorEnum E>
inline void log_error(
    E err_enum,
    std::initializer_list<
        std::pair<std::string_view, std::string>> kv = {})
{
    // Obtém a configuração atual
    auto cfg = Config::get();
    if (!cfg) return; // Segurança

    // Verifica se a severidade está acima do limiar configurado
    const Severity sev = ErrorTraits<E>::default_severity(err_enum);
    if (sev < cfg->min_severity) {
        return; // Abaixo do limiar, não loga
    }

    // Seleciona o template de mensagem baseado no idioma (RNF08)
    const Language lang = cfg->language;
    const std::string_view tmpl =
        (lang == Language::PtBR)
            ? ErrorTraits<E>::ptBR(err_enum)
            : ErrorTraits<E>::enUS(err_enum);

    // Renderiza a mensagem substituindo {key} por value
    std::string out(tmpl);
    for (const auto& [k, v] : kv) {
        std::string token = "{";
        token += k;
        token += "}";

        std::string::size_type pos = 0;
        while ((pos = out.find(token, pos)) != std::string::npos) {
            out.replace(pos, token.size(), v);
            pos += v.size(); // Avança após o valor inserido
        }
    }

    // Cria o registro e envia para o ErrorManager
    ErrorManager::log(ErrorRecord{
        .code = code(err_enum), .severity = sev, .message = std::move(out)});
}

DETAIL_NAMESPACE_CLOSE
ERROR_NAMESPACE_CLOSE
FVMGRIDMAKER_NAMESPACE_CLOSE