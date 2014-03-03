#
# Copyright (c) 2014 Samsung Electronics Co., Ltd.
#
# Licensed under the Flora License, Version 1.0 (the License);
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://floralicense.org/license/

#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an AS IS BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

m4_define([dali_version],[0.1.1])
AC_INIT([dali], [dali_version])
AM_INIT_AUTOMAKE([-Wall -Wfloat-equal -Werror foreign])

AC_PROG_CXX
AC_PROG_LIBTOOL
AC_PROG_MKDIR_P

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

LT_INIT

DALI_VERSION=dali_version
AC_SUBST(DALI_VERSION)

PKG_CHECK_MODULES(OPENGLES20, opengl-es-20)

DALI_CFLAGS=-DPLATFORM_SLP

AC_ARG_ENABLE(exportall,
              [AC_HELP_STRING([--enable-exportall],
                              [enables the exporting of all the symbols in the library])],
              [enable_exportall=yes],
              [enable_exportall=no])

AC_ARG_ENABLE([debug],
              [AC_HELP_STRING([--enable-debug],
                              [Turns on debugging])],
              [enable_debug=$enableval],
              [enable_debug=no])

if test "x$enable_debug" = "xyes"; then
  DALI_CFLAGS="$DALI_CFLAGS -DDEBUG_ENABLED -DDEBUG_BACKTRACE"
fi

if test "x$enable_debug" = "xno" -a "x$enable_exportall" = "xno"; then
  DALI_CFLAGS="$DALI_CFLAGS -fvisibility=hidden -DHIDE_DALI_INTERNALS"
fi

AC_ARG_ENABLE([performance-monitor],
              [AC_HELP_STRING([--enable-performance-monitor],
                              [Enables the performance monitor output])],
              [enable_performance_monitor=$enableval],
              [enable_performance_monitor=no])

if test "x$enable_performance_monitor" = "xyes"; then
  DALI_CFLAGS="$DALI_CFLAGS -DPERFORMANCE_MONITOR_ENABLED"
fi

if test x$DALI_DATA_RW_DIR != x; then
  dataReadWriteDir=$DALI_DATA_RW_DIR
else
  dataReadWriteDir=${prefix}/share/dali/
fi

if test x$DALI_DATA_RO_DIR != x; then
  dataReadOnlyDir=$DALI_DATA_RO_DIR
else
  dataReadOnlyDir=${prefix}/share/dali/
fi

AC_SUBST(dataReadWriteDir)
AC_SUBST(dataReadOnlyDir)
AC_SUBST(DALI_CFLAGS)

# Specify the include directory for development headers
devincludepath=${includedir}
AC_SUBST(devincludepath)

AC_CONFIG_FILES([
 Makefile
 dali-core/Makefile
 dali-core.pc
 dali-test-suite-utils.pc
 ../../automated-tests/rules.mk
])

AC_OUTPUT

echo "
Configuration
-------------
  Prefix:                           $prefix
  Debug Build:                      $enable_debug
  Performance Monitor:              $enable_performance_monitor
  Data Dir (Read/Write):            $dataReadWriteDir
  Data Dir (Read Only):             $dataReadOnlyDir
"
