// ----------------------------------------------------------------------------
// File: namespace.h
// Author: FVMGridMaker Team
// Version: 1.2
// Date: 2025-10-26
// Description: Macros para abrir/fechar a hierarquia de namespaces usada
//              pelo projeto FVMGridMaker, evitando erros de escopo e
//              repetição de código em .cpp/.hpp. Inclui atalhos para
//              'detail' e variantes compostas (ex.: distribution::detail).
// License: GNU GPL v3
// ----------------------------------------------------------------------------

/**
 * @file    namespace.h
 * @author  FVMGridMaker Team
 * @version 1.2
 * @date    2025-10-26
 *
 * @brief   Macros auxiliares para abrir/fechar a hierarquia de namespaces do
 *          FVMGridMaker, reduzindo boilerplate e erros de escopo.
 *
 * @details
 * Define:
 *  - Atalhos de nomes (grid, grid1d, patterns…)
 *  - Macros OPEN/CLOSE para cada nível de namespace
 *  - Blocos compostos úteis (grid1d::builders, …)
 *
 * Observação sobre documentação:
 *  - Para citar a sintaxe de Doxygen literalmente dentro deste bloco, evite
 *    escrever diretamente a sequência que inicia/encerra comentários aninhados.
 *    Se necessário, use a forma escapada para não ativar o aviso do compilador.
 */

#pragma once

// ----------------------------------------------------------------------------
// Nomes "curtos" dos namespaces
// ----------------------------------------------------------------------------
#define FVMGRIDMAKER_NS        FVMGridMaker
#define GRID_NS                grid
#define GRID1D_NS              grid1d
#define API_NS                 api
#define BUILDERS_NS            builders
#define UTILS_NS               utils
#define PATTERNS_NS            patterns
#define CENTERING_NS           centering
#define DISTRIBUTION_NS        distribution
#define ERROR_NS               error
#define CORE_NS                core
#define DETAIL_NS              detail

// ----------------------------------------------------------------------------
// Aberturas (OPEN)
// ----------------------------------------------------------------------------
#define FVMGRIDMAKER_NAMESPACE_OPEN           namespace FVMGRIDMAKER_NS {
#define GRID_NAMESPACE_OPEN                   namespace GRID_NS {
#define GRID1D_NAMESPACE_OPEN                 namespace GRID1D_NS {
#define API_NAMESPACE_OPEN                    namespace API_NS {
#define BUILDERS_NAMESPACE_OPEN               namespace BUILDERS_NS {
#define UTILS_NAMESPACE_OPEN                  namespace UTILS_NS {
#define PATTERNS_NAMESPACE_OPEN               namespace PATTERNS_NS {
#define CENTERING_NAMESPACE_OPEN              namespace CENTERING_NS {
#define DISTRIBUTION_NAMESPACE_OPEN           namespace DISTRIBUTION_NS {
#define ERROR_NAMESPACE_OPEN                  namespace ERROR_NS {
#define CORE_NAMESPACE_OPEN                   namespace CORE_NS {
#define DETAIL_NAMESPACE_OPEN                 namespace DETAIL_NS {

// ----------------------------------------------------------------------------
// Fechamentos (CLOSE)
// ----------------------------------------------------------------------------
#define FVMGRIDMAKER_NAMESPACE_CLOSE          }
#define GRID_NAMESPACE_CLOSE                  }
#define GRID1D_NAMESPACE_CLOSE                }
#define API_NAMESPACE_CLOSE                   }
#define BUILDERS_NAMESPACE_CLOSE              }
#define UTILS_NAMESPACE_CLOSE                 }
#define PATTERNS_NAMESPACE_CLOSE              }
#define CENTERING_NAMESPACE_CLOSE             }
#define DISTRIBUTION_NAMESPACE_CLOSE          }
#define ERROR_NAMESPACE_CLOSE                 }
#define CORE_NAMESPACE_CLOSE                  }
#define DETAIL_NAMESPACE_CLOSE                }

// ----------------------------------------------------------------------------
// Conveniência: blocos compostos comuns
// ----------------------------------------------------------------------------
// grid1d::builders
#define FVMG_GRID1D_BUILDERS_OPEN   FVMGRIDMAKER_NAMESPACE_OPEN GRID_NAMESPACE_OPEN GRID1D_NAMESPACE_OPEN BUILDERS_NAMESPACE_OPEN
#define FVMG_GRID1D_BUILDERS_CLOSE  BUILDERS_NAMESPACE_CLOSE GRID1D_NAMESPACE_CLOSE GRID_NAMESPACE_CLOSE FVMGRIDMAKER_NAMESPACE_CLOSE

