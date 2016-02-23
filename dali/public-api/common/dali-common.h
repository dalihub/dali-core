#ifndef __DALI_COMMON_H__
#define __DALI_COMMON_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// EXTERNAL INCLUDES

#ifdef EMSCRIPTEN
#include <emscripten/emscripten.h>
#endif

/*
 * Definitions for shared library support
 *
 * If a library is configured with --enable-exportall or --enable-debug
 * then HIDE_DALI_INTERNALS is not defined, and nothing is hidden.
 * If it is configured without these options (the default), then HIDE_INTERNALS
 * is defined when building the library, visibility is automatically hidden, and the explicit
 * defines below come into use.
 * When building a library that uses DALI, HIDE_DALI_INTERNALS
 */
#if __GNUC__ >= 4
#  ifndef HIDE_DALI_INTERNALS
#    define DALI_EXPORT_API
#    define DALI_IMPORT_API
#    define DALI_INTERNAL
#  else
#    define DALI_EXPORT_API __attribute__ ((visibility ("default")))
#    define DALI_IMPORT_API __attribute__ ((visibility ("default")))
#    define DALI_INTERNAL   __attribute__ ((visibility ("hidden")))
#  endif
#else
/** Visibility attribute to show method definitions */
#  define DALI_EXPORT_API
/** Visibility attribute to show declarations */
#  define DALI_IMPORT_API
/** Visibility attribute to hide declarations */
#  define DALI_INTERNAL
#endif

#if defined (__GXX_EXPERIMENTAL_CXX0X__) || (__cplusplus >= 201103L)
// C++0x support
#define _CPP11
#else
// C++0x not supported
#endif

#ifdef EMSCRIPTEN

#ifndef __clang__
# error not clang?
#endif

// clang cpp11 check is per feature
#if !__has_feature(cxx_constexpr)
# error constexpr needed for compile-time-math. Use -std=c+11
#endif

#define _CPP11

#endif

/**
 * @brief Two macros to provide branch predictor information.
 * DALI_LIKELY should be used when a branch is taken in almost all cases so the
 * branch predictor can avoid pre-fetching the code for else branch
 * DALI_UNLIKELY should be used when a branch is almost never taken
 * @SINCE_1_0.0
 */
#define DALI_LIKELY(expression)   __builtin_expect( !!(expression), 1 )
#define DALI_UNLIKELY(expression) __builtin_expect( !!(expression), 0 )

/**
 * @brief The DALi namespace
 * @SINCE_1_0.0
 */
namespace Dali
{
/**
 * @addtogroup dali_core_common
 * @{
 */

/**
 * @brief Method to log assertion message in DALI_ASSERT_ALWAYS macro below.
 *
 * @SINCE_1_0.0
 * @param[in] location Where the assertion occurred
 * @param[in] condition The assertion condition
 */
DALI_IMPORT_API void DaliAssertMessage( const char* location, const char* condition );

/**
 * @brief Exception class for Dali Core library - Raised by assertions in codebase.
 * @SINCE_1_0.0
 */
class DALI_IMPORT_API DaliException
{
public:
  /**
   * @brief Constructor.
   *
   * Will always display a backtrace when raised in a debug build.
   *
   * @SINCE_1_0.0
   * @param[in] location The location of the assertion
   * @param[in] condition The assertion condition
   */
  DaliException( const char* location, const char* condition );

  const char* location;
  const char* condition;
};

/**
 * @}
 */
}// Dali

/**
 * @brief An invariant concurrent assertion to ensure its argument always evaluates TRUE.
 *
 * Use this for rules that must always be true regardless of build options.
 * For example, Actor must only ever have one parent.
 * To be clear, this test remains compiled into release builds that are deployed
 * on the platform.
 * Semantically, a failure of this test is signalling that dali is giving up and
 * quitting.
 * Since we don't catch the exception, a failure on any thread other than event
 * will propagate up the call stack and kill that thread.
 * A failure on the event thread may give the application an opportunity to
 * recover if there is an application-written exception handler on the call
 * stack between the throw site and the thread root and the application is built
 * with a compatible ABI.
 * Handle this macro with care at the level you would if it expanded to:
 *    if(!cond) { exit(EXIT_FAILURE); }
 * (which it is often equivalent to in effect).
 * It should not be used for simple parameter validation for instance.
 */

/**
 * @brief Strip assert location for release builds, assert text is descriptive enough
 * This is to save space for low spec devices
 * @SINCE_1_0.0
 */
#if defined(DEBUG_ENABLED)
#define ASSERT_LOCATION __PRETTY_FUNCTION__
#else
#define ASSERT_LOCATION NULL
#endif

#ifdef EMSCRIPTEN

#define DALI_ASSERT_ALWAYS(cond)                \
  if(DALI_UNLIKELY(!(cond))) \
  { \
    Dali::DaliAssertMessage( ASSERT_LOCATION, #cond );   \
    throw Dali::DaliException( ASSERT_LOCATION, #cond );  \
    EM_ASM(print(new Error().stack)); \
  }
#else

#define DALI_ASSERT_ALWAYS(cond)                \
  if(DALI_UNLIKELY(!(cond))) \
  { \
    Dali::DaliAssertMessage( ASSERT_LOCATION, #cond );   \
    throw Dali::DaliException( ASSERT_LOCATION, #cond );  \
  }
#endif

/**
 * @brief An invariant concurrent assertion to ensure its argument evaluates TRUE in debug builds.
 *
 * Use this to sanity check algorithms and prevent internal programming errors
 * @SINCE_1_0.0
 */
#if defined(DEBUG_ENABLED)
#define DALI_ASSERT_DEBUG(cond) DALI_ASSERT_ALWAYS(cond)
#else
#define DALI_ASSERT_DEBUG(cond)
#endif

#endif // __DALI_COMMON_H__
