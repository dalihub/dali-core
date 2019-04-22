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

# Build Instructions

## 1. Building for Ubuntu desktop

### Requirements

 - Ubuntu 14.04 or later
 - GCC version 6

DALi requires a compiler supporting C++11 features.
Ubuntu 16.04 is the first version to offer this by default (GCC v5.4.0).

GCC version 6 is recommended since it has fixes for issues in version 5
e.g. it avoids spurious 'defined but not used' warnings in header files.


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

### Building and executing test cases

See the README.md in dali-core/automated-tests.

## 2. GBS Builds

### NON-SMACK Targets

         $ gbs build -A [TARGET_ARCH]

### SMACK enabled Targets

         $ gbs build -A [TARGET_ARCH] --define "%enable_dali_smack_rules 1"

### DEBUG Builds

         $ gbs build -A [TARGET_ARCH] --define "%enable_debug 1"

