#ifndef DALI_TEST_COMPARE_TYPES_H
#define DALI_TEST_COMPARE_TYPES_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
 */

#include <dali/public-api/dali-core.h>
using namespace Dali;


template <typename Type>
inline bool CompareType(Type value1, Type value2, float epsilon)
{
  return value1 == value2;
}

/**
 * A helper for fuzzy-comparing Vector2 objects
 * @param[in] vector1 the first object
 * @param[in] vector2 the second object
 * @param[in] epsilon difference threshold
 * @returns true if difference is smaller than epsilon threshold, false otherwise
 */
template <>
inline bool CompareType<float>(float value1, float value2, float epsilon)
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
inline bool CompareType<Vector2>(Vector2 vector1, Vector2 vector2, float epsilon)
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
inline bool CompareType<Vector3>(Vector3 vector1, Vector3 vector2, float epsilon)
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
inline bool CompareType<Vector4>(Vector4 vector1, Vector4 vector2, float epsilon)
{
  return fabsf(vector1.x - vector2.x)<epsilon &&
         fabsf(vector1.y - vector2.y)<epsilon &&
         fabsf(vector1.z - vector2.z)<epsilon &&
         fabsf(vector1.w - vector2.w)<epsilon;
}

template <>
inline bool CompareType<Quaternion>(Quaternion q1, Quaternion q2, float epsilon)
{
  Quaternion q2N = -q2; // These quaternions represent the same rotation
  return CompareType<Vector4>(q1.mVector, q2.mVector, epsilon) || CompareType<Vector4>(q1.mVector, q2N.mVector, epsilon);
}

template <>
inline bool CompareType<Radian>(Radian q1, Radian q2, float epsilon)
{
  return CompareType<float>(q1.radian, q2.radian, epsilon);
}

template <>
inline bool CompareType<Degree>(Degree q1, Degree q2, float epsilon)
{
  return CompareType<float>(q1.degree, q2.degree, epsilon);
}

template <>
inline bool CompareType<Property::Value>(Property::Value q1, Property::Value q2, float epsilon)
{
  Property::Type type = q1.GetType();
  if( type != q2.GetType() )
  {
    return false;
  }

  bool result = false;
  switch(type)
  {
    case Property::BOOLEAN:
    {
      bool a, b;
      q1.Get(a);
      q2.Get(b);
      result =  a == b;
      break;
    }
    case Property::INTEGER:
    {
      int a, b;
      q1.Get(a);
      q2.Get(b);
      result =  a == b;
      break;
    }
    case Property::FLOAT:
    {
      float a, b;
      q1.Get(a);
      q2.Get(b);
      result =  CompareType<float>(a, b, epsilon);
      break;
    }
    case Property::VECTOR2:
    {
      Vector2 a, b;
      q1.Get(a);
      q2.Get(b);
      result = CompareType<Vector2>(a, b, epsilon);
      break;
    }
    case Property::VECTOR3:
    {
      Vector3 a, b;
      q1.Get(a);
      q2.Get(b);
      result = CompareType<Vector3>(a, b, epsilon);
      break;
    }
    case Property::RECTANGLE:
    case Property::VECTOR4:
    {
      Vector4 a, b;
      q1.Get(a);
      q2.Get(b);
      result = CompareType<Vector4>(a, b, epsilon);
      break;
    }
    case Property::ROTATION:
    {
      Quaternion a, b;
      q1.Get(a);
      q2.Get(b);
      result = CompareType<Quaternion>(a, b, epsilon);
      break;
    }
    case Property::MATRIX:
    case Property::MATRIX3:
    case Property::STRING:
    case Property::ARRAY:
    case Property::MAP:
    {
      //TODO: Implement this?
      DALI_ASSERT_ALWAYS( 0 && "Not implemented");
      result = false;
      break;
    }
    case Property::NONE:
    {
      result = false;
      break;
    }
  }

  return result;
}



#endif
