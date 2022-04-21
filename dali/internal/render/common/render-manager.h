#ifndef DALI_INTERNAL_SCENE_GRAPH_RENDER_MANAGER_H
#define DALI_INTERNAL_SCENE_GRAPH_RENDER_MANAGER_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/graphics-api/graphics-controller.h>
#include <dali/integration-api/core-enumerations.h>
#include <dali/internal/common/shader-saver.h>
#include <dali/internal/event/rendering/texture-impl.h>
#include <dali/internal/render/renderers/render-vertex-buffer.h>
#include <dali/public-api/math/rect.h>

namespace Dali
{
namespace Integration
{
class RenderStatus;
class Scene;
} // namespace Integration

struct Vector4;

namespace Internal
{
class ProgramCache;
class ShaderSaver;

namespace Render
{
class FrameBuffer;
class Renderer;
struct Sampler;
class RenderTracker;
class Geometry;
class Texture;
} // namespace Render

namespace SceneGraph
{
class RenderQueue;
class RenderInstruction;
class RenderInstructionContainer;
class Shader;
class Scene;

/**
 * RenderManager is responsible for rendering the result of the previous "update", which
 * is provided in a RenderCommand during UpdateManager::Update().
 */
class RenderManager
{
public:
  /**
   * Construct a new RenderManager.
   * @param[in]  graphicsController         The graphics controller for a given rendering backend
   * @param[in]  depthBufferAvailable       Whether the depth buffer is available
   * @param[in]  stencilBufferAvailable     Whether the stencil buffer is available
   */
  static RenderManager* New(Graphics::Controller&               graphicsController,
                            Integration::DepthBufferAvailable   depthBufferAvailable,
                            Integration::StencilBufferAvailable stencilBufferAvailable,
                            Integration::PartialUpdateAvailable partialUpdateAvailable);

  /**
   * Non-virtual destructor; not intended as a base class
   */
  ~RenderManager();

  /**
   * Retrieve the RenderQueue. Messages should only be added to this from the update-thread.
   * @return The render queue.
   */
  RenderQueue& GetRenderQueue();

  /**
   * Set the upstream interface for compiled shader binaries to be sent back to for eventual
   * caching and saving.
   * @param[in] upstream The abstract interface to send any received ShaderDatas onwards to..
   * @note This should be called during core initialisation if shader binaries are to be used.
   */
  void SetShaderSaver(ShaderSaver& upstream);

  // The foltlowing methods should be called via RenderQueue messages

  /*
   * Set the frame time delta (time elapsed since the last frame.
   * @param[in] deltaTime the delta time
   */
  void SetFrameDeltaTime(float deltaTime);

  /**
   * Add a Renderer to the render manager.
   * @param[in] renderer The renderer to add.
   * @post renderer is owned by RenderManager
   */
  void AddRenderer(OwnerPointer<Render::Renderer>& renderer);

  /**
   * Remove a Renderer from the render manager.
   * @param[in] renderer The renderer to remove.
   * @post renderer is destroyed.
   */
  void RemoveRenderer(Render::Renderer* renderer);

  /**
   * Add a sampler to the render manager.
   * @param[in] sampler The sampler to add.
   * @post sampler is owned by RenderManager
   */
  void AddSampler(OwnerPointer<Render::Sampler>& sampler);

  /**
   * Remove a sampler from the render manager.
   * @param[in] sampler The sampler to remove.
   * @post sampler is destroyed.
   */
  void RemoveSampler(Render::Sampler* sampler);

  /**
   * Set minification and magnification filter modes for a sampler
   * @param[in] minFilterMode Filter mode to use when the texture is minificated
   * @param[in] magFilterMode Filter mode to use when the texture is magnified
   */
  void SetFilterMode(Render::Sampler* sampler, uint32_t minFilterMode, uint32_t magFilterMode);

  /**
   * Set wrapping mode for a sampler
   * @param[in] rWrapMode Wrap mode in the z direction
   * @param[in] uWrapMode Wrap mode in the x direction
   * @param[in] vWrapMode Wrap mode in the y direction
   */
  void SetWrapMode(Render::Sampler* sampler, uint32_t rWrapMode, uint32_t sWrapMode, uint32_t tWrapMode);

  /**
   * Add a property buffer to the render manager.
   * @param[in] vertexBuffer The property buffer to add.
   * @post propertBuffer is owned by RenderManager
   */
  void AddVertexBuffer(OwnerPointer<Render::VertexBuffer>& vertexBuffer);

