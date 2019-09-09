CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
CMAKE_POLICY(SET CMP0012 NEW) # Prevent dereferencing of OFF/ON as variables

SET(name "dali-core")

PROJECT(${name})
SET(PKG_NAME ${name})

# API VERSION (Not DALi release version)
SET(${name}_VERSION_MAJOR 0)
SET(${name}_VERSION_MINOR 0)
SET(${name}_VERSION_PATCH 0)
SET(${name}_VERSION ${${name}_VERSION_MAJOR}.${${name}_VERSION_MINOR}.${${name}_VERSION_PATCH})

SET(DALI_VERSION ${${name}_VERSION} )

# Define options to CMake

OPTION(ENABLE_EXPORTALL      "Enable Export all symbols" OFF)
OPTION(ENABLE_DEBUG          "Enable Debug" OFF)
OPTION(ENABLE_BACKTRACE      "Enable Backtrace" OFF)
OPTION(ENABLE_LOCK_BACKTRACE "Enable" OFF)
OPTION(ENABLE_COVERAGE       "Coverage" OFF)
OPTION(ENABLE_TRACE          "Enable Trace" OFF)

# Set up compiler definitions

IF(CMAKE_BUILD_TYPE MATCHES Debug)
  MESSAGE( STATUS "CMAKE_BUILD_TYPE: " Debug )
  SET( ENABLE_DEBUG ON )
ELSE()
  MESSAGE( STATUS "CMAKE_BUILD_TYPE: " Release )
ENDIF()

ADD_DEFINITIONS(-DPLATFORM_TIZEN -DDALI_COMPILATION)
ADD_DEFINITIONS(-DPIC -DSTDC_HEADERS)

IF("${ARCH}" STREQUAL "arm")
  ADD_DEFINITIONS("-DTARGET")
ENDIF("${ARCH}" STREQUAL "arm")

IF( ENABLE_DEBUG )
  ADD_DEFINITIONS( "-DDEBUG_ENABLED" )
  SET( ENABLE_EXPORTALL ON )
ENDIF( ENABLE_DEBUG )

IF( NOT ${ENABLE_EXPORTALL} )
  ADD_DEFINITIONS( "-fvisibility=hidden -DHIDE_DALI_INTERNALS" )
ENDIF( NOT ${ENABLE_EXPORTALL} )

IF( ENABLE_BACKTRACE OR ENABLE_LOCK_BACKTRACE )
  ADD_DEFINITIONS( "-DBACKTRACE_ENABLED" )
ENDIF( ENABLE_BACKTRACE OR ENABLE_LOCK_BACKTRACE )

IF( ENABLE_LOCK_BACKTRACE )
  ADD_DEFINITIONS( "-DLOCK_BACKTRACE_ENABLED" )
ENDIF( ENABLE_LOCK_BACKTRACE )

# Deployment folder should come from spec file or command line:
SET( PREFIX ${CMAKE_INSTALL_PREFIX})
SET( EXEC_PREFIX ${CMAKE_INSTALL_PREFIX})

# from root/build/tizen, get back to root
SET(ROOT_SRC_DIR ${CMAKE_SOURCE_DIR}/../..)

# Make sure the path is absolute
GET_FILENAME_COMPONENT(ROOT_SRC_DIR ${ROOT_SRC_DIR} ABSOLUTE)

# Set up the include dir
SET( INCLUDE_DIR $ENV{includedir} )
IF( NOT INCLUDE_DIR )
  SET( INCLUDE_DIR ${CMAKE_INSTALL_INCLUDEDIR} )
ENDIF()
IF( NOT INCLUDE_DIR )
  SET( INCLUDE_DIR ${PREFIX}/include )
ENDIF()

# Set up the lib dir
SET( LIB_DIR $ENV{libdir} )
IF( NOT LIB_DIR )
  SET( LIB_DIR ${CMAKE_INSTALL_LIBDIR} )
ENDIF()
IF( NOT LIB_DIR )
  SET( LIB_DIR ${PREFIX}/lib )
ENDIF()

# Configure the pkg-config file
# Requires the following variables to be setup:
# @PREFIX@ @EXEC_PREFIX@ @DALI_VERSION@ @LIB_DIR@ @DEV_INCLUDE_PATH@
SET( DEV_INCLUDE_PATH ${INCLUDE_DIR} )
SET( CORE_PKG_CFG_FILE dali-core.pc )
CONFIGURE_FILE( ${CORE_PKG_CFG_FILE}.in ${CORE_PKG_CFG_FILE} @ONLY )

# Set up compiler flags and warnings
#ADD_COMPILE_OPTIONS( -std=c++11 -ggdb )
ADD_COMPILE_OPTIONS( -std=c++11 -gdwarf-4 )
ADD_COMPILE_OPTIONS( -Wnon-virtual-dtor -Woverloaded-virtual -Wold-style-cast )

