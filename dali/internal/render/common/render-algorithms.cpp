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
 *
 */

// CLASS HEADER
#include <dali/internal/render/common/render-algorithms.h>

// INTERNAL INCLUDES
#include <dali/integration-api/trace.h>
#include <dali/internal/render/common/render-debug.h>
#include <dali/internal/render/common/render-instruction.h>
#include <dali/internal/render/common/render-list.h>
#include <dali/internal/render/renderers/render-renderer.h>
#include <dali/internal/update/nodes/scene-graph-layer.h>
#include <dali/public-api/math/uint-16-pair.h>

using Dali::Internal::SceneGraph::RenderInstruction;
using Dali::Internal::SceneGraph::RenderItem;
using Dali::Internal::SceneGraph::RenderList;
using Dali::Internal::SceneGraph::RenderListContainer;

namespace Dali
{
namespace Internal
{
namespace Render
{
namespace
{
struct GraphicsDepthCompareOp
{
  constexpr explicit GraphicsDepthCompareOp(DepthFunction::Type compareOp)
  {
    switch(compareOp)
    {
      case DepthFunction::NEVER:
        op = Graphics::CompareOp::NEVER;
        break;
      case DepthFunction::LESS:
        op = Graphics::CompareOp::LESS;
        break;
      case DepthFunction::EQUAL:
        op = Graphics::CompareOp::EQUAL;
        break;
      case DepthFunction::LESS_EQUAL:
        op = Graphics::CompareOp::LESS_OR_EQUAL;
        break;
      case DepthFunction::GREATER:
        op = Graphics::CompareOp::GREATER;
        break;
      case DepthFunction::NOT_EQUAL:
        op = Graphics::CompareOp::NOT_EQUAL;
        break;
      case DepthFunction::GREATER_EQUAL:
        op = Graphics::CompareOp::GREATER_OR_EQUAL;
        break;
      case DepthFunction::ALWAYS:
        op = Graphics::CompareOp::ALWAYS;
        break;
    }
  }
  Graphics::CompareOp op{Graphics::CompareOp::NEVER};
};

struct GraphicsStencilCompareOp
{
  constexpr explicit GraphicsStencilCompareOp(StencilFunction::Type compareOp)
  {
    switch(compareOp)
    {
      case StencilFunction::NEVER:
        op = Graphics::CompareOp::NEVER;
        break;
      case StencilFunction::LESS:
        op = Graphics::CompareOp::LESS;
        break;
      case StencilFunction::EQUAL:
        op = Graphics::CompareOp::EQUAL;
        break;
      case StencilFunction::LESS_EQUAL:
        op = Graphics::CompareOp::LESS_OR_EQUAL;
        break;
      case StencilFunction::GREATER:
        op = Graphics::CompareOp::GREATER;
        break;
      case StencilFunction::NOT_EQUAL:
        op = Graphics::CompareOp::NOT_EQUAL;
        break;
      case StencilFunction::GREATER_EQUAL:
        op = Graphics::CompareOp::GREATER_OR_EQUAL;
        break;
      case StencilFunction::ALWAYS:
        op = Graphics::CompareOp::ALWAYS;
        break;
    }
  }
  Graphics::CompareOp op{Graphics::CompareOp::NEVER};
};

struct GraphicsStencilOp
{
  constexpr explicit GraphicsStencilOp(StencilOperation::Type stencilOp)
  {
    switch(stencilOp)
    {
      case Dali::StencilOperation::KEEP:
        op = Graphics::StencilOp::KEEP;
        break;
      case Dali::StencilOperation::ZERO:
        op = Graphics::StencilOp::ZERO;
        break;
      case Dali::StencilOperation::REPLACE:
        op = Graphics::StencilOp::REPLACE;
        break;
      case Dali::StencilOperation::INCREMENT:
        op = Graphics::StencilOp::INCREMENT_AND_CLAMP;
        break;
      case Dali::StencilOperation::DECREMENT:
        op = Graphics::StencilOp::DECREMENT_AND_CLAMP;
        break;
      case Dali::StencilOperation::INVERT:
        op = Graphics::StencilOp::INVERT;
        break;
      case Dali::StencilOperation::INCREMENT_WRAP:
        op = Graphics::StencilOp::INCREMENT_AND_WRAP;
        break;
      case Dali::StencilOperation::DECREMENT_WRAP:
        op = Graphics::StencilOp::DECREMENT_AND_WRAP;
        break;
    }
  }
  Graphics::StencilOp op{Graphics::StencilOp::KEEP};
};

inline Graphics::Viewport ViewportFromClippingBox(const Uint16Pair& sceneSize, ClippingBox clippingBox, int orientation)
{
  Graphics::Viewport viewport{static_cast<float>(clippingBox.x), static_cast<float>(clippingBox.y), static_cast<float>(clippingBox.width), static_cast<float>(clippingBox.height), 0.0f, 1.0f};

  if(orientation == 90 || orientation == 270)
  {
    if(orientation == 90)
    {
      viewport.x = sceneSize.GetY() - (clippingBox.y + clippingBox.height);
      viewport.y = clippingBox.x;
    }
    else // orientation == 270
    {
      viewport.x = clippingBox.y;
      viewport.y = sceneSize.GetX() - (clippingBox.x + clippingBox.width);
    }
    viewport.width  = static_cast<float>(clippingBox.height);
    viewport.height = static_cast<float>(clippingBox.width);
  }
  else if(orientation == 180)
  {
    viewport.x = sceneSize.GetX() - (clippingBox.x + clippingBox.width);
    viewport.y = sceneSize.GetY() - (clippingBox.y + clippingBox.height);
  }
  viewport.minDepth = 0;
  viewport.maxDepth = 1;
  return viewport;
}

inline Graphics::Rect2D RecalculateRect(Graphics::Rect2D rect, int orientation, Graphics::Viewport viewport)
{
  Graphics::Rect2D newRect;

  // scissor's value should be set based on the default system coordinates.
  // when the surface is rotated, the input valus already were set with the rotated angle.
  // So, re-calculation is needed.
  if(orientation == 90)
  {
    newRect.x      = viewport.height - (rect.y + rect.height);
    newRect.y      = rect.x;
    newRect.width  = rect.height;
    newRect.height = rect.width;
  }
  else if(orientation == 180)
  {
    newRect.x      = viewport.width - (rect.x + rect.width);
    newRect.y      = viewport.height - (rect.y + rect.height);
    newRect.width  = rect.width;
    newRect.height = rect.height;
  }
  else if(orientation == 270)
  {
    newRect.x      = rect.y;
    newRect.y      = viewport.width - (rect.x + rect.width);
    newRect.width  = rect.height;
    newRect.height = rect.width;
  }
  else
  {
    newRect.x      = rect.x;
    newRect.y      = rect.y;
    newRect.width  = rect.width;
    newRect.height = rect.height;
  }
  return newRect;
}

inline Graphics::Rect2D Rect2DFromClippingBox(ClippingBox clippingBox, int orientation, Graphics::Viewport viewport)
{
  Graphics::Rect2D rect2D{clippingBox.x, clippingBox.y, static_cast<uint32_t>(abs(clippingBox.width)), static_cast<uint32_t>(abs(clippingBox.height))};
  return RecalculateRect(rect2D, orientation, viewport);
}

inline Graphics::Rect2D Rect2DFromRect(Dali::Rect<int> rect, int orientation, Graphics::Viewport viewport)
{
  Graphics::Rect2D rect2D{rect.x, rect.y, static_cast<uint32_t>(abs(rect.width)), static_cast<uint32_t>(abs(rect.height))};
  return RecalculateRect(rect2D, orientation, viewport);
}

/**
 * @brief Find the intersection of two AABB rectangles.
 * This is a logical AND operation. IE. The intersection is the area overlapped by both rectangles.
 * @param[in]     aabbA                  Rectangle A
 * @param[in]     aabbB                  Rectangle B
 * @return                               The intersection of rectangle A & B (result is a rectangle)
 */
inline ClippingBox IntersectAABB(const ClippingBox& aabbA, const ClippingBox& aabbB)
{
  ClippingBox intersectionBox;

  // First calculate the largest starting positions in X and Y.
  intersectionBox.x = std::max(aabbA.x, aabbB.x);
  intersectionBox.y = std::max(aabbA.y, aabbB.y);

  // Now calculate the smallest ending positions, and take the largest starting
  // positions from the result, to get the width and height respectively.
  // If the two boxes do not intersect at all, then we need a 0 width and height clipping area.
  // We use max here to clamp both width and height to >= 0 for this use-case.
  intersectionBox.width  = std::max(std::min(aabbA.x + aabbA.width, aabbB.x + aabbB.width) - intersectionBox.x, 0);
  intersectionBox.height = std::max(std::min(aabbA.y + aabbA.height, aabbB.y + aabbB.height) - intersectionBox.y, 0);

  return intersectionBox;
}

/**
 * @brief Set up the stencil and color buffer for automatic clipping (StencilMode::AUTO).
 * @param[in]     item                     The current RenderItem about to be rendered
 * @param[in,out] commandBuffer            The command buffer to write stencil commands into
 * @param[in,out] lastClippingDepth        The stencil depth of the last renderer drawn.
 * @param[in,out] lastClippingId           The clipping ID of the last renderer drawn.
 */
inline void SetupStencilClipping(const RenderItem& item, Graphics::CommandBuffer& commandBuffer, uint32_t& lastClippingDepth, uint32_t& lastClippingId)
{
  const Dali::Internal::SceneGraph::Node* node       = item.mNode;
  const uint32_t                          clippingId = node->GetClippingId();
  // If there is no clipping Id, then either we haven't reached a clipping Node yet, or there aren't any.
  // Either way we can skip clipping setup for this renderer.
  if(clippingId == 0u)
  {
    // Exit immediately if there are no clipping actions to perform (EG. we have not yet hit a clipping node).
    commandBuffer.SetStencilTestEnable(false);
    return;
  }
  commandBuffer.SetStencilTestEnable(true);

  const uint32_t clippingDepth = node->GetClippingDepth();

  // Pre-calculate a mask which has all bits set up to and including the current clipping depth.
  // EG. If depth is 3, the mask would be "111" in binary.
  const uint32_t currentDepthMask = (1u << clippingDepth) - 1u;

  // Are we are writing to the stencil buffer?
  if(item.mNode->GetClippingMode() == Dali::ClippingMode::CLIP_CHILDREN)
  {
    // We are writing to the stencil buffer.
    // If clipping Id is 1, this is the first clipping renderer within this render-list.
    if(clippingId == 1u)
    {
      // We are enabling the stencil-buffer for the first time within this render list.
      // Clear the buffer at this point.

      commandBuffer.SetStencilWriteMask(0xFF);
      commandBuffer.ClearStencilBuffer();
    }
    else if((clippingDepth < lastClippingDepth) ||
            ((clippingDepth == lastClippingDepth) && (clippingId > lastClippingId)))
    {
      // The above if() statement tests if we need to clear some (not all) stencil bit-planes.
      // We need to do this if either of the following are true:
      //   1) We traverse up the scene-graph to a previous stencil depth
      //   2) We are at the same stencil depth but the clipping Id has increased.
      //
      // This calculation takes the new depth to move to, and creates an inverse-mask of that number of consecutive bits.
      // This has the effect of clearing everything except the bit-planes up to (and including) our current depth.
      const uint32_t stencilClearMask = (currentDepthMask >> 1u) ^ 0xff;

      commandBuffer.SetStencilWriteMask(stencilClearMask);
      commandBuffer.ClearStencilBuffer();
    }

    // We keep track of the last clipping Id and depth so we can determine when we are
    // moving back up the scene graph and require some of the stencil bit-planes to be deleted.
    lastClippingDepth = clippingDepth;
    lastClippingId    = clippingId;

    // We only ever write to bit-planes up to the current depth as we may need
    // to erase individual bit-planes and revert to a previous clipping area.
    // Our reference value for testing (in StencilFunc) is written to to the buffer, but we actually
    // want to test a different value. IE. All the bit-planes up to but not including the current depth.
    // So we use the Mask parameter of StencilFunc to mask off the top bit-plane when testing.
    // Here we create our test mask to innore the top bit of the reference test value.
    // As the mask is made up of contiguous "1" values, we can do this quickly with a bit-shift.
    const uint32_t testMask = currentDepthMask >> 1u;

    // Write to the new stencil bit-plane (the other previous bit-planes are also written to).
    commandBuffer.SetStencilWriteMask(currentDepthMask);

    // Test against existing stencil bit-planes. All must match up to (but not including) this depth.
    commandBuffer.SetStencilState(Graphics::CompareOp::EQUAL, currentDepthMask, testMask, Graphics::StencilOp::KEEP, Graphics::StencilOp::REPLACE, Graphics::StencilOp::REPLACE);
  }
  else
  {
    // We are reading from the stencil buffer. Set up the stencil accordingly
    // This calculation sets all the bits up to the current depth bit.
    // This has the effect of testing that the pixel being written to exists in every bit-plane up to the current depth.
    commandBuffer.SetStencilState(Graphics::CompareOp::EQUAL, currentDepthMask, currentDepthMask, Graphics::StencilOp::KEEP, Graphics::StencilOp::KEEP, Graphics::StencilOp::KEEP);
  }
}

/**
 * @brief Sets up the depth buffer for reading and writing based on the current render item.
 * The items read and write mode are used if specified.
 *  - If AUTO is selected for reading, the decision will be based on the Layer Behavior.
 *  - If AUTO is selected for writing, the decision will be based on the items opacity.
 * @param[in]     item                The RenderItem to set up the depth buffer for.
 * @param[in,out] secondaryCommandBuffer The secondary command buffer to write depth commands to
 * @param[in]     depthTestEnabled    True if depth testing has been enabled.
 * @param[in/out] firstDepthBufferUse Initialize to true on the first call, this method will set it to false afterwards.
 */
inline void SetupDepthBuffer(const RenderItem& item, Graphics::CommandBuffer& commandBuffer, bool depthTestEnabled, bool& firstDepthBufferUse)
{
  // Set up whether or not to write to the depth buffer.
  const DepthWriteMode::Type depthWriteMode = item.mRenderer->GetDepthWriteMode();

  // Most common mode (AUTO) is tested first.
  const bool enableDepthWrite = ((depthWriteMode == DepthWriteMode::AUTO) && depthTestEnabled && item.mIsOpaque) ||
                                (depthWriteMode == DepthWriteMode::ON);

  // Set up whether or not to read from (test) the depth buffer.
  const DepthTestMode::Type depthTestMode = item.mRenderer->GetDepthTestMode();

  // Most common mode (AUTO) is tested first.
  const bool enableDepthTest = ((depthTestMode == DepthTestMode::AUTO) && depthTestEnabled) ||
                               (depthTestMode == DepthTestMode::ON);

  // Is the depth buffer in use?
  if(enableDepthWrite || enableDepthTest)
  {
    // The depth buffer must be enabled if either reading or writing.
    commandBuffer.SetDepthTestEnable(true);

    // Look-up the depth function from the Dali::DepthFunction enum, and set it.
    commandBuffer.SetDepthCompareOp(GraphicsDepthCompareOp(item.mRenderer->GetDepthFunction()).op);

    // If this is the first use of the depth buffer this RenderTask, perform a clear.
    // Note: We could do this at the beginning of the RenderTask and rely on the
    // graphics implementation to ignore the clear if not required, but, we would
    // have to enable the depth buffer to do so, which could be a redundant enable.

    //@todo Why are we clearing here? Shouldn't this be part of BeginRenderPass nowadays?!
    if(DALI_UNLIKELY(firstDepthBufferUse))
    {
      // This is the first time the depth buffer is being written to or read.
      firstDepthBufferUse = false;

      // Note: The buffer will only be cleared if written to since a previous clear.
      commandBuffer.SetDepthWriteEnable(true);
      commandBuffer.ClearDepthBuffer();
      if(!enableDepthWrite)
      {
        commandBuffer.SetDepthWriteEnable(enableDepthWrite); // Disable again if necessary.
      }
    }
    else
    {
      // Set up the depth mask based on our depth write setting.
      commandBuffer.SetDepthWriteEnable(enableDepthWrite);
    }
  }
  else
  {
    // The depth buffer is not being used by this renderer, so we must disable it to stop it being tested.
    commandBuffer.SetDepthTestEnable(false);
  }
}

DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_RENDER_PROCESS, false);
} // Unnamed namespace

/**
 * @brief This method is responsible for making decisions on when to apply and unapply scissor clipping, and what rectangular dimensions should be used.
 * A stack of scissor clips at each depth of clipping is maintained, so it can be applied and unapplied.
 * As the clips are hierarchical, this RenderItems AABB is clipped against the current "active" scissor bounds via an intersection operation.
 * @param[in]     item                     The current RenderItem about to be rendered
 * @param[in,out] commandBuffer            The command buffer to write into
 * @param[in]     instruction              The render-instruction to process.
 * @param[in]     orientation              The Scene's surface orientation.
 */
inline void RenderAlgorithms::SetupScissorClipping(
  const RenderItem&        item,
  Graphics::CommandBuffer& commandBuffer,
  const RenderInstruction& instruction,
  int                      orientation)
{
  // Get the number of child scissors in the stack (do not include layer or root box).
  size_t         childStackDepth = mScissorStack.size() - 1u;
  const uint32_t scissorDepth    = item.mNode->GetScissorDepth();
  const bool     clippingNode    = item.mNode->GetClippingMode() == Dali::ClippingMode::CLIP_TO_BOUNDING_BOX;
  bool           traversedUpTree = false;

  // If we are using scissor clipping and we are at the same depth (or less), we need to undo previous clips.
  // We do this by traversing up the scissor clip stack and then apply the appropriate clip for the current render item.
  // To know this, we use clippingDepth. This value is set on *every* node, but only increased as clipping nodes are hit depth-wise.
  // So we know if we are at depth 4 and the stackDepth is 5, that we have gone up.
  // If the depth is the same then we are effectively part of a different sub-tree from the parent, we must also remove the current clip.
  // Note: Stack depth must always be at least 1, as we will have the layer or stage size as the root value.
  if((childStackDepth > 0u) && (scissorDepth < childStackDepth))
  {
    while(scissorDepth < childStackDepth)
    {
      mScissorStack.pop_back();
      --childStackDepth;
    }

    // We traversed up the tree, we need to apply a new scissor rectangle (unless we are at the root).
    traversedUpTree = true;
  }
  if(clippingNode && childStackDepth > 0u && childStackDepth == scissorDepth) // case of sibling clip area
  {
    mScissorStack.pop_back();
    --childStackDepth;
  }

  // If we are on a clipping node, or we have traveled up the tree and gone back past a clipping node, may need to apply a new scissor clip.
  if(clippingNode || traversedUpTree)
  {
    // First, check if we are a clipping node.
    if(clippingNode)
    {
      // This is a clipping node. We generate the AABB for this node and intersect it with the previous intersection further up the tree.

      // Get the AABB bounding box for the current render item.
      const ClippingBox scissorBox(RenderItem::CalculateViewportSpaceAABB(item.mModelViewMatrix, Vector3::ZERO, item.mSize, mViewportRectangle.width, mViewportRectangle.height));

      // Get the AABB for the parent item that we must intersect with.
      const ClippingBox& parentBox(mScissorStack.back());

      // We must reduce the clipping area based on the parents area to allow nested clips. This is a set intersection function.
      // We add the new scissor box to the stack so we can return to it if needed.
      mScissorStack.emplace_back(IntersectAABB(parentBox, scissorBox));
    }

    // The scissor test is enabled if we have any children on the stack, OR, if there are none but it is a user specified layer scissor box.
    // IE. It is not enabled if we are at the top of the stack and the layer does not have a specified clipping box.
    const bool scissorEnabled = (!mScissorStack.empty()) || mHasLayerScissor;

    // Enable the scissor test based on the above calculation
    if(scissorEnabled)
    {
      commandBuffer.SetScissorTestEnable(scissorEnabled);
    }

    // If scissor is enabled, we use the calculated screen-space coordinates (now in the stack).
    if(scissorEnabled)
    {
      ClippingBox useScissorBox(mScissorStack.back());

      if(instruction.mFrameBuffer && instruction.GetCamera()->IsYAxisInverted())
      {
        useScissorBox.y = (instruction.mFrameBuffer->GetHeight() - useScissorBox.height) - useScissorBox.y;
      }

      Graphics::Viewport graphicsViewport = ViewportFromClippingBox(Uint16Pair{0, 0}, mViewportRectangle, 0);
      commandBuffer.SetScissor(Rect2DFromClippingBox(useScissorBox, orientation, graphicsViewport));
    }
  }
  else
  {
    // If there is render callback on the Renderer we need to calculate the scissor box and provide it to the
    // callback so it may be clipped
    if(item.mRenderer->GetRenderCallback())
    {
      ClippingBox useClippingBox(RenderItem::CalculateViewportSpaceAABB(item.mModelViewMatrix, Vector3::ZERO, item.mSize, mViewportRectangle.width, mViewportRectangle.height));

      Graphics::Viewport graphicsViewport   = ViewportFromClippingBox(Uint16Pair{0, 0}, mViewportRectangle, 0);
      Graphics::Rect2D   graphicsScissorBox = Rect2DFromClippingBox(useClippingBox, orientation, graphicsViewport);

      // store clipping box inside the render callback input structure
      auto& input       = item.mRenderer->GetRenderCallbackInput();
      input.clippingBox = ClippingBox(graphicsScissorBox.x, graphicsScissorBox.y, graphicsScissorBox.width, graphicsScissorBox.height);
    }
  }
}

inline void RenderAlgorithms::SetupClipping(const RenderItem&                   item,
                                            Graphics::CommandBuffer&            commandBuffer,
                                            bool&                               usedStencilBuffer,
                                            uint32_t&                           lastClippingDepth,
                                            uint32_t&                           lastClippingId,
                                            Integration::StencilBufferAvailable stencilBufferAvailable,
                                            const RenderInstruction&            instruction,
                                            int                                 orientation)
{
  RenderMode::Type renderMode = RenderMode::AUTO;
  RendererKey      renderer   = item.mRenderer;
  if(renderer)
  {
    renderMode = renderer->GetRenderMode();
  }

  // Setup the stencil using either the automatic clipping feature, or, the manual per-renderer stencil API.
  // Note: This switch is in order of most likely value first.
  switch(renderMode)
  {
    case RenderMode::AUTO:
    {
      // Turn the color buffer on as we always want to render this renderer, regardless of clipping hierarchy.
      commandBuffer.SetColorMask(true);

      // The automatic clipping feature will manage the scissor and stencil functions, only if stencil buffer is available for the latter.
      // As both scissor and stencil clips can be nested, we may be simultaneously traversing up the scissor tree, requiring a scissor to be un-done. Whilst simultaneously adding a new stencil clip.
      // We process both based on our current and old clipping depths for each mode.
      // Both methods with return rapidly if there is nothing to be done for that type of clipping.
      SetupScissorClipping(item, commandBuffer, instruction, orientation);

      if(stencilBufferAvailable == Integration::StencilBufferAvailable::TRUE)
      {
        SetupStencilClipping(item, commandBuffer, lastClippingDepth, lastClippingId);
      }
      break;
    }

    case RenderMode::NONE:
    case RenderMode::COLOR:
    {
      // No clipping is performed for these modes.
      // Note: We do not turn off scissor clipping as it may be used for the whole layer.
      // The stencil buffer will not be used at all, but we only need to disable it if it's available.
      if(stencilBufferAvailable == Integration::StencilBufferAvailable::TRUE)
      {
        commandBuffer.SetStencilTestEnable(false);
      }

      // Setup the color buffer based on the RenderMode.
      commandBuffer.SetColorMask(renderMode == RenderMode::COLOR);
      break;
    }

    case RenderMode::STENCIL:
    case RenderMode::COLOR_STENCIL:
    {
      if(stencilBufferAvailable == Integration::StencilBufferAvailable::TRUE)
      {
        // We are using the low-level Renderer Stencil API.
        // The stencil buffer must be enabled for every renderer with stencil mode on, as renderers in between can disable it.
        // Note: As the command state is cached, it is only sent when needed.
        commandBuffer.SetStencilTestEnable(true);

        // Setup the color buffer based on the RenderMode.
        commandBuffer.SetColorMask(renderMode == RenderMode::COLOR_STENCIL);

        // If this is the first use of the stencil buffer within this RenderList, clear it (this avoids unnecessary clears).
        if(!usedStencilBuffer)
        {
          commandBuffer.ClearStencilBuffer();
          usedStencilBuffer = true;
        }

        // Setup the stencil buffer based on the renderer's properties.
        commandBuffer.SetStencilState(GraphicsStencilCompareOp(renderer->GetStencilFunction()).op,
                                      renderer->GetStencilFunctionReference(),
                                      renderer->GetStencilFunctionMask(),
                                      GraphicsStencilOp(renderer->GetStencilOperationOnFail()).op,
                                      GraphicsStencilOp(renderer->GetStencilOperationOnZPass()).op,
                                      GraphicsStencilOp(renderer->GetStencilOperationOnZFail()).op);
        commandBuffer.SetStencilWriteMask(renderer->GetStencilMask());
      }
      break;
    }
  }
}

inline void RenderAlgorithms::ProcessRenderList(const RenderList&                   renderList,
                                                BufferIndex                         bufferIndex,
                                                const Matrix&                       viewMatrix,
                                                const Matrix&                       projectionMatrix,
                                                Integration::DepthBufferAvailable   depthBufferAvailable,
                                                Integration::StencilBufferAvailable stencilBufferAvailable,
                                                const RenderInstruction&            instruction,
                                                const Rect<int32_t>&                viewport,
                                                const Rect<int>&                    rootClippingRect,
                                                int                                 orientation,
                                                const Uint16Pair&                   sceneSize,
                                                Graphics::RenderPass*               renderPass,
                                                Graphics::RenderTarget*             renderTarget)
{
  DALI_PRINT_RENDER_LIST(renderList);

  // Note: The depth buffer is enabled or disabled on a per-renderer basis.
  // Here we pre-calculate the value to use if these modes are set to AUTO.
  const bool        autoDepthTestMode((depthBufferAvailable == Integration::DepthBufferAvailable::TRUE) &&
                               !(renderList.GetSourceLayer()->IsDepthTestDisabled()) &&
                               renderList.HasColorRenderItems());
  const std::size_t count = renderList.Count();
  uint32_t          lastClippingDepth(0u);
  uint32_t          lastClippingId(0u);
  bool              usedStencilBuffer(false);
  bool              firstDepthBufferUse(true);

  mViewportRectangle = viewport;

  auto* mutableRenderList      = const_cast<RenderList*>(&renderList);
  auto& secondaryCommandBuffer = mutableRenderList->GetCommandBuffer(mGraphicsController);
  secondaryCommandBuffer.Reset();

  // We are always "inside" a render pass here.
  Graphics::CommandBufferBeginInfo info;
  info.SetUsage(0 | Graphics::CommandBufferUsageFlagBits::RENDER_PASS_CONTINUE)
    .SetRenderPass(*renderPass)
    .SetRenderTarget(*renderTarget);
  secondaryCommandBuffer.Begin(info);

  secondaryCommandBuffer.SetViewport(ViewportFromClippingBox(sceneSize, mViewportRectangle, orientation));
  mHasLayerScissor = false;

  // Setup Scissor testing (for both viewport and per-node scissor)
  mScissorStack.clear();

  // Add root clipping rect (set manually for Render function by partial update for example)
  // on the bottom of the stack
  Graphics::Viewport graphicsViewport = ViewportFromClippingBox(sceneSize, mViewportRectangle, 0);
  if(!rootClippingRect.IsEmpty())
  {
    secondaryCommandBuffer.SetScissorTestEnable(true);
    secondaryCommandBuffer.SetScissor(Rect2DFromRect(rootClippingRect, orientation, graphicsViewport));
    mScissorStack.push_back(rootClippingRect);
  }
  // We are not performing a layer clip and no clipping rect set. Add the viewport as the root scissor rectangle.
  else if(!renderList.IsClipping())
  {
    secondaryCommandBuffer.SetScissorTestEnable(false);
    //@todo Vk requires a scissor to be set, as we have turned on dynamic state scissor in the pipelines.
    secondaryCommandBuffer.SetScissor(Rect2DFromClippingBox(mViewportRectangle, orientation, graphicsViewport));
    mScissorStack.push_back(mViewportRectangle);
  }

  if(renderList.IsClipping())
  {
    secondaryCommandBuffer.SetScissorTestEnable(true);
    const ClippingBox& layerScissorBox = renderList.GetClippingBox();
    secondaryCommandBuffer.SetScissor(Rect2DFromClippingBox(layerScissorBox, orientation, graphicsViewport));
    mScissorStack.push_back(layerScissorBox);
    mHasLayerScissor = true;
  }

  // Prepare Render::Renderer Render for this secondary command buffer.
  Renderer::PrepareCommandBuffer();

  // Modify by the clip matrix if necessary (transforms from GL clip space to alternative clip space)
  Matrix clippedProjectionMatrix(projectionMatrix);
  if(mGraphicsController.HasClipMatrix())
  {
    Matrix::Multiply(clippedProjectionMatrix, projectionMatrix, mGraphicsController.GetClipMatrix());
  }

  // Loop through all RenderItems in the RenderList, set up any prerequisites to render them, then perform the render.
  for(uint32_t index = 0u; index < count; ++index)
  {
    const RenderItem& item = renderList.GetItem(index);

    // Discard renderers outside the root clipping rect
    bool skip = true;
    if(!rootClippingRect.IsEmpty())
    {
      Vector4 updateArea = item.mRenderer ? item.mRenderer->GetVisualTransformedUpdateArea(bufferIndex, item.mUpdateArea) : item.mUpdateArea;
      auto    rect       = RenderItem::CalculateViewportSpaceAABB(item.mModelViewMatrix, Vector3(updateArea.x, updateArea.y, 0.0f), Vector3(updateArea.z, updateArea.w, 0.0f), mViewportRectangle.width, mViewportRectangle.height);

      if(rect.Intersect(rootClippingRect))
      {
        skip = false;
      }
    }
    else
    {
      skip = false;
    }

    DALI_PRINT_RENDER_ITEM(item);

    // Set up clipping based on both the Renderer and Actor APIs.
    // The Renderer API will be used if specified. If AUTO, the Actors automatic clipping feature will be used.
    SetupClipping(item, secondaryCommandBuffer, usedStencilBuffer, lastClippingDepth, lastClippingId, stencilBufferAvailable, instruction, orientation);

    if(DALI_LIKELY(item.mRenderer))
    {
      // Set up the depth buffer based on per-renderer flags if depth buffer is available
      // If the per renderer flags are set to "ON" or "OFF", they will always override any Layer depth mode or
      // draw-mode state, such as Overlays.
      // If the flags are set to "AUTO", the behavior then depends on the type of renderer. Overlay Renderers will
      // always disable depth testing and writing. Color Renderers will enable them if the Layer does.
      if(depthBufferAvailable == Integration::DepthBufferAvailable::TRUE)
      {
        SetupDepthBuffer(item, secondaryCommandBuffer, autoDepthTestMode, firstDepthBufferUse);
      }

      // Depending on whether the renderer has draw commands attached or not the rendering process will
      // iterate through all the render queues. If there are no draw commands attached, only one
      // iteration must be done and the default behaviour of the renderer will be executed.
      // The queues allow to iterate over the same renderer multiple times changing the state of the renderer.
      // It is similar to the multi-pass rendering.
      if(!skip)
      {
        auto const MAX_QUEUE = item.mRenderer->GetDrawCommands().empty() ? 1 : DevelRenderer::RENDER_QUEUE_MAX;
        for(auto queue = 0u; queue < MAX_QUEUE; ++queue)
        {
          // Render the item. It will write into the command buffer everything it has to render
          item.mRenderer->Render(secondaryCommandBuffer, bufferIndex, *item.mNode, item.mModelMatrix, item.mModelViewMatrix, viewMatrix, clippedProjectionMatrix, item.mScale, item.mSize, !item.mIsOpaque, instruction, renderTarget, queue);
        }
      }
    }
  }
  secondaryCommandBuffer.End();
}

RenderAlgorithms::RenderAlgorithms(Graphics::Controller& graphicsController)
: mGraphicsController(graphicsController),
  mViewportRectangle(),
  mHasLayerScissor(false)
{
}

void RenderAlgorithms::ResetCommandBuffer(std::vector<Graphics::CommandBufferResourceBinding>* resourceBindings)
{
  // Reset main command buffer
  if(!mGraphicsCommandBuffer)
  {
    mGraphicsCommandBuffer = mGraphicsController.CreateCommandBuffer(
      Graphics::CommandBufferCreateInfo()
        .SetLevel(Graphics::CommandBufferLevel::PRIMARY),
      nullptr);
  }
  else
  {
    mGraphicsCommandBuffer->Reset();
  }

  Graphics::CommandBufferBeginInfo info;
  info.resourceBindings = resourceBindings; // set resource bindings, currently only programs
  info.usage            = 0 | Graphics::CommandBufferUsageFlagBits::ONE_TIME_SUBMIT;
  mGraphicsCommandBuffer->Begin(info);
}

void RenderAlgorithms::SubmitCommandBuffer()
{
  // Submit main command buffer
  mGraphicsCommandBuffer->End();

  Graphics::SubmitInfo submitInfo;
  submitInfo.cmdBuffer.push_back(mGraphicsCommandBuffer.get());
  submitInfo.flags = 0 | Graphics::SubmitFlagBits::FLUSH;
  mGraphicsController.SubmitCommandBuffers(submitInfo);
}

void RenderAlgorithms::DestroyCommandBuffer()
{
  mGraphicsCommandBuffer.reset();
}

void RenderAlgorithms::ProcessRenderInstruction(const RenderInstruction&            instruction,
                                                BufferIndex                         bufferIndex,
                                                Integration::DepthBufferAvailable   depthBufferAvailable,
                                                Integration::StencilBufferAvailable stencilBufferAvailable,
                                                const Rect<int32_t>&                viewport,
                                                const Rect<int>&                    rootClippingRect,
                                                int                                 orientation,
                                                const Uint16Pair&                   sceneSize,
                                                Graphics::RenderPass*               renderPass,
                                                Graphics::RenderTarget*             renderTarget)
{
  DALI_TRACE_BEGIN_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_RENDER_INSTRUCTION_PROCESS", [&](std::ostringstream& oss) { oss << "[" << instruction.RenderListCount() << "]"; });

  DALI_PRINT_RENDER_INSTRUCTION(instruction, bufferIndex);

  const Matrix* viewMatrix       = instruction.GetViewMatrix(bufferIndex);
  const Matrix* projectionMatrix = instruction.GetProjectionMatrix(bufferIndex);

  DALI_ASSERT_DEBUG(viewMatrix);
  DALI_ASSERT_DEBUG(projectionMatrix);

  if(viewMatrix && projectionMatrix)
  {
    std::vector<const Graphics::CommandBuffer*> buffers;
    const RenderListContainer::SizeType         count = instruction.RenderListCount();

    // Iterate through each render list in order. If a pair of render lists
    // are marked as interleaved, then process them together.
    for(RenderListContainer::SizeType index = 0; index < count; ++index)
    {
      const RenderList* renderList = instruction.GetRenderList(index);

      if(renderList && !renderList->IsEmpty())
      {
        ProcessRenderList(*renderList,
                          bufferIndex,
                          *viewMatrix,
                          *projectionMatrix,
                          depthBufferAvailable,
                          stencilBufferAvailable,
                          instruction, // added for reflection effect
                          viewport,
                          rootClippingRect,
                          orientation,
                          sceneSize,
                          renderPass,
                          renderTarget);

        // Execute command buffer
        auto* commandBuffer = renderList->GetCommandBuffer();
        if(commandBuffer)
        {
          buffers.push_back(commandBuffer);
        }
      }
    }
    if(!buffers.empty())
    {
      mGraphicsCommandBuffer->ExecuteCommandBuffers(std::move(buffers));
    }
  }
  DALI_TRACE_END(gTraceFilter, "DALI_RENDER_INSTRUCTION_PROCESS");
}

} // namespace Render

} // namespace Internal

} // namespace Dali
