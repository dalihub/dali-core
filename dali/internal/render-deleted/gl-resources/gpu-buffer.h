#ifndef __DALI_INTERNAL_GPU_BUFFER_H__
#define __DALI_INTERNAL_GPU_BUFFER_H__

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

// INTERNAL INCLUDES
#include <dali/internal/render/gl-resources/context.h>

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
 */
class GpuBuffer
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
    STREAM_DRAW,  ///< GL_STREAM_DRAW
    STATIC_DRAW,  ///< GL_STATIC_DRAW
    DYNAMIC_DRAW, ///< GL_DYNAMIC_DRAW
  };

public:

  /**
   * constructor
   * @param context drawing context
   */
  GpuBuffer( Context& context );

  /**
   * Destructor, non virtual as no virtual methods or inheritance
   */
  ~GpuBuffer();

  /**
   *
   * Creates or updates a buffer object and binds it to the target.
   * @param size Specifies the size in bytes of the buffer object's new data store.
   * @param data pointer to the data to load
   * @param usage How the buffer will be used
   * @param target The target buffer to update
   */
  void UpdateDataBuffer(GLsizeiptr size, const GLvoid *data, Usage usage, Target target);

  /**
   * Bind the buffer object to the target
   * Will assert if the buffer size is zero
   */
  void Bind(Target target) const;

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

  /**
   * Needs to be called when GL context is destroyed
   */
  void GlContextDestroyed();

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

  bool               mBufferCreated:1;     ///< whether buffer has been created

};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_GPU_BUFFER_H__
