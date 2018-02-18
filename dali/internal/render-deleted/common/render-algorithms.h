#ifndef DALI_INTERNAL_RENDER_ALGORITHMS_H
#define DALI_INTERNAL_RENDER_ALGORITHMS_H

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
 *
 */
#if 0

// INTERNAL INCLUDES
#include <dali/internal/common/buffer-index.h>

namespace Dali
{

namespace Internal
{
class Context;

namespace SceneGraph
{
class RenderInstruction;
struct RenderItem;
}

namespace Render
{

/**
 * @brief The responsibility of the RenderAlgorithms object is to action renders required by a RenderInstruction.
 */
class RenderAlgorithms
{
  public:

    /**
     * Constructor.
     */
    RenderAlgorithms();

    /**
     * Process a render-instruction.
     * @param[in] instruction The render-instruction to process.
     * @param[in] context     The GL context.
     * @param[in] bufferIndex The current render buffer index (previous update buffer)
     */
    void ProcessRenderInstruction( const SceneGraph::RenderInstruction& instruction, Context& context, BufferIndex bufferIndex );

  private:

    /**
     * @brief Calculate a 2D AABB (axis aligned bounding box) in screen space.
     * The RenderItems dimensions are translated and a Z value of 0 is assumed for this purpose.
     * No projection is performed, but rotation on Z is supported.
     * @param[in] item The RenderItem to generate an AABB for
     * @return         The generated AABB in screen space
     */
    inline Dali::ClippingBox CalculateScreenSpaceAABB( const Dali::Internal::SceneGraph::RenderItem& item );

    /**
     * @brief Perform any scissor clipping related operations based on the current RenderItem.
     * This includes:
     *  - Determining if any action is to be taken (so the method can be exited early if not).
     *  - If the node is a clipping node, apply the nodes clip intersected with the current/parent scissor clip.
     *  - If we have gone up the scissor hierarchy, and need to un-apply a scissor clip.
     *  - Disable scissor clipping completely if it is not needed
     * @param[in] item     The current RenderItem (about to be rendered)
     * @param[in] context  The current Context
     */
    inline void SetupScissorClipping( const Dali::Internal::SceneGraph::RenderItem& item, Context& context );

    /**
     * @brief Set up the clipping based on the specified clipping settings.
     * @param[in]     item                     The current RenderItem (about to be rendered)
     * @param[in]     context                  The context
     * @param[in/out] usedStencilBuffer        True if the stencil buffer has been used so far within this RenderList. Used by StencilMode::ON.
     * @param[in/out] lastClippingDepth        The stencil depth of the last renderer drawn. Used by the clipping feature.
     * @param[in/out] lastClippingId           The clipping ID of the last renderer drawn.   Used by the clipping feature.
     */
    inline void SetupClipping( const Dali::Internal::SceneGraph::RenderItem& item, Context& context, bool& usedStencilBuffer, uint32_t& lastClippingDepth, uint32_t& lastClippingId );

    /**
     * @brief Process a render-list.
     * @param[in] renderList       The render-list to process.
     * @param[in] context          The GL context.
     * @param[in] buffer           The current render buffer index (previous update buffer)
     * @param[in] viewMatrix       The view matrix from the appropriate camera.
     * @param[in] projectionMatrix The projection matrix from the appropriate camera.
     */
    inline void ProcessRenderList( const Dali::Internal::SceneGraph::RenderList& renderList, Context& context, BufferIndex bufferIndex, const Matrix& viewMatrix, const Matrix& projectionMatrix );

    // Prevent copying:
    RenderAlgorithms( RenderAlgorithms& rhs );
    RenderAlgorithms& operator=( const RenderAlgorithms& rhs );


    // Member variables:

    using ScissorStackType = std::vector<Dali::ClippingBox>;      ///< The container type used to maintain the applied scissor hierarchy

    ScissorStackType                        mScissorStack;        ///< Contains the currently applied scissor hierarchy (so we can undo clips)
    Dali::ClippingBox                       mViewportRectangle;   ///< The viewport dimensions, used to translate AABBs to scissor coordinates
    bool                                    mHasLayerScissor:1;   ///< Marks if the currently process render instruction has a layer-based clipping region
};

} // namespace Render

} // namespace Internal

} // namespace Dali

#endif

#endif // DALI_INTERNAL_RENDER_ALGORITHMS_H
