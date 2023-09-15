/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/trace.h>

// EXTERNAL INCLUDES
#include <cstdarg>
#include <list>
#include <memory>

namespace Dali
{
namespace Integration
{
namespace Trace
{
thread_local LogContextFunction gThreadLocalLogContextFunction = nullptr;

void InstallLogContextFunction(const LogContextFunction& logContextFunction)
{
  gThreadLocalLogContextFunction = logContextFunction;
}

void LogContext(bool start, const char* tag, const char* message)
{
  if(!gThreadLocalLogContextFunction)
  {
    return;
  }
  gThreadLocalLogContextFunction(start, tag, message);
}

#ifdef TRACE_ENABLED

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

Filter* Filter::New(bool trace, const char* environmentVariableName)
{
  char* environmentVariableValue = getenv(environmentVariableName);
  if(environmentVariableValue)
  {
    char envTraceString(0);
    sscanf(environmentVariableValue, "%c", &envTraceString);

    // Just use 'f' and 't' as it's faster than doing full string comparisons
    if(envTraceString == '1' || envTraceString == 't')
    {
      trace = true;
    }
    else if(envTraceString == '0' || envTraceString == 'f')
    {
      trace = false;
    }
  }

  Filter* filter = new Filter(trace);

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

/**
 * Begin Trace
 */
void Filter::BeginTrace(const char* tagName)
{
  Dali::Integration::Trace::LogContext(true, tagName, nullptr);
}

void Filter::BeginTrace(const char* tagName, const char* message)
{
  Dali::Integration::Trace::LogContext(true, tagName, message);
}

/**
 * End Trace
 */
void Filter::EndTrace(const char* tagName)
{
  Dali::Integration::Trace::LogContext(false, tagName, nullptr);
}

void Filter::EndTrace(const char* tagName, const char* message)
{
  Dali::Integration::Trace::LogContext(false, tagName, message);
}

/**
 * Tracer Constructor
 */
Tracer::Tracer(Filter* filter, const char* tag)
: mTag(tag),
  mFilter(filter)
{
  if(mFilter && mFilter->IsTraceEnabled())
  {
    mFilter->BeginTrace(mTag);
  }
}

/**
 * Tracer Destructor
 */
Tracer::~Tracer()
{
  if(mFilter && mFilter->IsTraceEnabled())
  {
    mFilter->EndTrace(mTag);
  }
}

#endif //TRACE_ENABLED

} // namespace Trace

} // namespace Integration

} // namespace Dali
