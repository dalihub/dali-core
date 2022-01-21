#ifndef TEST_TRACE_CALL_STACK_H
#define TEST_TRACE_CALL_STACK_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace Dali
{
template<typename T>
std::string ToString(const T& x)
{
  return "undefined";
}

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
  class NamedParams
  {
  public:
    struct NameValue
    {
      std::string        parameterName;
      std::ostringstream value;
      NameValue(std::string pname, std::string aValue)
      : parameterName(pname),
        value(aValue)
      {
      }
      NameValue(const NameValue& rhs)
      : parameterName(rhs.parameterName),
        value(rhs.value.str())
      {
      }
      NameValue& operator=(const NameValue& rhs)
      {
        if(this != &rhs)
        {
          this->parameterName = rhs.parameterName;
          this->value.str(rhs.value.str());
        }
        return *this;
      }

      bool operator==(const NameValue& rhs)
      {
        return !parameterName.compare(rhs.parameterName) && !value.str().compare(rhs.value.str());
      }

      bool operator==(int match) const;
    };

    auto find(const std::string& param) const
    {
      auto iter = mParams.begin();
      for(; iter != mParams.end(); ++iter)
      {
        if(!iter->parameterName.compare(param))
        {
          break;
        }
      }
      return iter;
    }

    auto begin() const
    {
      return mParams.begin();
    }
    auto end() const
    {
      return mParams.end();
    }

    std::ostringstream& operator[](std::string name)
    {
      auto iter = mParams.begin();
      for(; iter != mParams.end(); ++iter)
      {
        if(!iter->parameterName.compare(name))
        {
          break;
        }
      }

      if(iter != mParams.end())
      {
        return iter->value;
      }
      else
      {
        mParams.push_back(NameValue(name, ""));
        return mParams.back().value;
      }
    }

    const std::ostringstream& operator[](std::string name) const
    {
      static std::ostringstream empty;
      auto                      iter = mParams.begin();
      for(; iter != mParams.end(); ++iter)
      {
        if(!iter->parameterName.compare(name))
        {
          break;
        }
      }

      if(iter != mParams.end())
      {
        return iter->value;
      }
      return empty;
    }

    std::string str() const
    {
      std::ostringstream out;
      bool               first = true;
      for(auto& elem : mParams)
      {
        out << (first ? "" : " ") << elem.parameterName << ": " << elem.value.str();
        first = false;
      }
      return out.str();
    }
    std::vector<NameValue> mParams{};
  };

  /**
   * Constructor
   */
  TraceCallStack(bool logging, std::string prefix);

  TraceCallStack(const TraceCallStack&) = delete;
  TraceCallStack(TraceCallStack&&)      = delete;

  /**
   * Destructor
   */
  ~TraceCallStack();

  /**
   * Turn on / off tracing
   */
  void Enable(bool enable);

  bool IsEnabled();

  void EnableLogging(bool enable);

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
  bool FindMethodAndGetParameters(std::string method, std::string& params) const;

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
  bool FindMethodAndParamsFromStartIndex(std::string method, std::string params, size_t& startIndex) const;

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
   * Search for the most recent occurrence of the method with the given (partial) parameters.
   * @param[in] method The name of the method
   * @param[in] params A map of named parameter values to match
   * @return The full named parameters of the matching call.
   */
  const NamedParams* FindLastMatch(std::string method, const TraceCallStack::NamedParams& params) const;

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
    std::size_t       functionCount = mCallStack.size();
    for(std::size_t i = 0; i < functionCount; ++i)
    {
      Dali::TraceCallStack::FunctionCall functionCall = mCallStack[i];
      traceStream << "StackTrace: Index:" << i << ",  Function:" << functionCall.method << ",  ParamList:" << functionCall.paramList << std::endl;
    }

    return traceStream.str();
  }

public:
  bool        mTraceActive{false}; ///< True if the trace is active
  bool        mLogging{false};     ///< True if the trace is logged to stdout
  std::string mPrefix;

  struct FunctionCall
  {
    std::string method;
    std::string paramList;
    NamedParams namedParams;
    FunctionCall(const std::string& aMethod, const std::string& aParamList)
    : method(aMethod),
      paramList(aParamList)
    {
    }
    FunctionCall(const std::string& aMethod, const std::string& aParamList, const NamedParams& altParams)
    : method(aMethod),
      paramList(aParamList),
      namedParams(altParams)
    {
    }
  };

  std::vector<FunctionCall> mCallStack; ///< The call stack
};

} // namespace Dali

#endif // TEST_TRACE_CALL_STACK_H
