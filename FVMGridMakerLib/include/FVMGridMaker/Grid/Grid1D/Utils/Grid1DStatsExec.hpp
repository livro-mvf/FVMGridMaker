#pragma once
// ============================================================================
// Arquivo   : Grid1DStatsExec.hpp
// Projeto   : FVMGridMaker
// Módulo    : Grid / Grid1D / Utils
// Descrição : Estatísticas "básicas" com política de execução SERIAL/PARALELA,
//             com fallback automático e uma única fonte de verdade.
// Versão    : 1.2
// Data      : 2025-10-26
//
// Leia antes de usar:
//
// ► O que este utilitário faz
//   Expõe uma função única (`basic_exec`) para calcular as estatísticas
//   básicas de dXFace (min, max, mean, std, aspect, CV). Quando houver
//   suporte a Parallel STL (PSTL) e backend (ex.: oneTBB) o cálculo
//   "básico" pode rodar em paralelo. Caso contrário, cai no caminho
//   SERIAL chamando `Grid1DStats::basic(...)`.
//
// ► Como escolho SERIAL ou PARALELO?
//   - Em compilação: a presença do macro `FVMG_HAVE_PSTL_EXEC` indica
//     que este TU pode compilar a via paralela. Um .cpp pode "forçar
//     serial" localmente com `#undef FVMG_HAVE_PSTL_EXEC` (ex.: usando
//     o controle `FVMG_EX_ENABLE_PAR=0` mostrado no exemplo).
//   - Em execução: o chamador escolhe a política via `ExecPolicy`
//     (`Auto`, `Serial` ou `Parallel`). Em `Auto`, usa paralelo se
//     disponível; caso contrário, serial. Em `Parallel`, solicita PSTL;
//     se indisponível, faz fallback para serial.
//
// ► Pré-requisitos para paralelismo real
//   - Instalar e linkar um backend PSTL (ex.: oneTBB).
//     * Ubuntu/WSL: `sudo apt install libtbb-dev`
//     * CMake do projeto: habilitar o link (ex.: `-DFVMG_ENABLE_TBB=ON`),
//       e `target_link_libraries(... TBB::tbb)` em quem usar.
//
// ► Observações
//   - Apenas o "núcleo" básico entra em PSTL (reduções min/max/somas).
//     As demais métricas (uniformidade, adjacência, etc.) continuam
//     disponíveis via `Grid1DStats` como antes (normalmente seriais).
// ============================================================================

// ----------------------------------------------------------------------------
// includes FVMGridMaker (ordem alfabética por caminho)
// ----------------------------------------------------------------------------
#include <FVMGridMaker/Core/type.h>
#include <FVMGridMaker/Grid/Grid1D/API/Grid1D.h>
#include <FVMGridMaker/Grid/Grid1D/Utils/Grid1DStats.hpp>

// ----------------------------------------------------------------------------
// includes C++ (ordem alfabética)
// ----------------------------------------------------------------------------
#include <cmath>
#ifdef FVMG_HAVE_PSTL_EXEC
  #include <execution>
#endif
#include <limits>
#include <numeric>
#include <type_traits>
#include <utility>

