#ifndef __DALI_INTERNAL_SCENE_GRAPH_RENDERABLE_ATTACHMENT_H__
#define __DALI_INTERNAL_SCENE_GRAPH_RENDERABLE_ATTACHMENT_H__

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
#include <dali/internal/common/blending-options.h>
#include <dali/internal/common/event-to-update.h>
#include <dali/internal/common/internal-constants.h>
#include <dali/internal/common/type-abstraction-enums.h>
#include <dali/internal/update/controllers/scene-controller.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/node-attachments/node-attachment.h>
#include <dali/internal/update/resources/resource-manager-declarations.h>
#include <dali/internal/render/renderers/scene-graph-renderer-declarations.h>

namespace Dali
{

namespace Internal
{
class ResourceManager;
class ResourceTracker;

namespace SceneGraph
{
class Renderer;
class Shader;

/**
 * RenderableAttachments are responsible for preparing textures, meshes, matrices etc. during the Update.
 * These resources are then passed to a renderer, for use in the next Render.
 */
class RenderableAttachment : public NodeAttachment
{
public: // API

  /**
   * @See Dali::RenderableActor::SetBlendMode().
   */
  void SetBlendingMode( BlendingMode::Type mode );

  /**
   * @copydoc Dali::RenderableActor::GetBlendMode().
   */
  BlendingMode::Type GetBlendingMode() const;

  /**
   * Flag to check if any geometry scaling is needed, inlined as called from update algorithm often
   * @return true if the derived renderable uses geometry scaling
   */
  bool UsesGeometryScaling() const
  {
    return mUsesGeometryScaling;
  }

  /**
   * Triggers scale for size update. GetScaleForSize will be called in this frame
   */
  void SetRecalculateScaleForSize();

  /**
   * Returns the scaling dirty flag, inlined as called from update algorithm often
   * @return if scale for size is dirty, i.e. scaling has changed
   */
  bool IsScaleForSizeDirty() const
  {
    return mScaleForSizeDirty;
  }

  /**
   * Retrieve scale-for-size for given node size
   * Clears the scale for size flag
   * @param[in] nodeSize to scale to
   * @param[out] scaling factors
   */
  void GetScaleForSize( const Vector3& nodeSize, Vector3& scaling );


public: // For use during in the update algorithm only

  /**
   * @param[in] updateBufferIndex The current update buffer index.
   * @return visible tells if this renderer can be potentially seen
   */
  bool ResolveVisibility( BufferIndex updateBufferIndex );

  /**
   * if this renderable actor has visible size and color
   * @return true if you can potentially see this actor
   */
  bool HasVisibleSizeAndColor() const
  {
    return mHasSizeAndColorFlag;
  }

  /**
   * Check whether the attachment has been marked as ready to render
   * @param[out] ready TRUE if the attachment has resources to render
   * @param[out] complete TRUE if the attachment's resources are complete
   * (e.g. image has finished loading, framebuffer is ready to render, native image
   * framebuffer has been rendered)
   */
  void GetReadyAndComplete(bool& ready, bool& complete) const;

  /**
   * Query whether the attachment has blending enabled.
   * @param[in] updateBufferIndex The current update buffer index.
   * @return true if blending is enabled, false otherwise.
   */
  bool IsBlendingOn( BufferIndex updateBufferIndex );

  /**
   * Check if the blending mode has changed - if it has, send message to renderer
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] useBlending True if the renderer should use blending option
   */
  void ChangeBlending( BufferIndex updateBufferIndex, bool useBlending );

  /**
   * Prepare the object for rendering.
   * This is called by the UpdateManager when an object is due to be rendered in the current frame.
   * @param[in] updateBufferIndex The current update buffer index.
   */
  void PrepareRender( BufferIndex updateBufferIndex );

public: // API for derived classes

  /**
   * Retrieve a Renderer used by this attachment; implemented in derived classes.
   * @note The first Renderer is the main renderer for the attachment, and
   * should always exist during the lifetime of the RenderableAttachment.
   * @return A Renderer.
   */
  virtual Renderer& GetRenderer() = 0;

  /**
   * Retrieve a Renderer used by this attachment.
   * @note The first Renderer is the main renderer for the attachment, and
   * should always exist during the lifetime of the RenderableAttachment.
   * Other renderers are for effects such as shadows and reflections.
   * @return A Renderer.
   */
  virtual const Renderer& GetRenderer() const = 0;

  /**
   * Prepare the object resources.
   * This must be called by the UpdateManager before calling PrepareRender, for each frame.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] resourceManager The resource manager.
   */
  void PrepareResources( BufferIndex updateBufferIndex, ResourceManager& resourceManager );

  /**
   * If the resource is being tracked, then follow it. ( Further ready tests will use this
   * list ) Otherwise, if it's not complete, set mHasUntrackedResources.
   * @param[in] The resource id
   */
  void FollowTracker( Integration::ResourceId id );

