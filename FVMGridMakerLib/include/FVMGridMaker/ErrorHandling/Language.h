// ----------------------------------------------------------------------------
// File: Language.h
// Author: FVMGridMaker Team
// Version: 1.0 
// Description: Seleção de idioma para mensagens de erro (RNF08).
// License: GNU GPL v3
// ----------------------------------------------------------------------------


#pragma once

// ----------------------------------------------------------------------------
// includes FVMGridMaker
// ----------------------------------------------------------------------------
#include <FVMGridMaker/Core/namespace.h>

/**
 * @file Language.h
 * @brief Tag de idioma para mensagens localizadas.
 * @ingroup error
 */

 FVMGRIDMAKER_NAMESPACE_OPEN
 ERROR_NAMESPACE_OPEN
/** @brief Idiomas suportados para localização. */
enum class Language {
    EnUS,  ///< Inglês (US)
    PtBR   ///< Português (Brasil)
};

ERROR_NAMESPACE_CLOSE
FVMGRIDMAKER_NAMESPACE_CLOSE

