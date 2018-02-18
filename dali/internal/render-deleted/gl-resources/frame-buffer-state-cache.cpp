/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include "frame-buffer-state-cache.h"

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/gl-defines.h>

namespace Dali
{

namespace Internal
{

FrameBufferStateCache::FrameBufferStateCache()
:mCurrentFrameBufferId(0)
{
}

FrameBufferStateCache::~FrameBufferStateCache()
{
}

GLbitfield FrameBufferStateCache::GetClearMask( GLbitfield mask, bool forceClear, bool scissorTestEnabled )
{
  if( scissorTestEnabled )
  {
    // don't do anything if scissor test is enabled, in the future we could
    // potentially keep track of frame buffer size vs scissor test size to see if the entire
    // buffer is cleared or not.
    return mask;
  }
  FrameBufferState* state = GetFrameBufferState( mCurrentFrameBufferId );
  if( !state )
  {
    DALI_LOG_ERROR("FrameBuffer not found %d \n", mCurrentFrameBufferId);
    return mask;
  }

  // if we are forcing the clear operation, then just update the internal cached values
  if( forceClear )
  {
    SetClearState( state, mask );
    return mask;
  }

  // use the cached values
  if( mask & GL_COLOR_BUFFER_BIT)
  {
    // check if color buffer is currently clean
    if( state->mState & COLOR_BUFFER_CLEAN )
    {
      // remove clear color buffer flag from bitmask, no need to clear twice
      mask&= ~GL_COLOR_BUFFER_BIT;
    }
  }
  if( mask & GL_DEPTH_BUFFER_BIT)
  {
    // check if depth buffer is currently clean
    if( state->mState & DEPTH_BUFFER_CLEAN )
    {
      // remove clear depth buffer flag from bitmask, no need to clear twice
      mask&= ~GL_DEPTH_BUFFER_BIT;
    }
  }
  if( mask & GL_STENCIL_BUFFER_BIT)
  {
    // check if stencil buffer is currently clean
    if( state->mState & STENCIL_BUFFER_CLEAN )
    {
      // remove clear stencil buffer flag from bitmask, no need to clear twice

      mask&= ~GL_STENCIL_BUFFER_BIT;
    }
  }

  // set the clear state based, what's about to be cleared
  SetClearState( state, mask );

  return mask;
}

void FrameBufferStateCache::SetCurrentFrameBuffer( GLuint frameBufferId )
{
  mCurrentFrameBufferId = frameBufferId;
}

void FrameBufferStateCache::FrameBuffersDeleted( GLsizei count, const GLuint* const frameBuffers )
{
  for( GLsizei i = 0; i < count; ++i )
  {
    DeleteFrameBuffer( frameBuffers[i] );
  }
}
void FrameBufferStateCache::FrameBuffersCreated( GLsizei count, const GLuint* const frameBuffers )
{
  for( GLsizei i = 0; i < count; ++i )
  {
    // check the frame buffer doesn't exist already
    GLuint id = frameBuffers[i];

    FrameBufferState* state =  GetFrameBufferState( id );
    if( state )
    {
      DALI_LOG_ERROR("FrameBuffer already exists%d \n", id );
      // reset its state
      state->mState = GetInitialFrameBufferState();
      continue;
    }

    FrameBufferState newFrameBuffer( frameBuffers[i], GetInitialFrameBufferState() );
    mFrameBufferStates.PushBack( newFrameBuffer );
  }
}

void FrameBufferStateCache::DrawOperation( bool colorBuffer, bool depthBuffer, bool stencilBuffer )
{
  FrameBufferState* state = GetFrameBufferState( mCurrentFrameBufferId );
  if( !state )
  {
    // an error will have already been logged by the clear operation
    return;
  }

  if( colorBuffer )
  {
    // un-set the clean bit
    state->mState &= ~COLOR_BUFFER_CLEAN;
  }
  if( depthBuffer )
  {
    // un-set the clean bit
    state->mState &= ~DEPTH_BUFFER_CLEAN;
  }
  if( stencilBuffer )
  {
    // un-set the clean bit
    state->mState &= ~STENCIL_BUFFER_CLEAN;
  }

}

void FrameBufferStateCache::Reset()
{
  mFrameBufferStates.Clear();

  // create the default frame buffer
  GLuint id = 0; // 0 == default frame buffer id
  FrameBuffersCreated( 1, &id );
}

void FrameBufferStateCache::SetClearState( FrameBufferState* state, GLbitfield mask )
{
  if( mask & GL_COLOR_BUFFER_BIT)
  {
    // set the color buffer to clean
    state->mState |= COLOR_BUFFER_CLEAN;
  }
  if( mask & GL_DEPTH_BUFFER_BIT)
  {
    // set the depth buffer to clean
    state->mState |= DEPTH_BUFFER_CLEAN;
  }
  if( mask & GL_STENCIL_BUFFER_BIT)
  {
    // set the stencil buffer to clean
    state->mState |= STENCIL_BUFFER_CLEAN;
  }
}

FrameBufferStateCache::FrameBufferState* FrameBufferStateCache::GetFrameBufferState(  GLuint frameBufferId )
{
  for( FrameBufferStateVector::SizeType i = 0; i < mFrameBufferStates.Count(); ++i )
  {
    FrameBufferState& state = mFrameBufferStates[i];
    if( state.mId == frameBufferId )
    {
      return &state;
    }
  }
  return NULL;
}

void FrameBufferStateCache::DeleteFrameBuffer( GLuint frameBufferId )
{
  FrameBufferStateVector::Iterator iter = mFrameBufferStates.Begin();
  FrameBufferStateVector::Iterator endIter = mFrameBufferStates.End();

  for( ; iter != endIter ; ++iter )
  {
   if( (*iter).mId == frameBufferId )
   {
     mFrameBufferStates.Erase( iter);
     return;
   }
 }
 DALI_LOG_ERROR("FrameBuffer not found %d \n", frameBufferId);
}

unsigned int FrameBufferStateCache::GetInitialFrameBufferState()
{
  return COLOR_BUFFER_CLEAN | DEPTH_BUFFER_CLEAN | STENCIL_BUFFER_CLEAN;
}


} // namespace Internal

} // namespace Dali
