#ifndef __DALI_INTERNAL_SCENE_GRAPH_BUFFERS_H__
#define __DALI_INTERNAL_SCENE_GRAPH_BUFFERS_H__

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
#include <dali/internal/common/buffer-index.h>
#include <atomic>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

/**
 * Node values (position etc.) are double-buffered.  A SceneGraphBuffers object
 * can be used to keep track of which buffers are being written or read.
 */
class SceneGraphBuffers
{
public:

  static BufferIndex INITIAL_EVENT_BUFFER_INDEX;  // 0
  static BufferIndex INITIAL_UPDATE_BUFFER_INDEX; // 1

  /**
   * Create a SceneGraphBuffers object.
   */
  SceneGraphBuffers();

  /**
   * Non-virtual destructor; not intended as a base class.
   */
  ~SceneGraphBuffers();

  /**
   * Retrieve the current event-buffer index.
   * @return The buffer index.
   */
  BufferIndex GetEventBufferIndex() const { return mEventBufferIndex; }

  /**
   * Retrieve the current update-buffer index.
   * @return The buffer index.
   */
  BufferIndex GetUpdateBufferIndex() const { return mUpdateBufferIndex; }

  /**
   * Swap the Event & Update buffer indices.
  */
  void Swap();

private:

  // Undefined
  SceneGraphBuffers(const SceneGraphBuffers&);

  // Undefined
  SceneGraphBuffers& operator=(const SceneGraphBuffers& rhs);

private:
  std::atomic<BufferIndex> mEventBufferIndex;  ///< 0 or 1 (opposite of mUpdateBufferIndex)
  BufferIndex mUpdateBufferIndex; ///< 0 or 1 (opposite of mEventBufferIndex)
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_BUFFERS_H__
