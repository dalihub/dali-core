#ifndef DALI_INTEGRATION_TRACE_H
#define DALI_INTEGRATION_TRACE_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

namespace Integration
{

namespace Trace
{

/**
 * Used by tracing macros to log a context message
 * @param start a bool to indicate start (true) or end (false) of the tracing / logging
 * @param tag a unique event tag name
 */
DALI_CORE_API void LogContext( bool start, const char* tag );

/**
 * typedef for the LogContextFunction function.
 */
typedef void ( *LogContextFunction )( bool start, const char* tag );

/**
 * A LogContextFunction function has to be installed for every thread that wants to use tracing.
 * This should be done by the adaptor.
 * The LogContextFunction function can be different for each thread.
 * @param LogContextFunction the Log Context function to install
 */
DALI_CORE_API void InstallLogContextFunction( const LogContextFunction& logContextFunction );

/********************************************************************************
 *                                    Filter                                    *
 ********************************************************************************/

#ifdef TRACE_ENABLED

/**
 * The Filter object is used by the DALI_TRACE_BEGIN macro and others to determine if the tracing
 * should take place, and routes the tracing via the platform abstraction's LogMessage.
 *
 * It provides the ability to turn tracing on or off.
 *
 */
class DALI_CORE_API Filter
{
public:

  /**
   * Test if trace is enabled for this filter.
   * @return true if trace is enabled;
   */
  inline bool IsTraceEnabled() { return mTraceEnabled; }

  /**
   * Enable tracing on this filter.
   */
  inline void EnableTrace() { mTraceEnabled = true; }

  /**
   * Disable tracing on this filter.
   */
  inline void DisableTrace() { mTraceEnabled = false; }

  /**
   * Create a new filter whose trace can be modified through the use of an environment variable.
   *
   * @param[in] trace The default trace level. If true, function tracing is on.
   * @param[in] environmentVariableName The environment variable name used in order to change the trace.
   *
   * @info To modify trace at runtime, you should define your filter as shown below:
   *
   * @code
   * Trace::Filter* filter = Trace::Filter::New( false, "TRACE_ENV" );
   * @endcode
   *
   * And to use it when running an executable:
   * @code
   * TRACE_ENV=1 dali-demo  // Trace ON
   * TRACE_ENV=0 dali-demo  // Trace OFF
   * @endcode
   */
  static Filter* New( bool trace, const char * environmentVariableName );

  /**
   * Begin trace.
   * @param[in] tagName - a unique event tag name.
   */
  void BeginTrace( const char* tagName );

  /**
   * End trace.
   * @param[in] tagName - a unique event tag name.
   */
  void EndTrace( const char* tagName );

  /**
   * Enable trace on all filters.
   */
  static void EnableGlobalTrace();

  /**
   * Disable trace on all filters.
   */
  static void DisableGlobalTrace();

private:

  /**
   * Constructor.
   * @param[in] trace - whether this filter allows tracing.
   */
  Filter( bool trace ) : mTraceEnabled( trace ) {}

private:
  bool mTraceEnabled;
};

/********************************************************************************
 *                                  Trace Macros                                *
 ********************************************************************************/

/*
 * These macros allow the instrumentation of methods.
 */

/**
 * The Tracer object is used by the DALI_TRACE_SCOPE and DALI_TRACE_FUNCTION macros
 * and uses filter object which in tun routes the tracing via the platform abstraction's LogMessage.
 *
 */
class DALI_CORE_API Tracer final
{
public:
  Tracer( Filter* filter, const char* tag );
  ~Tracer();

public:
  const char* mTag;
  Filter* mFilter;
};

/**
 * For initialization of trace filter, please use DALI_INIT_TRACE_FILTER macro i.e. DALI_INIT_TRACE_FILTER( gFilter, "TRACE_COMBINED", true );
 * To start tracing, please use DALI_TRACE_BEGIN macro i.e. DALI_TRACE_BEGIN( gFilter, "RENDERING" );
 * To end tracing, please use DALI_TRACE_END macro i.e. DALI_TRACE_END( gFilter, "RENDERING" );
 * For scoped tracing, please use DALI_TRACE_SCOPE macro i.e. DALI_TRACE_SCOPE( gFilter, "RENDERING" );
 * For function tracing, please use DALI_TRACE_FUNCTION macro i.e. DALI_TRACE_FUNCTION( gFilter );
 */

/**
 * Initialization of trace filter
 * @ref Filter::New
 */
#define DALI_INIT_TRACE_FILTER( name, environmentVariableName, enable )                                               \
namespace                                                                                                             \
{                                                                                                                     \
  Dali::Integration::Trace::Filter* name = Dali::Integration::Trace::Filter::New( enable, #environmentVariableName ); \
}

/**
 * Start of tracing
 */
#define DALI_TRACE_BEGIN( filter, tag ) \
  if( filter && filter->IsTraceEnabled() ) { filter->BeginTrace( tag ); }

/**
 * End of tracing
 */
#define DALI_TRACE_END( filter, tag ) \
  if( filter && filter->IsTraceEnabled() ) { filter->EndTrace( tag ); }

/**
 * Used for function tracing. It logs tracing of the fuction from start to end.
 */
#define DALI_TRACE_FUNCTION( filter ) \
  Dali::Integration::Trace::Tracer logTraceFunction( filter, __PRETTY_FUNCTION__ );

/**
 * Used for scope tracing. It logs tracing around a scope.
 */
#define DALI_TRACE_SCOPE( filter, tag ) \
  Dali::Integration::Trace::Tracer logTracerScope( filter, tag );

#else // TRACE_ENABLED

#define DALI_INIT_TRACE_FILTER( name, tag, enable )
#define DALI_TRACE_BEGIN( filter, tag )
#define DALI_TRACE_END( filter, tag )
#define DALI_TRACE_FUNCTION( filter )
#define DALI_TRACE_SCOPE( filter, tag )

#endif

} // Trace

} // Integration

} // Dali

#endif // DALI_INTEGRATION_TRACE_H
