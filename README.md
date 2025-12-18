# SimpleMediaSorter-MP

**SimpleMediaSorter-MP** is a premium, modular media management suite built with C++ and Qt 6. It provides a powerful set of tools to organize, rename, backup, and process your media collection with a modern, responsive user interface.

![Version](https://img.shields.io/badge/version-1.0.0.5-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)
![Qt](https://img.shields.io/badge/Qt-6.10.1-41cd52.svg)

## ✨ Features

The application is divided into specialized modules, each following a safe **Config -> Plan -> Execute** workflow:

*   **📂 Renamer**: Batch rename files using flexible patterns and Regular Expressions. Supports live preview.
*   **📅 Sorter**: Automatically organize files into a date-based folder hierarchy (e.g., `YYYY/MM/DD`) based on file metadata.
*   **💾 Backup**: Copy files from source to destination while strictly preserving the original directory structure.
*   **🖼️ Image Processor**: Bulk resize and convert images (JPG, PNG, WEBP, BMP) with quality control.
*   **🎥 Video Transcoder**: Batch convert videos using FFmpeg. Supports various containers (MP4, MKV) and codecs (H.264, H.265, ProRes).

## 🛠️ Technology Stack

*   **Core**: C++20
*   **UI Framework**: Qt 6.10.1 (Widgets, SQL, Concurrent)
*   **Build System**: CMake 3.16+
*   **Compiler**: MinGW-w64 (GCC 13.1.0)
*   **Scripting**: Python (for build tooling/assets), PowerShell

## 🚀 Build & Deployment

### Prerequisites
*   **Qt 6.10.1** (MinGW 64-bit kit)
*   **CMake**
*   **Ninja** or **MinGW Makefiles**
*   **Python 3.x** (with `Pillow` library for icon generation)

### Building the Project
We provide a PowerShell script to automate the build and deployment process:

```powershell
.\build_project.ps1
```

This script will:
1.  Setup the build environment (Qt & MinGW paths).
2.  Clean previous build artifacts.
3.  Run `cmake` configuration.
4.  Compile the project.
5.  Run `windeployqt` to deploy all necessary DLLs and plugins.

### 📦 Standalone Release
After a successful build, the `build/` directory will contain a standalone version of the application:
*   `SimpleMediaSorter-MP.exe`
*   Required Qt DLLs and plugins.

You can copy this folder to any Windows machine to run the application without installing Qt.

## 📖 Usage

1.  **Launch** the application (`SimpleMediaSorter-MP.exe`).
2.  **Navigation**: Use the sidebar to switch between modules.
3.  **Workflow**:
    *   **Step 1**: Configure your inputs (Source folder, Options).
    *   **Step 2**: Click **Preview Plan** to generate a "Dry Run". Review the changes in the table.
    *   **Step 3**: Click **Execute** to apply the changes.
4.  **Status**: Monitor progress via the progress bar and log window.

## 🤝 Contributing

1.  Fork the repository.
2.  Create a feature branch (`git checkout -b feature/NewFeature`).
3.  Commit your changes.
4.  Push to the branch.
5.  Open a Pull Request.

## 📄 License

This project is licensed under the MIT License - see the LICENSE file for details.
