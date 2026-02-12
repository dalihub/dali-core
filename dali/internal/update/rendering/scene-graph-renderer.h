#ifndef DALI_INTERNAL_SCENE_GRAPH_RENDERER_H
#define DALI_INTERNAL_SCENE_GRAPH_RENDERER_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/internal/common/blending-options.h>
#include <dali/internal/common/memory-pool-key.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/render/data-providers/render-data-provider.h>
#include <dali/internal/render/renderers/render-renderer.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/common/uniform-map.h>
#include <dali/internal/update/nodes/node-declarations.h>
#include <dali/internal/update/rendering/scene-graph-visual-renderer.h>
#include <dali/public-api/rendering/geometry.h>
#include <dali/public-api/rendering/renderer.h> // Dali::Renderer

#include <unordered_map>

namespace Dali
{
namespace Internal
{
namespace Render
{
class Renderer;
class Geometry;
class UniformBlock;
} // namespace Render

namespace SceneGraph
{
class MemoryPoolCollection;
class RenderManagerDispatcher;

class Renderer;
class TextureSet;
class Geometry;
class Node;

using RendererKey = MemoryPoolKey<SceneGraph::Renderer>;

} // namespace SceneGraph
} // namespace Internal

// Ensure RendererKey can be used in Dali::Vector
template<>
struct TypeTraits<Internal::SceneGraph::RendererKey> : public BasicTypes<Internal::SceneGraph::RendererKey>
{
  enum
  {
    IS_TRIVIAL_TYPE = true
  };
};

namespace Internal
{
namespace SceneGraph
{
class NodeDataProvider;
using RendererContainer = Dali::Vector<RendererKey>;
using RendererIter      = RendererContainer::Iterator;
using RendererConstIter = RendererContainer::ConstIterator;

class Renderer : public PropertyOwner,
                 public UniformMapDataProvider,
                 public RenderDataProvider,
                 public VisualRenderer::VisualRendererPropertyObserver
{
public:
  enum OpacityType
  {
    OPAQUE,
    TRANSPARENT,
    TRANSLUCENT
  };

  /**
   * Construct a new Renderer
   */
  static RendererKey NewKey();

  /**
   * Register memory pool of renderer.
   * This should be called at the begin of Core.
   */
  static void RegisterMemoryPoolCollection(MemoryPoolCollection& memoryPoolCollection);

  /**
   * Unregister memory pool of renderer.
   * This should be called at the end of Core.
   */
  static void UnregisterMemoryPoolCollection();

  /**
   * Destructor
   */
  ~Renderer() override;

  /**
   * Overriden delete operator
   * Deletes the renderer from its global memory pool
   */
  void operator delete(void* ptr);

  /**
   * Get a pointer to the object from the given key.
   * Used by MemoryPoolKey to provide pointer semantics.
   */
  static Renderer* Get(RendererKey::KeyType);

  /**
   * Get the key of the given renderer in the associated memory pool.
   * @param[in] renderer the given renderer
   * @return The key in the associated memory pool.
   */
  static RendererKey GetKey(const SceneGraph::Renderer& renderer);

  /**
   * Get the key of the given renderer in the associated memory pool.
   * @param[in] renderer the given renderer
   * @return The key in the associated memory pool, or -1 if not
   * found.
   */
  static RendererKey GetKey(SceneGraph::Renderer* renderer);

  /**
   * Set the texture set for the renderer
   * @param[in] textureSet The texture set this renderer will use
   */
  void SetTextures(TextureSet* textureSet);

  /**
   * Get the associated texture set
   * @return the texture set.
   */
  const SceneGraph::TextureSet* GetTextureSet() const
  {
    return mTextureSet;
  }

  /**
   * @copydoc RenderDataProvider::GetTextures()
   */
  const Vector<Render::TextureKey>* GetTextures() const override;

  /**
   * @copydoc RenderDataProvider::GetSamplers()
   */
  const Vector<Render::Sampler*>* GetSamplers() const override;

  /**
   * Set the shader for the renderer
   * @param[in] shader The shader this renderer will use
   */
  void SetShader(Shader* shader);

  /**
   * @copydoc RenderDataProvider::GetShader()
   */
  const Shader& GetShader() const override
  {
    return *mShader;
  }

  /**
   * Set the geometry for the renderer
   * @param[in] geometry The geometry this renderer will use
   */
  void SetGeometry(Render::Geometry* geometry);

  /**
   * Set the depth index
   * @param[in] depthIndex the new depth index to use
   */
  void SetDepthIndex(int depthIndex);

  /**
   * @brief Get the depth index
   * @return The depth index
   */
  int GetDepthIndex() const
  {
    return mDepthIndex;
  }

  /**
   * Set the face culling mode
   * @param[in] faceCullingMode to use
   */
  void SetFaceCullingMode(FaceCullingMode::Type faceCullingMode);

  /**
   * Get face culling mode
   * @return The face culling mode
   */
  FaceCullingMode::Type GetFaceCullingMode() const;

  /**
   * Set the blending mode
   * @param[in] blendingMode to use
   */
  void SetBlendMode(BlendMode::Type blendingMode);

  /**
   * Get the blending mode
   * @return The the blending mode
   */
  BlendMode::Type GetBlendMode() const;

  /**
   * Set the blending options. This should only be called from the update thread.
   * @param[in] options A bitmask of blending options.
   */
  void SetBlendingOptions(uint32_t options);

  /**
   * Get the blending options
   * @return The the blending mode
   */
  uint32_t GetBlendingOptions() const;

  /**
   * Set the blend color for blending operation
   * @param blendColor to pass to GL
   */
  void SetBlendColor(const Vector4& blendColor);

  /**
   * Get the blending color
   * @return The blend color
   */
  Vector4 GetBlendColor() const;

  /**
   * Set the index of first element for indexed draw
   * @param[in] firstElement index of first element to draw
   */
  void SetIndexedDrawFirstElement(uint32_t firstElement);

  /**
   * Get the index of first element for indexed draw
   * @return The index of first element for indexed draw
   */
  uint32_t GetIndexedDrawFirstElement() const;

  /**
   * Set the number of elements to draw by indexed draw
   * @param[in] elementsCount number of elements to draw
   */
  void SetIndexedDrawElementsCount(uint32_t elementsCount);

  /**
   * Get the number of elements to draw by indexed draw
   * @return The number of elements to draw by indexed draw
   */
  uint32_t GetIndexedDrawElementsCount() const;

  /**
   * @brief Set whether the Pre-multiplied Alpha Blending is required
   * @param[in] preMultipled whether alpha is pre-multiplied.
   */
  void EnablePreMultipliedAlpha(bool preMultipled);

  /**
   * @brief Query whether alpha is pre-multiplied.
   * @return True is alpha is pre-multiplied, false otherwise.
   */
  bool IsPreMultipliedAlphaEnabled() const;

  /**
   * Sets the depth buffer write mode
   * @param[in] depthWriteMode The depth buffer write mode
   */
  void SetDepthWriteMode(DepthWriteMode::Type depthWriteMode);

  /**
   * Get the depth buffer write mode
   * @return The depth buffer write mode
   */
  DepthWriteMode::Type GetDepthWriteMode() const;

  /**
   * Sets the depth buffer test mode
   * @param[in] depthTestMode The depth buffer test mode
   */
  void SetDepthTestMode(DepthTestMode::Type depthTestMode);

  /**
   * Get the depth buffer test mode
   * @return The depth buffer test mode
   */
  DepthTestMode::Type GetDepthTestMode() const;

  /**
   * Sets the depth function
   * @param[in] depthFunction The depth function
   */
  void SetDepthFunction(DepthFunction::Type depthFunction);

  /**
   * Get the depth function
   * @return The depth function
   */
  DepthFunction::Type GetDepthFunction() const;

  /**
   * Sets the render mode
   * @param[in] mode The render mode
   */
  void SetRenderMode(RenderMode::Type mode);

  /**
   * Sets the stencil function
   * @param[in] stencilFunction The stencil function
   */
  void SetStencilFunction(StencilFunction::Type stencilFunction);

  /**
   * Sets the stencil function mask
   * @param[in] stencilFunctionMask The stencil function mask
   */
  void SetStencilFunctionMask(int stencilFunctionMask);

  /**
   * Sets the stencil function reference
   * @param[in] stencilFunctionReference The stencil function reference
   */
  void SetStencilFunctionReference(int stencilFunctionReference);

  /**
   * Sets the stencil mask
   * @param[in] stencilMask The stencil mask
   */
  void SetStencilMask(int stencilMask);

  /**
   * Sets the stencil operation for when the stencil test fails
   * @param[in] stencilOperationOnFail The stencil operation
   */
  void SetStencilOperationOnFail(StencilOperation::Type stencilOperationOnFail);

  /**
   * Sets the stencil operation for when the depth test fails
   * @param[in] stencilOperationOnZFail The stencil operation
   */
  void SetStencilOperationOnZFail(StencilOperation::Type stencilOperationOnZFail);

  /**
   * Sets the stencil operation for when the depth test passes
   * @param[in] stencilOperationOnZPass The stencil operation
   */
  void SetStencilOperationOnZPass(StencilOperation::Type stencilOperationOnZPass);

  /**
   * Gets the stencil parameters
   * @return The stencil parameters
   */
  const Render::Renderer::StencilParameters& GetStencilParameters() const;

  /**
   * Bakes the mixColor
   * @param[in] mixColor The mix color
   */
  void BakeMixColor(const Vector4& mixColor);

  /**
   * Bakes the component of mixColor
   * @param[in] componentValue The value of mix colorcomponent.
   * @param[in] componentIndex The index of mix color component.
   */
  void BakeMixColorComponent(float componentValue, uint8_t componentIndex);

  /**
   * @copydoc RenderDataProvider::GetMixColor()
   */
  Vector4 GetMixColor() const override;

  /**
   * Sets the rendering behavior
   * @param[in] renderingBehavior The rendering behavior required.
   */
  void SetRenderingBehavior(DevelRenderer::Rendering::Type renderingBehavior);

  /**
   * Gets the rendering behavior
   * @return The rendering behavior
   */
  DevelRenderer::Rendering::Type GetRenderingBehavior() const;

  /**
   * Prepare the object for rendering.
   * This is called by the UpdateManager when an object is due to be rendered in the current frame.
   * @return Whether this renderer has been updated in the current frame
   */
  bool PrepareRender();

  /**
   * Retrieve the Render thread renderer
   * @return The associated render thread renderer
   */
  Render::RendererKey GetRenderer() const;

  /**
   * Query whether the renderer is fully opaque, fully transparent or transparent.
   * @param[in] renderPass render pass for this render instruction
   * @return OPAQUE if fully opaque, TRANSPARENT if fully transparent and TRANSLUCENT if in between
   */
  OpacityType GetOpacityType(uint32_t renderPass, const Node& node) const;

  /**
   * Connect the object to the scene graph
   *
   * @param[in] renderManagerDispacher The render manager dispatcher - used for sending messages to render thread
   */
  void ConnectToSceneGraph(RenderManagerDispatcher& renderManagerDispacher);

  /**
   * Disconnect the object from the scene graph
   * @param[in] renderManagerDispacher The render manager dispatcher - used for sending messages to render thread
   */
  void DisconnectFromSceneGraph(RenderManagerDispatcher& renderManagerDispacher);

  /**
   * Attached to the scene graph object.
   * @param[in] node node who attach this renderer.
   */
  void AttachToNode(const Node& node);

  /**
   * Detached from the scene graph object.
   * @param[in] node node who detach this renderer.
   */
  void DetachFromNode(const Node& node);

  /**
   * @copydoc RenderDataProvider::GetUniformMapDataProvider()
   */
  const UniformMapDataProvider& GetUniformMapDataProvider() const override
  {
    return *this;
  };

  /**
   * @copydoc RenderDataProvider::IsUpdated()
   */
  bool IsUpdated() const override;

  /**
   * @copydoc RenderDataProvider::GetVisualTransformedUpdateArea()
   */
  Vector4 GetVisualTransformedUpdateArea(const Vector4& originalUpdateArea) noexcept override;

  uint32_t GetInstanceCount() const override
  {
    return mInstanceCount;
  }

  void SetInstanceCount(uint32_t instanceCount)
  {
    mInstanceCount = instanceCount;
  }

  /**
   * @brief Set the extents of update area. It will be used when we calculate damaged rect.
   */
  void SetUpdateAreaExtents(const Dali::Extents& updateAreaExtents);

  /**
   * @brief Get the extents of update area. It will be used when we calculate damaged rect.
   */
  Dali::Extents GetUpdateAreaExtents() const
  {
    return mUpdateAreaExtents;
  }

  /**
   * Sets RenderCallback object
   *
   * @param[in] callback Valid pointer to RenderCallback object
   */
  void SetRenderCallback(RenderCallback* callback);

  /**
   * @brief Remove RenderCallback what native rendering used.
   *
   * @param[in] invokeCallback Invoke render callbacks forcibly if we need to catch terminate case at callback.
   */
  void TerminateRenderCallback(bool invokeCallback);

  /**
   * Returns currently set RenderCallback pointer or not.
   *
   * @return True if RenderCallback applied
   */
  [[nodiscard]] bool HasRenderCallback() const;

  /**
   * Merge shader uniform map into renderer uniform map if any of the
   * maps have changed.  Only update uniform map if added to render
   * instructions.
   */
  void UpdateUniformMap();

  /**
   * Set the given external draw commands on this renderer.
   */
  void SetDrawCommands(Dali::DevelRenderer::DrawCommand* pDrawCommands, uint32_t size);

  /**
   * Query whether a renderer is dirty.
   * @return true if the renderer is dirty.
   * @note It is used to decide whether to reuse the RenderList. We can't reuse the RenderList if this is dirty.
   */
  bool IsDirty() const;

  /**
   * @brief True if this renderer could be renderable. False otherwise.
   */
  [[nodiscard]] bool IsRenderable() const;

  /**
   * Update the result of Query of IsDirty() + IsUpdated() result.
   * The result will be updated only 1 times per each frame.
   */
  void CheckDirtyUpdated() const;

public: // From PropertyOwner
  /**
   * @copydoc PropertyOwner::OnMappingChanged
   */
  void OnMappingChanged() override;

  /**
   * @copydoc PropertyOwner::ResetUpdated
   */
  void ResetUpdated() override;

public: // From UniformMapDataProvider
  /**
   * @copydoc UniformMapDataProvider::GetCollectedUniformMap
   */
  const CollectedUniformMap& GetCollectedUniformMap() const override;

public: // For VisualRenderer
  /**
   * Mark whether we should consider shared uniform block, or ignore it.
   */
  void EnableSharedUniformBlock(bool enabled);

  /**
   * @return True if we are VisualRenderer, and ObservingNode is world-ignored.
   */
  bool IsObservingNodeDeactivated() const;

public: // For VisualProperties
  /**
   * To be used only for 1st stage initialization in event thread.
   */
  void SetDummyVisualProperties();

  /**
   * Set the visual propertiess at render thread.
   * @param[in] visualProperties The visual properties to set
   */
  void SetVisualProperties(OwnerPointer<VisualRenderer::VisualProperties>& visualProperties)
  {
    if(DALI_UNLIKELY(mOwnsVisualProperties))
    {
      delete mVisualProperties;
    }
    mVisualProperties     = visualProperties.Release();
    mOwnsVisualProperties = true;

    // Initialize visual dirty flags.
    mVisualPropertiesDirtyFlags = BAKED_FLAG;
  }

  /**
   * May be accessed from event thread
   */
  const VisualRenderer::VisualProperties* GetVisualProperties() const
  {
    return mVisualProperties;
  }

public: // For DecoratedVisualProperties
  /**
   * To be used only for 1st stage initialization in event thread.
   */
  void SetDummyDecoratedVisualProperties();

  /**
   * Set the decorated visual propertiess at render thread.
   * @param[in] decoratedVisualProperties The decorated visual properties to set
   */
  void SetDecoratedVisualProperties(OwnerPointer<VisualRenderer::DecoratedVisualProperties>& decoratedVisualProperties)
  {
    if(DALI_UNLIKELY(mOwnsDecoratedVisualProperties))
    {
      delete mDecoratedVisualProperties;
    }
    mDecoratedVisualProperties     = decoratedVisualProperties.Release();
    mOwnsDecoratedVisualProperties = true;

    // Initialize visual dirty flags.
    mVisualPropertiesDirtyFlags = BAKED_FLAG;
  }

  /**
   * May be accessed from event thread
   */
  const VisualRenderer::DecoratedVisualProperties* GetDecoratedVisualProperties() const
  {
    return mDecoratedVisualProperties;
  }

public: // From VisualRenderer::VisualRendererPropertyObserver
  /**
   * @copydoc VisualRenderer::VisualRendererPropertyObserver::OnVisualRendererPropertyUpdated
   */
  void OnVisualRendererPropertyUpdated(bool bake) override;

  /**
   * @copydoc VisualRenderer::VisualRendererPropertyObserver::GetUpdatedFlag
   */
  uint8_t GetUpdatedFlag() const override;

private:
  /**
   * Protected constructor; See also Renderer::New()
   */
  Renderer();

private:
  enum Decay
  {
    DONE    = 0,
    LAST    = 1,
    INITIAL = 2
  };

private:
  CollectedUniformMap mCollectedUniformMap; ///< Uniform maps collected by the renderer

  Render::RendererKey mRenderer;   ///< Key to the renderer (that's owned by RenderManager)
  TextureSet*         mTextureSet; ///< The texture set this renderer uses. (Not owned)
  Shader*             mShader;     ///< The shader this renderer uses. (Not owned)

  const Node* mAttachedNode{nullptr}; ///< Node attached to this visual renderer (not owned)

  VisualRenderer::VisualProperties*          mVisualProperties;          ///< VisualProperties (optional/owned if flagged)
  VisualRenderer::DecoratedVisualProperties* mDecoratedVisualProperties; ///< DecoratedVisualProperties (optional/owned if flagged)

  uint32_t             mInstanceCount{0};            ///< The number of instances to be drawn
  UniformMap::SizeType mUniformMapChangeCounter{0u}; ///< Value to check if uniform data should be updated
  UniformMap::SizeType mShaderMapChangeCounter{0u};  ///< Value to check if uniform data should be updated

  Dali::Extents mUpdateAreaExtents;

  BlendMode::Type                mBlendMode : 3;            ///< Local copy of the mode of blending
  DevelRenderer::Rendering::Type mRenderingBehavior : 2;    ///< The rendering behavior
  Decay                          mUpdateDecay : 2;          ///< Update decay (aging)
  bool                           mRegenerateUniformMap : 1; ///< true if the map should be regenerated

  uint8_t mVisualPropertiesDirtyFlags : 2; ///< Update decay for visual properties (aging)
  uint8_t mIsRenderableFlag : 3;
  bool    mAdvancedBlendEquationApplied : 1;  ///< true if advanced blend equation applied.
  bool    mOwnsVisualProperties : 1;          ///< Whether this renderer owns the VisualProperties
  bool    mOwnsDecoratedVisualProperties : 1; ///< Whether this renderer owns the DecoratedVisualProperties

  mutable uint8_t mDirtyUpdated; ///< Dirty flag that we can change 1 times per each frame.

public:
  AnimatableProperty<Vector4> mMixColor;   ///< The mix color value
  int32_t                     mDepthIndex; ///< Used only in PrepareRenderInstructions
};

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_RENDERER_H
