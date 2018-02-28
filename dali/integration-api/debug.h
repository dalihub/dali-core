#ifndef __DALI_INTEGRATION_DEBUG_H__
#define __DALI_INTEGRATION_DEBUG_H__

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
#include <sstream>
#include <list>
#include <stdint.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/property-map.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

// Using Debug namespace alias shortens the log usage significantly
namespace Dali{namespace Integration{namespace Log{}}}
namespace Debug = Dali::Integration::Log;

namespace Dali
{

struct Vector2;
struct Vector3;
struct Vector4;
class Matrix3;
class Matrix;
class Quaternion;

#if defined(DEBUG_ENABLED)

// Less opaque types for debugger
typedef std::vector<Dali::Property::Value> DebugPropertyValueArray;
typedef std::pair< Property::Index, Property::Value > DebugIndexValuePair;
typedef std::vector<Dali::StringValuePair> DebugStringValueContainer;
typedef std::vector< DebugIndexValuePair > DebugIndexValueContainer;

struct DebugPropertyValueMap
{
  DebugStringValueContainer stringValues;
  DebugIndexValueContainer  intValues;
};

// Fake globals for gdb typedefs
extern Dali::DebugPropertyValueArray gValueArray;
extern Dali::DebugPropertyValueMap   gValueMap;

#endif

namespace Integration
{
namespace Log
{

enum DebugPriority
{
  DebugInfo,
  DebugWarning,
  DebugError
};

/**
 * Used by logging macros to log a message along with function/class name
 * @param level debug level
 * @param format string format
 */
DALI_IMPORT_API void LogMessage(enum DebugPriority level,const char *format, ...);

/**
 * typedef for the logging function.
 */
typedef void (*LogFunction)(DebugPriority priority, std::string& message);

/**
 * A log function has to be installed for every thread that wants to use logging.
 * This should be done by the adaptor.
 * The log function can be different for each thread.
 * @param logFunction the log function to install
 * @param logOpts the log options to save in thread
 */
DALI_IMPORT_API void InstallLogFunction(const LogFunction& logFunction);

/**
 * A log function has to be uninstalled for every thread that wants to use logging.
 * The log function can be different for each thread.
 */
DALI_IMPORT_API void UninstallLogFunction();

/********************************************************************************
 *                            Error/Warning  macros.                            *
 ********************************************************************************/

/**
 * Provides unfiltered logging for global error level messages
 */
#define DALI_LOG_ERROR(format, args...)     Dali::Integration::Log::LogMessage(Dali::Integration::Log::DebugError,   "%s " format, __PRETTY_FUNCTION__, ## args)

#define DALI_LOG_ERROR_NOFN(format, args...)     Dali::Integration::Log::LogMessage(Dali::Integration::Log::DebugError, format, ## args)

#define DALI_LOG_WARNING_NOFN(format, args...)     Dali::Integration::Log::LogMessage(Dali::Integration::Log::DebugWarning, format, ## args)

/**
 * Provides unfiltered logging for fps monitor
 */
#define DALI_LOG_FPS(format, args...)     Dali::Integration::Log::LogMessage(Dali::Integration::Log::DebugInfo, format, ## args)

/**
 * Provides unfiltered logging for update status
 */
#define DALI_LOG_UPDATE_STATUS(format, args...)     Dali::Integration::Log::LogMessage(Dali::Integration::Log::DebugInfo, format, ## args)

/**
 * Provides unfiltered logging for render information
 */
#define DALI_LOG_RENDER_INFO(format, args...)     Dali::Integration::Log::LogMessage(Dali::Integration::Log::DebugInfo, format, ## args)

/**
 * Provides unfiltered logging for release
 */
#define DALI_LOG_RELEASE_INFO(format, args...)     Dali::Integration::Log::LogMessage(Dali::Integration::Log::DebugInfo, format, ## args)

#ifdef DEBUG_ENABLED

/**
 * Provides unfiltered logging for global warning level messages
 */
#define DALI_LOG_WARNING(format, args...)   Dali::Integration::Log::LogMessage(Dali::Integration::Log::DebugWarning, "%s " format, __PRETTY_FUNCTION__, ## args)


#else // DEBUG_ENABLED

// Don't warn on release build
#define DALI_LOG_WARNING(format, args...)

#endif

/********************************************************************************
 *                                    Filter                                    *
 ********************************************************************************/

#ifdef DEBUG_ENABLED

/**
 * Enumeration of logging levels.
 * Used by the filters to provide multiple log levels.
 * In general, the higher the value, the more debug is available for that filter.
 */
enum LogLevel
{
  NoLogging   = 0,
  Concise     = 1,
  General     = 2,
  Verbose     = 3
};


/**
 * The Filter object is used by the DALI_LOG_INFO macro and others to determine if the logging
 * should take place, and routes the logging via the platform abstraction's LogMessage.
 *
 * It provides a logging level. If this is set to zero, then DALI_LOG_INFO won't log anything.
 * It provides the ability to turn tracing on or off.
 *
 */
class DALI_IMPORT_API Filter
{
public:
  typedef std::list<Filter*>           FilterList;
  typedef std::list<Filter*>::iterator FilterIter;

public:

