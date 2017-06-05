# Copyright (c) 2014 Samsung Electronics Co., Ltd.

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

# http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


m4_define([dali_version],[0.1.1])
AC_INIT([dali], [dali_version])
AM_INIT_AUTOMAKE([-Wall foreign])

AC_PROG_CXX
AC_PROG_LIBTOOL
AC_PROG_MKDIR_P

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

LT_INIT

DALI_VERSION=dali_version
AC_SUBST(DALI_VERSION)

DALI_CFLAGS=-DPLATFORM_TIZEN

# Use C++ 11
DALI_CFLAGS="$DALI_CFLAGS -std=c++11 -Wnon-virtual-dtor -Woverloaded-virtual -Wold-style-cast"

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

AC_ARG_ENABLE([emscripten],
              [AC_HELP_STRING([--enable-emscripten],
                              [Emscripten builds])],
              [enable_emscripten=$enableval],
              [enable_emscripten=no])

AC_ARG_ENABLE([backtrace],
              [AC_HELP_STRING([--enable-backtrace],
                              [Backtrace for exceptions])],
              [enable_backtrace=$enableval],
              [enable_backtrace=yes])

AC_ARG_ENABLE([lock_backtrace],
              [AC_HELP_STRING([--enable-lock-backtrace],
                              [Backtrace for when more than 1 lock is held on the same thread])],
              [enable_lock_backtrace=$enableval],
              [enable_lock_backtrace=no])

AC_ARG_ENABLE([gles],
              [AC_HELP_STRING([--enable-gles],
                              [Specify the OpenGL ES version for backwards compatibility])],
              [enable_gles=$enableval],
              [enable_gles=20])

AC_ARG_ENABLE([vulkan],
              [AC_HELP_STRING([--enable-vulkan],
                              [Enables Vulkan support])],
              [enable_vulkan=$enableval],
              [enable_vulkan=no])

DALI_CFLAGS="$DALI_CFLAGS -DDALI_GLES_VERSION=${enable_gles}"

if test "x$enable_debug" = "xyes"; then
  DALI_CFLAGS="$DALI_CFLAGS -DDEBUG_ENABLED"
fi

if test "x$enable_debug" = "xno" -a "x$enable_exportall" = "xno"; then
  DALI_CFLAGS="$DALI_CFLAGS -fvisibility=hidden -DHIDE_DALI_INTERNALS"
fi

if test "x$enable_emscripten" = "xyes"; then
  DALI_CFLAGS="$DALI_CFLAGS -DEMSCRIPTEN -std=c++11"
  # Automatically turn off backtrace support and mutex locks
  enable_backtrace="no"
  enable_lock_backtrace="no"
fi

# Must come after Emscripten feature test
if test "x$enable_lock_backtrace" = "xyes"; then
  DALI_CFLAGS="$DALI_CFLAGS -DLOCK_BACKTRACE_ENABLED"
  enable_backtrace="yes"
fi

# Must come after Emscripten & locks backtrace feature test
if test "x$enable_backtrace" = "xyes"; then
  DALI_CFLAGS="$DALI_CFLAGS -DBACKTRACE_ENABLED"
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

DALI_CFLAGS="$DALI_CFLAGS -I${includedir}"
DALI_LDFLAGS="-L${libdir}"

# forward VULKAN_SDK path to the graphics library if set correctly
if test x"$enable_vulkan" != x"no"; then
  PKG_CHECK_MODULES(VULKAN, vulkan-${enable_vulkan}, AC_DEFINE(ENABLE_VULKAN, 1, [Vulkan available as package]),
                    [ AC_MSG_ERROR([Vulkan not available as a package!]) ]
                    )
  DALI_LDFLAGS+=" ${VULKAN_LIBS}"
  DALI_CFLAGS+=" ${VULKAN_CFLAGS}"
  AC_SUBST(VULKAN_SDK)
  AC_SUBST(VULKAN_LIBS)
  AC_SUBST(VULKAN_CFLAGS)
fi
AM_CONDITIONAL([VULKAN_SDK_SET], [test x"$enable_vulkan" != x"no"])

AC_SUBST(dataReadWriteDir)
AC_SUBST(dataReadOnlyDir)
AC_SUBST(DALI_CFLAGS)
AC_SUBST(DALI_LDFLAGS)


# Specify the include directory for development headers
devincludepath=${includedir}
AC_SUBST(devincludepath)

AC_CONFIG_FILES([
 Makefile
 dali-core/Makefile
 dali-core/graphics/Makefile
 dali-core.pc
])

AC_OUTPUT

echo "
Configuration
-------------
  Prefix:                           $prefix
  Debug Build:                      $enable_debug
  Data Dir (Read/Write):            $dataReadWriteDir
  Data Dir (Read Only):             $dataReadOnlyDir
  Emscripten:                       $enable_emscripten
  Backtrace:                        $enable_backtrace
  ScopedLock Backtrace:             $enable_lock_backtrace
  Vulkan Support:                   $enable_vulkan
"
