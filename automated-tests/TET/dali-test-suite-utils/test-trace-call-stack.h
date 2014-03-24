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

/**
 * Helper class to track method calls in the abstraction and search for them in test cases
 */
class TraceCallStack
{
public:
  /**
   * Constructor
   */
  TraceCallStack() : mTraceActive(false) { }

  /**
   * Destructor
   */
  ~TraceCallStack() { }

  /**
   * Turn on / off tracing
   */
  void Enable(bool enable) { mTraceActive = enable; }

  /**
   * Push a call onto the stack if the trace is active
   * @param[in] method The name of the method
   * @param[in] params A comma separated list of parameter values
   */
  void PushCall(std::string method, std::string params)
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
  bool FindMethod(std::string method) const
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

  /**
   * Search for a method in the stack with the given parameter list
   * @param[in] method The name of the method
   * @param[in] params A comma separated list of parameter values
   * @return true if the method was in the stack
   */
  bool FindMethodAndParams(std::string method, std::string params) const
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
  bool TestMethodAndParams(int index, std::string method, std::string params) const
  {
    return ( 0 == mCallStack[index][0].compare(method) && 0 == mCallStack[index][1].compare(params) );
  }

  /**
   * Reset the call stack
   */
  void Reset()
  {
    mCallStack.clear();
  }

  /**
   * Get the call stack
   * @return The call stack object (Vector of vector[2] of method/paramlist strings)
   */
  const std::vector< std::vector< std::string > >& GetCallStack() { return mCallStack; }

private:
  bool mTraceActive; ///< True if the trace is active
  std::vector< std::vector< std::string > > mCallStack; ///< The call stack
};

#endif //__TEST_TRACE_CALL_STACK_H__