  /**
   * Test if the filter is enabled for the given logging level
   * @param[in] level - the level to test.
   * @return true if this level of logging is enabled.
   */
  bool IsEnabledFor(LogLevel level) { return level != Debug::NoLogging && level <= mLoggingLevel;}

  /**
   * Test if trace is enabled for this filter.
   * @return true if trace is enabled;
   */
  bool IsTraceEnabled() { return mTraceEnabled; }

  /**
   * Enable tracing on this filter.
   */
  void EnableTrace() { mTraceEnabled = true; }

  /**
   * Disable tracing on this filter.
   */
  void DisableTrace() { mTraceEnabled = false; }

  /**
   * Set the log level for this filter. Setting to a higher value than Debug::General also
   * enables General;
   */
  void SetLogLevel(LogLevel level) { mLoggingLevel = level; }

  /**
   * Perform the logging for this filter.
   */
  void Log(LogLevel level, const char* format, ...);

  /**
   * Create a new filter whose debug level and trace can be modified through the use of an
   * environment variable.
   *
   * @param[in] level The default log level
   * @param[in] trace The default trace level. If true, function tracing is on.
   * @param[in] environmentVariableName The environment variable name used in order to change the
   *                                    log level or trace.
   *
   * @info To modify logg level/trace at runtime, you can should define your filter as shown below:
   *
   * @code
   * Debug::Filter* filter = Debug::Filter::New( Debug::NoLogging, false, "FILTER_ENV" );
   * @endcode
   *
   * And to use it when running an executable:
   * @code
   * FILTER_ENV=3 dali-demo        // LogLevel Verbose,   Trace using default
   * FILTER_ENV=1,true dali-demo   // LogLevel Concise,   Trace ON
   * FILTER_ENV=2,false dali-demo  // LogLevel General,   Trace OFF
   * FILTER_ENV=0,true dali-demo   // LogLevel NoLogging, Trace ON
   * @endcode
   */
  static Filter* New(LogLevel level, bool trace, const char * environmentVariableName );

  /**
   * Enable trace on all filters.
   */
  void EnableGlobalTrace();

  /**
   * Disable trace on all filters.
   */
  void DisableGlobalTrace();

private:

  /**
   * Constructor.
   * @param[in] level - the highest log level.
   * @param[in] trace - whether this filter allows tracing.
   */
  Filter(LogLevel level, bool trace) : mLoggingLevel(level), mTraceEnabled(trace), mNesting(0) {}

  static FilterList* GetActiveFilters();

public:
  // High level filters. If these filters are too broad for your current requirement, then
  // you can add a filter to your own class or source file. If you do, use Filter::New()
  // to tell this class about it.

