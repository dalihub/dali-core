<img src="https://dalihub.github.io/images/DaliLogo320x200.png">

# Table of Contents

   * [Build Instructions](#build-instructions)
      * [1. Building for Ubuntu desktop](#1-building-for-ubuntu-desktop)
         * [Minimum Requirements](#minimum-requirements)
         * [Creating a DALi Environment](#creating-a-dali-environment)
         * [Building the Repository](#building-the-repository)
         * [Build target options](#build-target-options)
         * [Building and executing test cases](#building-and-executing-test-cases)
      * [2. GBS Builds](#2-gbs-builds)
         * [NON-SMACK Targets](#non-smack-targets)
         * [SMACK enabled Targets](#smack-enabled-targets)
         * [DEBUG Builds](#debug-builds)
      * [3. Building DALi as Javascript using Emscripten](#3-building-dali-as-javascript-using-emscripten)
         * [Environment for Emscripten Javascript builds](#environment-for-emscripten-javascript-builds)
         * [Building the repository as Javascript using Emscripten](#building-the-repository-as-javascript-using-emscripten)

# Build Instructions

## 1. Building for Ubuntu desktop

### Minimum Requirements

 - Ubuntu 14.04 or later

### Creating a DALi Environment

To build for desktop first ensure ALL sources are selected:
 - Go to Ubuntu Settings and then to "Software & Updates"
 - In the "Ubuntu Software" tab, ensure ALL software sources are ticked

(This is required because we install some community-maintained free & open-source software)

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

Then run the following commands:

         $ autoreconf --install
         $ ./configure --prefix=$DESKTOP_PREFIX
         $ make install -j8

### Build target options

OpenGL ES context:

When building, the OpenGL ES version of the target should be specified.

Valid version options are 20, 30, 31

With configure:
Add: *--enable-gles=X*

With gbs:
Add to the gbs build line: *--define "%target_gles_version X"*


### Building and executing test cases

See the README.md in dali-core/automated-tests.

## 2. GBS Builds

### NON-SMACK Targets

         $ gbs build -A [TARGET_ARCH]

### SMACK enabled Targets

         $ gbs build -A [TARGET_ARCH] --define "%enable_dali_smack_rules 1"

### DEBUG Builds

         $ gbs build -A [TARGET_ARCH] --define "%enable_debug 1"

## 3. Building DALi as Javascript using Emscripten


### Environment for Emscripten Javascript builds

To build the Javascript version of DALi, the Emscripten SDK is required.
Installation of this is built in to the main dali_env script.

First create a directory for the environment:

         $ mkdir -p ~/dali-env-emscripten

Now run the dali_env script. This will download and build the Emscripten SDK, which may take several minutes.
The script may ask for your password as it will need to install any package prerequisites.
Note the "-e" option for Emscripten.

         $ cd ~/dali-env-emscripten
         $ PATH-TO-DALI-CORE/build/scripts/dali_env -c -e

Create the setenv script. A new section for Emscripten will be created at the top.

         $ ./dali-env/opt/bin/dali_env -s > setenv

Run the setenv script to setup environment variables.

         $ . ./setenv

Warning: As of 19/01/2015 with a userID > 6 digits there was a bug in llvm that caused
the build to fail. So I've used the incoming branches of the emscripten sdk and not latest.

### Building the repository as Javascript using Emscripten

Clean the build first:

         $ cd ./dali-core/build/tizen
         $ git clean -dxf
         $ autoreconf --install

Use llvm's drop in replacements for configure & make:

         $ emconfigure ./configure --verbose --prefix=$DESKTOP_PREFIX --enable-emscripten=yes --enable-debug=yes --enable-profile=UBUNTU --enable-gles=20
         $ emmake make install -j8


