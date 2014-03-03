#ifndef __DALI_INTERNAL_SCENE_GRAPH_IMAGE_ATTACHMENT_H__
#define __DALI_INTERNAL_SCENE_GRAPH_IMAGE_ATTACHMENT_H__

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
#include <dali/public-api/actors/image-actor.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/shader-effects/shader-effect.h>
#include <dali/internal/common/event-to-update.h>
#include <dali/internal/update/node-attachments/scene-graph-renderable-attachment.h>
#include <dali/internal/update/resources/bitmap-metadata.h>

namespace Dali
{

namespace Internal
{

// value types used by messages
template <> struct ParameterType< Dali::ImageActor::Style >
: public BasicType< Dali::ImageActor::Style > {};

namespace SceneGraph
{
class ImageRenderer;
class Shader;
class RenderQueue;

/**
 * An attachment for rendering images in various ways.
 */
class ImageAttachment : public RenderableAttachment
{
public:

  typedef Dali::ImageActor::Style     Style;
  typedef Dali::ImageActor::PixelArea PixelArea;

  /**
   * Create a new ImageAttachment.
   * @param [in] textureId The resource ID of a texture.
   * @return The newly allocated ImageAttachment.
   */
  static ImageAttachment* New( unsigned int textureId );

  /**
   * Virtual destructor
   */
  virtual ~ImageAttachment();

  /**
   * @copydoc RenderableAttachment::GetRenderer().
   */
  virtual Renderer& GetRenderer();

  /**
   * @copydoc RenderableAttachment::GetRenderer().
   */
  virtual const Renderer& GetRenderer() const;

  /**
   * Set the ID used to retrieve a texture from ResourceManager.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] textureId The texture ID.
   */
  void SetTextureId( BufferIndex updateBufferIndex, unsigned int textureId );

  /**
   * Set the area of the texture to display.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param [in] pixelArea The area to display, in pixels relative to the top-left (0,0) of the image.
   */
  void SetPixelArea( BufferIndex updateBufferIndex, const PixelArea& pixelArea );

  /**
   * Query whether a pixel area has been set.
   * @return True if a pixel area has been set.
   */
  bool IsPixelAreaSet() const
  {
    return mIsPixelAreaSet;
  }

  /**
   * Remove any pixel areas specified with SetPixelArea; the entire image will be displayed.
   * @pre image must be initialized.
   */
  void ClearPixelArea();

  /**
   * Set how the attachment is rendered; the default is STYLE_QUAD.
   * @param [in] style The new style.
   */
  void SetStyle( Style style );

  /**
   * Retrieve how the attachment is rendered.
   * @return The style.
   */
  Style GetStyle()
  {
    return mStyle;
  }

  /**
   * Set the border; this is applied with STYLE_NINE_PATCH.
   * @param [in] updateBufferIndex to use.
   * @param [in] border The new border setting.
   * @param [in] inPixels if the border is in pixels.
   */
  void SetBorder( BufferIndex updateBufferIndex, const Vector4& border, bool inPixels );

  /**
   * @copydoc RenderableAttachment::ShaderChanged()
   */
  virtual void ShaderChanged( BufferIndex updateBufferIndex );

  /**
   * @copydoc RenderableAttachment::SizeChanged()
   */
  virtual void SizeChanged( BufferIndex updateBufferIndex );

  /**
   * @copydoc RenderableAttachment::DoPrepareRender()
   */
  virtual void DoPrepareRender( BufferIndex updateBufferIndex );

  /**
   * @copydoc RenderableAttachment::IsFullyOpaque()
   */
  virtual bool IsFullyOpaque( BufferIndex updateBufferIndex );

protected:

  /**
   * Protected constructor
   */
  ImageAttachment( unsigned int textureId );

private:

  /**
   * @copydoc RenderableAttachment::ConnectToSceneGraph2().
   */
  virtual void ConnectToSceneGraph2( BufferIndex updateBufferIndex );

  /**
   * @copydoc RenderableAttachment::OnDestroy2().
   */
  virtual void OnDestroy2();

  /**
   * @copydoc RenderableAttachment::DoPrepareResources()
   */
  virtual bool DoPrepareResources( BufferIndex updateBufferIndex, ResourceManager& resourceManager );

  // Helper to check whether a geometry hint was set
  bool PreviousHintEnabled( Dali::ShaderEffect::GeometryHints hint ) const
  {
    return mPreviousRefreshHints & hint;
  }

  // Undefined
  ImageAttachment(const ImageAttachment&);

  // Undefined
  ImageAttachment& operator=(const ImageAttachment& rhs);

private: // Data

  ImageRenderer* mImageRenderer; ///< Raw-pointers to renderer that is owned by RenderManager
  unsigned int mTextureId;        ///< The resource ID for a texture

  // bitfields to fit in single byte
  bool mRefreshMeshData      : 1; ///< Whether the vertex/index buffers needs regenerating
  bool mIsPixelAreaSet       : 1; ///< Whether pixel area is set, cached for image actor to be able to ask for it
  int  mPreviousRefreshHints : 4; ///< The shader geometry hints, when the vertex buffer was last refreshed, 4 bits is enough as there's 4 flags
  Style mStyle               : 2; ///< rendering style, 2 bits is enough as only 2 values in the enum

  BitmapMetadata  mBitmapMetadata;///< The bitmap metadata
  Vector2 mGeometrySize;          ///< The size of the currently used geometry

};

// Messages for ImageAttachment

inline void SetTextureIdMessage( EventToUpdate& eventToUpdate, const ImageAttachment& attachment, unsigned int id )
{
  typedef MessageDoubleBuffered1< ImageAttachment, unsigned int > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &ImageAttachment::SetTextureId, id );
}

inline void SetPixelAreaMessage( EventToUpdate& eventToUpdate, const ImageAttachment& attachment, const Dali::ImageActor::PixelArea& area )
{
  typedef MessageDoubleBuffered1< ImageAttachment, Dali::ImageActor::PixelArea > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &ImageAttachment::SetPixelArea, area );
}

inline void ClearPixelAreaMessage( EventToUpdate& eventToUpdate, const ImageAttachment& attachment )
{
  typedef Message< ImageAttachment > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &ImageAttachment::ClearPixelArea );
}

inline void SetStyleMessage( EventToUpdate& eventToUpdate, const ImageAttachment& attachment, Dali::ImageActor::Style style )
{
  typedef MessageValue1< ImageAttachment, Dali::ImageActor::Style > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &ImageAttachment::SetStyle, style );
}

inline void SetNinePatchBorderMessage( EventToUpdate& eventToUpdate, const ImageAttachment& attachment, const Vector4& border, bool inPixels )
{
  typedef MessageDoubleBuffered2< ImageAttachment, Vector4, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &ImageAttachment::SetBorder, border, inPixels );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_IMAGE_ATTACHMENT_H__
