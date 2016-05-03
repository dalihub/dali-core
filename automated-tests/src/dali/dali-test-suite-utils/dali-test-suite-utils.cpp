/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include "dali-test-suite-utils.h"

// EXTERNAL INCLUDES
#include <ostream>

// INTERNAL INCLUDES
#include <dali/public-api/dali-core.h>

using namespace Dali;

int test_return_value = TET_UNDEF;

void tet_result(int value)
{
  // First TET_PASS should set to zero
  // first TET_FAIL should prevent any further TET_PASS from setting back to zero
  // Any TET_FAIL should set to fail or leave as fail
  if( test_return_value != 1 )
    test_return_value = value;
}

#define END_TEST \
  return ((test_return_value>0)?1:0)


void tet_infoline(const char* str)
{
  fprintf(stderr, "%s\n", str);
}

void tet_printf(const char *format, ...)
{
  va_list arg;
  va_start(arg, format);
  vfprintf(stderr, format, arg);
  va_end(arg);
}

bool operator==(TimePeriod a, TimePeriod b)
{
  return Equals(a.durationSeconds, b.durationSeconds) && Equals(a.delaySeconds, b.delaySeconds) ;
}

std::ostream& operator<<( std::ostream& ostream, TimePeriod value )
{
  return ostream << "( Duration:" << value.durationSeconds << " Delay:" << value.delaySeconds << ")";
}

std::ostream& operator<<( std::ostream& ostream, Radian angle )
{
  ostream << angle.radian;
  return ostream;
}

std::ostream& operator<<( std::ostream& ostream, Degree angle )
{
  ostream << angle.degree;
  return ostream;
}

void DALI_TEST_EQUALS( const BaseHandle& baseHandle1, const BaseHandle& baseHandle2, const char* location )
{
  DALI_TEST_EQUALS< const BaseHandle& >( baseHandle1, baseHandle2, location );
}

void DALI_TEST_EQUALS( const size_t value1, const unsigned int value2, const char* location )
{
  DALI_TEST_EQUALS< unsigned int>( ( unsigned int )( value1 ), value2, location );
}

void DALI_TEST_EQUALS( const unsigned int value1, const size_t value2, const char* location )
{
  DALI_TEST_EQUALS< unsigned int >( value1, ( unsigned int )( value2 ), location );
}

void DALI_TEST_EQUALS( const Matrix3& matrix1, const Matrix3& matrix2, const char* location)
{
  const float* m1 = matrix1.AsFloat();
  const float* m2 = matrix2.AsFloat();
  bool equivalent = true;

  for (int i=0;i<9;++i)
  {
    if( ! (fabsf(m1[i] - m2[i])< GetRangedEpsilon(m1[i], m2[i])) )
    {
      equivalent = false;
    }
  }

  if( !equivalent )
  {
    fprintf(stderr, "%s, checking\n"
               "(%f, %f, %f)    (%f, %f, %f)\n"
               "(%f, %f, %f) == (%f, %f, %f)\n"
               "(%f, %f, %f)    (%f, %f, %f)\n",
               location,
               m1[0],  m1[1], m1[2],   m2[0],  m2[1], m2[2],
               m1[3],  m1[4], m1[5],   m2[3],  m2[4], m2[5],
               m1[6],  m1[7], m1[8],   m2[6],  m2[7], m2[8]);

    tet_result(TET_FAIL);
  }
  else
  {
    tet_result(TET_PASS);
  }
}

void DALI_TEST_EQUALS( const Matrix3& matrix1, const Matrix3& matrix2, float epsilon, const char* location)
{
  const float* m1 = matrix1.AsFloat();
  const float* m2 = matrix2.AsFloat();
  bool equivalent = true;

  for (int i=0;i<9;++i)
  {
    equivalent &= (fabsf(m1[i] - m2[i])<epsilon);
  }

  if (!equivalent)
  {
    fprintf(stderr, "%s, checking\n"
               "(%f, %f, %f)    (%f, %f, %f)\n"
               "(%f, %f, %f) == (%f, %f, %f)\n"
               "(%f, %f, %f)    (%f, %f, %f)\n",
               location,
               m1[0],  m1[1], m1[2],   m2[0],  m2[1], m2[2],
               m1[3],  m1[4], m1[5],   m2[3],  m2[4], m2[5],
               m1[6],  m1[7], m1[8],   m2[6],  m2[7], m2[8]);

    tet_result(TET_FAIL);
  }
  else
  {
    tet_result(TET_PASS);
  }
}

void DALI_TEST_EQUALS( const Matrix& matrix1, const Matrix& matrix2, const char* location)
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
    fprintf(stderr, "%s, checking\n"
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

void DALI_TEST_EQUALS( const Matrix& matrix1, const Matrix& matrix2, float epsilon, const char* location)
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
    fprintf(stderr, "%s, checking\n"
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

void DALI_TEST_ASSERT( DaliException& e, std::string conditionSubString, const char* location )
{
  if( NULL == strstr( e.condition, conditionSubString.c_str() ) )
  {
    fprintf(stderr, "Expected substring '%s' : actual exception string '%s' : location %s\n", conditionSubString.c_str(), e.condition, location );
    tet_result(TET_FAIL);
  }
  else
  {
    tet_result(TET_PASS);
  }
}

// Functor to test whether an Applied signal is emitted
ConstraintAppliedCheck::ConstraintAppliedCheck( bool& signalReceived )
: mSignalReceived( signalReceived )
{
}

void ConstraintAppliedCheck::operator()( Constraint& constraint )
{
  mSignalReceived = true;
}

void ConstraintAppliedCheck::Reset()
{
  mSignalReceived = false;
}

void ConstraintAppliedCheck::CheckSignalReceived()
{
  if ( !mSignalReceived )
  {
    fprintf(stderr,  "Expected Applied signal was not received\n" );
    tet_result( TET_FAIL );
  }
  else
  {
    tet_result( TET_PASS );
  }
}

void ConstraintAppliedCheck::CheckSignalNotReceived()
{
  if ( mSignalReceived )
  {
    fprintf(stderr,  "Unexpected Applied signal was received\n" );
    tet_result( TET_FAIL );
  }
  else
  {
    tet_result( TET_PASS );
  }
}

BufferImage CreateBufferImage(int width, int height, const Vector4& color)
{
  BufferImage image = BufferImage::New(width, height, Pixel::RGBA8888);

  PixelBuffer* pixbuf = image.GetBuffer();

  // Using a 4x4 image gives a better blend with the GL implementation
  // than a 3x3 image
  for(size_t i=0; i<16; i++)
  {
    pixbuf[i*4+0] = color.r*255;
    pixbuf[i*4+1] = color.g*255;
    pixbuf[i*4+2] = color.b*255;
    pixbuf[i*4+3] = color.a*255;
  }

  return image;
}

BufferImage CreateBufferImage()
{
  return CreateBufferImage(4, 4, Color::WHITE);
}
