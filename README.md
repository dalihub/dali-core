<img src="https://dalihub.github.io/images/DaliLogo320x200.png">

# Table of Contents

   * [Build Instructions](#build-instructions)
      * [1. Building for Ubuntu desktop](#1-building-for-ubuntu-desktop)
         * [Minimum Requirements](#minimum-requirements)
         * [Creating a DALi Environment](#creating-a-dali-environment)
         * [Building the Repository](#building-the-repository)
         * [Building and executing test cases](#building-and-executing-test-cases)
      * [2. GBS Builds](#2-gbs-builds)
         * [NON-SMACK Targets](#non-smack-targets)
         * [SMACK enabled Targets](#smack-enabled-targets)
         * [DEBUG Builds](#debug-builds)
      * [3. Building for MS Windows](#3-building-for-ms-windows)
         * [Requirements](#requirements)
         * [Workspace layout](#workspace-layout)
         * [Installing Windows dependencies](#installing-windows-dependencies)
         * [Building and installing dali-core](#building-and-installing-dali-core)
         * [Using the runtime environment](#using-the-runtime-environment)
      * [4. Building for macOS](#4-building-for-macos)
         * [macOS Dependencies](#macos-dependencies)
         * [Build the repository](#build-the-repository)

# Build Instructions

## 1. Building for Ubuntu desktop

### Minimum Requirements

 - Ubuntu 20.04 or later
 - GCC version 9
 - Cmake version 3.8.2 or later

DALi requires a compiler supporting C++17 features.

### Creating a DALi Environment

To build for desktop first ensure ALL sources are selected:
 - Go to Ubuntu Settings and then to "Software & Updates"
 - In the "Ubuntu Software" tab, ensure ALL software sources are ticked
   (This is required because we install some community-maintained free & open-source software)
 - Install Perl Recursive support:

         $ sudo apt install libfile-copy-recursive-perl

Then you can create a dali-env folder in your home folder with:

         $ dali-core/build/scripts/dali_env -c

This will also download any dependencies that the dali repositories require.

You can save the environment variables to a file:

         $ dali-env/opt/bin/dali_env -s > setenv

This process only needs to be done once.

Next source these variables:

         $ . setenv

You will have to source these variables every time you open up a new terminal (or you can add to .bashrc if you prefer).


### Building the Repository

To build the repository enter the 'build/tizen' folder:

         $ cd dali-core/build/tizen

Then run the following command to set up the build:

         $ cmake -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX .

If a Debug build is required, then add -DCMAKE_BUILD_TYPE=Debug

To build run:

         $ make install -j8

### Building and executing test cases

See the README.md in dali-core/automated-tests.

## 2. GBS Builds

### NON-SMACK Targets

         $ gbs build -A [TARGET_ARCH]

### SMACK enabled Targets

         $ gbs build -A [TARGET_ARCH] --define "%enable_dali_smack_rules 1"

### DEBUG Builds

         $ gbs build -A [TARGET_ARCH] --define "%enable_debug 1"

### GPU PROFILE Builds

         $ gbs build -A [TARGET_ARCH] --define "%enable_gpu_memory_profile 1"

## 3. Building for MS Windows

The current Windows backend is built with CMake, Ninja, and MSVC. The supported
entry point is the repository-local PowerShell script described below. It
configures, builds, and installs dali-core in one command, so users do not need
to set vcpkg or installation paths manually.

For the complete Windows SDK workflow, including binary dependency releases,
TizenVG handling, dali-adaptor, dali-ui, and samples, see the
[windows-dependencies quick start](https://github.com/dalihub/windows-dependencies/blob/master/WINDOWS-DEVELOPMENT-QUICKSTART-ko.md).

### Requirements

- Windows 10 or Windows 11, x64
- Visual Studio 2022 or Build Tools 2022
- The `Desktop development with C++` workload
- MSVC v143 x64/x86 build tools
- A Windows 10 or Windows 11 SDK
- CMake tools for Windows
- Git for Windows
- Windows PowerShell 5.1 or PowerShell 7

The scripts locate Visual Studio, CMake, and Ninja automatically. A Git Bash
shell is not required.

### Workspace layout

Clone `dali-core` and `windows-dependencies` below the same parent directory.
The parent path and drive are not fixed. A complete DALi workspace normally
uses this layout:

```text
<workspace>\
  dali-core\
  dali-adaptor\
  dali-ui\
  windows-dependencies\
```

The scripts create or use these sibling directories:

```text
<workspace>\WindowsDependenciesSDK\  # third-party headers, libraries, and tools
<workspace>\dali-env\                # installed DALi headers, libraries, and runtime files
```

Do not set `VCPKG_FOLDER`, `DALI_ENV_FOLDER`, `DALI_WINDOWS_SDK_ROOT`, or
`DALI_PREFIX` for the standard workspace layout. The scripts derive all paths
from the repository locations.

### Installing Windows dependencies

Open PowerShell and run:

```powershell
cd <workspace>\windows-dependencies
.\install.ps1
```

`install.ps1` first downloads the `windows-sdk-latest` prerelease and verifies
its SHA-256 checksum. If no usable release is available, it builds the same
`WindowsDependenciesSDK` layout from source. On the Samsung network it also
tries to build TizenVG into that SDK; outside the network it continues without
TizenVG when the internal repository is unavailable.

To test an SDK release from another GitHub fork, specify the repository:

```powershell
.\install.ps1 -ReleaseRepository "owner/windows-dependencies"
```

### Building and installing dali-core

Run the build script from the dali-core repository:

```powershell
cd <workspace>\dali-core
.\build\windows\build.ps1
```

The default configuration is `Release`. Intermediate files are kept in
`dali-core\_build\windows`, and the install target writes to
`<workspace>\dali-env`.

Useful options are:

```powershell
# Remove only dali-core\_build\windows before rebuilding.
.\build\windows\build.ps1 -Clean

# Build and install a Debug configuration.
.\build\windows\build.ps1 -Configuration Debug

# Select the parallel build job count.
.\build\windows\build.ps1 -Jobs 4
```

`-Clean` does not remove `WindowsDependenciesSDK`, `dali-env`, or another DALi
repository's build directory.

### Using the runtime environment

After installing the required DALi repositories, you must apply the runtime
environment to your PowerShell session. Open PowerShell and run:

```powershell
cd <workspace>
. .\dali-env\setenv.ps1
```

This is required every time you open a new PowerShell terminal to develop or run
DALi applications. It sets up all necessary environment variables like
`DALI_PREFIX`, `PATH`, and `LD_LIBRARY_PATH` equivalents.

For a Debug installation, use:

```powershell
. .\dali-env\setenv.ps1 -Configuration Debug
```

To set a custom window resolution before launching an application:

```powershell
. .\dali-env\setenv.ps1
$env:DALI_WINDOW_WIDTH = "1920"
$env:DALI_WINDOW_HEIGHT = "1080"
& "$env:DALI_PREFIX\bin\your-application.example.exe"
```

## 4. Building for macOS

### macOS Dependencies

Ensure you have followed the instructions in the macos-dependencies repo [here](https://github.com/dalihub/macos-dependencies)
to create the DALi environment on the macOS.
You can clone it using:
```zsh
% git clone https://github.com/dalihub/macos-dependencies.git
```

### Build the repository

To build the repository enter the 'build/tizen' folder:
```zsh
% cd dali-core/build/tizen
```
Then run the following command to set up the build:
```zsh
% cmake -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX -DCMAKE_TOOLCHAIN_FILE=$VCPKG_FOLDER/scripts/buildsystems/vcpkg.cmake -DINSTALL_CMAKE_MODULES=ON .
```
If a Debug build is required, then add `-DCMAKE_BUILD_TYPE=Debug -DENABLE_DEBUG=ON`

To build run:
```zsh
% make install -j8
```
