#ifndef __DALI_TEST_SUITE_UTILS_H__
#define __DALI_TEST_SUITE_UTILS_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/dali-core.h>

#include <tet_api.h>

#include <test-application.h>

namespace Dali
{

#define STRINGIZE_I(text) #text
#define STRINGIZE(text) STRINGIZE_I(text)

// the following is the other compilers way of token pasting, gcc seems to just concatenate strings automatically
//#define TOKENPASTE(x,y) x ## y
#define TOKENPASTE(x,y) x y
#define TOKENPASTE2(x,y) TOKENPASTE( x, y )
#define TEST_LOCATION TOKENPASTE2( "Test failed in ", TOKENPASTE2( __FILE__, TOKENPASTE2( ", line ", STRINGIZE(__LINE__) ) ) )

/**
 * Helper code to wrap each test function with an exception handler
 */

int numberOfTETTestFunctions;       /// Counter for the number of functions added so far

/**
 * This structure is used to initialize the list of functions.
 * Creating an object of this type will add the function to the list.
 */
struct AddFunction {
  AddFunction( void (*function)(), int type )
  {
    tet_testlist[numberOfTETTestFunctions].testfunc = function;
    tet_testlist[numberOfTETTestFunctions].icref = type;
    ++numberOfTETTestFunctions;
  }
};

#define TEST_FUNCTION( FUNCTION_NAME, TEST_TYPE )                           \
static void FUNCTION_NAME();                                                \
static void check ## FUNCTION_NAME ()                                       \
{                                                                           \
  tet_printf( "START OF TEST " #FUNCTION_NAME "\n" );                       \
  try {                                                                     \
    FUNCTION_NAME();                                                        \
  } catch (...) {                                                           \
    tet_printf( "Unhandled exception in function " #FUNCTION_NAME ".\n" );  \
    tet_result( TET_FAIL );                                                 \
  }                                                                         \
  tet_printf( "END OF TEST " #FUNCTION_NAME "\n" );                         \
}                                                                           \
AddFunction add ## FUNCTION_NAME ( check ## FUNCTION_NAME, TEST_TYPE );


/**
 * DALI_TEST_CHECK is a wrapper for tet_result.
 * If the condition evaluates to false, then the function & line number is printed.
 * @param[in] The boolean expression to check
 */
#define DALI_TEST_CHECK(condition)                                                        \
if ( (condition) )                                                                        \
{                                                                                         \
  tet_result(TET_PASS);                                                                   \
}                                                                                         \
else                                                                                      \
{                                                                                         \
  tet_printf("%s Failed in %s at line %d\n", __PRETTY_FUNCTION__, __FILE__, __LINE__);    \
  tet_result(TET_FAIL);                                                                   \
}

template <typename Type>
bool CompareType(Type value1, Type value2, float epsilon);



/**
 * A helper for fuzzy-comparing Vector2 objects
 * @param[in] vector1 the first object
 * @param[in] vector2 the second object
 * @param[in] epsilon difference threshold
 * @returns true if difference is smaller than epsilon threshold, false otherwise
 */
template <>
bool CompareType<float>(float value1, float value2, float epsilon)
{
  return fabsf(value1 - value2) < epsilon;
}

/**
 * A helper for fuzzy-comparing Vector2 objects
 * @param[in] vector1 the first object
 * @param[in] vector2 the second object
 * @param[in] epsilon difference threshold
 * @returns true if difference is smaller than epsilon threshold, false otherwise
 */
template <>
bool CompareType<Vector2>(Vector2 vector1, Vector2 vector2, float epsilon)
{
  return fabsf(vector1.x - vector2.x)<epsilon && fabsf(vector1.y - vector2.y)<epsilon;
}

/**
 * A helper for fuzzy-comparing Vector3 objects
 * @param[in] vector1 the first object
 * @param[in] vector2 the second object
 * @param[in] epsilon difference threshold
 * @returns true if difference is smaller than epsilon threshold, false otherwise
 */
template <>
bool CompareType<Vector3>(Vector3 vector1, Vector3 vector2, float epsilon)
{
  return fabsf(vector1.x - vector2.x)<epsilon &&
         fabsf(vector1.y - vector2.y)<epsilon &&
         fabsf(vector1.z - vector2.z)<epsilon;
}


/**
 * A helper for fuzzy-comparing Vector4 objects
 * @param[in] vector1 the first object
 * @param[in] vector2 the second object
 * @param[in] epsilon difference threshold
 * @returns true if difference is smaller than epsilon threshold, false otherwise
 */
template <>
bool CompareType<Vector4>(Vector4 vector1, Vector4 vector2, float epsilon)
{
  return fabsf(vector1.x - vector2.x)<epsilon &&
         fabsf(vector1.y - vector2.y)<epsilon &&
         fabsf(vector1.z - vector2.z)<epsilon &&
         fabsf(vector1.w - vector2.w)<epsilon;
}

template <>
bool CompareType<Quaternion>(Quaternion q1, Quaternion q2, float epsilon)
{
  Quaternion q2N = -q2; // These quaternions represent the same rotation
  return CompareType<Vector4>(q1.mVector, q2.mVector, epsilon) || CompareType<Vector4>(q1.mVector, q2N.mVector, epsilon);
}

template <>
bool CompareType<Radian>(Radian q1, Radian q2, float epsilon)
{
  return CompareType<float>(float(q1), float(q2), epsilon);
}

template <>
bool CompareType<Degree>(Degree q1, Degree q2, float epsilon)
{
  return CompareType<float>(float(q1), float(q2), epsilon);
}

bool operator==(TimePeriod a, TimePeriod b)
{
  return Equals(a.durationSeconds, b.durationSeconds) && Equals(a.delaySeconds, b.delaySeconds) ;
}

std::ostream& operator<< (std::ostream& o, const TimePeriod value)
{
  return o << "( Duration:" << value.durationSeconds << " Delay:" << value.delaySeconds << ")";
}

/**
 * Test whether two values are equal.
 * @param[in] value1 The first value
 * @param[in] value2 The second value
 * @param[in] location The TEST_LOCATION macro should be used here
 */
template<typename TypeA, typename TypeB>
void DALI_TEST_EQUALS(TypeA value1, TypeB value2, const char* location)
{
  if (!(value1 == value2))
  {
    std::ostringstream o;
    o << value1 << " == " << value2 << std::endl;
    tet_printf("%s, checking %s", location, o.str().c_str());
    tet_result(TET_FAIL);
  }
  else
  {
    tet_result(TET_PASS);
  }
}

template<typename Type>
void DALI_TEST_EQUALS(Type value1, Type value2, float epsilon, const char* location)
{
  if( !CompareType<Type>(value1, value2, epsilon) )
  {
    std::ostringstream o;
    o << value1 << " == " << value2 << std::endl;
    tet_printf("%s, checking %s", location, o.str().c_str());
    tet_result(TET_FAIL);
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
void DALI_TEST_EQUALS<TimePeriod>( TimePeriod value1, TimePeriod value2, float epsilon, const char* location)
{
  if ((fabs(value1.durationSeconds - value2.durationSeconds) > epsilon))
  {
    tet_printf("%s, checking durations %f == %f, epsilon %f\n", location, value1.durationSeconds, value2.durationSeconds, epsilon);
    tet_result(TET_FAIL);
  }
  else if ((fabs(value1.delaySeconds - value2.delaySeconds) > epsilon))
  {
    tet_printf("%s, checking delays %f == %f, epsilon %f\n", location, value1.delaySeconds, value2.delaySeconds, epsilon);
    tet_result(TET_FAIL);
  }
  else
  {
    tet_result(TET_PASS);
  }
}

/**
 * Test whether two Matrix objects are equal.
 * @param[in] matrix1 The first object
 * @param[in] matrix2 The second object
 * @param[in] location The TEST_LOCATION macro should be used here
 */
template <>
void DALI_TEST_EQUALS<const Matrix&>( const Matrix& matrix1, const Matrix& matrix2, const char* location)
{
  const float* m1 = matrix1.AsFloat();
  const float* m2 = matrix2.AsFloat();
  bool identical = true;

  int i;
  for (i=0;i<16;++i)
  {
    if(m1[i] != m2[i])
    {
      identical = false;
      break;
    }
  }

  if (!identical)
  {
    tet_printf("%s, checking\n"
               "(%f, %f, %f, %f)    (%f, %f, %f, %f)\n"
               "(%f, %f, %f, %f) == (%f, %f, %f, %f)\n"
               "(%f, %f, %f, %f)    (%f, %f, %f, %f)\n"
               "(%f, %f, %f, %f)    (%f, %f, %f, %f)\n", location,
               m1[0],  m1[1],  m1[2],  m1[3],   m2[0],  m2[1],  m2[2],  m2[3],
               m1[4],  m1[5],  m1[6],  m1[7],   m2[4],  m2[5],  m2[6],  m2[7],
               m1[8],  m1[9], m1[10], m1[11],   m2[8],  m2[9], m2[10], m2[11],
              m1[12], m1[13], m1[14], m1[15],  m2[12], m2[13], m2[14], m2[15]);

    tet_result(TET_FAIL);
  }
  else
  {
    tet_result(TET_PASS);
  }
}

/**
 * Test whether two Matrix objects are equal (fuzzy-compare).
 * @param[in] matrix1 The first object
 * @param[in] matrix2 The second object
 * @param[in] location The TEST_LOCATION macro should be used here
 */
template <>
void DALI_TEST_EQUALS<const Matrix&>( const Matrix& matrix1, const Matrix& matrix2, float epsilon, const char* location)
{
  const float* m1 = matrix1.AsFloat();
  const float* m2 = matrix2.AsFloat();
  bool equivalent = true;

  for (int i=0;i<16;++i)
  {
    equivalent &= (fabsf(m1[i] - m2[i])<epsilon);
  }

  if (!equivalent)
  {
    tet_printf("%s, checking\n"
               "(%f, %f, %f, %f)    (%f, %f, %f, %f)\n"
               "(%f, %f, %f, %f) == (%f, %f, %f, %f)\n"
               "(%f, %f, %f, %f)    (%f, %f, %f, %f)\n"
               "(%f, %f, %f, %f)    (%f, %f, %f, %f)\n", location,
               m1[0],  m1[1],  m1[2],  m1[3],   m2[0],  m2[1],  m2[2],  m2[3],
               m1[4],  m1[5],  m1[6],  m1[7],   m2[4],  m2[5],  m2[6],  m2[7],
               m1[8],  m1[9], m1[10], m1[11],   m2[8],  m2[9], m2[10], m2[11],
              m1[12], m1[13], m1[14], m1[15],  m2[12], m2[13], m2[14], m2[15]);

    tet_result(TET_FAIL);
  }
  else
  {
    tet_result(TET_PASS);
  }
}

template <>
void DALI_TEST_EQUALS<Matrix>( Matrix matrix1, Matrix matrix2, float epsilon, const char* location)
{
  const float* m1 = matrix1.AsFloat();
  const float* m2 = matrix2.AsFloat();
  bool equivalent = true;

  for (int i=0;i<16;++i)
  {
    equivalent &= (fabsf(m1[i] - m2[i])<epsilon);
  }

  if (!equivalent)
  {
    tet_printf("%s, checking\n"
               "(%f, %f, %f, %f)    (%f, %f, %f, %f)\n"
               "(%f, %f, %f, %f) == (%f, %f, %f, %f)\n"
               "(%f, %f, %f, %f)    (%f, %f, %f, %f)\n"
               "(%f, %f, %f, %f)    (%f, %f, %f, %f)\n", location,
               m1[0],  m1[1],  m1[2],  m1[3],   m2[0],  m2[1],  m2[2],  m2[3],
               m1[4],  m1[5],  m1[6],  m1[7],   m2[4],  m2[5],  m2[6],  m2[7],
               m1[8],  m1[9], m1[10], m1[11],   m2[8],  m2[9], m2[10], m2[11],
              m1[12], m1[13], m1[14], m1[15],  m2[12], m2[13], m2[14], m2[15]);

    tet_result(TET_FAIL);
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
void DALI_TEST_EQUALS<const char*>( const char* str1, const char* str2, const char* location)
{
  if (strcmp(str1, str2))
  {
    tet_printf("%s, checking '%s' == '%s'\n", location, str1, str2);
    tet_result(TET_FAIL);
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
void DALI_TEST_EQUALS<const std::string&>( const std::string &str1, const std::string &str2, const char* location)
{
  DALI_TEST_EQUALS(str1.c_str(), str2.c_str(), location);
}

/**
 * Test whether two strings are equal.
 * @param[in] str1 The first string
 * @param[in] str2 The second string
 * @param[in] location The TEST_LOCATION macro should be used here
 */
void DALI_TEST_EQUALS( const std::string &str1, const char* str2, const char* location)
{
  DALI_TEST_EQUALS(str1.c_str(), str2, location);
}

/**
 * Test whether two strings are equal.
 * @param[in] str1 The first string
 * @param[in] str2 The second string
 * @param[in] location The TEST_LOCATION macro should be used here
 */
void DALI_TEST_EQUALS( const char* str1, const std::string &str2, const char* location)
{
  DALI_TEST_EQUALS(str1, str2.c_str(), location);
}

/**
 * Test whether two UTF32 strings are equal.
 * @param[in] str1 The first string
 * @param[in] str2 The second string
 * @param[in] location The TEST_LOCATION macro should be used here
 */
template<>
void DALI_TEST_EQUALS<const TextArray&>( const TextArray& str1, const TextArray& str2, const char* location)
{
  if (!std::equal(str1.begin(), str1.end(), str2.begin()))
  {
    tet_printf("%s, checking '", location);

    for( unsigned int i = 0; i < str1.size(); ++i)
    {
      tet_printf("%c", str1[i]);
    }

    tet_printf("' == '");

    for( unsigned int i = 0; i < str2.size(); ++i)
    {
      tet_printf("%c", str2[i]);
    }

    tet_printf("'\n");

    tet_result(TET_FAIL);
  }
  else
  {
    tet_result(TET_PASS);
  }
}

/**
 * Test whether one unsigned integer value is greater than another.
 * Test succeeds if value1 > value2
 * @param[in] value1 The first value
 * @param[in] value2 The second value
 * @param[in] location The TEST_LOCATION macro should be used here
 */
void DALI_TEST_GREATER(unsigned int value1, unsigned int value2, const char* location)
{
  if (!(value1 > value2))
  {
    tet_printf("%s, checking %d > %d\n", location, value1, value2);
    tet_result(TET_FAIL);
  }
  else
  {
    tet_result(TET_PASS);
  }
}

/**
 * Test whether one float value is greater than another.
 * Test succeeds if value1 > value2
 * @param[in] value1 The first value
 * @param[in] value2 The second value
 * @param[in] location The TEST_LOCATION macro should be used here
 */
void DALI_TEST_GREATER( float value1, float value2, const char* location)
{
  if (!(value1 > value2))
  {
    tet_printf("%s, checking %f > %f\n", location, value1, value2);
    tet_result(TET_FAIL);
  }
  else
  {
    tet_result(TET_PASS);
  }
}

/**
 * Test whether the assertion condition that failed and thus triggered the
 * exception \b e contained a given substring at the start of its literal text.
 * @param[in] e The exception that we expect was fired by a runtime assertion
 *              failure.
 * @param[in] conditionSubString The text that we expect to be present in an
 *                               assertion which triggered the exception.
 * @param[in] location The TEST_LOCATION macro should be used here.
 *
 * @remark **Side-effects:** The result of the tet test is set to TET_PASS if
 *         the substring is at the start of the exception's condition and
 *         TET_FAIL if it isn't. Note, if the result of a test is set multiple
 *         times, a TET_FAIL will override any number of TET_PASSes.
 */
void DALI_TEST_ASSERT( DaliException& e, std::string conditionSubString, const char* location )
{
  if( 0u != e.mCondition.find( conditionSubString ))
  {
    tet_printf("Assertion %s failed at %s\n", conditionSubString.c_str(), location);
    tet_result(TET_FAIL);
  }
  else
  {
    tet_result(TET_PASS);
  }
}

/** Self-documenting wrapper for DALI_TEST_ASSERT.
 * @copydoc DALI_TEST_ASSERT()
 */
void DALI_TEST_ASSERT_CONDITION_STARTS_WITH_SUBSTRING( DaliException& exceptionFromAssertion, std::string conditionSubString, const char* location )
{
  DALI_TEST_ASSERT(exceptionFromAssertion, conditionSubString, location);
}

// Functor to test whether an Applied signal is emitted
struct ConstraintAppliedCheck
{
  ConstraintAppliedCheck( bool& signalReceived )
  : mSignalReceived( signalReceived )
  {
  }

  void operator()( ActiveConstraint& constraint )
  {
    mSignalReceived = true;
  }

  void Reset()
  {
    mSignalReceived = false;
  }

  void CheckSignalReceived()
  {
    if ( !mSignalReceived )
    {
      tet_printf( "Expected Applied signal was not received\n" );
      tet_result( TET_FAIL );
    }
    else
    {
      tet_result( TET_PASS );
    }
  }

  void CheckSignalNotReceived()
  {
    if ( mSignalReceived )
    {
      tet_printf( "Unexpected Applied signal was received\n" );
      tet_result( TET_FAIL );
    }
    else
    {
      tet_result( TET_PASS );
    }
  }

  bool& mSignalReceived; // owned by individual tests
};

/**
 * A Helper to test default functions
 */
template <typename T>
struct DefaultFunctionCoverage
{
  DefaultFunctionCoverage()
  {
    T a;
    T *b = new T(a);
    DALI_TEST_CHECK(b);
    a = *b;
    delete b;
  }
};

} // namespace Dali

#endif // __DALI_TEST_SUITE_UTILS_H__
