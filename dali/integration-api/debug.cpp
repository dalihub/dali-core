/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali/integration-api/debug.h>

// EXTERNAL INCLUDES
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <iomanip>

// INTERNAL INCLUDES
#include <dali/public-api/common/constants.h>
#include <dali/public-api/math/matrix3.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/math/quaternion.h>
#include <dali/internal/event/common/thread-local-storage.h>

namespace Dali
{

namespace // unnamed namespace
{

/**
 * Generic function to print out any 2-Dimensional array
 * @param[in] data pointer to the source data stored as float[rows][cols]
 * @param[in] rows number of rows in 2D array
 * @param[in] cols number of columns in 2D array
 * @param[in] precision - the precision to write the float data.
 * @param[in] indent - the indent level to use.
 * @return string - the text representation of the 2D array
 */
std::string Array2DToString(const float *data, unsigned int rows, unsigned int cols, size_t precision, size_t indent)
{
  std::ostringstream oss;

  std::ios_base::fmtflags mask = oss.flags();
  mask &= ~std::ios_base::scientific;
  mask |=  std::ios_base::fixed;

  for(unsigned int rowIdx = 0; rowIdx < rows; rowIdx++)
  {
    oss  << std::setw(indent) << std::setfill(' ') << ' ' << "[ ";
    oss  << std::setfill(' ') << std::setprecision(precision) << std::right << std::setiosflags(mask);

    for(unsigned int colIdx = 0; colIdx < cols; colIdx++)
    {
      oss << std::setw(precision + 6) << (*data++) << ' ';
    }

    oss << ']' << std::endl;
  }

  return oss.str();
}

}

namespace Integration
{

namespace Log
{

__thread LogFunction gthreadLocalLogFunction = NULL;

/* Forward declarations */
std::string FormatToString(const char *format, ...);
std::string ArgListToString(const char *format, va_list args);

void LogMessage(DebugPriority priority, const char* format, ...)
{
  if ( !gthreadLocalLogFunction )
  {
    return;
  }

  va_list arg;
  va_start(arg, format);
  std::string message = ArgListToString(format, arg);
  va_end(arg);

  gthreadLocalLogFunction(priority,message);
}

void InstallLogFunction(const LogFunction& logFunction)
{
  // TLS stores a pointer to an object.
  // It needs to be allocated on the heap, because TLS will destroy it when the thread exits.

  gthreadLocalLogFunction = logFunction;
}

void UninstallLogFunction()
{
  gthreadLocalLogFunction = NULL;
}

#ifdef DEBUG_ENABLED

/*Change false to true if trace is needed but don't commit to codeline*/
Filter* Filter::gRender     = Filter::New(Debug::Concise, false, "LOG_RENDER");
Filter* Filter::gResource   = Filter::New(Debug::Concise, false, "LOG_RESOURCE");
Filter* Filter::gGLResource = Filter::New(Debug::Concise, false, "LOG_GL_RESOURCE");
Filter* Filter::gObject     = NULL;
Filter* Filter::gImage      = Filter::New(Debug::Concise, false, "LOG_IMAGE");
Filter* Filter::gModel      = Filter::New(Debug::Concise, false, "LOG_MODEL");
Filter* Filter::gNode       = NULL;
Filter* Filter::gElement    = NULL;
Filter* Filter::gActor      = Filter::New(Debug::Concise, false, "LOG_ACTOR");
Filter* Filter::gShader     = Filter::New(Debug::Concise, false, "LOG_SHADER");
Filter* Filter::gDynamics   = Filter::New(Debug::Concise, false, "LOG_DYNAMICS");

Filter::FilterList* Filter::GetActiveFilters()
{
  static Filter::FilterList* activeFilters = new FilterList;
  return activeFilters;
}

Filter* Filter::New( LogLevel level, bool trace, const char * environmentVariableName )
{
  char * environmentVariableValue = getenv( environmentVariableName );
  if ( environmentVariableValue )
  {
    unsigned int envLevel(0);
    char envTraceString(0);
    sscanf( environmentVariableValue, "%u,%c", &envLevel, &envTraceString );

    if ( envLevel > Verbose )
    {
      envLevel = Verbose;
    }
    level = LogLevel( envLevel );

    // Just use 'f' and 't' as it's faster than doing full string comparisons
    if ( envTraceString == 't' )
    {
      trace = true;
    }
    else if ( envTraceString == 'f' )
    {
      trace = false;
    }
  }

  Filter* filter = new Filter(level, trace);
  filter->mNesting++;
  GetActiveFilters()->push_back(filter);
  return filter;
}

/**
 * Enable trace on all filters.
 */
void Filter::EnableGlobalTrace()
{
  for(FilterIter iter = GetActiveFilters()->begin(); iter != GetActiveFilters()->end(); iter++)
  {
    (*iter)->EnableTrace();
  }
}

/**
 * Disable trace on all filters.
 */
void Filter::DisableGlobalTrace()
{
  for(FilterIter iter = GetActiveFilters()->begin(); iter != GetActiveFilters()->end(); iter++)
  {
    (*iter)->DisableTrace();
  }
}


void Filter::Log(LogLevel level, const char* format, ...)
{
  if(level <= mLoggingLevel)
  {
    va_list arg;
    va_start(arg, format);

    char *buffer = NULL;
    int numChars = asprintf(&buffer, "%-*c %s", mNesting, ':', format);
    if(numChars >= 0) // No error
    {
      std::string message = ArgListToString(buffer, arg);
      LogMessage(DebugInfo, message.c_str());
      free(buffer);
    }
    va_end(arg);
  }
}


TraceObj::TraceObj(Filter* filter, const char*format, ...) : mFilter(filter)
{
  if(mFilter && mFilter->IsTraceEnabled())
  {
    va_list arg;
    va_start(arg, format);
    mMessage = ArgListToString(format, arg);
    va_end(arg);

    LogMessage(DebugInfo, "Entr%-*c %s\n", mFilter->mNesting, ':', mMessage.c_str());
    ++mFilter->mNesting;
  }
}

TraceObj::~TraceObj()
{
  if(mFilter && mFilter->IsTraceEnabled())
  {
    if (mFilter->mNesting)
    {
      --mFilter->mNesting;
    }
    LogMessage(DebugInfo, "Exit%-*c %s\n", mFilter->mNesting, ':', mMessage.c_str());
  }
}

#endif // DEBUG_ENABLED


std::string ArgListToString(const char *format, va_list args)
{
  std::string str; // empty string
  if(format != NULL)
  {
    char* buffer = NULL;
    int err = vasprintf(&buffer, format, args);
    if(err >= 0) // No error
    {
      str = buffer;
      free(buffer);
    }
  }
  return str;
}

std::string FormatToString(const char *format, ...)
{
  va_list arg;
  va_start(arg, format);
  std::string s = ArgListToString(format, arg);
  va_end(arg);
  return s;
}

std::string ColorToString(const Vector4& color)
{
  std::ostringstream oss;
  oss << "<R:" << color.r << " G:" << color.g << " B:" << color.b << " A:" << color.a << ">";
  return oss.str();
}

std::string Vector4ToString(const Vector4& v, size_t precision, size_t indent)
{
  std::ostringstream oss;
  oss << std::setw(indent+3) << std::setfill(' ') << std::setprecision(precision) << std::right;
  oss << "<X:" << std::setw(precision+4) << v.x
      << " Y:" << std::setw(precision+4) << v.y
      << " Z:" << std::setw(precision+4) << v.z
      << " W:" << std::setw(precision+4) << v.w << ">";
  return oss.str();
}

std::string Vector3ToString(const Vector3& v, size_t precision, size_t indent)
{
  std::ostringstream oss;
  oss << std::setw(indent+3) << std::setfill(' ') << std::setprecision(precision) << std::right << std::setiosflags(std::ios_base::fixed);
  oss << "<X:" << std::setw(precision+4) << v.x
      << " Y:" << std::setw(precision+4) << v.y
      << " Z:" << std::setw(precision+4) << v.z << ">";
  return oss.str();
}

std::string QuaternionToString(const Quaternion& q, size_t precision, size_t indent)
{
  std::ostringstream oss;

  Vector3 axis;
  float angle;
  q.ToAxisAngle(axis, angle);

  oss << std::setw(indent+3) << std::setfill(' ') << std::setprecision(precision) << std::right;
  oss << "<A:" << std::setw(precision+4) << angle * 180.0 / Math::PI << ", " << Vector3ToString(axis, precision, 0) << ">";

  return oss.str();
}

std::string Matrix3ToString(const Matrix3& m, size_t precision, size_t indent)
{
  return Array2DToString(m.AsFloat(), 3, 3, precision, indent);
}

std::string MatrixToString(const Matrix& m, size_t precision, size_t indent)
{
  return Array2DToString(m.AsFloat(), 4, 4, precision, indent);
}

} // namespace Log

} // namespace Integration

} // namespace Dali
