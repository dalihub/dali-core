#ifndef __DALI_INTERNAL_SCENE_GRAPH_DOUBLE_BUFFERED_H__
#define __DALI_INTERNAL_SCENE_GRAPH_DOUBLE_BUFFERED_H__

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
 */
template <typename T>
class DoubleBuffered
{
public:

  DoubleBuffered()
  : mValue1(),
    mValue2()
  {
  }

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

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_DOUBLE_BUFFERED_H__
