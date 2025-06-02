# Structured Mesh Generator (GridMaker)

## Description
This repository contains the source code for a C++20 structured mesh generator named **GridMaker**, developed to accompany a book on finite volume methods. GridMaker provides a simple and flexible way to construct 1D and 2D (Cartesian, polar) meshes—and will later extend to 3D—without any external dependencies beyond the C++ standard library. Whenever possible, C++20 parallelization features are used to accelerate mesh generation.

> **Scope:**  
> GridMaker is designed **solely** to create structured meshes (grids) and will not advance beyond this functionality. Its purpose is to be used by individuals who are beginning to develop finite volume models.

All documentation and comments in the code follow the Doxygen standard and are written in English where applicable, while general documentation (outside code) is presented in Portuguese.

---

## Main Features
- **1D and 2D Structured Meshes**  
  - Uniform Cartesian mesh (X, Y).  
  - Polar mesh in 2D.  
- **3D Extensibility** (planned for future releases).  
- **Modern C++20 Design**  
  - Use of templates and concepts to handle dimensions and coordinates.  
  - STL containers (`std::vector`, `std::array`) and parallel algorithms.  
- **Modular Organization**  
  - Each mesh type is organized in its own folder.  
  - Easy to add new discretization schemes or export formats.  
- **VTK Export for Visualization**  
  - Mesh output compatible with ParaView and VisIt.  
- **Documentation**  
  - Doxygen comments throughout the code.  
  - Automatic HTML generation via Doxygen.

---

## Dependencies
- **C++20 Compiler** (GCC ≥ 10, Clang ≥ 10, or MSVC with C++20 support).  
- **CMake** ≥ 3.15  
- **Doxygen** (if documentation generation is needed).  
- **Standard STL** (no external libraries required).

---

## Build Instructions

1. Clone this repository:
   ```bash
   git clone https://github.com/YOUR_USERNAME/GridMaker.git
   cd GridMaker