  /**
   * Remove a property buffer from the render manager.
   * @param[in] vertexBuffer The property buffer to remove.
   * @post vertexBuffer is destroyed.
   */
  void RemoveVertexBuffer(Render::VertexBuffer* vertexBuffer);

  /**
   * Add a geometry to the render manager.
   * @param[in] geometry The geometry to add.
   * @post geometry is owned by RenderManager
   */
  void AddGeometry(OwnerPointer<Render::Geometry>& geometry);

  /**
   * Remove a geometry from the render manager.
   * @param[in] geometry The geometry to remove.
   * @post geometry is destroyed.
   */
  void RemoveGeometry(Render::Geometry* geometry);

  /**
   * Adds a property buffer to a geometry from the render manager.
   * @param[in] geometry The geometry
   * @param[in] vertexBuffer The property buffer to remove.
   */
  void AttachVertexBuffer(Render::Geometry* geometry, Render::VertexBuffer* vertexBuffer);

  /**
   * Remove a property buffer from a Render::Geometry from the render manager.
   * @param[in] geometry The geometry
   * @param[in] vertexBuffer The property buffer to remove.
   * @post property buffer is destroyed.
   */
  void RemoveVertexBuffer(Render::Geometry* geometry, Render::VertexBuffer* vertexBuffer);

  /**
   * Sets the format of an existing property buffer
   * @param[in] vertexBuffer The property buffer.
   * @param[in] format The new format of the buffer
   */
  void SetVertexBufferFormat(Render::VertexBuffer* vertexBuffer, OwnerPointer<Render::VertexBuffer::Format>& format);

  /**
   * Sets the data of an existing property buffer
   * @param[in] vertexBuffer The property buffer.
   * @param[in] data The new data of the buffer
   * @param[in] size The new size of the buffer
   */
  void SetVertexBufferData(Render::VertexBuffer* vertexBuffer, OwnerPointer<Vector<uint8_t>>& data, uint32_t size);

  /**
   * Sets the data for the index buffer of an existing geometry
   * @param[in] geometry The geometry
   * @param[in] data A vector containing the indices
   */
  void SetIndexBuffer(Render::Geometry* geometry, Dali::Vector<uint16_t>& data);

  /**
   * Set the geometry type of an existing render geometry
   * @param[in] geometry The render geometry
   * @param[in] geometryType The new geometry type
   */
  void SetGeometryType(Render::Geometry* geometry, uint32_t geometryType);

  /**
   * Adds a texture to the render manager
   * @param[in] texture The texture to add
   */
  void AddTexture(OwnerPointer<Render::Texture>& texture);

  /**
   * Removes a texture from the render manager
   * @param[in] texture The texture to remove
   */
  void RemoveTexture(Render::Texture* texture);

  /**
   * Uploads data to an existing texture
   * @param[in] texture The texture
   * @param[in] pixelData The pixel data object
   * @param[in] params The parameters for the upload
   */
  void UploadTexture(Render::Texture* texture, PixelDataPtr pixelData, const Texture::UploadParams& params);

  /**
   * Generates mipmaps for a given texture
   * @param[in] texture The texture
   */
  void GenerateMipmaps(Render::Texture* texture);

  /**
   * Adds a framebuffer to the render manager
   * @param[in] frameBuffer The framebuffer to add
   */
  void AddFrameBuffer(OwnerPointer<Render::FrameBuffer>& frameBuffer);

  /**
   * Removes a framebuffer from the render manager
   * @param[in] frameBuffer The framebuffer to remove
   */
  void RemoveFrameBuffer(Render::FrameBuffer* frameBuffer);

  /**
   * Attaches a texture as color output to the existing frame buffer
   * @param[in] frameBuffer The FrameBuffer
   * @param[in] texture The texture that will be used as output when rendering
   * @param[in] mipmapLevel The mipmap of the texture to be attached
   * @param[in] layer Indicates which layer of a cube map or array texture to attach. Unused for 2D textures
   */
  void AttachColorTextureToFrameBuffer(Render::FrameBuffer* frameBuffer, Render::Texture* texture, uint32_t mipmapLevel, uint32_t layer);

  /**
   * Attaches a texture as depth output to the existing frame buffer
   * @param[in] frameBuffer The FrameBuffer
   * @param[in] texture The texture that will be used as output when rendering
   * @param[in] mipmapLevel The mipmap of the texture to be attached
   */
  void AttachDepthTextureToFrameBuffer(Render::FrameBuffer* frameBuffer, Render::Texture* texture, uint32_t mipmapLevel);

