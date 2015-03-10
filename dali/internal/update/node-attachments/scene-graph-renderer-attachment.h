#ifndef DALI_INTERNAL_SCENE_GRAPH_RENDERER_ATTACHMENT_H
#define DALI_INTERNAL_SCENE_GRAPH_RENDERER_ATTACHMENT_H

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
 */

#include <dali/internal/update/common/double-buffered.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/controllers/render-message-dispatcher.h>
#include <dali/internal/update/controllers/scene-controller.h>
#include <dali/internal/update/node-attachments/scene-graph-renderable-attachment.h>
#include <dali/internal/render/data-providers/uniform-map-provider.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class Material;
class Geometry;
class NewRenderer;


class RendererAttachment : public RenderableAttachment, public PropertyOwner, public UniformMapProvider
{
public:
  /**
   * Create a new renderer attachment.
   * @return The newly allocated RendererAttachment
   */
  static RendererAttachment* New();

  /**
   * Constructor
   */
  RendererAttachment();

  /**
   * Destructor
   */
  virtual ~RendererAttachment();

  /**
   * Set the material for the renderer
   * @param[in] bufferIndex The current frame's buffer index
   * @param[in] material The material this renderer will use
   */
  void SetMaterial( BufferIndex bufferIndex, const Material* material);

  /**
   * Get the material of this renderer
   * @return the material this renderer uses
   */
  const Material& GetMaterial() const;

  /**
   * Set the geometry for the renderer
   * @param[in] bufferIndex The current frame's buffer index
   * @param[in] geometry The geometry this renderer will use
   */
  void SetGeometry( BufferIndex bufferIndex, const Geometry* geometry);

  /**
   * Get the geometry of this renderer
   * @return the geometry this renderer uses
   */
  const Geometry& GetGeometry() const;

  /**
   * Set the depth index
   * @param[in] index The depth index to use
   */
  void SetDepthIndex( int index );

  /**
   * Get the depth index
   * @return The depth index to use
   */
  int GetDepthIndex() const ;

protected: // From RenderableAttachment
  /**
   * @copydoc RenderableAttachment::GetRenderer().
   */
  virtual Renderer& GetRenderer();

  /**
   * @copydoc RenderableAttachment::GetRenderer().
   */
  virtual const Renderer& GetRenderer() const;

  /**
   * @copydoc RenderableAttachment::DoPrepareRender()
   */
  virtual void DoPrepareRender( BufferIndex updateBufferIndex );

  /**
   * @copydoc RenderableAttachment::IsFullyOpaque()
   */
  virtual bool IsFullyOpaque( BufferIndex updateBufferIndex );

  /**
   * @copydoc RenderableAttachment::SizeChanged()
   */
  virtual void SizeChanged( BufferIndex updateBufferIndex );

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
  virtual bool DoPrepareResources( BufferIndex updateBufferIndex,
                                   ResourceManager& resourceManager );

private:
  NewRenderer* mRenderer; ///< Raw pointer to the new renderer (that's owned by RenderManager)

  const Material* mMaterial; ///< The material this renderer uses. (Not owned)
  const Geometry* mGeometry; ///< The geometry this renderer uses. (Not owned)

  int mDepthIndex;     ///< Used only in PrepareRenderInstructions
};

// Messages for RendererAttachment

inline void SetMaterialMessage( EventToUpdate& eventToUpdate, const RendererAttachment& attachment, const Material& material )
{
  typedef MessageDoubleBuffered1< RendererAttachment, const Material* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &RendererAttachment::SetMaterial, &material );
}

inline void SetGeometryMessage( EventToUpdate& eventToUpdate, const RendererAttachment& attachment, const Geometry& geometry )
{
  typedef MessageDoubleBuffered1< RendererAttachment, const Geometry* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &RendererAttachment::SetGeometry, &geometry );
}

inline void SetDepthIndexMessage( EventToUpdate& eventToUpdate, const RendererAttachment& attachment, int depthIndex )
{
  typedef MessageValue1< RendererAttachment, int > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &RendererAttachment::SetDepthIndex, depthIndex );
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali


#endif //  DALI_INTERNAL_SCENE_GRAPH_RENDERER_ATTACHMENT_H
