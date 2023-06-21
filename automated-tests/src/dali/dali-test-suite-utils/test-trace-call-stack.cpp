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

#include "test-trace-call-stack.h"
#include <iostream>
#include <sstream>
#include "dali-test-suite-utils.h"

namespace Dali
{
std::string ToString(int x)
{
  std::stringstream out;
  out << x;
  return out.str();
}

std::string ToString(unsigned int x)
{
  std::stringstream out;
  out << x;
  return out.str();
}

std::string ToString(float x)
{
  std::stringstream out;
  out << x;
  return out.str();
}

/**
 * Constructor
 */
TraceCallStack::TraceCallStack(bool logging, std::string prefix)
: mTraceActive(false),
  mLogging(logging),
  mPrefix(prefix)
{
}

/**
 * Destructor
 */
TraceCallStack::~TraceCallStack()
{
}

/**
 * Turn on / off tracing
 */
void TraceCallStack::Enable(bool enable)
{
  mTraceActive = enable;
}

bool TraceCallStack::IsEnabled()
{
  return mTraceActive;
}

void TraceCallStack::EnableLogging(bool enablelogging)
{
  mLogging = enablelogging;
}

/**
 * Push a call onto the stack if the trace is active
 * @param[in] method The name of the method
 * @param[in] params A comma separated list of parameter values
 */
void TraceCallStack::PushCall(std::string method, std::string params)
{
  if(mTraceActive)
  {
    FunctionCall stackFrame(method, params);
    mCallStack.push_back(stackFrame);
  }
  if(mLogging)
  {
    fprintf(stderr, "%s%s(%s)\n", mPrefix.c_str(), method.c_str(), params.c_str());
  }
}

void TraceCallStack::PushCall(std::string method, std::string params, const TraceCallStack::NamedParams& altParams)
{
  if(mTraceActive)
  {
    FunctionCall stackFrame(method, params, altParams);
    mCallStack.push_back(stackFrame);
  }
  if(mLogging)
  {
    fprintf(stderr, "%s%s(%s)\n", mPrefix.c_str(), method.c_str(), params.c_str());
  }
}

/**
 * Search for a method in the stack
 * @param[in] method The name of the method
 * @return true if the method was in the stack
 */
bool TraceCallStack::FindMethod(std::string method) const
{
  bool found = false;
  for(size_t i = 0; i < mCallStack.size(); i++)
  {
    if(0 == mCallStack[i].method.compare(method))
    {
      found = true;
      break;
    }
  }
  if(!found)
  {
    fprintf(stderr, "Search for %s failed\n", method.c_str());
  }
  return found;
}

bool TraceCallStack::FindMethodAndGetParameters(std::string method, std::string& params) const
{
  bool found = false;
  for(size_t i = 0; i < mCallStack.size(); i++)
  {
    if(0 == mCallStack[i].method.compare(method))
    {
      found  = true;
      params = mCallStack[i].paramList;
      break;
    }
  }
  if(!found)
  {
    fprintf(stderr, "Search for %s() failed\n", method.c_str());
  }
  return found;
}

bool TraceCallStack::FindMethodAndGetParameters(std::string method, NamedParams& params) const
{
  bool found = false;
  for(size_t i = 0; i < mCallStack.size(); i++)
  {
    if(0 == mCallStack[i].method.compare(method))
    {
      found  = true;
      params = mCallStack[i].namedParams;
      break;
    }
  }
  if(!found)
  {
    fprintf(stderr, "Search for %s() failed\n", method.c_str());
  }
  return found;
}

int TraceCallStack::CountMethod(std::string method) const
{
  int numCalls = 0;
  for(size_t i = 0; i < mCallStack.size(); i++)
  {
    if(0 == mCallStack[i].method.compare(method))
    {
      numCalls++;
    }
  }
  return numCalls;
}

/**
 * Search for a method in the stack with the given parameter list
 * @param[in] method The name of the method
 * @param[in] params A comma separated list of parameter values
 * @return true if the method was in the stack
 */
bool TraceCallStack::FindMethodAndParams(std::string method, std::string params) const
{
  return FindIndexFromMethodAndParams(method, params) > -1;
}

bool TraceCallStack::FindMethodAndParams(std::string method, const NamedParams& params) const
{
  return FindIndexFromMethodAndParams(method, params) > -1;
}

bool TraceCallStack::FindMethodAndParamsFromStartIndex(std::string method, std::string params, size_t& startIndex) const
{
  for(size_t i = startIndex; i < mCallStack.size(); ++i)
  {
    if((mCallStack[i].method.compare(method) == 0) && (mCallStack[i].paramList.compare(params) == 0))
    {
      startIndex = i;
      return true;
    }
  }
  return false;
}

/**
 * Search for a method in the stack with the given parameter list
 * @param[in] method The name of the method
 * @param[in] params A comma separated list of parameter values
 * @return index in the stack where the method was found or -1 otherwise
 */
int32_t TraceCallStack::FindIndexFromMethodAndParams(std::string method, std::string params) const
{
  int32_t index = -1;
  for(size_t i = 0; i < mCallStack.size(); i++)
  {
    if(0 == mCallStack[i].method.compare(method) && 0 == mCallStack[i].paramList.compare(params))
    {
      index = static_cast<int32_t>(i);
      break;
    }
  }
  if(index == -1)
  {
    fprintf(stderr, "Search for %s(%s) failed\n", method.c_str(), params.c_str());
  }
  return index;
}

int TraceCallStack::FindIndexFromMethodAndParams(std::string method, const TraceCallStack::NamedParams& params) const
{
  int32_t index = -1;
  for(size_t i = 0; i < mCallStack.size(); i++)
  {
    if(0 == mCallStack[i].method.compare(method))
    {
      // Test each of the passed in parameters:
      bool match = true;

      for(auto iter = params.mParams.begin(); iter != params.mParams.end(); ++iter)
      {
        auto paramIter = mCallStack[i].namedParams.find(iter->parameterName);
        if(paramIter == mCallStack[i].namedParams.end())
        {
          match = false;
          break;
        }
        std::string value  = paramIter->value.str();
        std::string iValue = iter->value.str();
        if(value.compare(iValue))
        {
          match = false;
          break;
        }
      }
      if(match == true)
      {
        index = static_cast<int32_t>(i);
        break;
      }
    }
  }

  if(index == -1)
  {
    fprintf(stderr, "Search for %s(%s) failed\n", method.c_str(), params.str().c_str());
  }

  return index;
}

const TraceCallStack::NamedParams* TraceCallStack::FindLastMatch(std::string method, const TraceCallStack::NamedParams& params) const
{
  int index = -1;

  if(mCallStack.size() > 0)
  {
    for(index = static_cast<int>(mCallStack.size() - 1); index >= 0; --index)
    {
      if(0 == mCallStack[index].method.compare(method))
      {
        // Test each of the passed in parameters:
        bool match = true;

        for(auto iter = params.mParams.begin(); iter != params.mParams.end(); ++iter)
        {
          auto        paramIter = mCallStack[index].namedParams.find(iter->parameterName);
          std::string value     = paramIter->value.str();
          std::string iValue    = iter->value.str();

          if(paramIter == mCallStack[index].namedParams.end() || value.compare(iValue))
          {
            match = false;
            break;
          }
        }
        if(match == true)
        {
          break;
        }
      }
    }
  }
  if(index >= 0)
  {
    return &mCallStack[index].namedParams;
  }
  return nullptr;
}

/**
 * Test if the given method and parameters are at a given index in the stack
 * @param[in] index Index in the call stack
 * @param[in] method Name of method to test
 * @param[in] params A comma separated list of parameter values to test
 */
bool TraceCallStack::TestMethodAndParams(int index, std::string method, std::string params) const
{
  return (0 == mCallStack[index].method.compare(method) && 0 == mCallStack[index].paramList.compare(params));
}

/**
 * Reset the call stack
 */
void TraceCallStack::Reset()
{
  mCallStack.clear();
}

bool TraceCallStack::NamedParams::NameValue::operator==(int match) const
{
  std::ostringstream matchStr;
  matchStr << match;
  std::string valueStr = value.str();
  bool        retval   = !valueStr.compare(matchStr.str());
  if(!retval)
  {
    tet_printf("Comparing parameter \"%s\": %s with %s failed\n", parameterName.c_str(), value.str().c_str(), matchStr.str().c_str());
  }
  return retval;
}

} // namespace Dali
