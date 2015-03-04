#ifndef __DALI_INTERNAL_CONTEXT_FRAME_BUFFER_STATE_CACHE_H__
#define __DALI_INTERNAL_CONTEXT_FRAME_BUFFER_STATE_CACHE_H__

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

// INTERNAL INCLUDES
#include <dali/integration-api/gl-abstraction.h>
#include <dali/public-api/common/dali-vector.h>

namespace Dali
{

namespace Internal
{

/**
 * @brief Keeps track of color, depth and stencil buffer state within each frame buffer.
 * Used to avoid redundant glClear calls.
 *
 */
class FrameBufferStateCache
{
public:


  /**
   * @brief Constructor
   */
  FrameBufferStateCache();

  /**
   * @brief  non-virtual destructor
   */
  ~FrameBufferStateCache();

  /**
   * @brief Get the bitmask to pass to glClear based on the mask requested
   * and the current state of the frame buffer
   * @param[in] mask glClear bit mask
   * @param[in] forceClear whether to force the clear ( ignore cached state)
   * @param[in] scissorTestEnabled whether scissor test is enabled
   * @return new bitmask to pass to glClear
   */
  GLbitfield GetClearMask( GLbitfield mask, bool forceClear, bool scissorTestEnabled );

  /**
   * @brief Set the current bound frame buffer
   * @param[in] frameBufferId frame buffer id
   */
  void SetCurrentFrameBuffer( GLuint frameBufferId );

  /**
   * @brief Called when frame buffers are deleted
   * @param[in] count number of frame buffers
   * @param[in] framebuffers array of frame buffer ids
   */
  void FrameBuffersDeleted( GLsizei count, const GLuint* const frameBuffers );

  /**
   * @brief Called when frame buffers are created
   * @param[in] count number of frame buffers
   * @param[in] framebuffers array of frame buffer ids
   */
  void FrameBuffersCreated( GLsizei count, const GLuint* const frameBuffers );

  /**
   * @brief Draw operation performed on the current frame buffer
   * @param[in] colorBufferUsed whether the color buffer is being written to (glColorMask )
   * @param[in] depthBufferUsed whether the depth buffer is being written to (glDepthMask )
   * @param[in] stencilBufferUsed whether the stencil buffer is being written to (glStencilMask )
   */
  void DrawOperation( bool colorBufferUsed, bool depthBufferUsed, bool stencilBufferUsed );

  /**
   * Reset the cache
   */
  void Reset();

private:

  /**
   * Current status of the frame buffer
   */
  enum FrameBufferStatus
  {
    COLOR_BUFFER_CLEAN      = 1 << 0, ///< color buffer clean
    DEPTH_BUFFER_CLEAN      = 1 << 1, ///< depth buffer clean
    STENCIL_BUFFER_CLEAN    = 1 << 2, ///< stencil buffer clean
  };

  /**
   * POD to store the status of frame buffer regarding color,depth and stencil buffers
   */
  struct FrameBufferState
  {
    /**
     * Constructor
     */
    FrameBufferState( GLuint id, unsigned int state)
    :mId( id ),
    mState( state )
    {
    }
    GLuint mId;           ///< Frame buffer id
    unsigned int mState;  ///< State, bitmask of FrameBufferStatus flags
  };

  typedef Dali::Vector< FrameBufferState > FrameBufferStateVector;

  /**
   * @brief Set the clear state
   * @param[in] pointer to frame buffer state object
   * @param[in] mask clear mask
   */
  void SetClearState( FrameBufferState* state, GLbitfield mask );

  /**
   * @brief Helper
   * @param[in] frameBufferId frame buffer id
   * @return pointer to  frame buffer state object ( NULL if it doesn't exist)
   */
  FrameBufferState* GetFrameBufferState( GLuint frameBufferId );

  /**
   * @brief Helper to delete a frame buffer state object
   * @param[in] frameBufferId frame buffer id
   */
  void DeleteFrameBuffer( GLuint frameBufferId );

  /**
   * @brief Get the default state of a frame buffer, before it's used
   * @return initial state
   */
  unsigned int GetInitialFrameBufferState();

  FrameBufferStateCache( const FrameBufferStateCache& );            ///< undefined copy constructor

  FrameBufferStateCache& operator=( const FrameBufferStateCache& ); ///< undefined assignment operator

private: // data

  FrameBufferStateVector    mFrameBufferStates;     ///< state of the frame buffers
  GLuint                    mCurrentFrameBufferId;  ///< currently bound frame buffer
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_CONTEXT_FRAME_BUFFER_STATE_CACHE_H__