  static Filter *gRender;
  static Filter *gResource;
  static Filter *gGLResource;
  static Filter *gObject;
  static Filter *gImage;
  static Filter *gModel;
  static Filter *gNode;
  static Filter *gElement;
  static Filter *gActor;
  static Filter *gShader;

private:
  LogLevel mLoggingLevel;
  bool     mTraceEnabled;
public:
  int      mNesting;

};


#define  DALI_LOG_FILTER_SET_LEVEL(filter, level) filter->SetLogLevel(level)
#define  DALI_LOG_FILTER_ENABLE_TRACE(filter)     filter->EnableTrace()
#define  DALI_LOG_FILTER_DISABLE_TRACE(filter)    filter->DisableTrace()

#else

#define  DALI_LOG_FILTER_SET_LEVEL(filter, level)
#define  DALI_LOG_FILTER_ENABLE_TRACE(filter)
#define  DALI_LOG_FILTER_DISABLE_TRACE(filter)

#endif

/********************************************************************************
 *                            General Logging macros                            *
 ********************************************************************************/

#ifdef DEBUG_ENABLED

#define DALI_LOG_INFO(filter, level, format, args...)                        \
  if(filter && filter->IsEnabledFor(level)) { filter->Log(level, format,  ## args); }

#else // DEBUG_ENABLED

#define DALI_LOG_INFO(filter, level, format, args...)

#endif // DEBUG_ENABLED


/********************************************************************************
 *                                  Trace Macros                                *
 ********************************************************************************/

/*
 * These macros allow the instrumentation of methods. These translate into calls
 * to LogMessage(DebugInfo).
 */

#ifdef DEBUG_ENABLED

class DALI_IMPORT_API TraceObj
{
public:
  TraceObj(Filter* filter, const char* fmt, ...);
  ~TraceObj();

public:
  std::string mMessage;
  Filter* mFilter;
};


#define DALI_LOG_TRACE_METHOD_FMT(filter, format, args...)                 \
  Dali::Integration::Log::TraceObj debugTraceObj(filter, "%s: " format, __PRETTY_FUNCTION__, ## args)

#define DALI_LOG_TRACE_METHOD(filter)                                      \
  Dali::Integration::Log::TraceObj debugTraceObj(filter, __PRETTY_FUNCTION__)


#else // DEBUG_ENABLED

#define DALI_LOG_TRACE_METHOD_FMT(filter, format, args...)
#define DALI_LOG_TRACE_METHOD(filter)


#endif

/********************************************************************************
 *                              Extra object debug                              *
 ********************************************************************************/

#ifdef DEBUG_ENABLED

/**
 * Warning, this macro changes the current scope, so should usually be put at the
 * end of the class definition.
 *
 * Suggest that the value is usually set to the object's name.
 * Warning - this will increase the size of the object for a debug build.
 */
#define DALI_LOG_OBJECT_STRING_DECLARATION \
public: \
  std::string mDebugString;

/**
 * Print all the actor tree names
 **/
#define DALI_LOG_ACTOR_TREE( node ) { \
  std::stringstream branch; \
  Node* tempNode = node; \
  while( tempNode ) { \
    branch << "<" << tempNode->mDebugString << ">::"; \
    tempNode = tempNode->GetParent(); \
  } \
  DALI_LOG_ERROR_NOFN("Actor tree: %s\n", branch.str().c_str()); \
}

/**
 * Allows one object to set another object's debug string
 */
#define DALI_LOG_SET_OBJECT_STRING(object, string) (object->mDebugString = string)

/**
 * Allows one object to set another object's std::string easily
 */
#define DALI_LOG_FMT_OBJECT_STRING(object, fmt, args...) (object->mDebugString = FormatToString(fmt, ## args))

/**
 * Allows one object to get another object's debug string
 */
#define DALI_LOG_GET_OBJECT_STRING(object) (object->mDebugString)

/**
 * Get the C string (for use in formatted logging)
 */
#define DALI_LOG_GET_OBJECT_C_STR(object) (object->mDebugString.c_str())

/**
 * Filtered logging of the object's debug string
 */
#define DALI_LOG_OBJECT(filter, object)  DALI_LOG_INFO(filter, Debug::General, object->mDebugString)


#else // DEBUG_ENABLED

#define DALI_LOG_OBJECT_STRING_DECLARATION
#define DALI_LOG_ACTOR_TREE(node)
#define DALI_LOG_SET_OBJECT_STRING(object, string)
#define DALI_LOG_FMT_OBJECT_STRING(object, fmt, args...)
#define DALI_LOG_GET_OBJECT_STRING(object)
#define DALI_LOG_GET_OBJECT_C_STR(object) ""
#define DALI_LOG_OBJECT(filter, object)

#endif

/********************************************************************************
 *                                Helper writers                                *
 ********************************************************************************/

/**
 * Helper method to translate a color to a string.
 * @param[in] color - the color to translate
 * @return string - the text representation of the color.
 */
DALI_IMPORT_API std::string ColorToString(const Vector4& color);

/**
 * Helper method to translate a vector4 to a string.
 * @param[in] v - the vector
 * @param[in] precision - the precision to write the float data.
 * @param[in] indent - the indent level to use.
 * @return string - the text representation of the vector.
 */
DALI_IMPORT_API std::string Vector4ToString(const Vector4& v, size_t precision=3, size_t indent=0);

/**
 * Helper method to translate a vector4 to a string.
 * @param[in] v - the vector
 * @param[in] precision - the precision to write the float data.
 * @param[in] indent - the indent level to use.
 * @return string - the text representation of the vector.
 */
DALI_IMPORT_API std::string Vector3ToString(const Vector3& v, size_t precision=3, size_t indent=0);

/**
 * Helper method to translate a quaternion to a string.
 * @param[in] q the quaternion
 * @param[in] precision - the precision to write the float data.
 * @param[in] indent - the indent level to use.
 * @return string - the text representation of the quaternion.
 */
DALI_IMPORT_API std::string QuaternionToString(const Quaternion& q, size_t precision=3, size_t indent=0);

/**
 * Helper method to translate a 3x3 matrix to a string.
 * @param[in] m - the matrix
 * @param[in] precision - the precision to write the float data.
 * @param[in] indent - the indent level to use.
 * @return string - the text representation of the vector.
 */
DALI_IMPORT_API std::string Matrix3ToString(const Matrix3& m, size_t precision=3, size_t indent=0);

/**
 * Helper method to translate a 4x4 matrix to a string.
 * @param[in] m - the matrix
 * @param[in] precision - the precision to write the float data.
 * @param[in] indent - the indent level to use.
 * @return string - the text representation of the vector.
 */
DALI_IMPORT_API std::string MatrixToString(const Matrix& m, size_t precision=3, size_t indent=0);

#ifdef DEBUG_ENABLED


/**
 * Filtered write of a matrix
 */
#define DALI_LOG_MATRIX(filter, matrix)  DALI_LOG_INFO(filter, Debug::General, MatrixToString(matrix))

/**
 * Filtered write of a vector
 */
#define DALI_LOG_VECTOR4(filter, vector) DALI_LOG_INFO(filter, Debug::General, Vector4ToString(vector))

/**
 * Filtered write of a vector
 */
#define DALI_LOG_VECTOR3(filter, vector) DALI_LOG_INFO(filter, Debug::General, Vector3ToString(vector))

/**
 * Filtered write of a color
 */
#define DALI_LOG_COLOR(filter, color)    DALI_LOG_INFO(filter, Debug::General, ColorToString(color))

#else

#define DALI_LOG_MATRIX(filter, matrix)
#define DALI_LOG_VECTOR4(filter,vector)
#define DALI_LOG_VECTOR3(filter,vector)
#define DALI_LOG_COLOR(filter, color)

#endif

/********************************************************************************
 *                            Time instrumentation                              *
 ********************************************************************************/

#if defined(DEBUG_ENABLED)

void GetNanoseconds( uint64_t& timeInNanoseconds );

#define DALI_LOG_TIMER_START( timeVariable )      \
  uint64_t timeVariable##1; \
  Debug::GetNanoseconds( timeVariable##1 );

#define DALI_LOG_TIMER_END( timeVariable, filter, level, preString)  \
  uint64_t timeVariable##2; \
  Debug::GetNanoseconds( timeVariable##2 );                             \
  DALI_LOG_INFO( filter, level, preString " %ld uSec\n", ((timeVariable##2-timeVariable##1)/1000));

#else // DEBUG_ENABLED

#define DALI_LOG_TIMER_START( timeVariable )
#define DALI_LOG_TIMER_END( timeVariable, filter, level, preString)

#endif

} // Debug
} // Integration
} // Dali


#endif // __DALI_INTEGRATION_DEBUG_H__
