[CmdletBinding()]
param(
  [ValidateSet("Debug", "Release")]
  [string]$Configuration = "Debug",
  [string[]]$Modules = @(),
  [string]$VcpkgRoot = "",
  [string]$TestCase = "",
  [string]$Prefix = "",
  [string[]]$Labels = @(),
  [ValidateRange(1, 3600)]
  [int]$TimeoutSeconds = 30,
  [switch]$List,
  [switch]$Quiet,
  [string]$ResultsDirectory = ""
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
$TestRunners = @(
  [pscustomobject]@{ Module = "dali"; Executable = Join-Path $Context.InstallPrefix "bin\tct-dali-core.exe" }
  [pscustomobject]@{ Module = "dali-internal"; Executable = Join-Path $Context.InstallPrefix "bin\tct-dali-internal-core.exe" }
)
if($Modules.Count -gt 0)
{
  $TestRunners = @($TestRunners | Where-Object { $Modules -contains $_.Module })
}
$TestExecutables = @($TestRunners | ForEach-Object { $_.Executable })
Assert-DaliPaths -Paths $TestExecutables -Description "Windows core test executables; run automated-tests\build.ps1 first"

$RuntimePaths = @(
  (Join-Path $Context.InstallPrefix "bin"),
  (Join-Path $Context.InstallPrefix "lib"),
  (Join-Path $Context.InstallPrefix "bin\debug"),
  (Join-Path $Context.InstallPrefix "lib\debug"),
  (Join-Path $Context.InstallPrefix "bin\release"),
  (Join-Path $Context.InstallPrefix "lib\release"),
  (Join-Path $Context.SdkRoot "bin"),
  (Join-Path $Context.SdkRoot "lib"),
  (Join-Path $Context.VcpkgRoot "installed\x64-windows\debug\bin"),
  (Join-Path $Context.VcpkgRoot "installed\x64-windows\bin")
) | Where-Object { $_ -and (Test-Path -LiteralPath $_) }
$ExistingPaths = @($env:PATH -split ";" | Where-Object { $_ })
$env:PATH = (@($RuntimePaths) + $ExistingPaths | Select-Object -Unique) -join ";"
$env:DALI_WINDOWS_SDK_ROOT = $Context.SdkRoot
$env:DALI_PREFIX = $Context.InstallPrefix
$env:DALI_CONFIGURATION = $Configuration
$env:DESKTOP_PREFIX = $Context.InstallPrefix
$env:DALI_DATA_RO_DIR = Join-Path $Context.InstallPrefix "share\dali"
$env:DALI_DATA_RW_DIR = Join-Path $Context.InstallPrefix "share\dali"
$env:DALI_DATA_RO_INSTALL_DIR = Join-Path $Context.InstallPrefix "share\dali"
$env:FONTCONFIG_PATH = Join-Path $Context.SdkRoot "share\dali"
$env:FONTCONFIG_FILE = Join-Path $env:FONTCONFIG_PATH "fonts.conf"

$Tests = @(
  foreach($Runner in $TestRunners)
  {
    $TestLines = @(& $Runner.Executable --list)
    if($LASTEXITCODE -ne 0)
    {
      throw "Failed to list DALi core UTCs from $($Runner.Executable) (exit code $LASTEXITCODE)."
    }
    foreach($Line in $TestLines)
    {
      $Fields = $Line -split "\|", 3
      if($Fields.Count -ne 3)
      {
        throw "Unexpected test list entry: $Line"
      }
      [pscustomobject]@{
        Executable = $Runner.Executable
        Name = $Fields[0]
        Labels = @($Fields[1] -split "," | Where-Object { $_ })
        Description = $Fields[2]
      }
    }
  }
)

if($TestCase)
{
  $Tests = @($Tests | Where-Object { $_.Name -eq $TestCase })
}
if($Prefix)
{
  $Tests = @($Tests | Where-Object { $_.Name.StartsWith($Prefix, [StringComparison]::OrdinalIgnoreCase) })
}
if($Labels.Count -gt 0)
{
  $RequestedLabels = @($Labels | ForEach-Object { $_.ToLowerInvariant() })
  $Tests = @(
    $Tests | Where-Object {
      $CurrentLabels = @($_.Labels | ForEach-Object { $_.ToLowerInvariant() })
      $Matched = $false
      foreach($RequestedLabel in $RequestedLabels)
      {
        if($CurrentLabels -contains $RequestedLabel)
        {
          $Matched = $true
          break
        }
      }
      $Matched
    }
  )
}

if($Tests.Count -eq 0)
{
  Write-Host "No tests matched the requested filters." -ForegroundColor Red
  exit 2
}

if($List)
{
  foreach($Test in $Tests)
  {
    Write-Host ("{0,-42} [{1}]`n  {2}" -f $Test.Name, ($Test.Labels -join ","), $Test.Description)
  }
  exit 0
}

if(-not $ResultsDirectory)
{
  $Timestamp = Get-Date -Format "yyyy-MM-dd_HHmmss"
  $ResultsDirectory = Join-Path $TestRoot "results\windows\$Timestamp"
}
$ResultsDirectory = [IO.Path]::GetFullPath($ResultsDirectory)
$LogsDirectory = Join-Path $ResultsDirectory "logs"
New-Item -ItemType Directory -Force -Path $LogsDirectory | Out-Null

$EnvironmentInfo = [pscustomobject]@{
  Timestamp = (Get-Date).ToString("o")
  OperatingSystem = [Environment]::OSVersion.VersionString
  Is64BitOperatingSystem = [Environment]::Is64BitOperatingSystem
  Is64BitProcess = [Environment]::Is64BitProcess
  UserInteractive = [Environment]::UserInteractive
  SessionId = [Diagnostics.Process]::GetCurrentProcess().SessionId
  PowerShellVersion = $PSVersionTable.PSVersion.ToString()
  Configuration = $Configuration
  DaliPrefix = $Context.InstallPrefix
  WindowsDependenciesSdk = $Context.SdkRoot
  VcpkgRoot = $Context.VcpkgRoot
}
$EnvironmentInfo |
  ConvertTo-Json -Depth 3 |
  Set-Content -LiteralPath (Join-Path $ResultsDirectory "environment.json") -Encoding UTF8

$Results = @()
$SuiteStart = Get-Date

foreach($Test in $Tests)
{
  $Skipped = $Test.Labels -contains "windows-unsupported"
  if($Skipped)
  {
    if(-not $Quiet)
    {
      Write-Host "[  SKIPPED ] $($Test.Name): $($Test.Description)" -ForegroundColor Yellow
    }
    $Results += [pscustomobject]@{
      Name = $Test.Name
      Labels = $Test.Labels -join ","
      Passed = $false
      Skipped = $true
      TimedOut = $false
      ExitCode = 0
      ElapsedSeconds = 0.0
      Message = $Test.Description
      Output = ""
      Log = ""
    }
    continue
  }

  if(-not $Quiet)
  {
    Write-Host "[ RUN      ] $($Test.Name)" -ForegroundColor Cyan
  }

  $StdoutPath = Join-Path $LogsDirectory "$($Test.Name).stdout.log"
  $StderrPath = Join-Path $LogsDirectory "$($Test.Name).stderr.log"
  $CombinedPath = Join-Path $LogsDirectory "$($Test.Name).log"
  $Stopwatch = [Diagnostics.Stopwatch]::StartNew()
  $ProcessStartInfo = [Diagnostics.ProcessStartInfo]::new()
  $ProcessStartInfo.FileName = $Test.Executable
  $ProcessStartInfo.Arguments = "--test $($Test.Name) --timeout-ms $($TimeoutSeconds * 1000)"
  $ProcessStartInfo.UseShellExecute = $false
  $ProcessStartInfo.CreateNoWindow = $true
  $ProcessStartInfo.RedirectStandardOutput = $true
  $ProcessStartInfo.RedirectStandardError = $true
  $Process = [Diagnostics.Process]::new()
  $Process.StartInfo = $ProcessStartInfo
  [void]$Process.Start()
  $StdoutTask = $Process.StandardOutput.ReadToEndAsync()
  $StderrTask = $Process.StandardError.ReadToEndAsync()
  $Completed = $Process.WaitForExit($TimeoutSeconds * 1000 + 5000)
  $TimedOut = -not $Completed
  if($TimedOut)
  {
    $Process.Kill()
    $Process.WaitForExit()
  }
  $Stdout = $StdoutTask.GetAwaiter().GetResult()
  $Stderr = $StderrTask.GetAwaiter().GetResult()
  $Stdout | Set-Content -LiteralPath $StdoutPath -Encoding UTF8
  $Stderr | Set-Content -LiteralPath $StderrPath -Encoding UTF8
  $Stopwatch.Stop()
  @(
    "Test: $($Test.Name)"
    "Labels: $($Test.Labels -join ',')"
    "ElapsedSeconds: $([Math]::Round($Stopwatch.Elapsed.TotalSeconds, 3))"
    "TimedOut: $TimedOut"
    "ExitCode: $(if($TimedOut) { 4 } else { $Process.ExitCode })"
    ""
    "=== stdout ==="
    $Stdout
    "=== stderr ==="
    $Stderr
  ) | Set-Content -LiteralPath $CombinedPath -Encoding UTF8

  $ExitCode = if($TimedOut) { 4 } else { $Process.ExitCode }
  $Passed = $ExitCode -eq 0
  $Message = if($TimedOut)
  {
    "The external runner timed out after $TimeoutSeconds seconds."
  }
  elseif($Passed)
  {
    "Passed"
  }
  elseif($Stderr)
  {
    $Stderr.Trim()
  }
  else
  {
    "Test process exited with code $ExitCode."
  }

  $Results += [pscustomobject]@{
    Name = $Test.Name
    Labels = $Test.Labels -join ","
    Passed = $Passed
    Skipped = $false
    TimedOut = $TimedOut
    ExitCode = $ExitCode
    ElapsedSeconds = $Stopwatch.Elapsed.TotalSeconds
    Message = $Message
    Output = ($Stdout + $Stderr)
    Log = $CombinedPath
  }

  if(-not $Quiet)
  {
    if($Passed)
    {
      Write-Host "[       OK ] $($Test.Name) ($([Math]::Round($Stopwatch.Elapsed.TotalSeconds, 2))s)" -ForegroundColor Green
    }
    else
    {
      Write-Host "[  FAILED  ] $($Test.Name): $Message" -ForegroundColor Red
    }
  }
}

$XmlSettings = New-Object System.Xml.XmlWriterSettings
$XmlSettings.Indent = $true
$XmlSettings.Encoding = New-Object System.Text.UTF8Encoding($false)
$XmlPath = Join-Path $ResultsDirectory "junit.xml"
$Writer = [System.Xml.XmlWriter]::Create($XmlPath, $XmlSettings)
try
{
  $Failures = @($Results | Where-Object { -not $_.Passed -and -not $_.Skipped }).Count
  $SkippedCount = @($Results | Where-Object { $_.Skipped }).Count
  $Writer.WriteStartDocument()
  $Writer.WriteStartElement("testsuite")
  $Writer.WriteAttributeString("name", "dali-core-common-windows")
  $Writer.WriteAttributeString("tests", "$($Results.Count)")
  $Writer.WriteAttributeString("failures", "$Failures")
  $Writer.WriteAttributeString("skipped", "$SkippedCount")
  $SuiteSeconds = [Math]::Round(((Get-Date) - $SuiteStart).TotalSeconds, 3)
  $Writer.WriteAttributeString(
    "time",
    $SuiteSeconds.ToString([Globalization.CultureInfo]::InvariantCulture))

  foreach($Result in $Results)
  {
    $Writer.WriteStartElement("testcase")
    $Writer.WriteAttributeString("classname", "dali-core.common.windows")
    $Writer.WriteAttributeString("name", $Result.Name)
    $Writer.WriteAttributeString("time", ([Math]::Round($Result.ElapsedSeconds, 3)).ToString([Globalization.CultureInfo]::InvariantCulture))
    if($Result.Skipped)
    {
      $Writer.WriteStartElement("skipped")
      $Writer.WriteAttributeString("message", $Result.Message)
      $Writer.WriteEndElement()
    }
    elseif(-not $Result.Passed)
    {
      $Writer.WriteStartElement("failure")
      $Writer.WriteAttributeString("type", $(if($Result.TimedOut) { "timeout" } else { "failure" }))
      $Writer.WriteAttributeString("message", $Result.Message)
      $Writer.WriteString($Result.Output)
      $Writer.WriteEndElement()
    }
    $Writer.WriteStartElement("system-out")
    $Writer.WriteString($Result.Output)
    $Writer.WriteEndElement()
    $Writer.WriteEndElement()
  }

  $Writer.WriteEndElement()
  $Writer.WriteEndDocument()
}
finally
{
  $Writer.Dispose()
}

$SummaryPath = Join-Path $ResultsDirectory "summary.json"
$Results | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath $SummaryPath -Encoding UTF8

$PassedCount = @($Results | Where-Object { $_.Passed }).Count
$SkippedCount = @($Results | Where-Object { $_.Skipped }).Count
$FailedCount = $Results.Count - $PassedCount - $SkippedCount
Write-Host "`nDALi core common UTC summary on Windows: $PassedCount passed, $FailedCount failed, $SkippedCount skipped."
Write-Host "Results: $ResultsDirectory"

if(@($Results | Where-Object { $_.TimedOut }).Count -gt 0)
{
  exit 4
}
if($FailedCount -gt 0)
{
  exit 1
}
exit 0