  /**
   * @copydoc RenderableAttachment::PrepareRender()
   */
  virtual void DoPrepareRender( BufferIndex updateBufferIndex ) = 0;

  /**
   * Query whether the attachment is fully opaque.
   * @param[in] updateBufferIndex The current update buffer index.
   * @return True if fully opaque.
   */
  virtual bool IsFullyOpaque( BufferIndex updateBufferIndex ) = 0;

  /**
   * Called to notify that the size has been changed
   * The implementation may tell the renderer to recalculate scale
   * based on the new size
   * @param[in] updateBufferIndex The current update buffer index.
   */
  virtual void SizeChanged( BufferIndex updateBufferIndex ) = 0;

  /**
   * Chained from NodeAttachment::ConnectToSceneGraph()
   */
  virtual void ConnectToSceneGraph2( BufferIndex updateBufferIndex ) = 0;

  /**
   * Chained from NodeAttachment::OnDestroy()
   */
  virtual void OnDestroy2() = 0;

  /**
   * Retrieve the scale-for-size for given node size. Default implementation returns Vector3::ZERO
   * @param[in] nodeSize to scale to
   * @param[out] scaling factors
   */
  virtual void DoGetScaleForSize( const Vector3& nodeSize, Vector3& scaling );


  /**
   * Set the sort-modifier for the attachment.
   * @param[in] modifier The depth-sort modifier.
   */
  void SetSortModifier(float modifier);

  /**
   * Retrieve the sort-modifier for the attachment.
   * @return The sort-modifier.
   */
  float GetSortModifier() const
  {
    // inlined as its called a lot when sorting transparent renderers
    return mSortModifier;
  }

protected:

  /**
   * Protected constructor; only base classes can be instantiated.
   * @param usesGeometryScaling should be false if the derived class does not need geometry scaling
   */
  RenderableAttachment( bool usesGeometryScaling );

  /**
   * Virtual destructor, no deletion through this interface
   */
  virtual ~RenderableAttachment();

private: // From NodeAttachment

  /**
   * @copydoc NodeAttachment::ConnectToSceneGraph().
   */
  virtual void ConnectToSceneGraph( SceneController& sceneController, BufferIndex updateBufferIndex );

  /**
   * @copydoc NodeAttachment::DisconnectFromSceneGraph().
   */
  virtual void OnDestroy();

  /**
   * @copydoc NodeAttachment::GetRenderable()
   */
  virtual RenderableAttachment* GetRenderable();

private:

  /**
   * Prepare the object resources.
   * This must be called by the UpdateManager before calling PrepareRender, for each frame.
   * @param[in] updateBufferIndex The current buffer index.
   * @param[in] resourceManager The resource manager.
   * @return True if resources are ready, false will prevent PrepareRender being called for this attachment.
   */
  virtual bool DoPrepareResources( BufferIndex updateBufferIndex, ResourceManager& resourceManager ) = 0;

  // Undefined
  RenderableAttachment( const RenderableAttachment& );

  // Undefined
  RenderableAttachment& operator=( const RenderableAttachment& rhs );

protected:
  SceneController* mSceneController;   ///< Used for initializing renderers whilst attached
  Shader*          mShader;            ///< A pointer to the shader

  Dali::Vector< Integration::ResourceId > mTrackedResources; ///< Filled during PrepareResources if there are uncomplete, tracked resources.

  float mSortModifier;

  BlendingMode::Type mBlendingMode:2;  ///< Whether blending is used to render the renderable attachment. 2 bits is enough for 3 values

  bool mUsesGeometryScaling:1;         ///< True if the derived renderer uses scaling.
  bool mScaleForSizeDirty:1;           ///< True if mScaleForSize has changed in the current frame.
  bool mUseBlend:1;                    ///< True if the attachment & renderer should be considered opaque for sorting and blending.
  bool mHasSizeAndColorFlag:1;         ///< Set during the update algorithm to tell whether this renderer can potentially be seen
  bool mResourcesReady:1;              ///< Set during the Update algorithm; true if the attachment has resources ready for the current frame.
  bool mFinishedResourceAcquisition:1; ///< Set during DoPrepareResources; true if ready & all resource acquisition has finished (successfully or otherwise)
  bool mHasUntrackedResources:1;       ///< Set during PrepareResources, true if have tried to follow untracked resources
};

// Messages for RenderableAttachment

inline void SetBlendingModeMessage( EventToUpdate& eventToUpdate, const RenderableAttachment& attachment, BlendingMode::Type mode )
{
  typedef MessageValue1< RenderableAttachment, BlendingMode::Type > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  new (slot) LocalType( &attachment, &RenderableAttachment::SetBlendingMode, mode );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_RENDERABLE_ATTACHMENT_H__
