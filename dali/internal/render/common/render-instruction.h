#ifndef DALI_INTERNAL_SCENE_GRAPH_RENDER_INSTRUCTION_H
#define DALI_INTERNAL_SCENE_GRAPH_RENDER_INSTRUCTION_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/viewport.h>
#include <dali/internal/update/render-tasks/scene-graph-camera.h>
#include <dali/internal/render/common/render-list.h>
#include <dali/internal/render/renderers/render-frame-buffer.h>
#include <dali/internal/render/gl-resources/context.h>

namespace Dali
{

namespace Internal
{

namespace Render
{
class RenderTracker;
}

namespace SceneGraph
{

/**
 * A set of rendering instructions consisting of:
 * - The list(s) of renderers sorted in the correct rendering order.
 * - The camera related matrices to use when rendering.
 * - An optional off-screen render target.
 * There is one RenderInstruction per RenderTarget, all renderers of all layers
 * get collated onto the single list of lists (mRenderLists)
 */
class RenderInstruction
{
public:

  /**
   * Default constructor so this can be stored in STL containers
   */
  RenderInstruction();

  /**
   * Destructor
   */
  ~RenderInstruction();

  /**
   * Get the next free Renderlist
   * @param capacityRequired in this list
   * @return the renderlist
   */
  RenderList& GetNextFreeRenderList( size_t capacityRequired );

  /**
   * Inform the RenderInstruction that processing for this frame is complete
   * This method should only be called from Update thread
   */
  void UpdateCompleted();

  /**
   * @return the count of active Renderlists
   */
  RenderListContainer::SizeType RenderListCount() const;

  /**
   * Return the renderlist at given index
   * @pre index is inside the valid range of initialized lists
   * @param index of list to return
   * @return pointer to the renderlist, or null if the index is out of bounds.
   */
  const RenderList* GetRenderList( RenderListContainer::SizeType index ) const;

  /**
   * Reset render-instruction
   * render-lists are cleared but not released, while matrices and other settings reset in
   * preparation for building a set of instructions for the renderer.
   *
   * @param[in] camera to use to get view and projection matrices.
   * @param[in] offscreenId A resource Id of an off-screen render target, or 0
   * @param[in] viewport A pointer to a viewport, of NULL.
   * @param[in] clearColor A pointer to a color to clear with, or NULL if no clear is required.
   */
  void Reset( Camera* camera,
              Render::FrameBuffer* frameBuffer,
              const Viewport* viewport,
              const Vector4* clearColor );

  /**
   * Get the view matrix for rendering
   * @param index of the rendering side
   * @return the view matrix
   */
  const Matrix* GetViewMatrix( BufferIndex index ) const
  {
    // inlined as this is called once per frame per render instruction
    return &mCamera->GetViewMatrix( index );
  }

  /**
   * Get the projection matrix for rendering
   * @param index of the rendering side
   * @return the projection matrix
   */
  const Matrix* GetProjectionMatrix( BufferIndex index ) const
  {
    // inlined as this is called once per frame per render instruction
    return &mCamera->GetFinalProjectionMatrix( index );
  }
  // for reflection effect
  const Camera* GetCamera() const
  {
    return mCamera;
  }

private:

  // Undefined
  RenderInstruction(const RenderInstruction&);
  // Undefined
  RenderInstruction& operator=(const RenderInstruction& rhs);

public: // Data

  Render::RenderTracker* mRenderTracker;        ///< Pointer to an optional tracker object (not owned)

  Viewport mViewport;                   ///< Optional viewport
  Vector4  mClearColor;                 ///< Optional color to clear with
  bool     mIsViewportSet:1;            ///< Flag to determine whether the viewport is set
  bool     mIsClearColorSet:1;          ///< Flag to determine whether the clearColor is set
  bool     mIgnoreRenderToFbo:1;        ///< Whether to ignore the render to FBO option (used to measure the performance above 60 fps)

  Render::FrameBuffer* mFrameBuffer;

private: // Data

  Camera* mCamera;  ///< camera that is used
  RenderListContainer mRenderLists;     ///< container of all render lists
  RenderListContainer::SizeType mNextFreeRenderList;     ///< index for the next free render list

};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_RENDER_INSTRUCTION_H
