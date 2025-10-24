#pragma once
//==============================================================================
// Name         : namespace.h
// Project      : FVMGridMaker (FVMG)
// Description  : Helpers centralizados de namespace e documentação Doxygen.
//              - Raiz     : FVMGridMaker
//              - Core     : FVMGridMaker::core     (Grid, Tipos, Iteradores)
//              - Policies : FVMGridMaker::policies (Distribuição, Discretização)
//              - Builders : FVMGridMaker::builders (GridBuilder fluente)
//              - Geometry : FVMGridMaker::geometry (Máscaras, Periodicidade)
//              - IO       : FVMGridMaker::io       (Exportadores VTK, CSV)
//              - Error    : FVMGridMaker::error    (Exceções em Português)
// License      : GNU GPL v3
// Version      : 1.0.0
//==============================================================================

/**
 * @file namespace.h
 * @brief Utilitários de namespace e macros para o projeto FVMGridMaker.
 *
 * Namespace raiz: **FVMGridMaker**.
 * Sub-namespaces:
 * - **core** : Estruturas centrais (Grid, Tipos, Iteradores)
 * - **policies** : Políticas (ex: Distribuição, Discretização)
 * - **builders** : Construtor fluente da malha
 * - **geometry** : Cálculos geométricos, máscaras, periodicidade
 * - **io** : Exportadores de arquivos (VTK, CSV)
 * - **error** : Tratamento de exceções (RNF08)
 * - **detail** : Detalhes internos de implementação
 *
 * Macros fornecidas:
 * - FVMGRIDMAKER_NAMESPACE_OPEN / FVMGRIDMAKER_NAMESPACE_CLOSE
 * - CORE_NAMESPACE_OPEN / CORE_NAMESPACE_CLOSE
 * - POLICIES_NAMESPACE_OPEN / POLICIES_NAMESPACE_CLOSE
 * - BUILDERS_NAMESPACE_OPEN / BUILDERS_NAMESPACE_CLOSE
 * - GEOMETRY_NAMESPACE_OPEN / GEOMETRY_NAMESPACE_CLOSE
 * - IO_NAMESPACE_OPEN / IO_NAMESPACE_CLOSE
 * - ERROR_NAMESPACE_OPEN / ERROR_NAMESPACE_CLOSE
 * - DETAIL_NAMESPACE_OPEN / DETAIL_NAMESPACE_CLOSE
 */

//------------------------------------------------------------------------------
// Raiz: FVMGridMaker
//------------------------------------------------------------------------------
#ifndef FVMGRIDMAKER_NAMESPACE_OPEN
#   define FVMGRIDMAKER_NAMESPACE_OPEN  namespace FVMGridMaker {
#endif
#ifndef FVMGRIDMAKER_NAMESPACE_CLOSE
#   define FVMGRIDMAKER_NAMESPACE_CLOSE } /* namespace FVMGridMaker */
#endif

//------------------------------------------------------------------------------
// Core: FVMGridMaker::core
//------------------------------------------------------------------------------
#ifndef CORE_NAMESPACE_OPEN
#   define CORE_NAMESPACE_OPEN  namespace core {
#endif
#ifndef CORE_NAMESPACE_CLOSE
#   define CORE_NAMESPACE_CLOSE } /* namespace core */
#endif

//------------------------------------------------------------------------------
// Policies: FVMGridMaker::policies
//------------------------------------------------------------------------------
#ifndef POLICIES_NAMESPACE_OPEN
#   define POLICIES_NAMESPACE_OPEN  namespace policies {
#endif
#ifndef POLICIES_NAMESPACE_CLOSE
#   define POLICIES_NAMESPACE_CLOSE } /* namespace policies */
#endif

//------------------------------------------------------------------------------
// Builders: FVMGridMaker::builders
//------------------------------------------------------------------------------
#ifndef BUILDERS_NAMESPACE_OPEN
#   define BUILDERS_NAMESPACE_OPEN  namespace builders {
#endif
#ifndef BUILDERS_NAMESPACE_CLOSE
#   define BUILDERS_NAMESPACE_CLOSE } /* namespace builders */
#endif

//------------------------------------------------------------------------------
// Geometry: FVMGridMaker::geometry
//------------------------------------------------------------------------------
#ifndef GEOMETRY_NAMESPACE_OPEN
#   define GEOMETRY_NAMESPACE_OPEN  namespace geometry {
#endif
#ifndef GEOMETRY_NAMESPACE_CLOSE
#   define GEOMETRY_NAMESPACE_CLOSE } /* namespace geometry */
#endif

//------------------------------------------------------------------------------
// IO: FVMGridMaker::io
//------------------------------------------------------------------------------
#ifndef IO_NAMESPACE_OPEN
#   define IO_NAMESPACE_OPEN  namespace io {
#endif
#ifndef IO_NAMESPACE_CLOSE
#   define IO_NAMESPACE_CLOSE } /* namespace io */
#endif

//------------------------------------------------------------------------------
// Error: FVMGridMaker::error
//------------------------------------------------------------------------------
#ifndef ERROR_NAMESPACE_OPEN
#   define ERROR_NAMESPACE_OPEN  namespace error {
#endif
#ifndef ERROR_NAMESPACE_CLOSE
#   define ERROR_NAMESPACE_CLOSE } /* namespace error */
#endif

//------------------------------------------------------------------------------
// Detail: (para ser usado dentro de outros namespaces, ex: core::detail)
//------------------------------------------------------------------------------
#ifndef DETAIL_NAMESPACE_OPEN
#   define DETAIL_NAMESPACE_OPEN  namespace detail {
#endif
#ifndef DETAIL_NAMESPACE_CLOSE
#   define DETAIL_NAMESPACE_CLOSE } /* namespace detail */
#endif

//------------------------------------------------------------------------------
// Error: (para ser usado dentro de outros namespaces, ex: core::error)
//------------------------------------------------------------------------------
#ifndef ERROR_NAMESPACE_OPEN
#   define ERROR_NAMESPACE_OPEN  namespace error {
#endif
#ifndef ERROR_NAMESPACE_CLOSE
#   define ERROR_NAMESPACE_CLOSE } /* namespace error */
#endif
