#ifndef __DALI_COMMON_H__
#define __DALI_COMMON_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

/**
 * @addtogroup CAPI_DALI_FRAMEWORK
 * @{
 */

// EXTERNAL INCLUDES
#include <string>
#include <cstdio>


/**
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
#  define DALI_EXPORT_API
#  define DALI_IMPORT_API
#  define DALI_INTERNAL
#endif

#if defined (__GXX_EXPERIMENTAL_CXX0X__) || (__cplusplus >= 201103L)
// C++0x support
#define _CPP11
#else
// C++0x not supported
#endif

namespace Dali
{

/**
 * Method to log assertion message in DALI_ASSERT_ALWAYS macro below
 * @param[in] condition The assertion condition
 * @param[in] file The file in which the assertion occurred
 * @param[in] line The line number at which the assertion occured
 */
DALI_IMPORT_API void DaliAssertMessage(const char* condition, const char* file, int line);

/**
 * Exception class for Dali Core library - Raised by assertions in codebase.
 */
class DALI_IMPORT_API DaliException
{
public:
  /**
   * Constructor. Will always display a backtrace when raised in a debug build.
   *
   * @param[in] location  - the location of the assertion
   * @param[in] condition - The assertion condition
   */
  DALI_IMPORT_API DaliException(const char *location, const char* condition);

  std::string mLocation;
  std::string mCondition;
};

}// Dali

/**
 * An invariant concurrent assertion to ensure its argument always evaluates TRUE
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
#define DALI_ASSERT_ALWAYS(cond) \
  if(!(cond)) \
  { \
    Dali::DaliAssertMessage(#cond, __FILE__, __LINE__);   \
    throw Dali::DaliException(__PRETTY_FUNCTION__, #cond);  \
  }\

/**
 * An invariant concurrent assertion to ensure its argument evaluates TRUE in debug builds
 * Use this to sanity check algorithms and prevent internal programming errors
 */
#if defined(DEBUG_ENABLED)
#define DALI_ASSERT_DEBUG(cond) DALI_ASSERT_ALWAYS(cond)
#else
#define DALI_ASSERT_DEBUG(cond)
#endif

/**
 * @}
 */
#endif // __DALI_COMMON_H__
