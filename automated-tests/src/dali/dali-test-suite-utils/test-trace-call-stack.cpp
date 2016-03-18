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

#include "test-trace-call-stack.h"
#include <sstream>

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
TraceCallStack::TraceCallStack() : mTraceActive(false) { }

/**
 * Destructor
 */
TraceCallStack::~TraceCallStack() { }

/**
 * Turn on / off tracing
 */
void TraceCallStack::Enable(bool enable) { mTraceActive = enable; }

bool TraceCallStack::IsEnabled() { return mTraceActive; }

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
    mCallStack.push_back( stackFrame );
  }
}

void TraceCallStack::PushCall(std::string method, std::string params, const TraceCallStack::NamedParams& altParams)
{
  if(mTraceActive)
  {
    FunctionCall stackFrame(method, params, altParams);
    mCallStack.push_back( stackFrame );
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
  for( size_t i=0; i < mCallStack.size(); i++ )
  {
    if( 0 == mCallStack[i].method.compare(method) )
    {
      found = true;
      break;
    }
  }
  return found;
}

int TraceCallStack::CountMethod(std::string method) const
{
  int numCalls = 0;
  for( size_t i=0; i < mCallStack.size(); i++ )
  {
    if( 0 == mCallStack[i].method.compare(method) )
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
  return FindIndexFromMethodAndParams( method, params ) > -1;
}

bool TraceCallStack::FindMethodAndParams(std::string method, const NamedParams& params) const
{
  return FindIndexFromMethodAndParams( method, params ) > -1;
}

bool TraceCallStack::FindMethodAndParamsFromStartIndex( std::string method, std::string params, size_t& startIndex ) const
{
  for( size_t i = startIndex; i < mCallStack.size(); ++i )
  {
    if( ( mCallStack[i].method.compare( method ) == 0 ) && ( mCallStack[i].paramList.compare( params ) == 0 ) )
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
int TraceCallStack::FindIndexFromMethodAndParams(std::string method, std::string params) const
{
  int index = -1;
  for( size_t i=0; i < mCallStack.size(); i++ )
  {
    if( 0 == mCallStack[i].method.compare(method) && 0 == mCallStack[i].paramList.compare(params) )
    {
      index = i;
      break;
    }
  }
  return index;
}

int TraceCallStack::FindIndexFromMethodAndParams(std::string method, const TraceCallStack::NamedParams& params) const
{
  int index = -1;
  for( size_t i=0; i < mCallStack.size(); i++ )
  {
    if( 0 == mCallStack[i].method.compare(method) )
    {
      // Test each of the passed in parameters:
      bool match = true;
      for( NamedParams::const_iterator iter = params.begin() ; iter != params.end() ; ++iter )
      {
        NamedParams::const_iterator paramIter = mCallStack[i].namedParams.find(iter->first);
        if( paramIter == params.end() || paramIter->second.compare(iter->second) != 0 )
        {
          match = false;
          break;
        }
      }
      if( match == true )
      {
        index = i;
        break;
      }
    }
  }
  return index;
}


/**
 * Test if the given method and parameters are at a given index in the stack
 * @param[in] index Index in the call stack
 * @param[in] method Name of method to test
 * @param[in] params A comma separated list of parameter values to test
 */
bool TraceCallStack::TestMethodAndParams(int index, std::string method, std::string params) const
{
  return ( 0 == mCallStack[index].method.compare(method) && 0 == mCallStack[index].paramList.compare(params) );
}

/**
 * Reset the call stack
 */
void TraceCallStack::Reset()
{
  mCallStack.clear();
}


} // namespace Dali
