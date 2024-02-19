/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

#include "dali-test-suite-utils.h"
#include "test-trace-call-stack.h"
#include <sstream>
#include <cstring>
#include <unistd.h>

namespace Test
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

void TraceCallStack::PushCall(std::string method)
{
  if(mTraceActive)
  {
    FunctionCall stackFrame(method);
    mCallStack.push_back( stackFrame );
  }
}

void TraceCallStack::PushCall(std::string method, const TraceCallStack::NamedParams& params)
{
  if(mTraceActive)
  {
    FunctionCall stackFrame(method, params);
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

bool TraceCallStack::FindMethodAndGetParameters(std::string method, NamedParams& params ) const
{
  bool found = false;
  for( size_t i=0; i < mCallStack.size(); i++ )
  {
    if( 0 == mCallStack[i].method.compare(method) )
    {
      found = true;
      params = mCallStack[i].namedParams;
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

bool TraceCallStack::FindMethodAndParams(std::string method, const TraceCallStack::NamedParams& params) const
{
  return FindIndexFromMethodAndParams( method, params ) > -1;
}

bool AnyEquals( Dali::Any test, Dali::Any match )
{
  if( test.GetType() == match.GetType() )
  {
    if( test.GetType() == typeid(bool) )
    {
      return test.Get<bool>() == match.Get<bool>();
    }
    if( test.GetType() == typeid(size_t) )
    {
      return test.Get<size_t>() == match.Get<size_t>();
    }
    if( test.GetType() == typeid(uint32_t) )
    {
      return test.Get<uint32_t>() == match.Get<uint32_t>();
    }
    if( test.GetType() == typeid(int32_t) )
    {
      return test.Get<int32_t>() == match.Get<int32_t>();
    }
    if( test.GetType() == typeid(int) )
    {
      return test.Get<int>() == match.Get<int>();
    }
    else if( test.GetType() == typeid(float) )
    {
      return Equals(test.Get<float>(), match.Get<float>());
    }
    else if( test.GetType() == typeid(std::string) )
    {
      return test.Get<std::string>() == match.Get<std::string>();
    }
    else
    {
      //DALI_TEST_CHECK(false); // Fail the test case if it's constructed with an unknown type.
    }
  }
  return false;
}

bool TraceCallStack::FindMethodAndParamsFromStartIndex(
  std::string method,
  const TraceCallStack::NamedParams& params,
  uint32_t& startIndex ) const
{
  for( uint32_t i = startIndex; i < uint32_t(mCallStack.size()); ++i )
  {
    if( mCallStack[i].method.compare( method ) == 0 )
    {
      bool found = true;
      auto& namedParams = mCallStack[i].namedParams;
      for( auto param : params )
      {
        const std::string& paramName = param.first;
        auto iter = namedParams.find(paramName);
        if( iter == mCallStack[i].namedParams.end() )
        {
          found = false;
          break;
        }
        else
        {
          Any test = iter->second;
          Any match = param.second;
          if( !AnyEquals( test, match ) )
          {
            found = false;
            break;
          }
        }
      }
      if(found)
      {
        startIndex = i;
        return true;
      }
    }
  }
  return false;
}

int32_t TraceCallStack::FindIndexFromMethodAndParams(std::string method, const TraceCallStack::NamedParams& params) const
{
  uint32_t index = 0;
  if( FindMethodAndParamsFromStartIndex( method, params, index ) )
  {
    return int32_t(index);
  }
  return -1;
}

/**
 * Test if the given method and parameters are at a given index in the stack
 * @param[in] index Index in the call stack
 * @param[in] method Name of method to test
 * @param[in] params A comma separated list of parameter values to test
 */
bool TraceCallStack::TestMethodAndParams(uint32_t index, std::string method, const NamedParams& params) const
{
  uint32_t startIndex = index;
  return( FindMethodAndParamsFromStartIndex( method, params, startIndex ) && startIndex == index );
}

/**
 * Reset the call stack
 */
void TraceCallStack::Reset()
{
  mCallStack.clear();
}

std::ostream& operator<<( std::ostream& o, const Dali::Any& param)
{
  if(param.GetType() == typeid(bool))
  {
    o << param.Get<bool>();
  }
  else if( param.GetType() == typeid(uint32_t) )
  {
    o << param.Get<uint32_t>();
  }
  else if( param.GetType() == typeid(int32_t) )
  {
    o << param.Get<int32_t>();
  }
  else if(param.GetType() == typeid(int))
  {
    o << param.Get<int>();
  }
  else if(param.GetType() == typeid(size_t))
  {
    o << param.Get<size_t>();
  }
  else if(param.GetType() == typeid(float))
  {
    o << param.Get<float>();
  }
  else if(param.GetType() == typeid(std::string))
  {
    o << param.Get<std::string>();
  }
  return o;
}

std::ostream& operator<<( std::ostream& o, const TraceCallStack::NamedParams& params)
{
  bool first = true;
  for( auto& param : params )
  {
    if(!first) o << ", ";
    first = false;
    o << param.first << ":" << param.second;
  }
  return o;
}

std::string TraceCallStack::GetTraceString()
{
  std::stringstream traceStream;
  std::size_t functionCount = mCallStack.size();
  for( std::size_t i = 0; i < functionCount; ++i )
  {
    Test::TraceCallStack::FunctionCall functionCall = mCallStack[ i ];
    traceStream << "StackTrace: Index:" << i << ", " << functionCall.method << "(" << functionCall.namedParams << ")"<<std::endl;
  }

  return traceStream.str();
}


} // namespace Test