// grid1d::api
#define FVMG_GRID1D_API_OPEN        FVMGRIDMAKER_NAMESPACE_OPEN GRID_NAMESPACE_OPEN GRID1D_NAMESPACE_OPEN API_NAMESPACE_OPEN
#define FVMG_GRID1D_API_CLOSE       API_NAMESPACE_CLOSE GRID1D_NAMESPACE_CLOSE GRID_NAMESPACE_CLOSE FVMGRIDMAKER_NAMESPACE_CLOSE

// grid1d::utils
#define FVMG_GRID1D_UTILS_OPEN      FVMGRIDMAKER_NAMESPACE_OPEN GRID_NAMESPACE_OPEN GRID1D_NAMESPACE_OPEN UTILS_NAMESPACE_OPEN
#define FVMG_GRID1D_UTILS_CLOSE     UTILS_NAMESPACE_CLOSE GRID1D_NAMESPACE_CLOSE GRID_NAMESPACE_CLOSE FVMGRIDMAKER_NAMESPACE_CLOSE

// grid1d::patterns (raiz dos padrões)
#define FVMG_GRID1D_PATTERNS_OPEN   FVMGRIDMAKER_NAMESPACE_OPEN GRID_NAMESPACE_OPEN GRID1D_NAMESPACE_OPEN PATTERNS_NAMESPACE_OPEN
#define FVMG_GRID1D_PATTERNS_CLOSE  PATTERNS_NAMESPACE_CLOSE GRID1D_NAMESPACE_CLOSE GRID_NAMESPACE_CLOSE FVMGRIDMAKER_NAMESPACE_CLOSE

// grid1d::patterns::distribution
#define FVMG_GRID1D_DIST_OPEN       FVMGRIDMAKER_NAMESPACE_OPEN GRID_NAMESPACE_OPEN GRID1D_NAMESPACE_OPEN PATTERNS_NAMESPACE_OPEN DISTRIBUTION_NAMESPACE_OPEN
#define FVMG_GRID1D_DIST_CLOSE      DISTRIBUTION_NAMESPACE_CLOSE PATTERNS_NAMESPACE_CLOSE GRID1D_NAMESPACE_CLOSE GRID_NAMESPACE_CLOSE FVMGRIDMAKER_NAMESPACE_CLOSE

// grid1d::patterns::centering
#define FVMG_GRID1D_CENTERING_OPEN  FVMGRIDMAKER_NAMESPACE_OPEN GRID_NAMESPACE_OPEN GRID1D_NAMESPACE_OPEN PATTERNS_NAMESPACE_OPEN CENTERING_NAMESPACE_OPEN
#define FVMG_GRID1D_CENTERING_CLOSE CENTERING_NAMESPACE_CLOSE PATTERNS_NAMESPACE_CLOSE GRID1D_NAMESPACE_CLOSE GRID_NAMESPACE_CLOSE FVMGRIDMAKER_NAMESPACE_CLOSE

// error
#define FVMG_ERROR_OPEN             FVMGRIDMAKER_NAMESPACE_OPEN ERROR_NAMESPACE_OPEN
#define FVMG_ERROR_CLOSE            ERROR_NAMESPACE_CLOSE FVMGRIDMAKER_NAMESPACE_CLOSE

// core
#define FVMG_CORE_OPEN              FVMGRIDMAKER_NAMESPACE_OPEN CORE_NAMESPACE_OPEN
#define FVMG_CORE_CLOSE             CORE_NAMESPACE_CLOSE FVMGRIDMAKER_NAMESPACE_CLOSE

// ----------------------------------------------------------------------------
// Conveniência extra: distribution::detail (muito usado em geradores)
// ----------------------------------------------------------------------------
#define FVMG_GRID1D_DIST_DETAIL_OPEN   FVMGRIDMAKER_NAMESPACE_OPEN GRID_NAMESPACE_OPEN GRID1D_NAMESPACE_OPEN PATTERNS_NAMESPACE_OPEN DISTRIBUTION_NAMESPACE_OPEN DETAIL_NAMESPACE_OPEN
#define FVMG_GRID1D_DIST_DETAIL_CLOSE  DETAIL_NAMESPACE_CLOSE DISTRIBUTION_NAMESPACE_CLOSE PATTERNS_NAMESPACE_CLOSE GRID1D_NAMESPACE_CLOSE GRID_NAMESPACE_CLOSE FVMGRIDMAKER_NAMESPACE_CLOSE
