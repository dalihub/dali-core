/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <memory>

// INTERNAL INCLUDES
#include <dali/public-api/common/list-wrapper.h>
#include <dali/public-api/common/vector-wrapper.h>

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
namespace
{
void FormatPrintToStandardOutput(DebugPriority priority, const char* format, va_list args)
{
  if(format != nullptr)
  {
    char* buffer       = nullptr;
    int   bufferLength = vasprintf(&buffer, format, args); // Development note : bufferLength doesn't include null-terminated character.

    if(bufferLength >= 0) // No error
    {
      // TODO : We need to consider thread-safety way to print something.
      switch(priority)
      {
        case DebugPriority::DEBUG:
        case DebugPriority::INFO:
        {
          fprintf(stdout, "%.*s", bufferLength, buffer);
          break;
        }
        case DebugPriority::WARNING:
        case DebugPriority::ERROR:
        default:
        {
          fprintf(stderr, "%.*s", bufferLength, buffer);
          break;
        }
      }
      free(buffer);
    }
  }
}
} // namespace
thread_local LogFunction gthreadLocalLogFunction = nullptr;

/* Forward declarations */
std::string FormatToString(const char* format, ...);
std::string ArgListToString(const char* format, va_list args);

void LogMessage(DebugPriority priority, const char* format, ...)
{
  if(DALI_UNLIKELY(!gthreadLocalLogFunction))
  {
    va_list arg;
    va_start(arg, format);
    FormatPrintToStandardOutput(priority, format, arg);
    va_end(arg);
  }
  else
  {
    va_list arg;
    va_start(arg, format);
    std::string message = ArgListToString(format, arg);
    va_end(arg);

    gthreadLocalLogFunction(priority, message);
  }
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

typedef std::list<std::unique_ptr<Filter>>           FilterList;
typedef std::list<std::unique_ptr<Filter>>::iterator FilterIter;

namespace
{
static FilterList& GetActiveFilters()
{
  static FilterList activeFilters;
  return activeFilters;
}
} // namespace

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
  GetActiveFilters().push_back(std::unique_ptr<Filter>(filter));
  return filter;
}

/**
 * Enable trace on all filters.
 */
void Filter::EnableGlobalTrace()
{
  for(FilterIter iter = GetActiveFilters().begin(); iter != GetActiveFilters().end(); iter++)
  {
    (*iter)->EnableTrace();
  }
}

/**
 * Disable trace on all filters.
 */
void Filter::DisableGlobalTrace()
{
  for(FilterIter iter = GetActiveFilters().begin(); iter != GetActiveFilters().end(); iter++)
  {
    (*iter)->DisableTrace();
  }
}

void Filter::SetGlobalLogLevel(LogLevel level)
{
  for(FilterIter iter = GetActiveFilters().begin(); iter != GetActiveFilters().end(); iter++)
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
        LogMessage(INFO, message.c_str());
        free(buffer);
      }
    }
    else
    {
      std::string message = ArgListToString(format, arg);
      LogMessage(INFO, message.c_str());
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

    LogMessage(INFO, "Entr%-*c %s\n", mFilter->mNesting, ':', mMessage.c_str());
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
    LogMessage(INFO, "Exit%-*c %s\n", mFilter->mNesting, ':', mMessage.c_str());
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

void GetNanoseconds(uint64_t& timeInNanoseconds)
{
  // Get the time of a monotonic clock since its epoch.
  auto epoch = std::chrono::steady_clock::now().time_since_epoch();

  auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(epoch);

  timeInNanoseconds = static_cast<uint64_t>(duration.count());
}

} // namespace Log

namespace TimeChecker
{
typedef std::list<std::unique_ptr<ThresholdFilter>> FilterList;

namespace
{
constexpr uint64_t MILLISECONDS_TO_NANOSECONDS = 1'000'000;

static FilterList& GetActiveFilters()
{
  thread_local static FilterList activeFilters;
  return activeFilters;
}
} // namespace

struct ThresholdFilter::Impl
{
  std::vector<uint64_t> mTimeStamps;
};

ThresholdFilter* ThresholdFilter::New(ThresholdFilter::Milliseconds thresholdMilliSeconds, const char* environmentVariableName)
{
  try
  {
    char* environmentVariableValue = getenv(environmentVariableName);
    if(environmentVariableValue)
    {
      thresholdMilliSeconds = static_cast<Milliseconds>(std::atoi(environmentVariableValue));
    }
  }
  catch(...)
  {
    // Do nothing, just use default value.
  }

  ThresholdFilter* thresholdFilter = new ThresholdFilter(thresholdMilliSeconds);

  GetActiveFilters().push_back(std::unique_ptr<ThresholdFilter>(thresholdFilter));
  return thresholdFilter;
}

void ThresholdFilter::BeginTimeCheck()
{
  if(DALI_LIKELY(IsEnabled()))
  {
    uint64_t currentTime;
    Log::GetNanoseconds(currentTime);
    mImpl->mTimeStamps.push_back(currentTime);
  }
}

void ThresholdFilter::EndTimeCheck(const char* messagePrefix, const char* module, const char* function, const int line)
{
  if(DALI_LIKELY(IsEnabled() && !mImpl->mTimeStamps.empty()))
  {
    uint64_t currentTime;
    Log::GetNanoseconds(currentTime);

    uint64_t duration = currentTime - mImpl->mTimeStamps.back();
    mImpl->mTimeStamps.pop_back();

    if(duration >= static_cast<uint64_t>(mThresholdMilliSeconds) * MILLISECONDS_TO_NANOSECONDS)
    {
      // Use LogMessage function directly, to print correct module / function and line.
      Dali::Integration::Log::LogMessage(Dali::Integration::Log::INFO, DALI_LOG_FORMAT_PREFIX "%s takes [%.6lf ms]\n", module, function, line, messagePrefix ? messagePrefix : "", static_cast<double>(duration) / static_cast<double>(MILLISECONDS_TO_NANOSECONDS));
    }
  }
}

ThresholdFilter::ThresholdFilter(ThresholdFilter::Milliseconds thresholdMilliSeconds)
: mThresholdMilliSeconds(thresholdMilliSeconds),
  mImpl(new Impl())
{
}

ScopeTracer::ScopeTracer(ThresholdFilter* filter, const char* messagePrefix, const char* module, const char* function, const int line)
: mMessagePrefix(messagePrefix),
  mModule(module),
  mFunction(function),
  mLine(line),
  mFilter(filter)
{
  if(mFilter && mFilter->IsEnabled())
  {
    mFilter->BeginTimeCheck();
  }
}

ScopeTracer::~ScopeTracer()
{
  if(mFilter && mFilter->IsEnabled())
  {
    mFilter->EndTimeCheck(mMessagePrefix, mModule, mFunction, mLine);
  }
}
} // namespace TimeChecker

} // namespace Integration

} // namespace Dali
