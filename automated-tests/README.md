Testing environment   {#auto_testing}
===================

The new test environment from Tizen is the Web-TCT test suite. This was written for testing web components, but can easily be used for testing Dali.

Each of the DALi repositories, **dali-core**, **dali-adaptor** and **dali-toolkit**, have their own test suites under the `automated-tests` folder. Within the src folder are a number of secondary folders - these correspond to 'API' tests  and internal (for desktop testing only)

Installation
------------

There are usage instructions and installation instructions on the Tizen.org website [here](http://download.tizen.org/tct/2.2.1/Manual/Web_TCT_2.2.1_User_Guide_v1.0.pdf)

These are device specific instructions, however, installing the test suite will also provide the relevant packages for running tests on Ubuntu ( follow the first block of quickstart instructions below ).

If you are planning on running tests on device, then flash your handset with latest image, or turn off ssh: `set_usb_debug.sh --mtp-sdb` and plug it in, then follow the quickstart instructions repeated below.

Quickstart
----------

For target or desktop testing:

    cd ~/Packages
    wget http://download.tizen.org/tct/2.2.1/2.2.1_r1/web-tct_2.2.1_r1.tar.gz
    sudo tar xzf web-tct_2.2.1_r1.tar.gz
    cd web-tct_2.2.1_r1/tools
    sudo -E ./tct-config-host.sh


If you are planning on running tests on device, then plug in your freshly flashed device and run the following commands:

    sudo apt-get install sdb
    ./tct-config-device.sh

**NOTE:** After flashing a handset, you will need to run this step of the installation again.

Testing on desktop
==================

Building libraries with coverage options
----------------------------------------

Building dali core:

    cd dali-core  # the location of your dali-core repository
    cd build/tizen
    export CC=gcc
    export CXX=g++
    git clean -fxd . # Only do this in the build folder
    autoreconf --install
    CXXFLAGS='-g -O0 --coverage' LDFLAGS='--coverage' ./configure --prefix=$DESKTOP_PREFIX --enable-debug
    make -j8 install

Repeat for dali-adaptor and toolkit.

Note, you __must__ use a local build and not a distributed build, and you __must__ also build with debug enabled to allow *DALI_ASSERT_DEBUG* to trigger on wrong behaviour ( Which should always be a test case failure! )

Building the tests
------------------

Run the following commands:

    cd automated-tests
    ./build.sh

This will build dali and dali-internal test sets.

Test sets can be built individually:

    ./build.sh dali

They can also be built without regenerating test case scripts (Useful for quicker rebuilds)

    ./build.sh -n dali-internal

Or without cleaning down the build area (Useful for fast build/run/debug cycles)

    ./build.sh -n -r dali-internal


Executing the tests
-------------------

To execute tests, cd into automated-tests and run

    ./execute.sh

This will execute dali and dali-internal test sets. Note that the output summary for the first will be printed before running the second.

By default the tests execute in parallel, which is faster but does not produce a single output file (summary.xml).  Use this to execute the tests in series:

    ./execute.sh -s

To see the summary.xml results, execute the tests in series and open as follows:

    firefox --new-window summary.xml

To see a list of all of the options:

    ./execute.sh -h

To execute a subset of tests, you can run individual test sets, e.g.

    ./execute.sh dali

To get coverage output, run

    ./coverage.sh


Testing on target
=================

To build for target, first build and install dali-core, dali-adaptor and dali-toolkit, then build dali-capi without --keep-packs option.

You will need to install libconfig-tiny-perl:

sudo apt-get install libconfig-tiny-perl

If you use a non-standard `GBS_ROOT` then you will need to edit the tcbuild script to match your configuration - change line 96 and add a -B option with your GBS-ROOT path (line 96 = `gbs build -A armv7l --spec core-$1-tests.spec --include-all --keep-packs` ).
To install on device from a non-standard GBS_ROOT, also modify line 28 (`RPM_DIR="$HOME/GBS-ROOT/local/repos/$PROFILE/armv7l/RPMS"`).

For core Dali cd into automated-tests, and use:

sudo ./tcbuild build dali

    sudo ./tcbuild build dali
    ./tcbuild install dali

For Dali Adaptor, cd into automated-tests, and use:

    sudo ./tcbuild build dali-adaptor
    sudo ./tcbuild build dali-platform-abstraction
    ./tcbuild install dali-adaptor
    ./tcbuild install dali-platform-abstraction

Ensure your handset's filesystem is writable:

    sdb shell su -c "change-booting-mode.sh --update"

To execute tests, cd into automated-tests and run

    tct-mgr

This will bring up the java test suite program. You should see the Plan pane with a list of all tests in. Select the tct-dali-core-tests. and you will be offered a dialog to choose a test plan: either create a new one or use temp.
Select dali test suite, and click Run, then "Create a new plan", and call it "Dali-Core" or some such. It will now run the dali-test suite.

You can find the output files under /opt/tct/manager/result/


Adding tests
============

To Managed API
--------------

If you are adding test cases for new or existing managed API (CAPI), you need to add your changes to the src/dali mirror, and copy your change to the managed test suite in core-api. You need to inform HQ of your update.

For internal API
----------------

If you are adding tests for internal API, then this will only work on desktop, and you should add your tests to the src/dali-internal test suite.

General
-------

If you are adding test cases to existing files, then all you need to do is create functions with the method signature

    int UtcTestcase(void)
    {
      TestApplication application;
      ...
      END_TEST;
    }


Note that **there must be no extra whitespace in the method signature** (i.e., it must violate our coding convention and follow __exactly__ this pattern: `int UtcDaliMyTestcaseName(void)`), as it's parsed by an awk script to auto-generate the testcase arrays in the main header file.

You can contine to use the TET api, e.g. `tet_infoline`, `tet_result` and our test check methods `DALI_TEST_CHECK`, `DALI_TEST_EQUALS`, etc.

If you need any non-test methods or variables, ensure they are wrapped in an anonymous namespace.

If you are adding new test files, then you need to add the filename to the SET(TC_SOURCES...
section of CMakeLists.txt (this is also parsed by an awk script prior to building)

Debugging
=========

On desktop, you can debug the tests by running gdb on the test program:

    $ cd automated-tests
    $ gdb build/src/dali/tct-dali-core
    gdb> r <TestCase>

replace `<TestCase>` with the name of the failing testcase.

For example, using testcase UtcDaliNinePatch01 from the dali-core test suite:

    $ gdb build/src/dali/tct-dali-core
    gdb> r UtcDaliNinePatch01


On target, you can re-install the test RPM and associated debug RPMs manually using

    sdb push <test-package>.rpm /tmp

After installing the rpm and it's debug RPMs, you can find the executable in /opt/usr/bin/tct-dali-core. First ensure you have smack permissions set:

    chsmack -e "^" /usr/bin/gdb
    chsmack -e "^" /opt/usr/bin/tct-dali-core/tct-dali-core

then run it under gdb as above.


Troubleshooting
===============

If when running tct-mgr tests, if "Health-Check get" fails and leaves a white screen on the device, you will need to run `tct-config-device.sh` from your `web-tct/tools` directory (wherever you untarred it) and power cycle your handset. If that still fails, you can work-around the issue by running "`mkdir –p /opt/usr/media/Documents/tct/`" on target – you may also need to kill the getCapabilities app from App Manager on the handset)

If the test results show that the test cases fail with "Undefined reference to XXX", it means you have probably failed to update the dali packages on target.

If all the tests are failing then make sure that you have enabled the engineering mode on the target with the 'change-booting-mode.sh --update' command in sdb shell, as the tests may not have installed correctly
