#ifndef __TEST_TRACE_CALL_STACK_H__
#define __TEST_TRACE_CALL_STACK_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <string>
#include <vector>

namespace Dali
{

/**
 * Helper class to track method calls in the abstraction and search for them in test cases
 */
class TraceCallStack
{
public:
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

  /**
   * Push a call onto the stack if the trace is active
   * @param[in] method The name of the method
   * @param[in] params A comma separated list of parameter values
   */
  void PushCall(std::string method, std::string params);


  /**
   * Search for a method in the stack
   * @param[in] method The name of the method
   * @return true if the method was in the stack
   */
  bool FindMethod(std::string method) const;

  /**
   * Search for a method in the stack with the given parameter list
   * @param[in] method The name of the method
   * @param[in] params A comma separated list of parameter values
   * @return true if the method was in the stack
   */
  bool FindMethodAndParams(std::string method, std::string params) const;

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
   * Get the call stack
   * @return The call stack object (Vector of vector[2] of method/paramlist strings)
   */
  inline const std::vector< std::vector< std::string > >& GetCallStack() { return mCallStack; }

private:
  bool mTraceActive; ///< True if the trace is active
  std::vector< std::vector< std::string > > mCallStack; ///< The call stack
};

} // namespace dali

#endif //__TEST_TRACE_CALL_STACK_H__
