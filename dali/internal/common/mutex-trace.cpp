/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

// HEADER
#include <dali/internal/common/mutex-trace.h>

#ifdef LOCK_BACKTRACE_ENABLED
// EXTERNAL INCLUDES
#include <cstdlib>
#include <execinfo.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/integration-api/debug.h>

#endif // LOCK_BACKTRACE_ENABLED

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>

namespace Dali
{
#ifdef LOCK_BACKTRACE_ENABLED
extern std::string Demangle( const char* symbol );
#endif // LOCK_BACKTRACE_ENABLED

namespace Internal
{

namespace MutexTrace
{

namespace
{
#ifdef LOCK_BACKTRACE_ENABLED

// Constants
const unsigned int MAX_NUM_STACK_FRAMES = 4;
const unsigned int MAX_LOCK_SUPPORT = 5;

struct BackTraceInfo
{
  void * frameArray[ MAX_NUM_STACK_FRAMES ]; ///< Stores the frame array where the lock occurred
  int size;                                  ///< Number of frames in the frame array (can be less than MAX_NUM_STACK_FRAMES)
};

thread_local BackTraceInfo gBackTraceInfo[ MAX_LOCK_SUPPORT ]; ///< Thread local storage for the backtrace of the locks

#endif // LOCK_BACKTRACE_ENABLED

thread_local unsigned int gThreadLockCount = 0; ///< Thread local storage for the backtrace of the locks
} // unnamed namespace

void Lock()
{
  ++gThreadLockCount;

#ifdef LOCK_BACKTRACE_ENABLED

  if( gThreadLockCount <= MAX_LOCK_SUPPORT )
  {
    // Store the frame array for this lock
    int backTraceIndex = gThreadLockCount - 1;
    gBackTraceInfo[ backTraceIndex ].size = backtrace( gBackTraceInfo[ backTraceIndex ].frameArray, MAX_NUM_STACK_FRAMES );
  }
  else
  {
    DALI_LOG_ERROR( "Reached Maximum lock backtrace support. Previous Locks:\n" );
  }

  // If we've got more than one lock, then show a warning with a backtrace for all locks that we currently hold
  if( gThreadLockCount > 1 )
  {
    for( unsigned int i = 0; ( i < gThreadLockCount ) && ( i < MAX_LOCK_SUPPORT ); ++i )
    {
      DALI_LOG_WARNING( "[Lock %d]\n", i+1 );
      char** symbols = backtrace_symbols( gBackTraceInfo[ i ].frameArray, gBackTraceInfo[ i ].size );
      for( int j = 1; j < gBackTraceInfo[ i ].size; ++j )
      {
        std::string demangled_symbol = Demangle( symbols[ j ] );
        DALI_LOG_WARNING( "  [%02d] %s\n", j, demangled_symbol.c_str() );
      }
      free(symbols);
    }
    DALI_LOG_WARNING( "====================================\n" );
  }

#else

  // TODO: Uncomment when locks held per thread at any given time are 1
  //DALI_ASSERT_DEBUG( gThreadLockCount == 1 );

#endif // LOCK_BACKTRACE_ENABLED
}

void Unlock()
{
  --gThreadLockCount;
}

} // namespace MutexTrace

} // namespace Internal

} // namespace Dali
