#ifndef DALI_TEST_SUITE_UTILS_H
#define DALI_TEST_SUITE_UTILS_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/dali-core.h>
#include <test-compare-types.h>

extern "C"
{
  void tet_infoline(const char* str);
  void tet_printf(const char* format, ...);
}

#include "test-actor-utils.h"
#include "test-application.h"
#include "test-gesture-generator.h"

using namespace Dali;

#define STRINGIZE_I(text) #text
#define STRINGIZE(text) STRINGIZE_I(text)

/**
 * Inspired by https://stackoverflow.com/questions/1706346/file-macro-manipulation-handling-at-compile-time
 * answer by Chetan Reddy
 */
constexpr int32_t basenameIndex(const char* const path, const int32_t index = 0, const int32_t slashIndex = -1)
{
  return path[index]
           ? (path[index] == '/'
                ? basenameIndex(path, index + 1, index)
                : basenameIndex(path, index + 1, slashIndex))
           : (slashIndex + 1);
}

#define __FILELINE__ ({ static const int32_t basenameIdx = basenameIndex( __FILE__ ); \
                         static_assert (basenameIdx >= 0, "compile-time basename" );   \
                         __FILE__ ":" STRINGIZE(__LINE__) + basenameIdx ; })

#define TEST_LOCATION __FILELINE__
#define TEST_INNER_LOCATION(x) (std::string(x) + " (" + STRINGIZE(__LINE__) + ")").c_str()

#define TET_UNDEF 2
#define TET_FAIL 1
#define TET_PASS 0

extern int32_t test_return_value;

void tet_result(int32_t value);

#define END_TEST \
  return ((test_return_value > 0) ? 1 : 0)

void tet_infoline(const char* str);
void tet_printf(const char* format, ...);

/**
 * DALI_TEST_CHECK is a wrapper for tet_result.
 * If the condition evaluates to false, the test is stopped.
 * @param[in] The boolean expression to check
 */
