# package_release.ps1

$BuildDir = "build"
$DistDir = "dist"
$AppName = "SimpleMediaSorter-MP"
$Version = "v1.0.0.5-Alpha"
$ReleaseName = "$AppName-$Version"
$TargetDir = Join-Path $DistDir $AppName

# 1. Prepare Release Directory
Write-Host "Preparing release directory in $TargetDir..." -ForegroundColor Cyan
if (Test-Path $DistDir) { Remove-Item -Recurse -Force $DistDir }
New-Item -ItemType Directory -Path $TargetDir -Force | Out-Null

# 2. Copy Build Files (Application + Dependencies)
Write-Host "Copying files from build..." -ForegroundColor Cyan
Copy-Item -Path "$BuildDir\*" -Destination $TargetDir -Recurse

# 3. Cleanup Build Artifacts (Keep only runtime files)
Write-Host "Cleaning up build artifacts..." -ForegroundColor Yellow
$ArtifactsToRemove = @(
    "CMakeFiles",
    "SimpleMediaSorter-MP_autogen",
    "*.cmake",
    "CMakeCache.txt",
    "Makefile",
    "*.ninja",
    "*.log",
    "*.a",
    "*.o"
)

foreach ($pattern in $ArtifactsToRemove) {
    Get-ChildItem -Path $TargetDir -Recurse -Filter $pattern | Remove-Item -Recurse -Force
}

# 4. Create ZIP Archive
$ZipFile = "$ReleaseName.zip"
Write-Host "Creating archive: $ZipFile..." -ForegroundColor Green
Compress-Archive -Path $TargetDir -DestinationPath $ZipFile -Force

Write-Host "Release package ready: $ZipFile" -ForegroundColor Green
Write-Host "You can upload this file to your GitHub Release." -ForegroundColor Cyan