namespace FVMGridMaker::grid::grid1d::utils {

/**
 * @brief Políticas de execução para as estatísticas básicas.
 *
 * - `Serial`   : força caminho serial.
 * - `Parallel` : solicita PSTL; se indisponível neste TU, cai p/ serial.
 * - `Auto`     : usa PSTL se disponível; senão serial.
 */
enum class ExecPolicy { Auto, Serial, Parallel };

/**
 * @brief Informa se *este TU* foi compilado com suporte a PSTL.
 *
 * @details
 * O valor reflete a visibilidade do macro `FVMG_HAVE_PSTL_EXEC` **neste**
 * ponto de compilação (TU). Um `.cpp` pode "desligar" PSTL localmente,
 * por exemplo via:
 *
 * @code
 * #if !FVMG_EX_ENABLE_PAR
 *   #undef FVMG_HAVE_PSTL_EXEC
 * #endif
 * @endcode
 *
 * @return `true` se PSTL está disponível neste TU; `false` caso contrário.
 */
[[nodiscard]] constexpr bool has_parallel() noexcept {
#ifdef FVMG_HAVE_PSTL_EXEC
  return true;
#else
  return false;
#endif
}

/**
 * @brief Tipo de retorno igual ao de `Grid1DStats::basic(grid)`.
 *
 * @note Mantém compatibilidade com o restante da biblioteca.
 */
using BasicReturnT =
    decltype(Grid1DStats::basic(std::declval<const api::Grid1D&>()));

/**
 * @brief Estatísticas básicas com política de execução e fallback automático.
 *
 * @param grid               Instância de malha 1D.
 * @param policy             `ExecPolicy::Auto` (padrão), `Serial` ou `Parallel`.
 * @param used_parallel_out  (opcional) devolve `true` se PSTL foi usado.
 *
 * @details
 * - Se `policy == Serial`, usa o caminho serial da biblioteca
 *   (`Grid1DStats::basic(grid)`).
 * - Se `policy == Parallel` e o TU tem PSTL (`has_parallel()`), usa
 *   transform_reduce paralelo; caso contrário, faz fallback para serial.
 * - Se `policy == Auto`, usa PSTL quando disponível; senão serial.
 *
 * @warning Apenas o "núcleo" de min/max/somas é paralelizado; demais
 * métricas continuam disponíveis nas funções auxiliares de `Grid1DStats`.
 *
 * @return Estrutura com {min, max, mean, stddev, aspect, cv}.
 */
inline BasicReturnT basic_exec(const api::Grid1D& grid,
                               ExecPolicy policy [[maybe_unused]] = ExecPolicy::Auto,
                               bool* used_parallel_out = nullptr) {
  using core::Real;

  // Entrada vazia → delega ao caminho serial (comportamento consistente).
  const auto dF = grid.deltasFaces();
  if (dF.empty()) {
    if (used_parallel_out) *used_parallel_out = false;
    return Grid1DStats::basic(grid);
  }

#ifdef FVMG_HAVE_PSTL_EXEC
  const bool want_par =
      (policy == ExecPolicy::Parallel) ||
      (policy == ExecPolicy::Auto && has_parallel());
  if (want_par) {
    // --- Núcleo paralelo: min/max/soma/soma2 com PSTL ----------------------
    struct Acc {
      Real        minv = std::numeric_limits<Real>::infinity();
      Real        maxv = -std::numeric_limits<Real>::infinity();
      long double sum  = 0.0L;
      long double ss   = 0.0L;
    };
    auto unary = [](Real x) -> Acc {
      Acc a;
      a.minv = x;
      a.maxv = x;
      a.sum  = static_cast<long double>(x);
      a.ss   = static_cast<long double>(x) * static_cast<long double>(x);
      return a;
    };
    auto join = [](const Acc& a, const Acc& b) -> Acc {
      Acc r;
      r.minv = std::min(a.minv, b.minv);
      r.maxv = std::max(a.maxv, b.maxv);
      r.sum  = a.sum + b.sum;
      r.ss   = a.ss  + b.ss;
      return r;
    };

    const std::size_t n = dF.size();
    Acc init{};
    const Acc r = std::transform_reduce(std::execution::par,
                                        dF.begin(), dF.end(),
                                        init, join, unary);

    const Real mean   =
        static_cast<Real>(r.sum / static_cast<long double>(n));
    const Real var    =
        static_cast<Real>(r.ss  / static_cast<long double>(n)
                          - static_cast<long double>(mean) *
                            static_cast<long double>(mean));
    const Real stddev = (var > Real(0)) ? std::sqrt(var) : Real(0);
    const Real aspect = (r.minv > Real(0)) ? (r.maxv / r.minv) : Real(0);
    const Real cv     = (mean  > Real(0)) ? (stddev / mean)    : Real(0);

    if (used_parallel_out) *used_parallel_out = true;

    BasicReturnT out{};
    out.min    = r.minv;
    out.max    = r.maxv;
    out.mean   = mean;
    out.stddev = stddev;
    out.aspect = aspect;
    out.cv     = cv;
    return out;
  }
#endif // FVMG_HAVE_PSTL_EXEC

  // --- Serial (fonte única de verdade na biblioteca) -----------------------
  if (used_parallel_out) *used_parallel_out = false;
  return Grid1DStats::basic(grid);
}

} // namespace FVMGridMaker::grid::grid1d::utils
