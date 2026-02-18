# Conventions

## Coding Standards
- **Language:** C++17/20.
- **Naming:**
  - **Classes:** PascalCase (e.g., `DataManager`).
  - **Methods:** PascalCase (e.g., `AddNode`).
  - **Variables:** camelCase with `m_` prefix for members (e.g., `m_DataStorage`).
  - **Namespaces:** Primary namespace is `Imeka`.
- **Header Guards:** `#ifndef MODULE_FILENAME_HPP_INCLUDED`.

## Project Layout
- **Modules:** Headers in `Modules/[ModuleName]/[ModuleName]/`, implementation in `Modules/[ModuleName]/`.
- **Plugins:** Standard MITK/CTK plugin structure with `src/internal/` and `manifest_headers.cmake`.