#define DALI_TEST_CHECK(condition)                                                   \
  if((condition))                                                                    \
  {                                                                                  \
    tet_result(TET_PASS);                                                            \
  }                                                                                  \
  else                                                                               \
  {                                                                                  \
    fprintf(stderr, "Test failed in %s, condition: %s\n", __FILELINE__, #condition); \
    tet_result(TET_FAIL);                                                            \
    throw("TET_FAIL");                                                               \
  }

bool          operator==(TimePeriod a, TimePeriod b);
std::ostream& operator<<(std::ostream& ostream, TimePeriod value);
std::ostream& operator<<(std::ostream& ostream, Radian angle);
std::ostream& operator<<(std::ostream& ostream, Degree angle);
std::ostream& operator<<(std::ostream& ostream, BaseHandle handle);

/**
 * Test whether two values are equal.
 * @param[in] value1 The first value
 * @param[in] value2 The second value
 * @param[in] location The TEST_LOCATION macro should be used here
 */
template<typename Type>
inline void DALI_TEST_EQUALS(Type value1, Type value2, const char* location)
{
  if(!CompareType<Type>(value1, value2, 0.01f))
  {
    std::ostringstream o;
    o << value1 << " == " << value2 << std::endl;
    fprintf(stderr, "Test failed in %s, checking %s", location, o.str().c_str());
    tet_result(TET_FAIL);
    throw("TET_FAIL");
  }
  else
  {
    tet_result(TET_PASS);
  }
}

/**
 * Test whether two values are equal.
 * @param[in] value1 The first value
 * @param[in] value2 The second value
 */
#define DALI_TEST_EQUAL(v1, v2) DALI_TEST_EQUALS(v1, v2, __FILELINE__)

template<typename Type>
inline void DALI_TEST_EQUALS(Type value1, Type value2, float epsilon, const char* location)
{
  if(!CompareType<Type>(value1, value2, epsilon))
  {
    std::ostringstream o;
    o << value1 << " == " << value2 << std::endl;
    fprintf(stderr, "Test failed in %s, checking %s", location, o.str().c_str());
    tet_result(TET_FAIL);
    throw("TET_FAIL");
  }
  else
  {
    tet_result(TET_PASS);
  }
}

template<typename Type>
inline void DALI_TEST_NOT_EQUALS(Type value1, Type value2, float epsilon, const char* location)
{
  if(CompareType<Type>(value1, value2, epsilon))
  {
    std::ostringstream o;
    o << value1 << " !=  " << value2 << std::endl;
    fprintf(stderr, "Test failed in %s, checking %s", location, o.str().c_str());
    tet_result(TET_FAIL);
    throw("TET_FAIL");
  }
  else
  {
    tet_result(TET_PASS);
  }
}

/**
 * Test whether two TimePeriods are within a certain distance of each other.
 * @param[in] value1 The first value
 * @param[in] value2 The second value
 * @param[in] epsilon The values must be within this distance of each other
 * @param[in] location The TEST_LOCATION macro should be used here
 */
template<>
inline void DALI_TEST_EQUALS<TimePeriod>(TimePeriod value1, TimePeriod value2, float epsilon, const char* location)
{
  if((fabs(value1.durationSeconds - value2.durationSeconds) > epsilon))
  {
    fprintf(stderr, "Test failed in %s, checking durations %f == %f, epsilon %f\n", location, value1.durationSeconds, value2.durationSeconds, epsilon);
    tet_result(TET_FAIL);
    throw("TET_FAIL");
  }
  else if((fabs(value1.delaySeconds - value2.delaySeconds) > epsilon))
  {
    fprintf(stderr, "Test failed in %s, checking delays %f == %f, epsilon %f\n", location, value1.delaySeconds, value2.delaySeconds, epsilon);
    tet_result(TET_FAIL);
    throw("TET_FAIL");
  }
  else
  {
    tet_result(TET_PASS);
  }
}

/**
 * Test whether two base handles are equal.
 * @param[in] baseHandle1 The first value
 * @param[in] baseHandle2 The second value
 * @param[in] location The TEST_LOCATION macro should be used here
 */
void DALI_TEST_EQUALS(const BaseHandle& baseHandle1, const BaseHandle& baseHandle2, const char* location);

/**
 * Test whether a size_t value and an uint32_t are equal.
 * @param[in] value1 The first value
 * @param[in] value2 The second value
 * @param[in] location The TEST_LOCATION macro should be used here
 */
void DALI_TEST_EQUALS(const size_t value1, const uint32_t value2, const char* location);

/**
 * Test whether an uint32_t and a size_t value and are equal.
 * @param[in] value1 The first value
 * @param[in] value2 The second value
 * @param[in] location The TEST_LOCATION macro should be used here
 */
void DALI_TEST_EQUALS(const uint32_t value1, const size_t value2, const char* location);

/**
 * Test whether two Matrix3 objects are equal.
 * @param[in] matrix1 The first object
 * @param[in] matrix2 The second object
 * @param[in] location The TEST_LOCATION macro should be used here
 */
void DALI_TEST_EQUALS(const Matrix3& matrix1, const Matrix3& matrix2, const char* location);

/** Test whether two Matrix3 objects are equal (fuzzy compare).
 * @param[in] matrix1 The first object
 * @param[in] matrix2 The second object
 * @param[in] epsilon The epsilon to use for comparison
 * @param[in] location The TEST_LOCATION macro should be used here
 */
void DALI_TEST_EQUALS(const Matrix3& matrix1, const Matrix3& matrix2, float epsilon, const char* location);

/**
 * Test whether two Matrix objects are equal.
 * @param[in] matrix1 The first object
 * @param[in] matrix2 The second object
 * @param[in] location The TEST_LOCATION macro should be used here
 */
void DALI_TEST_EQUALS(const Matrix& matrix1, const Matrix& matrix2, const char* location);

/**
 * Test whether two Matrix objects are equal (fuzzy-compare).
 * @param[in] matrix1 The first object
 * @param[in] matrix2 The second object
 * @param[in] location The TEST_LOCATION macro should be used here
 */
void DALI_TEST_EQUALS(const Matrix& matrix1, const Matrix& matrix2, float epsilon, const char* location);

/**
 * Test whether two strings are equal.
 * @param[in] str1 The first string
 * @param[in] str2 The second string
 * @param[in] location The TEST_LOCATION macro should be used here
 */
template<>
inline void DALI_TEST_EQUALS<const char*>(const char* str1, const char* str2, const char* location)
{
  if(strcmp(str1, str2))
  {
    fprintf(stderr, "Test failed in %s, checking '%s' == '%s'\n", location, str1, str2);
    tet_result(TET_FAIL);
    throw("TET_FAIL");
  }
  else
  {
    tet_result(TET_PASS);
  }
}

/**
 * Test whether two strings are equal.
 * @param[in] str1 The first string
 * @param[in] str2 The second string
 * @param[in] location The TEST_LOCATION macro should be used here
 */
template<>
inline void DALI_TEST_EQUALS<const std::string&>(const std::string& str1, const std::string& str2, const char* location)
{
  DALI_TEST_EQUALS(str1.c_str(), str2.c_str(), location);
}

/**
 * Test whether two strings are equal.
 * @param[in] str1 The first string
 * @param[in] str2 The second string
 * @param[in] location The TEST_LOCATION macro should be used here
 */
void DALI_TEST_EQUALS(Property::Value& str1, const char* str2, const char* location);

/**
 * Test whether two strings are equal.
 * @param[in] str1 The first string
 * @param[in] str2 The second string
 * @param[in] location The TEST_LOCATION macro should be used here
 */
void DALI_TEST_EQUALS(const std::string& str1, const char* str2, const char* location);

/**
 * Test whether two strings are equal.
 * @param[in] str1 The first string
 * @param[in] str2 The second string
 * @param[in] location The TEST_LOCATION macro should be used here
 */
void DALI_TEST_EQUALS(const char* str1, const std::string& str2, const char* location);

/**
 * Test whether two strings are equal.
 * @param[in] str1 The first string
 * @param[in] str2 The second string
 * @param[in] location The TEST_LOCATION macro should be used here
 */
template<>
inline void DALI_TEST_EQUALS<const std::string_view>(std::string_view str1, std::string_view str2, const char* location)
{
  DALI_TEST_EQUALS(str1.data(), str2.data(), location);
}

inline void DALI_TEST_EQUALS(std::string_view str1, const char* str2, const char* location)
{
  DALI_TEST_EQUALS(str1.data(), str2, location);
}

inline void DALI_TEST_EQUALS(std::string_view str1, const std::string& str2, const char* location)
{
  DALI_TEST_EQUALS(str1.data(), str2.c_str(), location);
}

inline void DALI_TEST_EQUALS(const std::string& str2, std::string_view str1, const char* location)
{
  DALI_TEST_EQUALS(str2.c_str(), str1.data(), location);
}

inline void DALI_TEST_EQUALS(const char* str1, std::string_view str2, const char* location)
{
  DALI_TEST_EQUALS(str1, str2.data(), location);
}

/**
 * Test if a property value type is equal to a trivial type.
 */
template<typename Type>
inline void DALI_TEST_VALUE_EQUALS(Property::Value&& value1, Type value2, float epsilon, const char* location)
{
  Property::Value value2b(value2);
  DALI_TEST_EQUALS(value1, value2b, epsilon, location);
}

/**
 * Test whether one unsigned integer value is greater than another.
 * Test succeeds if value1 > value2
 * @param[in] value1 The first value
 * @param[in] value2 The second value
 * @param[in] location The TEST_LOCATION macro should be used here
 */
template<typename T>
void DALI_TEST_GREATER(T value1, T value2, const char* location)
{
  if(!(value1 > value2))
  {
    std::cerr << "Test failed in " << location << ", checking " << value1 << " > " << value2 << "\n";
    tet_result(TET_FAIL);
    throw("TET_FAIL");
  }
  else
  {
    tet_result(TET_PASS);
  }
}

/**
 * Test whether the assertion condition that failed and thus triggered the
 * exception \b e contained a given substring.
 * @param[in] e The exception that we expect was fired by a runtime assertion failure.
 * @param[in] conditionSubString The text that we expect to be present in an
 *                               assertion which triggered the exception.
 * @param[in] location The TEST_LOCATION macro should be used here.
 */
void DALI_TEST_ASSERT(DaliException& e, std::string conditionSubString, const char* location);

/**
 * Print the assert
 * @param[in] e The exception that we expect was fired by a runtime assertion failure.
 */
inline void DALI_TEST_PRINT_ASSERT(DaliException& e)
{
  tet_printf("Assertion %s failed at %s\n", e.condition, e.location);
}

/**
 * Test that given piece of code triggers the right assertion
 * Fails the test if the assert didn't occur.
 * Turns off logging during the execution of the code to avoid excessive false positive log output from the assertions
 * @param expressions code to execute
 * @param assertstring the substring expected in the assert
 */
#define DALI_TEST_ASSERTION(expressions, assertstring)                                                      \
  try                                                                                                       \
  {                                                                                                         \
    TestApplication::EnableLogging(false);                                                                  \
    expressions;                                                                                            \
    TestApplication::EnableLogging(true);                                                                   \
    fprintf(stderr, "Test failed in %s, expected assert: '%s' didn't occur\n", __FILELINE__, assertstring); \
    tet_result(TET_FAIL);                                                                                   \
    throw("TET_FAIL");                                                                                      \
  }                                                                                                         \
  catch(Dali::DaliException & e)                                                                            \
  {                                                                                                         \
    DALI_TEST_ASSERT(e, assertstring, TEST_LOCATION);                                                       \
  }

/**
 * Test that given piece of code triggers an exception
 * Fails the test if the exception didn't occur.
 * Turns off logging during the execution of the code to avoid excessive false positive log output from the assertions
 * @param expressions code to execute
 * @param except the exception expected in the assert
 */
#define DALI_TEST_THROWS(expressions, except)                                                             \
  try                                                                                                     \
  {                                                                                                       \
    TestApplication::EnableLogging(false);                                                                \
    expressions;                                                                                          \
    TestApplication::EnableLogging(true);                                                                 \
    fprintf(stderr, "Test failed in %s, expected exception: '%s' didn't occur\n", __FILELINE__, #except); \
    tet_result(TET_FAIL);                                                                                 \
    throw("TET_FAIL");                                                                                    \
  }                                                                                                       \
  catch(except&)                                                                                          \
  {                                                                                                       \
    tet_result(TET_PASS);                                                                                 \
  }                                                                                                       \
  catch(...)                                                                                              \
  {                                                                                                       \
    fprintf(stderr, "Test failed in %s, unexpected exception\n", __FILELINE__);                           \
    tet_result(TET_FAIL);                                                                                 \
    throw;                                                                                                \
  }

// Functor to test whether an Applied signal is emitted
struct ConstraintAppliedCheck
{
  ConstraintAppliedCheck(bool& signalReceived);
  void  operator()(Constraint& constraint);
  void  Reset();
  void  CheckSignalReceived();
  void  CheckSignalNotReceived();
  bool& mSignalReceived; // owned by individual tests
};

/**
 * A Helper to test default functions
 */
template<typename T>
struct DefaultFunctionCoverage
{
  DefaultFunctionCoverage()
  {
    T  a;
    T* b = new T(a);
    DALI_TEST_CHECK(b);
    a = *b;
    delete b;
  }
};

// Test namespace to prevent pollution of Dali namespace, add Test helper functions here
namespace Test
{
/**
 *  @brief
 *
 *  Helper to check object destruction occurred
 *  1) In main part of code create an ObjectDestructionTracker
 *  2) Within sub section of main create object Actor test and call Start with Actor to test for destruction
 *  3) Perform code which is expected to destroy Actor
 *  4) Back in main part of code use IsDestroyed() to test if Actor was destroyed
 */
class ObjectDestructionTracker : public ConnectionTracker
{
public:
  /**
   * @brief Call in main part of code
   * @param[in] objectRegistry The object Registry being used
   */
  ObjectDestructionTracker(ObjectRegistry objectRegistry);

  /**
   * @brief Call in sub bock of code where the Actor being checked is still alive.
   *
   * @param[in] actor Actor to be checked for destruction
   */
  void Start(Actor actor);

  /**
   * @brief Call to check if Actor alive or destroyed.
   *
   * @return bool true if Actor was destroyed
   */
  bool IsDestroyed();

private:
  ObjectRegistry mObjectRegistry;
  bool           mRefObjectDestroyed;
};

} // namespace Test

#endif // DALI_TEST_SUITE_UTILS_H
