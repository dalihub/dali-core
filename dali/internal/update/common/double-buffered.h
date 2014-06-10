#ifndef __DALI_INTERNAL_SCENE_GRAPH_DOUBLE_BUFFERED_H__
#define __DALI_INTERNAL_SCENE_GRAPH_DOUBLE_BUFFERED_H__

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
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/math/vector3.h>

namespace Dali
{

namespace Internal
{

// The number of buffers per scene-graph property
static const unsigned int NUM_SCENE_GRAPH_BUFFERS = 2;

// Buffer index used when reading off-stage values
static const unsigned int ARBITRARY_OFF_STAGE_BUFFER = 0;

namespace SceneGraph
{

/**
 * Templated class for a double-buffered value.
 * This simplifies init code, and forces explicit initialization.
 */
template <typename T>
class DoubleBuffered
{
public:

  DoubleBuffered(const T& val)
  : mValue1(val),
    mValue2(val)
  {
  }

  inline T& operator[](const size_t i)
  {
    DALI_ASSERT_DEBUG(i < NUM_SCENE_GRAPH_BUFFERS);

    return *(&mValue1+i);
  }

  inline const T& operator[](const size_t i) const
  {
    DALI_ASSERT_DEBUG(i < NUM_SCENE_GRAPH_BUFFERS);

    return *(&mValue1+i);
  }

private:

  // Undefined
  DoubleBuffered<T>(const DoubleBuffered<T>&);

  // Undefined
  DoubleBuffered<T>& operator=(const DoubleBuffered<T>& rhs);

private:

  T mValue1;
  T mValue2;
};

/**
 * Templated class for a double-buffered value, initialized with 3 value parameters.
 * This simplifies init code, and forces explicit initialization.
 */
template <typename T, typename P>
class DoubleBuffered3
{
public:

  DoubleBuffered3(const T& val)
  : mValue1(val),
    mValue2(val)
  {
  }

  DoubleBuffered3(P val1, P val2, P val3)
  : mValue1(val1, val2, val3),
    mValue2(val1, val2, val3)
  {
  }

  inline T& operator[](const size_t i)
  {
    DALI_ASSERT_DEBUG(i < NUM_SCENE_GRAPH_BUFFERS);

    return *(&mValue1+i);
  }

  inline const T& operator[](const size_t i) const
  {
    DALI_ASSERT_DEBUG(i < NUM_SCENE_GRAPH_BUFFERS);

    return *(&mValue1+i);
  }

private:

  // Undefined
  DoubleBuffered3<T,P>(const DoubleBuffered3<T,P>&);

  // Undefined
  DoubleBuffered3<T,P>& operator=(const DoubleBuffered3<T,P>& rhs);

private:

  T mValue1;
  T mValue2;
};

/**
 * Templated class for a double-buffered value, initialized with 4 value parameters.
 * This simplifies init code, and forces explicit initialization.
 */
template <typename T, typename P>
class DoubleBuffered4
{
public:

  DoubleBuffered4(const T& val)
  : mValue1(val),
    mValue2(val)
  {
  }

  DoubleBuffered4(P val1, P val2, P val3, P val4)
  : mValue1(val1, val2, val3, val4),
    mValue2(val1, val2, val3, val4)
  {
  }

  inline T& operator[](const size_t i)
  {
    DALI_ASSERT_DEBUG(i < NUM_SCENE_GRAPH_BUFFERS);

    return *(&mValue1+i);
  }

  inline const T& operator[](const size_t i) const
  {
    DALI_ASSERT_DEBUG(i < NUM_SCENE_GRAPH_BUFFERS);

    return *(&mValue1+i);
  }

private:

  // Undefined
  DoubleBuffered4<T,P>(const DoubleBuffered4<T,P>&);

  // Undefined
  DoubleBuffered4<T,P>& operator=(const DoubleBuffered4<T,P>& rhs);

private:

  T mValue1;
  T mValue2;
};

typedef DoubleBuffered<int>   DoubleBufferedInt;
typedef DoubleBuffered<float> DoubleBufferedFloat;
typedef DoubleBuffered<bool>  DoubleBufferedBool;

typedef DoubleBuffered3<Vector3,float>      DoubleBufferedVector3;
typedef DoubleBuffered4<Vector4,float>      DoubleBufferedVector4;

typedef DoubleBuffered4<Quaternion, float>  DoubleBufferedQuaternion;

typedef DoubleBuffered<Matrix> DoubleBufferedMatrix;

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_DOUBLE_BUFFERED_H__
