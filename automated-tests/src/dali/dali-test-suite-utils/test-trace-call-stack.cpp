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

namespace Dali
{
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
    std::vector< std::string > functionCall;
    functionCall.push_back(method);
    functionCall.push_back(params);
    mCallStack.push_back( functionCall );
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
    if( 0 == mCallStack[i][0].compare(method) )
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
    if( 0 == mCallStack[i][0].compare(method) )
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
  bool found = false;
  for( size_t i=0; i < mCallStack.size(); i++ )
  {
    if( 0 == mCallStack[i][0].compare(method) && 0 == mCallStack[i][1].compare(params) )
    {
      found = true;
      break;
    }
  }
  return found;
}

/**
 * Test if the given method and parameters are at a given index in the stack
 * @param[in] index Index in the call stack
 * @param[in] method Name of method to test
 * @param[in] params A comma separated list of parameter values to test
 */
bool TraceCallStack::TestMethodAndParams(int index, std::string method, std::string params) const
{
  return ( 0 == mCallStack[index][0].compare(method) && 0 == mCallStack[index][1].compare(params) );
}

/**
 * Reset the call stack
 */
void TraceCallStack::Reset()
{
  mCallStack.clear();
}


} // namespace Dali
