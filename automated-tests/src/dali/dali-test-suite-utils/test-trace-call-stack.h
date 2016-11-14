#ifndef TEST_TRACE_CALL_STACK_H
#define TEST_TRACE_CALL_STACK_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

#include <string>
#include <vector>
#include <map>
#include <sstream>

namespace Dali
{
std::string ToString(int x);
std::string ToString(unsigned int x);
std::string ToString(float x);

/**
 * Helper class to track method calls in the abstraction and search for them in test cases
 */
class TraceCallStack
{
public:

  /// Typedef for passing and storing named parameters
  typedef std::map< std::string, std::string > NamedParams;

  /**
   * Constructor
   */
  TraceCallStack();

  /**
   * Destructor
   */
  ~TraceCallStack();

  /**
   * Turn on / off tracing
   */
  void Enable(bool enable);

  bool IsEnabled();

  /**
   * Push a call onto the stack if the trace is active
   * @param[in] method The name of the method
   * @param[in] params A comma separated list of parameter values
   */
  void PushCall(std::string method, std::string params);

  /**
   * Push a call onto the stack if the trace is active
   * @param[in] method The name of the method
   * @param[in] params A comma separated list of parameter values
   * @param[in] altParams A map of named parameter values
   */
  void PushCall(std::string method, std::string params, const NamedParams& altParams);

  /**
   * Search for a method in the stack
   * @param[in] method The name of the method
   * @return true if the method was in the stack
   */
  bool FindMethod(std::string method) const;

  /**
   * Search for a method in the stack and return its parameters if found
   * @param[in] method The name of the method
   * @param[out] params of the method
   * @return true if the method was in the stack
   */
  bool FindMethodAndGetParameters(std::string method, std::string& params ) const;

  /**
   * Count how many times a method was called
   * @param[in] method The name of the method
   * @return The number of times it was called
   */
  int CountMethod(std::string method) const;

  /**
   * Search for a method in the stack with the given parameter list
   * @param[in] method The name of the method
   * @param[in] params A comma separated list of parameter values
   * @return true if the method was in the stack
   */
  bool FindMethodAndParams(std::string method, std::string params) const;

  /**
   * Search for a method in the stack with the given parameter list
   * @param[in] method The name of the method
   * @param[in] params A map of named parameters to test for
   * @return true if the method was in the stack
   */
  bool FindMethodAndParams(std::string method, const NamedParams& params) const;

  /**
   * Search for a method in the stack with the given parameter list.
   * The search is done from a given index.
   * This allows the order of methods and parameters to be checked.
   * @param[in] method The name of the method
   * @param[in] params A comma separated list of parameter values
   * @param[in/out] startIndex The method index to start looking from.
   *                This is updated if a method is found so subsequent
   *                calls can search for methods occuring after this one.
   * @return True if the method was in the stack
   */
  bool FindMethodAndParamsFromStartIndex( std::string method, std::string params, size_t& startIndex ) const;

  /**
   * Search for a method in the stack with the given parameter list
   * @param[in] method The name of the method
   * @param[in] params A comma separated list of parameter values
   * @return index in the stack where the method was found or -1 otherwise
   */
  int FindIndexFromMethodAndParams(std::string method, std::string params) const;

  /**
   * Search for a method in the stack with the given parameter list
   * @param[in] method The name of the method
   * @param[in] params A map of named parameter values to match
   * @return index in the stack where the method was found or -1 otherwise
   */
  int FindIndexFromMethodAndParams(std::string method, const NamedParams& params) const;

  /**
   * Test if the given method and parameters are at a given index in the stack
   * @param[in] index Index in the call stack
   * @param[in] method Name of method to test
   * @param[in] params A comma separated list of parameter values to test
   */
  bool TestMethodAndParams(int index, std::string method, std::string params) const;

  /**
   * Reset the call stack
   */
  void Reset();

  /**
   * Method to display contents of the TraceCallStack.
   * @return A string containing a list of function calls and parameters (may contain newline characters)
   */
  std::string GetTraceString()
  {
    std::stringstream traceStream;
    int functionCount = mCallStack.size();
    for( int i = 0; i < functionCount; ++i )
    {
      Dali::TraceCallStack::FunctionCall functionCall = mCallStack[ i ];
      traceStream << "StackTrace: Index:" << i << ",  Function:" << functionCall.method << ",  ParamList:" << functionCall.paramList << std::endl;
    }

    return traceStream.str();
  }

private:
  bool mTraceActive; ///< True if the trace is active

  struct FunctionCall
  {
    std::string method;
    std::string paramList;
    NamedParams namedParams;
    FunctionCall( const std::string& aMethod, const std::string& aParamList )
    : method( aMethod ), paramList( aParamList )
    {
    }
    FunctionCall( const std::string& aMethod, const std::string& aParamList, const NamedParams& altParams )
    : method( aMethod ), paramList( aParamList ), namedParams( altParams )
    {
    }
  };

  std::vector< FunctionCall > mCallStack; ///< The call stack
};

} // namespace dali

#endif // TEST_TRACE_CALL_STACK_H
