#ifndef DALI_INTERNAL_FRAME_BUFFER_H
#define DALI_INTERNAL_FRAME_BUFFER_H

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
#include <dali/public-api/common/dali-common.h> // DALI_ASSERT_ALWAYS
#include <dali/public-api/common/intrusive-ptr.h> // Dali::IntrusivePtr
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/rendering/frame-buffer.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/event/rendering/texture-impl.h>

namespace Dali
{

namespace Integration
{
class RenderSurface;
}

namespace Internal
{
namespace Render
{
class FrameBuffer;
}

class FrameBuffer;
typedef IntrusivePtr<FrameBuffer> FrameBufferPtr;

class FrameBuffer : public BaseObject
{
public:

  using Mask = Dali::FrameBuffer::Attachment::Mask;

  /**
   * @brief Create a new FrameBuffer
   *
   * @param[in] width       The width of the FrameBuffer
   * @param[in] height      The height of the FrameBuffer
   * @param[in] attachments The attachments comprising the format of the FrameBuffer (bit-mask)
   * @return A smart-pointer to the newly allocated Texture.
   */
  static FrameBufferPtr New( uint32_t width, uint32_t height, Mask attachments );

  /**
   * @brief Create a new FrameBuffer
   *
   * @param[in] renderSurface  The render surface
   * @param[in] attachments    The attachments comprising the format of the FrameBuffer (bit-mask)
   * @return A smart-pointer to the newly allocated Texture.
   */
  static FrameBufferPtr New( Dali::Integration::RenderSurface& renderSurface, Mask attachments );

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~FrameBuffer();

  /**
   * @brief Get the FrameBuffer render object
   *
   * @return the FrameBuffer render object
   */
  Render::FrameBuffer* GetRenderObject() const;

  /**
   * @copydoc Dali::FrameBuffer::AttachColorTexture()
   */
  void AttachColorTexture( TexturePtr texture, uint32_t mipmapLevel, uint32_t layer );

  /**
   * @copydoc Dali::FrameBuffer::GetColorTexture()
   */
  Texture* GetColorTexture();

private: // implementation

  /**
   * Constructor
   * @param[in] width       The width of the FrameBuffer
   * @param[in] height      The height of the FrameBuffer
   * @param[in] attachments The attachments comprising the format of the FrameBuffer (bit-mask)
   */
  FrameBuffer( uint32_t width, uint32_t height, Mask attachments );

  /**
   * Second stage initialization of the Texture
   */
  void Initialize( Integration::RenderSurface* renderSurface = nullptr );

protected:

private: // unimplemented methods

  FrameBuffer() = delete;
  FrameBuffer( const FrameBuffer& ) = delete;
  FrameBuffer& operator=( const FrameBuffer& ) = delete;

private: // data

  Internal::EventThreadServices& mEventThreadServices; ///< Used to send messages to the render thread via update thread
  Internal::Render::FrameBuffer* mRenderObject;        ///< The Render::Texture associated to this texture

  TexturePtr mColor;
  uint32_t mWidth;
  uint32_t mHeight;
  Mask mAttachments;                           ///< Bit-mask of type FrameBuffer::Attachment::Mask

  bool mIsSurfaceBacked:1;

};

} // namespace Internal

// Helpers for public-api forwarding methods
inline Internal::FrameBuffer& GetImplementation(Dali::FrameBuffer& handle)
{
  DALI_ASSERT_ALWAYS(handle && "FrameBuffer handle is empty");

  BaseObject& object = handle.GetBaseObject();

  return static_cast<Internal::FrameBuffer&>(object);
}

inline const Internal::FrameBuffer& GetImplementation(const Dali::FrameBuffer& handle)
{
  DALI_ASSERT_ALWAYS(handle && "FrameBuffer handle is empty");

  const BaseObject& object = handle.GetBaseObject();

  return static_cast<const Internal::FrameBuffer&>(object);
}

} // namespace Dali

#endif // DALI_INTERNAL_FRAME_BUFFER_H