  /**
   * Attaches a texture as depth/stencil output to the existing frame buffer
   * @param[in] frameBuffer The FrameBuffer
   * @param[in] texture The texture that will be used as output when rendering
   * @param[in] mipmapLevel The mipmap of the texture to be attached
   */
  void AttachDepthStencilTextureToFrameBuffer(Render::FrameBuffer* frameBuffer, Render::Texture* texture, uint32_t mipmapLevel);

  /**
   * Initializes a Scene to the render manager
   * @param[in] scene The Scene to initialize
   */
  void InitializeScene(SceneGraph::Scene* scene);

  /**
   * Uninitializes a Scene to the render manager
   * @param[in] scene The Scene to uninitialize
   */
  void UninitializeScene(SceneGraph::Scene* scene);

  /**
   * This is called when the surface of the scene has been replaced.
   * @param[in] scene The scene.
   */
  void SurfaceReplaced(SceneGraph::Scene* scene);

  /**
   * Adds a render tracker to the RenderManager. RenderManager takes ownership of the
   * tracker. The lifetime of the tracker is related to the lifetime of the tracked
   * object, usually an offscreen render task.
   * @param[in] renderTracker The render tracker
   */
  void AddRenderTracker(Render::RenderTracker* renderTracker);

  /**
   * Removes a render tracker from the RenderManager.
   * @param[in] renderTracker The render tracker to remove.
   */
  void RemoveRenderTracker(Render::RenderTracker* renderTracker);

  // This method should be called from Core::PreRender()

  /**
   * This is called before rendering any scene in the next frame. This method should be preceded
   * by a call up Update.
   * Multi-threading note: this method should be called from a dedicated rendering thread.
   * @pre The graphics implementation must be initialized
   * @param[out] status showing whether update is required to run.
   * @param[in] forceClear force the Clear on the framebuffer even if nothing is rendered.
   */
  void PreRender(Integration::RenderStatus& status, bool forceClear);

  // This method should be called from Core::PreRender()

  /**
   * This is called before rendering any scene in the next frame. This method should be preceded
   * by a call up Update.
   * Multi-threading note: this method should be called from a dedicated rendering thread.
   * @pre The graphics implementation must be initialized
   * @param[in] scene The scene to be rendered.
   * @param[out] damagedRects The list of damaged rects for the current render pass.
   */
  void PreRender(Integration::Scene& scene, std::vector<Rect<int>>& damagedRects);

  // This method should be called from Core::RenderScene()

  /**
   * Render a scene in the next frame. This method should be preceded by a call up PreRender.
   * This method should be called twice. The first pass to render off-screen frame buffers if any,
   * and the second pass to render the surface.
   * Multi-threading note: this method should be called from a dedicated rendering thread.
   * @pre The graphics implementation must be initialized
   * @param[out] status contains the rendering flags.
   * @param[in] scene The scene to be rendered.
   * @param[in] renderToFbo True to render off-screen frame buffers only if any, and False to render the surface only.
   */
  void RenderScene(Integration::RenderStatus& status, Integration::Scene& scene, bool renderToFbo);

  /**
   * Render a scene in the next frame. This method should be preceded by a call up PreRender.
   * This method should be called twice. The first pass to render off-screen frame buffers if any,
   * and the second pass to render the surface.
   * Multi-threading note: this method should be called from a dedicated rendering thread.
   * @pre The graphics implementation must be initialized
   * @param[out] status contains the rendering flags.
   * @param[in] scene The scene to be rendered.
   * @param[in] renderToFbo True to render off-screen frame buffers only if any, and False to render the surface only.
   * @param[in] clippingRect The clipping rect for the rendered scene.
   */
  void RenderScene(Integration::RenderStatus& status, Integration::Scene& scene, bool renderToFbo, Rect<int>& clippingRect);

  // This method should be called from Core::PostRender()

  /**
   * This is called after rendering all the scenes in the next frame. This method should be
   * followed by a call up RenderScene.
   * Multi-threading note: this method should be called from a dedicated rendering thread.
   * @pre The graphics implementation must be initialized
   */
  void PostRender();

private:
  /**
   * Construct a new RenderManager.
   */
  RenderManager();

  // Undefined
  RenderManager(const RenderManager&);

  // Undefined
  RenderManager& operator=(const RenderManager& rhs);

private:
  struct Impl;
  Impl* mImpl;
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_RENDER_MANAGER_H
