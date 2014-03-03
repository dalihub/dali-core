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

// CLASS HEADER
#include <dali/internal/render/gl-resources/gpu-buffer.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

namespace Internal
{

GpuBuffer::GpuBuffer(Context& context,GLenum target,GLenum usage)
:mCapacity(0),
 mSize(0),
 mContext(context),
 mBufferId(0),
 mBufferCreated(false),
 mTarget(target),
 mUsage(usage)
{
  // the buffer is owned by the GPU so if we lose context, we lose the buffer
  // so we need to know when we lose context
  mContext.AddObserver(*this);
}
GpuBuffer::~GpuBuffer()
{
  mContext.RemoveObserver(*this);

  GlContextToBeDestroyed();
}

/*
 * Creates or updates the buffer data depending on whether it
 * already exists or not.
 */
void GpuBuffer::UpdateDataBuffer(GLsizeiptr size,const GLvoid *data)
{
  DALI_ASSERT_DEBUG( size > 0 );
  mSize = size;
  // make sure we have a buffer name/id before uploading
  if (mBufferId == 0)
  {
    mContext.GenBuffers(1,&mBufferId);
    DALI_ASSERT_DEBUG(mBufferId);
  }

  // make sure the buffer is bound, don't perform any checks because size may be zero
  BindNoChecks(mBufferId);

  // if the buffer has already been created, just update the data providing it fits
  if (mBufferCreated )
  {
    // if the data will fit in the existing buffer, just update it
    if (size <= mCapacity )
    {
      mContext.BufferSubData(mTarget,0, size, data);
    }
    else
    {
      // create a new buffer of the larger size,
      // gl should automatically deallocate the old buffer
      mContext.BufferData(mTarget, size, data, mUsage);
      mCapacity = size;
    }
  }
  else
  {
    // create the buffer
    mContext.BufferData(mTarget, size, data, mUsage);
    mBufferCreated = true;
    mCapacity = size;
  }

  switch (mTarget)
  {
    case GL_ARRAY_BUFFER:
    {
      mContext.BindArrayBuffer(0);
      break;
    }
    case GL_ELEMENT_ARRAY_BUFFER:
    {
      mContext.BindElementArrayBuffer(0);
      break;
    }
    case GL_TRANSFORM_FEEDBACK_BUFFER:
    {
      mContext.BindTransformFeedbackBuffer(0);
      break;
    }
    default:
      DALI_ASSERT_DEBUG(false && "Unsupported type");
  }
}

void GpuBuffer::Bind() const
{
  DALI_ASSERT_DEBUG(mCapacity);

  BindNoChecks(mBufferId);
}

bool GpuBuffer::BufferIsValid() const
{
  return mBufferCreated && (0 != mCapacity );
}

/*
 * If the context is about to go, and we have a buffer then delete it.
 */
void GpuBuffer::GlContextToBeDestroyed()
{
  if (mBufferId)
  {
    // If the buffer is currently bound, then unbind it by setting the
    // currently bound buffer to zero.
    if (mContext.GetCurrentBoundArrayBuffer(mTarget) == mBufferId)
    {
      BindNoChecks(0);
    }

    mCapacity = 0;
    mSize = 0;
    mContext.DeleteBuffers(1,&mBufferId);
    mBufferId = 0;
    mBufferCreated = false;
  }
}

void GpuBuffer::GlContextCreated()
{
  // set some default values, just incase we don't get a
  // GlContextToBeDestroyed when the context is lost and then
  // is recreated (should never happen).
  mCapacity = 0;
  mSize = 0;
  mBufferId = 0;
  mBufferCreated = false;
}

void GpuBuffer::BindNoChecks(GLuint bufferId) const
{
  // context currently only supports two targets, element and array
  // and it caches both of them (as in it won't bind them if the
  // buffer id is already bound).

  if (mTarget == GL_ARRAY_BUFFER)
  {
    mContext.BindArrayBuffer(bufferId);
  }
  else
  {
    mContext.BindElementArrayBuffer(bufferId);
  }
}


} // namespace Internal

} //namespace Dali