# TODO: Clang is a lot more strict with warnings, we should address
# those issues at some point.
IF( NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" )
  ADD_COMPILE_OPTIONS( -Werror )
ENDIF()

ADD_COMPILE_OPTIONS( -Wall -Wextra -Wno-unused-parameter -Wfloat-equal )

IF( ENABLE_COVERAGE OR "$ENV{CXXFLAGS}" MATCHES --coverage )
  ADD_COMPILE_OPTIONS( --coverage )
  SET(ENABLE_COVERAGE ON)
  SET(COVERAGE --coverage)
ENDIF()

IF( ENABLE_TRACE )
  ADD_DEFINITIONS("-DTRACE_ENABLED")
ENDIF()


INCLUDE_DIRECTORIES(
  ${ROOT_SRC_DIR}
)

SET(SOURCE_DIR "${ROOT_SRC_DIR}/dali")
SET(PACKAGE_DOXY_SRC_DIR "${ROOT_SRC_DIR}/doc")

SET(SOURCES "")

# could use INCLUDE to get the source lists a different way;
# would then need to specify a build folder somehow...
INCLUDE( ${ROOT_SRC_DIR}/dali/internal/file.list )
INCLUDE( ${ROOT_SRC_DIR}/dali/integration-api/file.list )
INCLUDE( ${ROOT_SRC_DIR}/dali/public-api/file.list )
INCLUDE( ${ROOT_SRC_DIR}/dali/devel-api/file.list )
INCLUDE( ${ROOT_SRC_DIR}/doc/file.list )

SET(LIBTYPE SHARED)
IF(DEFINED STATIC)
  SET(LIBTYPE STATIC)
ENDIF()

ADD_LIBRARY( ${name} ${LIBTYPE} ${SOURCES} )

# pthread required due to gcc issue
FIND_LIBRARY(PTHREAD pthread)
IF(PTHREAD)
  SET(OPTIONAL_LIBS ${OPTIONAL_LIBS} -lpthread)
ENDIF()

TARGET_LINK_LIBRARIES( ${name}
  ${OPTIONAL_LIBS}
  ${COVERAGE}
)

SET_TARGET_PROPERTIES( ${name}
  PROPERTIES
  VERSION ${DALI_VERSION}
  SOVERSION ${${name}_VERSION_MAJOR}
  CLEAN_DIRECT_OUPUT 1
)

# Install the library so file and symlinks
INSTALL( TARGETS ${name} DESTINATION ${LIB_DIR} )

# Install the pkg-config file
INSTALL( FILES ${CMAKE_BINARY_DIR}/${CORE_PKG_CFG_FILE} DESTINATION ${LIB_DIR}/pkgconfig )

# macro for installing headers by replacing prefix. (TODO, investigate
# if there is a CMAKE way of doing this automatically)
MACRO(INSTALL_HEADERS_WITH_DIRECTORY HEADER_LIST STRIP_PREFIX REPLACE_PREFIX)
  GET_FILENAME_COMPONENT( SPREFIX ${STRIP_PREFIX} ABSOLUTE )
  FOREACH(HEADER ${${HEADER_LIST}})
    STRING(REGEX MATCH "(.*)[/]" DIR ${HEADER})
    STRING(REPLACE ${SPREFIX} ${REPLACE_PREFIX} NEWDIR ${DIR})
    INSTALL(FILES ${HEADER} DESTINATION ${INCLUDE_DIR}/${NEWDIR})
  ENDFOREACH(HEADER)
ENDMACRO(INSTALL_HEADERS_WITH_DIRECTORY)

# Install headers using lists defined by ADD_SUBDIRECTORY
INSTALL_HEADERS_WITH_DIRECTORY(PUBLIC_API_HEADERS ${ROOT_SRC_DIR}/dali "dali")
INSTALL_HEADERS_WITH_DIRECTORY(DEVEL_API_HEADERS ${ROOT_SRC_DIR}/dali "dali")
INSTALL_HEADERS_WITH_DIRECTORY(INTEGRATION_API_HEADERS ${ROOT_SRC_DIR}/dali "dali")
INSTALL_HEADERS_WITH_DIRECTORY(PACKAGE_DOXY_HEADERS ${PACKAGE_DOXY_SRC_DIR} "dali/doc" )

IF( UNIX )
  FIND_PROGRAM( LCOV_BIN "lcov" )
  IF( LCOV_BIN )

    # Define custom rules for coverage
    SET(COVERAGE_DIR .cov)
    SET(COVERAGE_OUTPUT_DIR doc/coverage)

    # lcov prior to 1.10 doesn't have -rc option; this and subsequent version don't output
    # branch coverage. Determine the lcov version, and enable branch coverage accordingly.
    EXECUTE_PROCESS( COMMAND bash -c "${LCOV_BIN} --version | cut -d' ' -f4" OUTPUT_VARIABLE LCOV_VERSION )
    STRING( REPLACE "." ";" LCOV_VLIST ${LCOV_VERSION})
    IF( NOT $<VERSION_LESS:${LCOV_VERSION},"1.10"> )
      SET(LCOV_OPTS --rc lcov_branch_coverage=1)
    ENDIF()

    ADD_CUSTOM_TARGET( rename_cov_data ./rename-cov-data )

    ADD_CUSTOM_TARGET( cov_data ${LCOV_BIN} ${LCOV_OPTS} --base-directory . --directory . -c -o dali.info
      COMMAND ${LCOV_BIN} ${LCOV_OPTS} --remove dali.info \"*/dali-env/*\" \"/usr/include/*\" \"public-api/shader-effects/*\" \"*/image-actor*\" -o dali.info )

    ADD_CUSTOM_TARGET( coverage genhtml ${LCOV_OPTS} -o ${COVERAGE_OUTPUT_DIR} dali.info
      DEPENDS cov_data )

    ADD_CUSTOM_TARGET( reset_coverage @${LCOV_BIN} -z --directory `pwd` )

    # Define custom rule for distclean
    ADD_CUSTOM_TARGET( distclean @echo cleaning for source distribution )
    ADD_CUSTOM_COMMAND(
          DEPENDS clean
          COMMENT "distribution clean"
          COMMAND find
          ARGS    .
          -not -name config.cmake -and \(
          -name tester.c -or
          -name Testing -or
          -name CMakeFiles -or
          -name doc -or
          -name cmake.depends -or
          -name cmake.check_depends -or
          -name CMakeCache.txt -or
          -name cmake.check_cache -or
          -name *.cmake -or
          -name Makefile -or
          -name core -or
          -name core.* -or
          -name gmon.out -or
          -name install_manifest.txt -or
          -name *.pc -or
          -name *.gcov -or
          -name *.gcno -or
          -name *.gcda -or
          -name *~ -or
          -name libdali*.so* \)
          | grep -v TC | xargs rm -rf
          TARGET  distclean
          VERBATIM
          )

  ENDIF( LCOV_BIN )
ENDIF( UNIX )

# Linker Test
SET( DALI_TEST_SUITE_DIR ${ROOT_SRC_DIR}/automated-tests/src/dali/dali-test-suite-utils )
SET( LINKER_TEST_NAME linker-test )
SET( LINKER_TEST_SOURCES
  linker-test.cpp
  ${DALI_TEST_SUITE_DIR}/test-application.cpp
  ${DALI_TEST_SUITE_DIR}/test-platform-abstraction.cpp
  ${DALI_TEST_SUITE_DIR}/test-render-controller.cpp
  ${DALI_TEST_SUITE_DIR}/test-render-surface.cpp
  ${DALI_TEST_SUITE_DIR}/test-gl-abstraction.cpp
  ${DALI_TEST_SUITE_DIR}/test-gl-sync-abstraction.cpp
  ${DALI_TEST_SUITE_DIR}/test-trace-call-stack.cpp
)
ADD_EXECUTABLE( ${LINKER_TEST_NAME} ${LINKER_TEST_SOURCES} )
TARGET_LINK_LIBRARIES(${LINKER_TEST_NAME} ${name} )
TARGET_INCLUDE_DIRECTORIES( ${LINKER_TEST_NAME} PRIVATE ${DALI_TEST_SUITE_DIR} )

# Configuration Messages
MESSAGE( STATUS "Configuration:\n" )
MESSAGE( STATUS "Prefix:                " ${PREFIX} )
MESSAGE( STATUS "Lib Dir:               " ${LIB_DIR} )
MESSAGE( STATUS "Include Dir:           " ${INCLUDE_DIR} )
MESSAGE( STATUS "Debug build:           " ${ENABLE_DEBUG} )
MESSAGE( STATUS "Export all symbols:    " ${ENABLE_EXPORTALL} )
MESSAGE( STATUS "Backtrace:             " ${ENABLE_BACKTRACE} )
MESSAGE( STATUS "Scoped Lock backtrace: " ${ENABLE_LOCK_BACKTRACE} )
MESSAGE( STATUS "Coverage:              " ${ENABLE_COVERAGE} )
MESSAGE( STATUS "Trace:                 " ${ENABLE_TRACE} )
MESSAGE( STATUS "CXXFLAGS:              " ${CMAKE_CXX_FLAGS} )
MESSAGE( STATUS "LDFLAGS:               " ${CMAKE_SHARED_LINKER_FLAGS_INIT}${CMAKE_SHARED_LINKER_FLAGS} )
