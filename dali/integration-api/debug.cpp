/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <chrono>
#include <cstdarg>

namespace Dali
{
#ifdef DEBUG_ENABLED

// Fake globals for gdb typedefs
Dali::DebugPropertyValueArray gValueArray;
Dali::DebugPropertyValueMap   gValueMap;

#endif

namespace Integration
{
namespace Log
{
thread_local LogFunction gthreadLocalLogFunction = nullptr;

/* Forward declarations */
std::string FormatToString(const char* format, ...);
std::string ArgListToString(const char* format, va_list args);

void LogMessage(DebugPriority priority, const char* format, ...)
{
  if(!gthreadLocalLogFunction)
  {
    return;
  }

  va_list arg;
  va_start(arg, format);
  std::string message = ArgListToString(format, arg);
  va_end(arg);

  gthreadLocalLogFunction(priority, message);
}

void InstallLogFunction(const LogFunction& logFunction)
{
  // TLS stores a pointer to an object.
  // It needs to be allocated on the heap, because TLS will destroy it when the thread exits.

  gthreadLocalLogFunction = logFunction;
}

void UninstallLogFunction()
{
  gthreadLocalLogFunction = nullptr;
}

#ifdef DEBUG_ENABLED

/*Change false to true if trace is needed but don't commit to codeline*/
Filter* Filter::gRender     = Filter::New(Debug::Concise, false, "LOG_RENDER");
Filter* Filter::gResource   = Filter::New(Debug::Concise, false, "LOG_RESOURCE");
Filter* Filter::gGLResource = Filter::New(Debug::Concise, false, "LOG_GL_RESOURCE");
Filter* Filter::gObject     = nullptr;
Filter* Filter::gImage      = Filter::New(Debug::Concise, false, "LOG_IMAGE");
Filter* Filter::gModel      = Filter::New(Debug::Concise, false, "LOG_MODEL");
Filter* Filter::gNode       = nullptr;
Filter* Filter::gElement    = nullptr;
Filter* Filter::gActor      = Filter::New(Debug::Concise, false, "LOG_ACTOR");
Filter* Filter::gShader     = Filter::New(Debug::Concise, false, "LOG_SHADER");

Filter::FilterList* Filter::GetActiveFilters()
{
  static Filter::FilterList* activeFilters = new FilterList;
  return activeFilters;
}

Filter* Filter::New(LogLevel level, bool trace, const char* environmentVariableName)
{
  char* environmentVariableValue = getenv(environmentVariableName);
  if(environmentVariableValue)
  {
    unsigned int envLevel(0);
    char         envTraceString(0);
    sscanf(environmentVariableValue, "%u,%c", &envLevel, &envTraceString);

    if(envLevel > Verbose)
    {
      envLevel = Verbose;
    }
    level = LogLevel(envLevel);

    // Just use 'f' and 't' as it's faster than doing full string comparisons
    if(envTraceString == 't')
    {
      trace = true;
    }
    else if(envTraceString == 'f')
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

void Filter::SetGlobalLogLevel(LogLevel level)
{
  for(FilterIter iter = GetActiveFilters()->begin(); iter != GetActiveFilters()->end(); iter++)
  {
    (*iter)->SetLogLevel(level);
  }
}

void Filter::Log(LogLevel level, const char* format, ...)
{
  if(level <= mLoggingLevel)
  {
    va_list arg;
    va_start(arg, format);

    if(mTraceEnabled)
    {
      char* buffer   = nullptr;
      int   numChars = asprintf(&buffer, "    %-*c %s", mNesting, ':', format);
      if(numChars >= 0) // No error
      {
        std::string message = ArgListToString(buffer, arg);
        LogMessage(DebugInfo, message.c_str());
        free(buffer);
      }
    }
    else
    {
      std::string message = ArgListToString(format, arg);
      LogMessage(DebugInfo, message.c_str());
    }
    va_end(arg);
  }
}

TraceObj::TraceObj(Filter* filter, const char* format, ...)
: mFilter(filter)
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
    if(mFilter->mNesting)
    {
      --mFilter->mNesting;
    }
    LogMessage(DebugInfo, "Exit%-*c %s\n", mFilter->mNesting, ':', mMessage.c_str());
  }
}

#endif // DEBUG_ENABLED

std::string ArgListToString(const char* format, va_list args)
{
  std::string str; // empty string
  if(format != nullptr)
  {
    char* buffer = nullptr;
    int   err    = vasprintf(&buffer, format, args);
    if(err >= 0) // No error
    {
      str = buffer;
      free(buffer);
    }
  }
  return str;
}

std::string FormatToString(const char* format, ...)
{
  va_list arg;
  va_start(arg, format);
  std::string s = ArgListToString(format, arg);
  va_end(arg);
  return s;
}

#ifdef DEBUG_ENABLED

void GetNanoseconds(uint64_t& timeInNanoseconds)
{
  // Get the time of a monotonic clock since its epoch.
  auto epoch = std::chrono::steady_clock::now().time_since_epoch();

  auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(epoch);

  timeInNanoseconds = static_cast<uint64_t>(duration.count());
}

#endif // DEBUG_ENABLED

} // namespace Log

} // namespace Integration

} // namespace Dali
