#ifndef __DALI_INTERNAL_SCENE_GRAPH_RENDERABLE_ATTACHMENT_H__
#define __DALI_INTERNAL_SCENE_GRAPH_RENDERABLE_ATTACHMENT_H__

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
#include <dali/public-api/actors/layer.h>
#include <dali/internal/common/blending-options.h>
#include <dali/internal/common/event-to-update.h>
#include <dali/internal/common/internal-constants.h>
#include <dali/internal/common/type-abstraction-enums.h>
#include <dali/internal/update/controllers/scene-controller.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/node-attachments/node-attachment.h>
#include <dali/internal/update/common/double-buffered.h>
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

/**
 * RenderableAttachments are responsible for preparing textures, meshes, matrices etc. during the Update.
 * These resources are then passed to a renderer, for use in the next Render.
 */
class RenderableAttachment : public NodeAttachment
{
public: // API

  /**
   * Set the sort-modifier for the attachment.
   * @param[in] modifier The depth-sort modifier.
   */
  void SetSortModifier(float modifier);

  /**
   * Retrieve the sort-modifier for the attachment.
   * @return The sort-modifier.
   */
  float GetSortModifier() const;

  /**
   * @See Dali::RenderableActor::SetBlendMode().
   */
  void SetBlendingMode( BlendingMode::Type mode );

  /**
   * @copydoc Dali::RenderableActor::GetBlendMode().
   */
  BlendingMode::Type GetBlendingMode() const;

  /**
   * Check if the blending mode has changed - if it has, send message to renderer
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] useBlending True if the renderer should use blending option
   */
  void ChangeBlending( BufferIndex updateBufferIndex, bool useBlending );

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
   * TODO this method should not be virtual but because mesh attachment is a mess, it needs to be
   * considered visible regardless of its size... need to remove geometry scaling to fix this!!!
   * @param[in] updateBufferIndex The current update buffer index.
   * @return visible tells if this renderer can be potentially seen
   */
  virtual bool ResolveVisibility( BufferIndex updateBufferIndex )
  {
    mHasSizeAndColorFlag = false;
    const Vector4& color = mParent->GetWorldColor( updateBufferIndex );
    if( color.a > FULLY_TRANSPARENT )               // not fully transparent
    {
      const Vector3& size = mParent->GetSize( updateBufferIndex );
      if( ( size.width > Math::MACHINE_EPSILON_1000 )&&   // width is greater than a very small number
          ( size.height > Math::MACHINE_EPSILON_1000 ) )  // height is greater than a very small number
      {
        mHasSizeAndColorFlag = true;
      }
    }
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
   * Called to notify that the shader might have been changed
   * The implementation should recalculate geometry and scale based on the
   * hints from the new shader
   * @param[in] updateBufferIndex The current update buffer index.
   * @return Return true if the geometry changed.
   */
  virtual void ShaderChanged( BufferIndex updateBufferIndex ) = 0;

  /**
   * Called to notify that the size has been changed
   * The implementation may tell the renderer to recalculate geometry and scale based on the new size
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

  BlendingMode::Type mBlendingMode:2;    ///< Whether blending is used to render the renderable attachment. 2 bits is enough for 3 values

  bool mUsesGeometryScaling:1;         ///< True if the derived renderer uses scaling.
  bool mScaleForSizeDirty:1;           ///< True if mScaleForSize has changed in the current frame.
  bool mUseBlend:1;                    ///< True if the attachment & renderer should be considered opaque for sorting and blending.
  bool mHasSizeAndColorFlag:1;         ///< Set during the update algorithm to tell whether this renderer can potentially be seen
  bool mResourcesReady:1;              ///< Set during the Update algorithm; true if the attachment has resources ready for the current frame.
  bool mFinishedResourceAcquisition:1; ///< Set during DoPrepareResources; true if ready & all resource acquisition has finished (successfully or otherwise)
  bool mHasUntrackedResources:1;       ///< Set during PrepareResources, true if have tried to follow untracked resources
  CullFaceMode mCullFaceMode:3;        ///< Cullface mode, 3 bits is enough for 4 values

  float mSortModifier;

  Dali::Vector< Integration::ResourceId > mTrackedResources; ///< Filled during PrepareResources if there are uncomplete, tracked resources.
};

// Messages for RenderableAttachment

inline void SetSortModifierMessage( EventToUpdate& eventToUpdate, const RenderableAttachment& attachment, float modifier )
{
  typedef MessageValue1< RenderableAttachment, float > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &RenderableAttachment::SetSortModifier, modifier );
}

inline void SetCullFaceMessage( EventToUpdate& eventToUpdate, const RenderableAttachment& attachment, CullFaceMode mode )
{
  typedef MessageDoubleBuffered1< RenderableAttachment, CullFaceMode > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &RenderableAttachment::SetCullFace, mode );
}

inline void SetBlendingModeMessage( EventToUpdate& eventToUpdate, const RenderableAttachment& attachment, BlendingMode::Type mode )
{
  typedef MessageValue1< RenderableAttachment, BlendingMode::Type > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  new (slot) LocalType( &attachment, &RenderableAttachment::SetBlendingMode, mode );
}

inline void SetBlendingOptionsMessage( EventToUpdate& eventToUpdate, const RenderableAttachment& attachment, unsigned int options )
{
  typedef MessageDoubleBuffered1< RenderableAttachment, unsigned int > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  new (slot) LocalType( &attachment, &RenderableAttachment::SetBlendingOptions, options );
}

inline void SetBlendColorMessage( EventToUpdate& eventToUpdate, const RenderableAttachment& attachment, const Vector4& color )
{
  typedef MessageDoubleBuffered1< RenderableAttachment, Vector4 > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  new (slot) LocalType( &attachment, &RenderableAttachment::SetBlendColor, color );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_RENDERABLE_ATTACHMENT_H__
