[CmdletBinding()]
param(
  [ValidateSet("Debug", "Release")]
  [string]$Configuration = "Debug",
  [string[]]$Modules = @(),
  [string]$VcpkgRoot = "",
  [switch]$Clean,
  [int]$Jobs = 8
)

$ErrorActionPreference = "Stop"
$TestRoot = $PSScriptRoot
$RepoRoot = Split-Path -Parent $TestRoot
$WorkspaceRoot = Split-Path -Parent $RepoRoot
$WindowsDependenciesRoot = Join-Path $WorkspaceRoot "windows-dependencies"
$CommonScript = Join-Path $WindowsDependenciesRoot "vcpkg-script\dali-build-common.ps1"
$SupportedModules = @("dali", "dali-internal")

if($Modules.Count -gt 0)
{
  $UnsupportedModules = @($Modules | Where-Object { $SupportedModules -notcontains $_ })
  if($UnsupportedModules.Count -gt 0)
  {
    throw "Unsupported Windows test module: $($UnsupportedModules -join ', '). Supported modules: $($SupportedModules -join ', ')"
  }
}

if(-not (Test-Path -LiteralPath $CommonScript))
{
  throw "windows-dependencies must be beside dali-core: $WindowsDependenciesRoot"
}
. $CommonScript

$Context = New-DaliBuildContext `
  -WindowsDependenciesRoot $WindowsDependenciesRoot `
  -VcpkgRoot $VcpkgRoot
Initialize-DaliBuildEnvironment -Context $Context

$CorePackage = Join-Path $Context.InstallPrefix "share\dali2-core"
Assert-DaliPaths -Paths @(
  (Join-Path $CorePackage "dali2-core-config.cmake")
) -Description "DALi core package; build dali-core first"

$Arguments = (Get-DaliCommonCMakeArguments -Context $Context -Configuration $Configuration) + @(
  "-Ddali2-core_DIR=$CorePackage"
)

Invoke-DaliCMakeProject `
  -Name "dali-core common UTC on Windows" `
  -SourceDirectory (Join-Path $TestRoot "windows-common") `
  -BuildDirectory (Join-Path $TestRoot "_build\windows") `
  -ConfigureArguments $Arguments `
  -Clean:$Clean `
  -Jobs $Jobs

Install-DaliRuntimeScripts -Context $Context
$TestExecutables = @(
  (Join-Path $Context.InstallPrefix "bin\tct-dali-core.exe"),
  (Join-Path $Context.InstallPrefix "bin\tct-dali-internal-core.exe")
)
Assert-DaliPaths -Paths $TestExecutables -Description "Windows core test executables"

Write-Host "`nDALi core common UTCs installed for Windows." -ForegroundColor Green
Write-Host "Run from the workspace root: .\dali-core\automated-tests\execute.ps1"
