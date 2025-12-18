# build_project.ps1

# -----------------------------------------------------------------------------
# CONFIGURATION - PLEASE UPDATE THESE PATHS
# -----------------------------------------------------------------------------

# 1. The root directory where Qt is installed.
$QtRootDir = "C:\Qt"

# 2. Your Qt version (e.g., 6.5.3, 6.7.0, 6.8.0). Check the folder name in C:\Qt.
$QtVersion = "6.10.1" 

# 3. The MinGW compiler version folder name in C:\Qt\Tools (e.g., mingw1120_64, mingw1310_64).
#    If you installed MinGW separately, set the full path to the 'bin' directory below.
$MinGWToolName = "mingw1310_64"

# -----------------------------------------------------------------------------
# SETUP ENVIRONMENT
# -----------------------------------------------------------------------------

Write-Host "Setting up environment..." -ForegroundColor Cyan

# Path to Qt libraries (bin folder contains DLLs)
$QtBinPath = Join-Path $QtRootDir "$QtVersion\mingw_64\bin"

# Path to MinGW compiler (g++, gcc, etc.)
$MinGWBinPath = Join-Path $QtRootDir "Tools\$MinGWToolName\bin"

# Check if paths exist
if (-not (Test-Path $QtBinPath)) {
    Write-Error "Qt bin path not found: $QtBinPath"
    Write-Warning "Please edit this script and update `$QtVersion` or `$QtRootDir`."
    exit 1
}

if (-not (Test-Path $MinGWBinPath)) {
    Write-Error "MinGW bin path not found: $MinGWBinPath"
    Write-Warning "Please edit this script and update `$MinGWToolName`."
    exit 1
}

# Add to PATH temporarily for this session
$Env:PATH = "$MinGWBinPath;$QtBinPath;$Env:PATH"

# Verify tools
if (Get-Command g++ -ErrorAction SilentlyContinue) {
    Write-Host "Found g++: $(Get-Command g++ | Select-Object -ExpandProperty Source)" -ForegroundColor Green
}
else {
    Write-Error "g++ not found even after updating PATH."
    exit 1
}

if (Get-Command cmake -ErrorAction SilentlyContinue) {
    Write-Host "Found cmake: $(Get-Command cmake | Select-Object -ExpandProperty Source)" -ForegroundColor Green
}
else {
    Write-Error "cmake not found. Please install CMake and add it to your system PATH."
    exit 1
}

# -----------------------------------------------------------------------------
# BUILD PROCESS
# -----------------------------------------------------------------------------

$BuildDir = "build"

# Clean build directory to ensure correct generator is used
if (Test-Path $BuildDir) {
    Write-Host "Cleaning previous build..." -ForegroundColor Yellow
    Remove-Item -Recurse -Force $BuildDir
}

New-Item -ItemType Directory -Path $BuildDir | Out-Null

Set-Location $BuildDir

Write-Host "`nConfiguring project..." -ForegroundColor Cyan
# We use -G "MinGW Makefiles" to ensure CMake uses MinGW
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug ..
if ($LASTEXITCODE -ne 0) { Write-Error "Configuration failed."; exit 1 }

Write-Host "`nBuilding project..." -ForegroundColor Cyan
cmake --build .
if ($LASTEXITCODE -ne 0) { Write-Error "Build failed."; exit 1 }

Write-Host "`nBuild successful!" -ForegroundColor Green

# -----------------------------------------------------------------------------
# RUN
# -----------------------------------------------------------------------------

$ExeName = "SMSorter.exe" # Based on CMakeLists.txt project name
if (Test-Path $ExeName) {
    Write-Host "Launching $ExeName..." -ForegroundColor Cyan
    & ".\$ExeName"
}
else {
    Write-Warning "Executable $ExeName not found in build directory."
}
