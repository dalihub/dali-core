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
   * param[in] context The GL context
   * @param[in] texture The texture that will be used as output when rendering
   * @param[in] mipmapLevel The mipmap of the texture to be attached
   * @param[in] layer Indicates which layer of a cube map or array texture to attach. Unused for 2D textures
   */
  void AttachColorTexture( SceneGraph::Texture* texture, unsigned int mipmapLevel, unsigned int layer );

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

private:
  Integration::Graphics::Graphics* mGraphics; ///< Graphics interface object

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
