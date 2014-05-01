#ifndef __DALI_INTERNAL_GPU_BUFFER_H__
#define __DALI_INTERNAL_GPU_BUFFER_H__

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
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/render/gl-resources/context-observer.h>

namespace Dali
{

namespace Internal
{

/**
 * Used to create and update a GPU memory buffer.
 *
 * The buffer can be used for storing vertex data, index arrays (indices)
 * or pixel data (PBO).
 *
 * The buffer allows data to be stored in high-performance
 * graphics memory on the server side and
 * promotes efficient data transfer.
 *
 */
class GpuBuffer : public ContextObserver
{
public:

  /**
   * Enum to encapsulate the GL buffer type. This is to avoid having to store a whole int for type
   */
  enum Target
  {
    ARRAY_BUFFER,             ///< GL_ARRAY_BUFFER
    ELEMENT_ARRAY_BUFFER,     ///< GL_ELEMENT_ARRAY_BUFFER
    TRANSFORM_FEEDBACK_BUFFER ///< GL_TRANSFORM_FEEDBACK_BUFFER
  };

  /**
   * Enum to encapsulate the GL draw mode. This is to avoid having to store a whole int for mode
   */
  enum Usage
  {
    STREAM_DRAW, ///< GL_STREAM_DRAW
    STATIC_DRAW, ///< GL_STATIC_DRAW
    DYNAMIC_DRAW, ///< GL_DYNAMIC_DRAW
  };

public:

  /**
   * constructor
   * @param context drawing context
   * @param target the type of buffer to create @see Type
   * @param usage how the buffer will be used @see DrawMode
   */
  GpuBuffer( Context& context, Target target, Usage usage );

  /**
   * Destructor
   */
  virtual ~GpuBuffer();

  /**
   *
   * Creates or updates a buffer object and binds it to the target.
   * @param size Specifies the size in bytes of the buffer object's new data store.
   * @param data pointer to the data to load
   */
  void UpdateDataBuffer(GLsizeiptr size,const GLvoid *data);

  /**
   * Bind the buffer object to the target
   * Will assert if the buffer size is zero
   */
  void Bind() const;

  /**
   * @return true if the GPU buffer is valid, i.e. its created and not empty
   */
  bool BufferIsValid() const;

  /**
   * Get the size of the buffer
   * @return size
   */
  GLsizeiptr GetBufferSize() const
  {
    return mSize;
  }

private: // From Context::Observer

  /**
   * @copydoc ContextObserver::GlContextToBeDestroyed
   */
  virtual void GlContextToBeDestroyed();

  /**
   * @copydoc ContextObserver::GlContextCreated
   */
  virtual void GlContextCreated();

private:

  /**
   * Perfoms a bind without checking the size of the buffer
   * @param bufferId to bind
   */
  void BindNoChecks(GLuint bufferId) const;

private: // Data

  Context&           mContext;             ///< dali drawing context
  GLsizeiptr         mCapacity;            ///< buffer capacity
  GLsizeiptr         mSize;                ///< buffer size
  GLuint             mBufferId;            ///< buffer object name(id)

  Target             mTarget:2;            ///< type of buffer (array/element), 2 bits are enough
  Usage              mUsage:2;             ///< how the buffer is used (read, read/write etc), 2 bits are enough
  bool               mBufferCreated:1;     ///< whether buffer has been created

};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_GPU_BUFFER_H__
