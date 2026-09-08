param([switch]$Check)
$ErrorActionPreference = "Stop"
Set-Location (Join-Path $PSScriptRoot "..")

$files = Get-ChildItem -Path src -Recurse -Include *.hpp,*.cpp -File
if ($files.Count -eq 0) { Write-Host "no source files"; exit 0 }

if ($Check) {
    $failed = $false
    foreach ($f in $files) {
        clang-format --dry-run --Werror $f.FullName
        if ($LASTEXITCODE -ne 0) { $failed = $true }
    }
    if ($failed) { exit 1 } else { exit 0 }
} else {
    foreach ($f in $files) { clang-format -i $f.FullName }
    Write-Host "formatted $($files.Count) files"
}
