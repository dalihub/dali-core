#ifndef __DALI_INTERNAL_SCENE_GRAPH_IMAGE_ATTACHMENT_H__
#define __DALI_INTERNAL_SCENE_GRAPH_IMAGE_ATTACHMENT_H__

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
#include <dali/public-api/actors/image-actor.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/shader-effects/shader-effect.h>
#include <dali/internal/event/common/event-thread-services.h>
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
   * Set the blending options. This should only be called from the update-thread.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] options A bitmask of blending options.
   */
  void SetBlendingOptions( BufferIndex updateBufferIndex, unsigned int options );

  /**
   * Set the blend-color. This should only be called from the update-thread.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] color The new blend-color.
   */
  void SetBlendColor( BufferIndex updateBufferIndex, const Vector4& color );

  /**
   * Set the face-culling mode.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] mode The face-culling mode.
   */
  void SetCullFace( BufferIndex updateBufferIndex, CullFaceMode mode );

  /**
   * Set the sampler used to render the texture for this renderable.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] samplerBitfield The image sampler packed options to set.
   */
  void SetSampler( BufferIndex updateBufferIndex, unsigned int samplerBitfield );

  /**
   * Apply a shader on the renderable
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] shader to apply.
   */
  void ApplyShader( BufferIndex updateBufferIndex, Shader* shader );

  /**
   * Remove the shader from the renderable
   * @param[in] updateBufferIndex The current update buffer index.
   */
  void RemoveShader( BufferIndex updateBufferIndex );

  /**
   * Called to notify that the shader might have been changed
   * The implementation should recalculate geometry and scale based on the
   * hints from the new shader
   * @param[in] updateBufferIndex The current update buffer index.
   * @return Return true if the geometry changed.
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
   * Sends the shader to the renderer
   * @param updateBufferIndex for the message buffer
   */
  void SendShaderChangeMessage( BufferIndex updateBufferIndex );

  /**
   * @copydoc RenderableAttachment::Initialize2().
   */
  virtual void Initialize2( BufferIndex updateBufferIndex );

  /**
   * @copydoc RenderableAttachment::OnDestroy2().
   */
  virtual void OnDestroy2();

  /**
   * @copydoc NodeAttachment::ConnectedToSceneGraph()
   */
  virtual void ConnectedToSceneGraph();

  /**
   * @copydoc NodeAttachment::DisconnectedFromSceneGraph()
   */
  virtual void DisconnectedFromSceneGraph();

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
  CullFaceMode mCullFaceMode : 3; ///< Cullface mode, 3 bits is enough for 4 values
  bool mUseBlend             : 1; ///< True if the attachment & renderer should be considered opaque for sorting and blending.

  BitmapMetadata  mBitmapMetadata;///< The bitmap metadata
  Vector2 mGeometrySize;          ///< The size of the currently used geometry

};

// Messages for ImageAttachment

inline void SetTextureIdMessage( EventThreadServices& eventThreadServices, const ImageAttachment& attachment, unsigned int id )
{
  typedef MessageDoubleBuffered1< ImageAttachment, unsigned int > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &ImageAttachment::SetTextureId, id );
}

inline void SetPixelAreaMessage( EventThreadServices& eventThreadServices, const ImageAttachment& attachment, const Dali::ImageActor::PixelArea& area )
{
  typedef MessageDoubleBuffered1< ImageAttachment, Dali::ImageActor::PixelArea > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &ImageAttachment::SetPixelArea, area );
}

inline void ClearPixelAreaMessage( EventThreadServices& eventThreadServices, const ImageAttachment& attachment )
{
  typedef Message< ImageAttachment > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &ImageAttachment::ClearPixelArea );
}

inline void SetStyleMessage( EventThreadServices& eventThreadServices, const ImageAttachment& attachment, Dali::ImageActor::Style style )
{
  typedef MessageValue1< ImageAttachment, Dali::ImageActor::Style > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &ImageAttachment::SetStyle, style );
}

inline void SetNinePatchBorderMessage( EventThreadServices& eventThreadServices, const ImageAttachment& attachment, const Vector4& border, bool inPixels )
{
  typedef MessageDoubleBuffered2< ImageAttachment, Vector4, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &ImageAttachment::SetBorder, border, inPixels );
}

inline void SetSortModifierMessage( EventThreadServices& eventThreadServices, const ImageAttachment& attachment, float modifier )
{
  typedef MessageValue1< ImageAttachment, float > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &ImageAttachment::SetSortModifier, modifier );
}

inline void SetCullFaceMessage( EventThreadServices& eventThreadServices, const ImageAttachment& attachment, CullFaceMode mode )
{
  typedef MessageDoubleBuffered1< ImageAttachment, CullFaceMode > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &ImageAttachment::SetCullFace, mode );
}

inline void SetBlendingOptionsMessage( EventThreadServices& eventThreadServices, const ImageAttachment& attachment, unsigned int options )
{
  typedef MessageDoubleBuffered1< ImageAttachment, unsigned int > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  new (slot) LocalType( &attachment, &ImageAttachment::SetBlendingOptions, options );
}

inline void SetBlendColorMessage( EventThreadServices& eventThreadServices, const ImageAttachment& attachment, const Vector4& color )
{
  typedef MessageDoubleBuffered1< ImageAttachment, Vector4 > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  new (slot) LocalType( &attachment, &ImageAttachment::SetBlendColor, color );
}

inline void SetSamplerMessage( EventThreadServices& eventThreadServices, const ImageAttachment& attachment, unsigned int samplerBitfield )
{
  typedef MessageDoubleBuffered1< ImageAttachment, unsigned int > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &ImageAttachment::SetSampler, samplerBitfield );
}

inline void ApplyShaderMessage( EventThreadServices& eventThreadServices, const ImageAttachment& attachment, const Shader& constShader )
{
  // Update thread can edit the object
  Shader& shader = const_cast< Shader& >( constShader );

  typedef MessageDoubleBuffered1< ImageAttachment, Shader* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &ImageAttachment::ApplyShader, &shader );
}

inline void RemoveShaderMessage( EventThreadServices& eventThreadServices, const ImageAttachment& attachment )
{
  typedef MessageDoubleBuffered0< ImageAttachment > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &ImageAttachment::RemoveShader );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_IMAGE_ATTACHMENT_H__
