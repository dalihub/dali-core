#ifndef DALI_INTERNAL_SCENE_GRAPH_FRAME_BUFFER_H
#define DALI_INTERNAL_SCENE_GRAPH_FRAME_BUFFER_H

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
 */

// INTERNAL INCLUDES
#include <dali/public-api/rendering/frame-buffer.h>
#include <dali/internal/update/rendering/scene-graph-texture.h>
#include <dali/graphics-api/graphics-api-framebuffer.h>

namespace Dali
{
namespace Internal
{

namespace SceneGraph
{

class FrameBuffer
{
public:

  /**
   * Constructor
   * @param[in] width The width of the FrameBuffer
   * @param[in] height The height of the FrameBuffer
   * @param[in] attachments The attachments comprising the format of the FrameBuffer (bit-mask)
   */
  FrameBuffer( unsigned int width, unsigned int height, unsigned int attachments );

  /**
   * Destructor
   */
  ~FrameBuffer();

  /**
   * Initialize the frame buffer object with the Graphics API when added to UpdateManager
   *
   * @param[in] graphics The Graphics API
   */
  void Initialize( Integration::Graphics::Graphics& graphics );

  /**
   * @brief Attach a texture for color rendering. Valid only for Framebuffers with COLOR attachments.
   * @param[in] texture The texture that will be used as output when rendering
   * @param[in] mipmapLevel The mipmap of the texture to be attached
   * @param[in] layer Indicates which layer of a cube map or array texture to attach. Unused for 2D textures
   */
  void AttachColorTexture( SceneGraph::Texture* texture, unsigned int mipmapLevel, unsigned int layer );

  /**
   * @brief Attach a texture for depth/stencil rendering.
   * @param[in] texture The texture that will be used as output when rendering
   * @param[in] format Whether the buffer is for depth and/or stencil channels
   * @param[in] mipmapLevel The mipmap of the texture to be attached
   * @param[in] layer Indicates which layer of a cube map or array texture to attach. Unused for 2D textures
   */
  void AttachDepthStencilTexture( SceneGraph::Texture* texture,
                                  Dali::FrameBuffer::Attachment::Mask format,
                                  unsigned int mipmapLevel,
                                  unsigned int layer );

  /**
   * @brief Get the width of the FrameBuffer
   * @return The width of the framebuffer
   */
  unsigned int GetWidth() const;

  /**
   * @brief Get the height of the FrameBuffer
   * @return The height of the framebuffer
   */
  unsigned int GetHeight() const;

  /**
   * Prepare the Graphics API framebuffer object when it's required
   */
  void PrepareFramebuffer();

  /**
   * Get the graphics object associated with this framebuffer
   */
  const Graphics::API::Framebuffer* GetGfxObject() const;

  /**
   * Destroy any graphics objects owned by this scene graph object
   */
  void DestroyGraphicsObjects();

private:
  Integration::Graphics::Graphics* mGraphics; ///< Graphics interface object
  std::unique_ptr<Graphics::API::Framebuffer> mGraphicsFramebuffer;

  struct Attachment
  {
    Attachment()
    : texture( nullptr ),
      format( Dali::FrameBuffer::Attachment::NONE ),
      mipmapLevel( 0u ),
      layer( 0u )
    {
    }

    SceneGraph::Texture* texture;
    Dali::FrameBuffer::Attachment::Mask format;
    unsigned int mipmapLevel;
    unsigned int layer;
  };

  Attachment mColorAttachment;
  Attachment mDepthAttachment; // Should also specify depth/stencil choice in DALi API.

  unsigned int mWidth;
  unsigned int mHeight;
};

inline void AttachColorTextureMessage( EventThreadServices& eventThreadServices,
                                       SceneGraph::FrameBuffer& frameBuffer,
                                       SceneGraph::Texture* texture,
                                       unsigned int mipmapLevel,
                                       unsigned int layer )
{
  typedef MessageValue3< SceneGraph::FrameBuffer, SceneGraph::Texture*, unsigned int, unsigned int  > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ), false );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &frameBuffer, &FrameBuffer::AttachColorTexture, texture, mipmapLevel, layer );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali


#endif // DALI_INTERNAL_SCENE_GRAPH_FRAME_BUFFER_H
