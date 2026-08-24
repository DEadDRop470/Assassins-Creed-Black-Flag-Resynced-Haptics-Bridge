$ErrorActionPreference = 'Stop'
$Expected = 'fb77b55728185e453fcc2046716f1d299bb78e92aededae906c9c56db58383b9'
$Path = Join-Path $PSScriptRoot 'Release\ACBFHapticsBridge-v7.asi'

if (-not (Test-Path $Path)) {
    Write-Host 'Missing Release\ACBFHapticsBridge-v7.asi'
    Write-Host 'Copy the exact NexusMods v2.1 ASI into Release\ first.'
    exit 2
}

$Actual = (Get-FileHash -Algorithm SHA256 $Path).Hash.ToLowerInvariant()
Write-Host "Expected: $Expected"
Write-Host "Actual:   $Actual"

if ($Actual -ne $Expected) {
    Write-Error 'Hash mismatch: this is NOT the expected r7/v2.1 ASI.'
}

python (Join-Path $PSScriptRoot 'Source\Tools\audit_r7.py') $Path
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

Write-Host 'v2.1/r7 verification PASS'
