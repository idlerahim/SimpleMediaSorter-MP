# Project Wiki: SimpleMediaSorter-MP

This document outlines the project's evolution, technical architecture, and roadmap.

## 1. Simple Media Sorter (Legacy Version)

The original version of this project was a collection of Python scripts designed to automate media organization.

### Key Features (Python)
*   **Media Organization**: Sorts files into date-based directories (e.g., `2023/12/25`).
*   **Renaming**: Renames files based on EXIF metadata (Date Taken).
*   **Cleaning**: Removes duplicate or empty directories.

### Roadmap for Legacy
*   **PIP Package**: The goal is to package the legacy python scripts into a distributable Python package published on PyPI.
    *   *Plan*: Organize scripts into a `src` layout, add `pyproject.toml`, and use `twine` to upload.

---

## 2. SimpleMediaSorter-MP (Modern Platform)

This is the current "MP" (Modern/Multi-Platform) iteration, rewritten from scratch to provide a robust, user-friendly desktop application.

### Type Stack & Philosophy
*   **Open Source Core**: Built strictly using open-source technologies.
    *   **Language**: C++20
    *   **Framework**: Qt 6.10.1 (LGPL/GPL)
    *   **Build System**: CMake

### Architecture & Features

#### A. User Interface (UI)
*   **Modern Layout**: Replaced CLI scripts with a responsive "Sidebar + Workspace" GUI.
*   **Modules**:
    *   **Renamer**: Regex and pattern-based renaming.
    *   **Sorter**: Date hierarchy organization.
    *   **Backup**: Structural cleaning and copying.
    *   **Image/Video**: Transcoding and processing.

#### B. Multithreading & Performance
*   **Non-Blocking UI**: All heavy operations (File I/O, Transcoding) run on background threads using `QThreadPool`.
*   **Worker Pattern**: A standardized `Worker` class handles `plan()` (preview) and `execute()` (run) phases separate from the UI thread.
*   **Concurrency**: Capable of processing thousands of files without freezing the interface.

### Release Strategy: Alpha (Windows)
*   **Standalone Executable**: The Windows Release is built to be "portable".
*   **No Dependencies Required**: We use `windeployqt` to bundle all necessary Qt DLLs and plugins with the `.exe`.
*   **Direct Run**: Users can unzip the release and run `SimpleMediaSorter-MP.exe` immediately, with no installation of Python or Qt required.

---

## 3. Comparison

| Feature | Legacy (Python) | Modern (MP - C++) |
| :--- | :--- | :--- |
| **Interface** | Command Line (CLI) | Graphical (GUI) |
| **Performance** | Single-threaded | Multi-threaded |
| **Deployment** | Requires Python env | Standalone EXE |
| **Safety** | Immediate Action | Plan/Preview Mode |
