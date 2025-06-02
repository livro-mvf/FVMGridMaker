# Gerador de Malhas Estruturadas (GridMaker)

## Descrição
Este repositório contém o código-fonte de um gerador de malhas estruturadas em C++20 chamado **GridMaker**, desenvolvido para acompanhar um livro sobre métodos de volumes finitos. O GridMaker fornece uma forma simples e flexível de construir malhas 1D e 2D (cartesiana, polar) — e futuramente 3D — sem dependências externas além da biblioteca padrão do C++. Sempre que possível, utilizam-se recursos de paralelização do C++20 para acelerar a geração da malha.

> **Escopo:**  
> GridMaker foi projetado **unicamente** para criar malhas estruturadas (grids) e não avançará além dessa funcionalidade. O objetivo é ser utilizado por quem está começando no desenvolvimento de modelos em volumes finitos.

Toda a documentação e comentários no código seguem o padrão Doxygen e estão em português.

---

## Principais Funcionalidades
- **Malhas Estruturadas 1D e 2D**  
  - Malha uniforme cartesiana (X, Y).  
  - Malha polar em 2D.  
- **Extensibilidade para 3D** (planejado para versões futuras).  
- **Design Moderno em C++20**  
  - Uso de templates e concepts para tratar dimensões e coordenadas.  
  - Contêineres da STL (`std::vector`, `std::array`) e algoritmos paralelos.  
- **Organização Modular**  
  - Cada tipo de malha fica em sua própria pasta.  
  - Fácil adicionar novos esquemas de discretização ou formatos de exportação.  
- **Exportação para VTK (Visualização)**  
  - Saída de malha compatível com ParaView e VisIt.  
- **Documentação**  
  - Comentários Doxygen espalhados pelo código.  
  - Geração automática de HTML via Doxygen.

---

## Dependências
- **Compilador C++20** (GCC ≥ 10, Clang ≥ 10 ou MSVC com suporte a C++20).  
- **CMake** ≥ 3.15  
- **Doxygen** (se for gerar documentação).  
- **Biblioteca STL padrão** (sem bibliotecas externas adicionais).

---

## Como Compilar

1. Clone este repositório:
   ```bash
   git clone https://github.com/SEU_USUARIO/GridMaker.git
   cd GridMaker
