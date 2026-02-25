param(
    [switch]$Debug,
    [switch]$Clean
)

$config = if ($Debug) { 'Debug' } else { 'Release' }
$triplet = if ($Debug) { 'x64-windows' } else { 'x64-windows-static' }
$buildDir = 'build'

Write-Host ""
Write-Host "RayRedRaid Build Script" -ForegroundColor Cyan
Write-Host "Config: $config, Triplet: $triplet"
Write-Host ""

if ($Clean) {
    Write-Host "Cleaning build directory..."
    Remove-Item -Recurse -Force $buildDir -ErrorAction SilentlyContinue
}

if (-not (Test-Path $buildDir)) {
    New-Item -ItemType Directory $buildDir -Force | Out-Null
}

cd $buildDir

cmake ".." -G "Visual Studio 18 2026" -A x64 -DCMAKE_TOOLCHAIN_FILE="C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake" -DVCPKG_TARGET_TRIPLET="$triplet"

if ($LASTEXITCODE -ne 0) {
    Write-Host "CMAKE FAILED" -ForegroundColor Red
    cd ..
    exit 1
}

cmake --build . --config $config

if ($LASTEXITCODE -ne 0) {
    Write-Host "BUILD FAILED" -ForegroundColor Red
    cd ..
    exit 1
}

$exePath = "RayRedRaid\$config\RayRedRaid.exe"
if (Test-Path $exePath) {
    $size = [math]::Round((Get-Item $exePath).Length / 1MB, 2)
    Write-Host ""
    Write-Host "BUILD SUCCESSFUL!" -ForegroundColor Green
    Write-Host "Output: $((Get-Location).Path)\$exePath"
    Write-Host "Size: $size MB"
    Write-Host ""
} else {
    Write-Host "EXE not found: $exePath" -ForegroundColor Red
    cd ..
    exit 1
}

cd ..
