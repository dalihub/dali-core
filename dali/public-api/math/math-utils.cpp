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

// CLASS HEADER
#include <dali/public-api/math/math-utils.h>

// EXTERNAL INCLUDES
#include <math.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/constants.h>


namespace Dali
{

unsigned int NextPowerOfTwo(unsigned int i)
{
  DALI_ASSERT_DEBUG(i <= 1U << (sizeof(unsigned) * 8 - 1) && "Return type cannot represent the next power of two greater than the argument.");
  if(i==0)
  {
    return 1;
  }

  i--;
  i |= i >> 1;
  i |= i >> 2;
  i |= i >> 4;
  i |= i >> 8;
  i |= i >> 16;
  i++;
  return i;
}

bool IsPowerOfTwo(unsigned int i)
{
  return (i != 0) && ((i & (i - 1)) == 0);
}

float GetRangedEpsilon(float a, float b)
{
  float abs_f = std::max(fabsf(a), fabsf(b));
  int abs_i = (int) abs_f;

  float epsilon = Math::MACHINE_EPSILON_10000;
  if (abs_f < 0.1f)
  {
    return Math::MACHINE_EPSILON_0;
  }
  else if (abs_i < 2)
  {
    return Math::MACHINE_EPSILON_1;
  }
  else if (abs_i < 20)
  {
    return Math::MACHINE_EPSILON_10;
  }
  else if (abs_i < 200)
  {
    return Math::MACHINE_EPSILON_100;
  }
  else if (abs_i < 2000)
  {
    return Math::MACHINE_EPSILON_1000;
  }
  return epsilon;
}

// TODO: Change this to use #pragma GCC diagnostic push / pop when the compiler is updated to 4.6.0+
#pragma GCC diagnostic ignored "-Wfloat-equal"
bool EqualsZero( float value )
{
  return value == 0.0f;
}
#pragma GCC diagnostic error "-Wfloat-equal"

bool Equals( float a, float b )
{
  return ( fabsf( a - b ) <= GetRangedEpsilon( a, b ) );
}

bool Equals( float a, float b, float epsilon )
{
  return ( fabsf( a - b ) <= epsilon );
}

float Round(float value, int pos)
{
  float temp;
  temp = value * powf( 10, pos );
  temp = floorf( temp + 0.5 );
  temp *= powf( 10, -pos );
  return temp;
}

float WrapInDomain(float x, float start, float end)
{
  float domain = end - start;
  x -= start;

  if(fabsf(domain) > Math::MACHINE_EPSILON_1)
  {
    return start + (x - floorf(x / domain) * domain);
  }

  return start;
}

float ShortestDistanceInDomain(float a, float b, float start, float end)
{
  //  (a-start + end-b)
  float size = end-start;
  float vect = b-a;

  if(vect > 0)
  {
    // +ve vector, let's try perspective 1 domain to the right,
    // and see if closer.
    float aRight = a+size;
    if( aRight-b < vect )
    {
      return b-aRight;
    }
  }
  else
  {
    // -ve vector, let's try perspective 1 domain to the left,
    // and see if closer.
    float aLeft = a-size;
    if( aLeft-b > vect )
    {
      return b-aLeft;
    }
  }

  return vect;
}

} // namespace Dali
