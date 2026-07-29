[CmdletBinding()]
param(
  [ValidateSet("Debug", "Release")]
  [string]$Configuration = "Release",
  [string]$VcpkgRoot = "",
  [switch]$Clean,
  [int]$Jobs = 8
)

$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$WorkspaceRoot = Split-Path -Parent $RepoRoot
$WindowsDependenciesRoot = Join-Path $WorkspaceRoot "windows-dependencies"
$CommonScript = Join-Path $WindowsDependenciesRoot "vcpkg-script\dali-build-common.ps1"
if(-not (Test-Path -LiteralPath $CommonScript))
{
  throw "windows-dependencies must be beside dali-core: $WindowsDependenciesRoot"
}
. $CommonScript

$Context = New-DaliBuildContext -WindowsDependenciesRoot $WindowsDependenciesRoot -VcpkgRoot $VcpkgRoot
Initialize-DaliBuildEnvironment -Context $Context
$WindowsDependenciesPackage = Join-Path $Context.SdkRoot "share\dali-windows-dependencies"
Assert-DaliPaths -Paths @(
  (Join-Path $WindowsDependenciesPackage "dali-windows-dependencies-config.cmake")
) -Description "WindowsDependenciesSDK; run windows-dependencies\install.ps1 first"

$Arguments = (Get-DaliCommonCMakeArguments -Context $Context -Configuration $Configuration) + @(
  "-DENABLE_PKG_CONFIGURE=OFF",
  "-DENABLE_LINK_TEST=OFF",
  "-DINSTALL_CMAKE_MODULES=ON",
  "-Ddali-windows-dependencies_DIR=$WindowsDependenciesPackage"
)
Invoke-DaliCMakeProject `
  -Name "dali-core" `
  -SourceDirectory (Join-Path $RepoRoot "build\tizen") `
  -BuildDirectory (Join-Path $RepoRoot "_build\windows") `
  -ConfigureArguments $Arguments `
  -Clean:$Clean `
  -Jobs $Jobs

Install-DaliRuntimeScripts -Context $Context
Assert-DaliPaths -Paths @(
  (Join-Path $Context.InstallPrefix "bin\dali2-core.dll"),
  (Join-Path $Context.InstallPrefix "share\dali2-core\dali2-core-config.cmake")
) -Description "dali-core installation output"
Write-Host "`ndali-core installed in $($Context.InstallPrefix)." -ForegroundColor Green
