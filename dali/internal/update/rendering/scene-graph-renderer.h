#ifndef DALI_INTERNAL_SCENE_GRAPH_RENDERER_H
#define DALI_INTERNAL_SCENE_GRAPH_RENDERER_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/internal/update/rendering/scene-graph-visual-renderer.h>
#include <dali/public-api/rendering/geometry.h>
#include <dali/public-api/rendering/renderer.h> // Dali::Renderer

namespace Dali
{
namespace Internal
{
namespace Render
{
class Renderer;
class Geometry;
} // namespace Render

namespace SceneGraph
{
class SceneController;

class Renderer;
class TextureSet;
class Geometry;

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
                 public RenderDataProvider
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
   * Bakes the opacity
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] opacity The opacity
   */
  void BakeOpacity(BufferIndex updateBufferIndex, float opacity);

  /**
   * @copydoc RenderDataProvider::GetOpacity()
   */
  float GetOpacity(BufferIndex updateBufferIndex) const override;

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
   * @param[in] updateBufferIndex The current update buffer index.
   * @return Whether this renderer has been updated in the current frame
   */
  bool PrepareRender(BufferIndex updateBufferIndex);

  /**
   * Retrieve the Render thread renderer
   * @return The associated render thread renderer
   */
  Render::RendererKey GetRenderer() const;

  /**
   * Query whether the renderer is fully opaque, fully transparent or transparent.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] renderPass render pass for this render instruction
   * @return OPAQUE if fully opaque, TRANSPARENT if fully transparent and TRANSLUCENT if in between
   */
  OpacityType GetOpacityType(BufferIndex updateBufferIndex, uint32_t renderPass, const Node& node) const;

  /**
   * Connect the object to the scene graph
   *
   * @param[in] sceneController The scene controller - used for sending messages to render thread
   * @param[in] bufferIndex The current buffer index - used for sending messages to render thread
   */
  void ConnectToSceneGraph(SceneController& sceneController, BufferIndex bufferIndex);

  /**
   * Disconnect the object from the scene graph
   * @param[in] sceneController The scene controller - used for sending messages to render thread
   * @param[in] bufferIndex The current buffer index - used for sending messages to render thread
   */
  void DisconnectFromSceneGraph(SceneController& sceneController, BufferIndex bufferIndex);

  /**
   * Detached from the scene graph object.
   * @param[in] node node who detach this renderer.
   */
  void DetachFromNodeDataProvider(const NodeDataProvider& node);

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
  Vector4 GetVisualTransformedUpdateArea(BufferIndex updateBufferIndex, const Vector4& originalUpdateArea) noexcept override;

  uint32_t GetInstanceCount() const override
  {
    return mInstanceCount;
  }

  void SetInstanceCount(uint32_t instanceCount)
  {
    mInstanceCount = instanceCount;
  }

  /**
   * Sets RenderCallback object
   *
   * @param[in] callback Valid pointer to RenderCallback object
   */
  void SetRenderCallback(RenderCallback* callback);

  /**
   * Returns currently set RenderCallback pointer
   *
   * @return RenderCallback pointer or nullptr
   */
  RenderCallback* GetRenderCallback()
  {
    return mRenderCallback;
  }

  /**
   * Merge shader uniform map into renderer uniform map if any of the
   * maps have changed.  Only update uniform map if added to render
   * instructions.
   * @param[in] updateBufferIndex The current update buffer index.
   */
  void UpdateUniformMap(BufferIndex updateBufferIndex);

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
   * @brief Reset to base values of all animatable properties.
   *
   * @param[in] updateBufferIndex the current buffer index
   */
  void ResetToBaseValues(BufferIndex updateBufferIndex);

  /**
   * @brief Mark all animatable properties as dirty.
   */
  void MarkAsDirty();

  /**
   * Get the capacity of the memory pools
   * @return the capacity of the memory pools
   */
  static uint32_t GetMemoryPoolCapacity();

public: // PropertyOwner::MappingChanged
  /**
   * @copydoc PropertyOwner::OnMappingChanged
   */
  void OnMappingChanged() override;

public: // PropertyOwner implementation
  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyOwner::ResetDefaultProperties()
   */
  virtual void ResetDefaultProperties(BufferIndex updateBufferIndex){};

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyOwner::AddInitializeResetter
   */
  void AddInitializeResetter(ResetterManager& manager) const override;

public: // From UniformMapDataProvider
  /**
   * @copydoc UniformMapDataProvider::GetCollectedUniformMap
   */
  const CollectedUniformMap& GetCollectedUniformMap() const override;

public: // For VisualProperties
  /**
   * To be used only for 1st stage initialization in event thread.
   */
  void SetVisualProperties(VisualRenderer::AnimatableVisualProperties* visualProperties)
  {
    mVisualProperties = visualProperties;
  }

  /**
   * May be accessed from event thread
   */
  const VisualRenderer::AnimatableVisualProperties* GetVisualProperties() const
  {
    return mVisualProperties.Get();
  }

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

  SceneController*    mSceneController; ///< Used for initializing renderers
  Render::RendererKey mRenderer;        ///< Key to the renderer (that's owned by RenderManager)
  TextureSet*         mTextureSet;      ///< The texture set this renderer uses. (Not owned)
  Render::Geometry*   mGeometry;        ///< The geometry this renderer uses. (Not owned)
  Shader*             mShader;          ///< The shader this renderer uses. (Not owned)

  OwnerPointer<VisualRenderer::AnimatableVisualProperties> mVisualProperties{nullptr}; ///< VisualProperties (optional/owned)
  OwnerPointer<Vector4>                                    mBlendColor;                ///< The blend color for blending operation

  Dali::Internal::Render::Renderer::StencilParameters mStencilParameters; ///< Struct containing all stencil related options

  uint32_t             mIndexedDrawFirstElement;     ///< first element index to be drawn using indexed draw
  uint32_t             mIndexedDrawElementsCount;    ///< number of elements to be drawn using indexed draw
  uint32_t             mInstanceCount{0};            ///< The number of instances to be drawn
  uint32_t             mBlendBitmask;                ///< The bitmask of blending options
  uint32_t             mResendFlag;                  ///< Indicate whether data should be resent to the renderer
  UniformMap::SizeType mUniformMapChangeCounter{0u}; ///< Value to check if uniform data should be updated
  UniformMap::SizeType mShaderMapChangeCounter{0u};  ///< Value to check if uniform data should be updated

  DepthFunction::Type            mDepthFunction : 4;     ///< Local copy of the depth function
  FaceCullingMode::Type          mFaceCullingMode : 3;   ///< Local copy of the mode of face culling
  BlendMode::Type                mBlendMode : 3;         ///< Local copy of the mode of blending
  DepthWriteMode::Type           mDepthWriteMode : 3;    ///< Local copy of the depth write mode
  DepthTestMode::Type            mDepthTestMode : 3;     ///< Local copy of the depth test mode
  DevelRenderer::Rendering::Type mRenderingBehavior : 2; ///< The rendering behavior
  Decay                          mUpdateDecay : 2;       ///< Update decay (aging)

  bool mRegenerateUniformMap : 1;     ///< true if the map should be regenerated
  bool mPremultipledAlphaEnabled : 1; ///< Flag indicating whether the Pre-multiplied Alpha Blending is required

  std::vector<Dali::DevelRenderer::DrawCommand> mDrawCommands;
  Dali::RenderCallback*                         mRenderCallback{nullptr};

public:
  AnimatableProperty<float> mOpacity;    ///< The opacity value
  int32_t                   mDepthIndex; ///< Used only in PrepareRenderInstructions
};

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_RENDERER_H
