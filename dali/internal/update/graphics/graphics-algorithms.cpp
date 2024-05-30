/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <dali/internal/update/graphics/graphics-algorithms.h>
#include <dali/internal/update/nodes/scene-graph-layer.h>

// EXTERNAL INCLUDES
#include <dali/graphics-api/graphics-controller.h>
#include <dali/graphics-api/graphics-command-buffer.h>
#include <dali/graphics-api/graphics-framebuffer.h>

// INTERNAL INCLUDES
#include <dali/graphics-api/graphics-types.h>

#include <dali/integration-api/core-enumerations.h>

#include <dali/devel-api/common/hash.h>
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/update/rendering/render-instruction-container.h>
#include <dali/internal/update/rendering/scene-graph-geometry.h>
#include <dali/internal/update/rendering/scene-graph-property-buffer.h>
#include <dali/internal/update/rendering/scene-graph-renderer.h>
#include <dali/internal/update/rendering/scene-graph-scene.h>
#include <dali/internal/update/rendering/scene-graph-shader.h>
#include <dali/internal/update/rendering/scene-graph-texture-set.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
namespace
{
// Size of uniform buffer page used when resizing
constexpr uint32_t UBO_PAGE_SIZE = 8192u;

// UBO allocation threshold below which the UBO will shrink
constexpr auto UBO_SHRINK_THRESHOLD = 0.75f;

#if defined( DEBUG_ENABLED )
Debug::Filter* gLogFilter = Debug::Filter::New( Debug::NoLogging, false, "LOG_RENDERER" );
#endif

constexpr float CLIP_MATRIX_DATA[] = {
  1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.5f, 1.0f };

const Matrix CLIP_MATRIX( CLIP_MATRIX_DATA );


// Helper to get the vertex input format
Dali::Graphics::VertexInputFormat GetPropertyVertexFormat(Property::Type propertyType)
{
  Dali::Graphics::VertexInputFormat type{};

  switch(propertyType)
  {
    case Property::BOOLEAN:
    {
      type = Dali::Graphics::VertexInputFormat::UNDEFINED; // type = GL_BYTE; @todo new type for this?
      break;
    }
    case Property::INTEGER:
    {
      type = Dali::Graphics::VertexInputFormat::INTEGER; // (short)
      break;
    }
    case Property::FLOAT:
    {
      type = Dali::Graphics::VertexInputFormat::FLOAT;
      break;
    }
    case Property::VECTOR2:
    {
      type = Dali::Graphics::VertexInputFormat::FVECTOR2;
      break;
    }
    case Property::VECTOR3:
    {
      type = Dali::Graphics::VertexInputFormat::FVECTOR3;
      break;
    }
    case Property::VECTOR4:
    {
      type = Dali::Graphics::VertexInputFormat::FVECTOR4;
      break;
    }
    default:
    {
      type = Dali::Graphics::VertexInputFormat::UNDEFINED;
    }
  }

  return type;
}


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

constexpr Graphics::BlendFactor ConvertBlendFactor( BlendFactor::Type blendFactor )
{
  switch( blendFactor )
  {
    case BlendFactor::ZERO:
      return Graphics::BlendFactor::ZERO;
    case BlendFactor::ONE:
      return Graphics::BlendFactor::ONE;
    case BlendFactor::SRC_COLOR:
      return Graphics::BlendFactor::SRC_COLOR;
    case BlendFactor::ONE_MINUS_SRC_COLOR:
      return Graphics::BlendFactor::ONE_MINUS_SRC_COLOR;
    case BlendFactor::SRC_ALPHA:
      return Graphics::BlendFactor::SRC_ALPHA;
    case BlendFactor::ONE_MINUS_SRC_ALPHA:
      return Graphics::BlendFactor::ONE_MINUS_SRC_ALPHA;
    case BlendFactor::DST_ALPHA:
      return Graphics::BlendFactor::DST_ALPHA;
    case BlendFactor::ONE_MINUS_DST_ALPHA:
      return Graphics::BlendFactor::ONE_MINUS_DST_ALPHA;
    case BlendFactor::DST_COLOR:
      return Graphics::BlendFactor::DST_COLOR;
    case BlendFactor::ONE_MINUS_DST_COLOR:
      return Graphics::BlendFactor::ONE_MINUS_DST_COLOR;
    case BlendFactor::SRC_ALPHA_SATURATE:
      return Graphics::BlendFactor::SRC_ALPHA_SATURATE;
    case BlendFactor::CONSTANT_COLOR:
      return Graphics::BlendFactor::CONSTANT_COLOR;
    case BlendFactor::ONE_MINUS_CONSTANT_COLOR:
      return Graphics::BlendFactor::ONE_MINUS_CONSTANT_COLOR;
    case BlendFactor::CONSTANT_ALPHA:
      return Graphics::BlendFactor::CONSTANT_ALPHA;
    case BlendFactor::ONE_MINUS_CONSTANT_ALPHA:
      return Graphics::BlendFactor::ONE_MINUS_CONSTANT_ALPHA;
  }
  return Graphics::BlendFactor{};
}

constexpr Graphics::BlendOp ConvertBlendEquation( BlendEquation::Type blendEquation )
{
  switch( blendEquation )
  {
    case BlendEquation::ADD:
      return Graphics::BlendOp::ADD;
    case BlendEquation::SUBTRACT:
      return Graphics::BlendOp::SUBTRACT;
    case BlendEquation::REVERSE_SUBTRACT:
      return Graphics::BlendOp::REVERSE_SUBTRACT;
  }
  return Graphics::BlendOp{};
}

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
  Graphics::Viewport viewport{static_cast<float>(clippingBox.x), static_cast<float>(clippingBox.y), static_cast<float>(clippingBox.width), static_cast<float>(clippingBox.height), 0.0f, 0.0f};

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
 * @brief Sets up the depth buffer for reading and writing based on the current render item.
 * The items read and write mode are used if specified.
 *  - If AUTO is selected for reading, the decision will be based on the Layer Behavior.
 *  - If AUTO is selected for writing, the decision will be based on the items opacity.
 * @param[in]     item                The RenderItem to set up the depth buffer for.
 * @param[in,out] secondaryCommandBuffer The secondary command buffer to write depth commands to
 * @param[in]     depthTestEnabled    True if depth testing has been enabled.
 * @param[in/out] firstDepthBufferUse Initialize to true on the first call, this method will set it to false afterwards.
 */
inline void SetupDepthBuffer( const RenderItem&        item,
                              Graphics::CommandBuffer& commandBuffer,
                              bool                     depthTestEnabled,
                              bool&                    firstDepthBufferUse )
{
  // Set up whether or not to write to the depth buffer.
  const DepthWriteMode::Type depthWriteMode = item.mRenderer->GetDepthWriteMode();
  // Most common mode (AUTO) is tested first.
  const bool enableDepthWrite = ( ( depthWriteMode == DepthWriteMode::AUTO ) && depthTestEnabled && item.mIsOpaque ) ||
                                ( depthWriteMode == DepthWriteMode::ON );

  // Set up whether or not to read from (test) the depth buffer.
  const DepthTestMode::Type depthTestMode = item.mRenderer->GetDepthTestMode();

  // Most common mode (AUTO) is tested first.
  const bool enableDepthTest =
    ( ( depthTestMode == DepthTestMode::AUTO ) && depthTestEnabled ) || ( depthTestMode == DepthTestMode::ON );

  // Is the depth buffer in use?
  if( enableDepthWrite || enableDepthTest )
  {
    // The depth buffer must be enabled if either reading or writing.
    commandBuffer.SetDepthTestEnable( true );

    // Look-up the depth function from the Dali::DepthFunction enum, and set it.
    commandBuffer.SetDepthCompareOp( GraphicsDepthCompareOp( item.mRenderer->GetDepthFunction() ).op );

    // If this is the first use of the depth buffer this RenderTask, perform a clear.
    // Note: We could do this at the beginning of the RenderTask and rely on the
    // graphics implementation to ignore the clear if not required, but, we would
    // have to enable the depth buffer to do so, which could be a redundant enable.
    if( DALI_UNLIKELY( firstDepthBufferUse ) )
    {
      // This is the first time the depth buffer is being written to or read.
      firstDepthBufferUse = false;

      // Note: The buffer will only be cleared if written to since a previous clear.
      commandBuffer.SetDepthWriteEnable( true );
      commandBuffer.ClearDepthBuffer();
    }

    // Set up the depth mask based on our depth write setting.
    commandBuffer.SetDepthWriteEnable( enableDepthWrite );
  }
  else
  {
    // The depth buffer is not being used by this renderer, so we must disable it to stop it being tested.
    commandBuffer.SetDepthTestEnable( false );
  }
}

ClippingBox IntersectAABB( const ClippingBox& aabbA, const ClippingBox& aabbB )
{
  ClippingBox intersectionBox;

  // First calculate the largest starting positions in X and Y.
  intersectionBox.x = std::max( aabbA.x, aabbB.x );
  intersectionBox.y = std::max( aabbA.y, aabbB.y );

  // Now calculate the smallest ending positions, and take the largest starting
  // positions from the result, to get the width and height respectively.
  // If the two boxes do not intersect at all, then we need a 0 width and height clipping area.
  // We use max here to clamp both width and height to >= 0 for this use-case.
  intersectionBox.width = std::max( std::min( aabbA.x + aabbA.width, aabbB.x + aabbB.width ) - intersectionBox.x, 0 );
  intersectionBox.height =
    std::max( std::min( aabbA.y + aabbA.height, aabbB.y + aabbB.height ) - intersectionBox.y, 0 );

  return intersectionBox;
}

void ComputeUniformBufferRequirements( const Scene* scene, BufferIndex bufferIndex,
                                       uint32_t& cpuSize, uint32_t& gpuSize)
{
  auto& instructions = const_cast<Scene*>(scene)->GetRenderInstructions();
  const uint32_t numberOfInstructions = instructions.Count(bufferIndex);
  for( uint32_t i = 0; i < numberOfInstructions; ++i)
  {
    RenderInstruction& instruction = instructions.At(bufferIndex, i);
    for(auto j = 0u; j<instruction.RenderListCount(); ++j)
    {
      const auto& renderList = instruction.GetRenderList(j);
      for(auto k = 0u; k<renderList->Count();k++)
      {
        auto& item = renderList->GetItem(k);
        if(item.mRenderer && item.mRenderer->GetGeometry() != nullptr)
        {
          auto shader = item.mRenderer->PrepareShader();
          if(shader)
          {
            const auto& memoryRequirements = shader->GetUniformBlockMemoryRequirements();
            cpuSize += memoryRequirements.totalCpuSizeRequired;
            gpuSize += memoryRequirements.totalGpuSizeRequired;
          }
        }
      }
    }
  }
}

} // namespace

GraphicsAlgorithms::GraphicsAlgorithms( Graphics::Controller& controller )
: mGraphicsController(controller),
  mViewportRectangle()
{
  mUniformBufferManager.reset( new UniformBufferManager( &controller ) );
}

void GraphicsAlgorithms::SetupScissorClipping(
  const RenderItem&        item,
  Graphics::CommandBuffer& commandBuffer,
  const RenderInstruction& instruction,
  int                      orientation)
{
  // Get the number of child scissors in the stack (do not include layer or root box).
  size_t childStackDepth = mScissorStack.size() - 1u;
  const uint32_t scissorDepth = item.mNode->GetScissorDepth();
  const bool clippingNode = item.mNode->GetClippingMode() == Dali::ClippingMode::CLIP_TO_BOUNDING_BOX;
  bool traversedUpTree = false;

  // If we are using scissor clipping, and we are at the same depth (or less), we need to undo previous clips.
  // We do this by traversing up the scissor clip stack and then apply the appropriate clip for the current render item.
  // To know this, we use clippingDepth. This value is set on *every* node, but only increased as clipping nodes are hit depth-wise.
  // So we know if we are at depth 4 and the stackDepth is 5, that we have gone up.
  // If the depth is the same then we are effectively part of a different sub-tree from the parent, we must also remove the current clip.
  // Note: Stack depth must always be at least 1, as we will have the layer or stage size as the root value.
  if( ( childStackDepth > 0u ) && ( scissorDepth < childStackDepth ) )
  {
    while( scissorDepth < childStackDepth )
    {
      mScissorStack.pop_back();
      --childStackDepth;
    }

    // We traversed up the tree, we need to apply a new scissor rectangle (unless we are at the root).
    traversedUpTree = true;
  }
  if( clippingNode && childStackDepth > 0u && childStackDepth == scissorDepth ) // case of sibling clip area
  {
    mScissorStack.pop_back();
    --childStackDepth;
  }

  // If we are on a clipping node, or we have traveled up the tree and gone back past a clipping node, may need to apply a new scissor clip.
  if( clippingNode || traversedUpTree )
  {
    // First, check if we are a clipping node.
    if( clippingNode )
    {
      // This is a clipping node. We generate the AABB for this node and intersect it with the previous intersection further up the tree.
      // If the viewport hasn't been set, and we're rendering to a framebuffer, then
      // set the size of the viewport to that of the framebuffer.
      // Get the AABB bounding box for the current render item.
      const ClippingBox scissorBox(RenderItem::CalculateViewportSpaceAABB(item.mModelViewMatrix, Vector3::ZERO, item.mSize, mViewportRectangle.width, mViewportRectangle.height));

      // Get the AABB for the parent item that we must intersect with.
      const ClippingBox& parentBox( mScissorStack.back() );

      // We must reduce the clipping area based on the parents area to allow nested clips. This is a set intersection function.
      // We add the new scissor box to the stack so we can return to it if needed.
      mScissorStack.emplace_back( IntersectAABB( parentBox, scissorBox ) );
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
}

void GraphicsAlgorithms::SetupStencilClipping(const RenderItem& item,
                                              Graphics::CommandBuffer& commandBuffer,
                                              uint32_t& lastClippingDepth,
                                              uint32_t& lastClippingId,
                                              const RenderInstruction& instruction,
                                              int orientation)
{
  const Dali::Internal::SceneGraph::Node* node = item.mNode;
  const uint32_t clippingId = node->GetClippingId();
  // If there is no clipping Id, then either we haven't reached a clipping Node yet, or there aren't any.
  // Either way we can skip clipping setup for this renderer.
  if( clippingId == 0u )
  {
    commandBuffer.SetStencilTestEnable(false);
    // Exit immediately if there are no clipping actions to perform (EG. we have not yet hit a clipping node).
    return;
  }
  commandBuffer.SetStencilTestEnable(true);

  const uint32_t clippingDepth = node->GetClippingDepth();

  // Pre-calculate a mask which has all bits set up to and including the current clipping depth.
  // EG. If depth is 3, the mask would be "111" in binary.
  const uint32_t currentDepthMask = ( 1u << clippingDepth ) - 1u;

  // Are we are writing to the stencil buffer?
  if( item.mNode->GetClippingMode() == Dali::ClippingMode::CLIP_CHILDREN )
  {
    // We are writing to the stencil buffer.
    // If clipping Id is 1, this is the first clipping renderer within this render-list.
    if( clippingId == 1u )
    {
      // We are enabling the stencil-buffer for the first time within this render list.
      // Clear the buffer at this point.
      commandBuffer.SetStencilWriteMask(0xFF);
      commandBuffer.ClearStencilBuffer();
    }
    else if( ( clippingDepth < lastClippingDepth ) ||
             ( ( clippingDepth == lastClippingDepth ) && ( clippingId > lastClippingId ) ) )
    {
      // The above if() statement tests if we need to clear some (not all) stencil bit-planes.
      // We need to do this if either of the following are true:
      //   1) We traverse up the scene-graph to a previous stencil depth
      //   2) We are at the same stencil depth but the clipping Id has increased.
      //
      // This calculation takes the new depth to move to, and creates an inverse-mask of that number of consecutive bits.
      // This has the effect of clearing everything except the bit-planes up to (and including) our current depth.
      const uint32_t stencilClearMask = ( currentDepthMask >> 1u ) ^ 0xff;

      commandBuffer.SetStencilWriteMask(stencilClearMask);
      commandBuffer.ClearStencilBuffer();
    }

    // We keep track of the last clipping Id and depth so we can determine when we are
    // moving back up the scene graph and require some of the stencil bit-planes to be deleted.
    lastClippingDepth = clippingDepth;
    lastClippingId = clippingId;

    // We only ever write to bit-planes up to the current depth as we may need
    // to erase individual bit-planes and revert to a previous clipping area.
    // Our reference value for testing (in StencilFunc) is written to to the buffer, but we actually
    // want to test a different value. IE. All the bit-planes up to but not including the current depth.
    // So we use the Mask parameter of StencilFunc to mask off the top bit-plane when testing.
    // Here we create our test mask to innore the top bit of the reference test value.
    // As the mask is made up of contiguous "1" values, we can do this quickly with a bit-shift.
    const uint32_t testMask = currentDepthMask >> 1u;

    // Test against existing stencil bit-planes. All must match up to (but not including) this depth.
    commandBuffer.SetStencilFunc(Graphics::CompareOp::EQUAL, currentDepthMask, testMask);
    // Write to the new stencil bit-plane (the other previous bit-planes are also written to).
    commandBuffer.SetStencilWriteMask(currentDepthMask);
    commandBuffer.SetStencilOp(Graphics::StencilOp::KEEP, Graphics::StencilOp::REPLACE, Graphics::StencilOp::REPLACE);
  }
  else
  {
    // We are reading from the stencil buffer. Set up the stencil accordingly
    // This calculation sets all the bits up to the current depth bit.
    // This has the effect of testing that the pixel being written to exists in every bit-plane up to the current depth.

    commandBuffer.SetStencilFunc(Graphics::CompareOp::EQUAL, currentDepthMask, currentDepthMask);
    commandBuffer.SetStencilOp(Graphics::StencilOp::KEEP, Graphics::StencilOp::KEEP, Graphics::StencilOp::KEEP);
  }
}

void GraphicsAlgorithms::SetupClipping(const RenderItem& item,
                                       Graphics::CommandBuffer& commandBuffer,
                                       bool& usedStencilBuffer,
                                       uint32_t& lastClippingDepth,
                                       uint32_t& lastClippingId,
                                       bool stencilBufferAvailable,
                                       const RenderInstruction& instruction,
                                       int                      orientation)
{
  RenderMode::Type renderMode = RenderMode::AUTO;
  const Renderer *renderer = item.mRenderer;
  if( renderer )
  {
    renderMode = renderer->GetStencilParameters().renderMode;
  }

  // Setup the stencil using either the automatic clipping feature, or, the manual per-renderer stencil API.
  // Note: This switch is in order of most likely value first.
  switch( renderMode )
  {
    case RenderMode::AUTO:
    {
      // Turn the color buffer on as we always want to render this renderer, regardless of clipping hierarchy.
      commandBuffer.SetColorMask(true);

      // The automatic clipping feature will manage the scissor and stencil functions, only if stencil buffer is available for the latter.
      // As both scissor and stencil clips can be nested, we may be simultaneously traversing up the scissor tree, requiring a scissor to be un-done. Whilst simultaneously adding a new stencil clip.
      // We process both based on our current and old clipping depths for each mode.
      // Both methods with return rapidly if there is nothing to be done for that type of clipping.
      SetupScissorClipping( item, commandBuffer, instruction, orientation );

      if( stencilBufferAvailable )
      {
        SetupStencilClipping( item, commandBuffer, lastClippingDepth, lastClippingId, instruction, orientation );
      }
      break;
    }
    case RenderMode::NONE:
    case RenderMode::COLOR:
    {
      // No clipping is performed for these modes.
      // Note: We do not turn off scissor clipping as it may be used for the whole layer.
      // The stencil buffer will not be used at all, but we only need to disable it if it's available.
      if(stencilBufferAvailable)
      {
        commandBuffer.SetStencilTestEnable(false);
      }

      // Setup the color buffer based on the RenderMode.
      commandBuffer.SetColorMask( renderMode == RenderMode::COLOR );
      break;
    }

    case RenderMode::STENCIL:
    case RenderMode::COLOR_STENCIL:
    {
      if(stencilBufferAvailable)
      {
        // We are using the low-level Renderer Stencil API.
        // The stencil buffer must be enabled for every renderer with stencil mode on, as renderers in between can disable it.
        // Note: As the command state is cached, it is only sent when needed.
        commandBuffer.SetStencilTestEnable(true);

        // Setup the color buffer based on the RenderMode.
        commandBuffer.SetColorMask( renderMode == RenderMode::COLOR_STENCIL );

        // If this is the first use of the stencil buffer within this RenderList, clear it (this avoids unnecessary clears).
        if( !usedStencilBuffer )
        {
          commandBuffer.ClearStencilBuffer();
          usedStencilBuffer = true;
        }

        // Setup the stencil buffer based on the renderers properties.
        auto& params = renderer->GetStencilParameters();
        commandBuffer.SetStencilOp(GraphicsStencilOp(params.stencilOperationOnFail).op,
                                   GraphicsStencilOp(params.stencilOperationOnZPass).op,
                                   GraphicsStencilOp(params.stencilOperationOnZFail).op);
        commandBuffer.SetStencilFunc(GraphicsStencilCompareOp(params.stencilFunction).op,
                                     params.stencilFunctionReference,
                                     params.stencilFunctionMask);
        commandBuffer.SetStencilWriteMask(params.stencilMask);
      }
      break;
    }
  }
}


bool GraphicsAlgorithms::SetupPipelineViewportState( Graphics::ViewportState& outViewportState )
{
  // The scissor test is enabled if we have more than one child (the first entry is always the viewport)
  // on the stack, or the stack is empty and there is a layer clipping box.
  // IE. It is not enabled if we are at the top of the stack and the layer does not have a specified clipping box.

  auto scissorEnabled = ( mScissorStack.size() > 1u );// || mHasLayerScissor;
  // If scissor is enabled, we use the calculated screen-space coordinates (now in the stack).
  if( scissorEnabled )
  {
    ClippingBox useScissorBox( mScissorStack.back() );
    outViewportState.SetScissorTestEnable( true  );
    outViewportState.SetScissor( { useScissorBox.x,
                                int32_t(outViewportState.viewport.height - float(useScissorBox.y + useScissorBox.height) ),
                                uint32_t(useScissorBox.width),
                                uint32_t(useScissorBox.height) } );
  }
  else
  {
    outViewportState.SetScissorTestEnable( false );
    outViewportState.SetScissor({});
  }
  return scissorEnabled;
}

void GraphicsAlgorithms::RecordRenderItemList(
  const RenderList& renderList,
  BufferIndex bufferIndex,
  const Matrix& viewMatrix,
  const Matrix& projectionMatrix,
  const RenderInstruction& instruction,
  const Rect<int32_t>& viewport,
  const Rect<int>& rootClippingRect,
  Uint16Pair sceneSize,
  bool depthBufferAvailable,
  bool stencilBufferAvailable)
{
  int orientation=0; // Ignore orientation for now.

  mViewportRectangle = viewport;
  // Note: The depth buffer is enabled or disabled on a per-renderer basis.
  // Here we pre-calculate the value to use if these modes are set to AUTO.
  const bool autoDepthTestMode(depthBufferAvailable &&
                               !(renderList.GetSourceLayer()->IsDepthTestDisabled()) &&
                               renderList.HasColorRenderItems());
  uint32_t lastClippingDepth = 0u;
  uint32_t lastClippingId = 0u;
  bool usedStencilBuffer=false;
  bool firstDepthBufferUse=true;

  auto* mutableRenderList      = const_cast<RenderList*>(&renderList);
  auto& secondaryCommandBuffer = mutableRenderList->GetCommandBuffer(mGraphicsController);
  secondaryCommandBuffer.Reset();
  mHasLayerScissor = false;
  mScissorStack.clear();

  auto width = float(instruction.mViewport.width);
  auto height = float(instruction.mViewport.height);

  // If the viewport hasn't been set, and we're rendering to a framebuffer, then
  // set the size of the viewport to that of the framebuffer.
  if( !instruction.mIsViewportSet && instruction.mFrameBuffer != nullptr )
  {
    width = float(instruction.mFrameBuffer->GetWidth());
    height = float(instruction.mFrameBuffer->GetHeight());
  }
  secondaryCommandBuffer.SetViewport( { float( instruction.mViewport.x ),
                                        float( instruction.mViewport.y ),
                                        width,
                                        height,
                                        0.0f , 1.0f } );

  // Setup scissor clip
  if(!rootClippingRect.IsEmpty())
  {
    Graphics::Viewport graphicsViewport = ViewportFromClippingBox(sceneSize, mViewportRectangle, 0);
    secondaryCommandBuffer.SetScissorTestEnable(true);
    secondaryCommandBuffer.SetScissor(Rect2DFromRect(rootClippingRect, orientation, graphicsViewport));
    mScissorStack.push_back(rootClippingRect);
  }
  // We are not performing a layer clip and no clipping rect set. Add the viewport as the root scissor rectangle.
  else if(!renderList.IsClipping())
  {
    secondaryCommandBuffer.SetScissorTestEnable(false);
    mScissorStack.push_back(mViewportRectangle);
  }
  if(renderList.IsClipping())
  {
    Graphics::Viewport graphicsViewport = ViewportFromClippingBox(sceneSize, mViewportRectangle, 0);
    secondaryCommandBuffer.SetScissorTestEnable(true);
    const ClippingBox& layerScissorBox = renderList.GetClippingBox();
    secondaryCommandBuffer.SetScissor(Rect2DFromClippingBox(layerScissorBox, orientation, graphicsViewport));
    mScissorStack.push_back(layerScissorBox);
    mHasLayerScissor = true;
  }

  auto numberOfRenderItems = renderList.Count();

  //@todo Add core config to change the projection matrix at runtime depending on backend
#if(VULKAN_ENABLED)
  Matrix vulkanProjectionMatrix;
  Matrix::Multiply( vulkanProjectionMatrix, projectionMatrix, CLIP_MATRIX );
  auto& projMatrix = vulkanProjectionMatrix;
#else
  auto& projMatrix = projectionMatrix;
#endif

  for( auto i = 0u; i < numberOfRenderItems; ++i )
  {
    auto& item = mutableRenderList->GetItem( i );

    // Discard renderers outside the root clipping rect
    bool skip = true;
    if(!rootClippingRect.IsEmpty())
    {
      Vector3 position = item.mNode->mPosition.Get(bufferIndex);
      auto rect = RenderItem::CalculateViewportSpaceAABB(item.mModelViewMatrix, position, item.mSize, mViewportRectangle.width, mViewportRectangle.height);

      if(rect.Intersect(rootClippingRect))
      {
        skip = false;
      }
    }
    else
    {
      skip = false;
    }

    // Set up clipping based on both the Renderer and Actor APIs.
    // The Renderer API will be used if specified. If AUTO, the Actors automatic clipping feature will be used.
    SetupClipping(item, secondaryCommandBuffer, usedStencilBuffer, lastClippingDepth, lastClippingId, stencilBufferAvailable, instruction, orientation);

    auto renderer = item.mRenderer;
    if(renderer)
    {
      if(depthBufferAvailable)
      {
        SetupDepthBuffer(item, secondaryCommandBuffer, autoDepthTestMode, firstDepthBufferUse);
      }
      if(!skip)
      {
        item.mRenderer->PrepareRender( secondaryCommandBuffer, viewMatrix, projMatrix, bufferIndex, instruction, item );
      }
    }
  }
}

void GraphicsAlgorithms::RecordInstruction(
  const RenderInstruction& instruction,
  bool renderToFbo,
  bool depthBufferAvailable,
  bool stencilBufferAvailable,
  const Rect<int32_t>& viewport,
  const Rect<int>& rootClippingRect,
  Uint16Pair sceneSize,
  BufferIndex bufferIndex)
{
  using namespace Graphics;

  // Create constant buffer with static uniforms: view matrix, projection matrix
  const Matrix* viewMatrix       = instruction.GetViewMatrix( bufferIndex );
  const Matrix* projectionMatrix = instruction.GetProjectionMatrix( bufferIndex );

  if(viewMatrix && projectionMatrix)
  {
    std::vector<const Graphics::CommandBuffer*> buffers;

    auto numberOfRenderLists = instruction.RenderListCount();
    for( auto i = 0u; i < numberOfRenderLists; ++i )
    {
      const RenderList* renderList = instruction.GetRenderList(i);
      if(renderList && !renderList->IsEmpty())
      {
        RecordRenderItemList(*renderList,
                             bufferIndex,
                             *viewMatrix,
                             *projectionMatrix,
                             instruction,
                             viewport,
                             rootClippingRect,
                             sceneSize,
                             depthBufferAvailable,
                             stencilBufferAvailable);

        // Execute command buffer
        auto* commandBuffer = renderList->GetCommandBufferPtr();
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
}

bool GraphicsAlgorithms::PrepareGraphicsPipeline(
  const RenderInstruction& instruction,
  const RenderList* renderList,
  RenderItem& item,
  BufferIndex bufferIndex)
{
  using namespace Dali::Graphics;

  // vertex input state
  VertexInputState vi{};

  auto *renderer = item.mRenderer;
  auto *geometry = renderer->GetGeometry();
  auto graphicsProgram = renderer->GetShader().GetGraphicsObject();

  if( !graphicsProgram )
  {
    return false;
  }
  auto& reflection = mGraphicsController.GetProgramReflection(*graphicsProgram);

  /**
   * Prepare vertex attribute buffer bindings
   */
  uint32_t bindingIndex{0u};

  for (auto &&vertexBuffer : geometry->GetVertexBuffers())
  {
    // update vertex buffer if necessary
    vertexBuffer->Update(mGraphicsController);

    auto attributeCountInForBuffer = vertexBuffer->GetAttributeCount();

    auto& vertexFormat = *vertexBuffer->GetFormat();
    // store buffer binding
    vi.bufferBindings.emplace_back( vertexFormat.size, VertexInputRate::PER_VERTEX );

    for( auto i = 0u; i < attributeCountInForBuffer; ++i )
    {
      // create attribute description
      vi.attributes.emplace_back( reflection.GetVertexAttributeLocation( vertexBuffer->GetAttributeName( i ) ),
                                  bindingIndex,
                                  vertexFormat.components[i].offset,
                                  GetPropertyVertexFormat( vertexFormat.components[i].type ) );
    }
    bindingIndex++;
  }

  ColorBlendState colorBlendState{};
  colorBlendState.SetBlendEnable(false);

  if( !item.mIsOpaque )
  {
    colorBlendState.SetBlendEnable(true);
    const auto& options = renderer->GetBlendingOptions();
    colorBlendState
      .SetSrcColorBlendFactor(ConvertBlendFactor(options.GetBlendSrcFactorRgb()))
      .SetSrcAlphaBlendFactor(ConvertBlendFactor(options.GetBlendSrcFactorAlpha()))
      .SetDstColorBlendFactor(ConvertBlendFactor(options.GetBlendDestFactorRgb()))
      .SetDstAlphaBlendFactor(ConvertBlendFactor(options.GetBlendDestFactorAlpha()))
      .SetColorBlendOp(ConvertBlendEquation(options.GetBlendEquationRgb()))
      .SetAlphaBlendOp(ConvertBlendEquation(options.GetBlendEquationAlpha()));
  }
  colorBlendState
    .SetColorComponentsWriteBits(0xff)
    .SetLogicOpEnable(false);


  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "PrepareGraphicsPipeline(renderer=%p), opaque:%s, blendMode:%d  => Blending:%s\n", item.mRenderer, item.mIsOpaque?"T":"F", renderer->GetBlendMode(), colorBlendState.blendEnable?"T":"F" );


  // set face culling
  auto cullMode { CullMode::NONE };
  switch( renderer->GetFaceCullingMode() )
  {
    case FaceCullingMode::BACK:
    {
      cullMode = CullMode::BACK;
      break;
    }
    case FaceCullingMode::FRONT:
    {
      cullMode = CullMode::FRONT;
      break;
    }
    case FaceCullingMode::FRONT_AND_BACK:
    {
      cullMode = CullMode::FRONT_AND_BACK;
      break;
    }
    case FaceCullingMode::NONE:
    {
      cullMode = CullMode::NONE;
    }
  }

  Graphics::ProgramState programState{};
  programState.SetProgram(*graphicsProgram);

  Graphics::InputAssemblyState inputAssembly{};
  inputAssembly
    .SetTopology(geometry->GetTopology())
    .SetPrimitiveRestartEnable(false);

  Graphics::RasterizationState rasterizationState{};
  rasterizationState
    .SetCullMode(cullMode)
    .SetPolygonMode(PolygonMode::FILL)
    .SetFrontFace(FrontFace::COUNTER_CLOCKWISE);

  Graphics::PipelineCreateInfo createInfo;
  createInfo
    .SetProgramState(&programState)
    .SetColorBlendState(&colorBlendState)
    .SetRasterizationState(&rasterizationState)
    .SetVertexInputState(&vi)
    .SetInputAssemblyState(&inputAssembly);

  // create pipeline
  renderer->SetPipeline(mGraphicsController.CreatePipeline( createInfo, nullptr ));

  return true;
}

void GraphicsAlgorithms::PrepareRendererPipelines(
  const RenderInstructionContainer& renderInstructions,
  BufferIndex bufferIndex )
{
  mUniformBlockAllocationCount = 0u;
  mUniformBlockAllocationBytes = 0u;
  mUniformBlockMaxSize = 0u;

  for( auto i = 0u; i < renderInstructions.Count( bufferIndex ); ++i )
  {
    const RenderInstruction &ri = renderInstructions.At(bufferIndex, i);

    for (auto renderListIndex = 0u; renderListIndex < ri.RenderListCount(); ++renderListIndex)
    {
      const auto *renderList = ri.GetRenderList(renderListIndex);
      for (auto renderItemIndex = 0u; renderItemIndex < renderList->Count(); ++renderItemIndex)
      {
        auto &item = renderList->GetItem(renderItemIndex);
        if( item.mRenderer )
        {
          PrepareGraphicsPipeline( ri, renderList, item, bufferIndex );
        }
      }
    }
  }
}

void GraphicsAlgorithms::ResetCommandBuffer()
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
}

void GraphicsAlgorithms::RenderScene(
  const SceneGraph::Scene* scene,
  BufferIndex bufferIndex,
  bool renderToFbo)
{
  Integration::DepthBufferAvailable  depthBufferAvailable;
  Integration::StencilBufferAvailable stencilBufferAvailable;
  scene->GetAvailableBuffers(depthBufferAvailable, stencilBufferAvailable);

  RenderInstructionContainer& renderInstructions = const_cast<Scene*>(scene)->GetRenderInstructions();

  PrepareRendererPipelines( renderInstructions, bufferIndex );
  /* New state of play: No depth/stencil flags in pipeline.

  // If state of depth/stencil has changed between frames then the pipelines must be
  // prepared again. Note, this stage does not recompile shader but collects necessary
  // data to compile pipelines at the further stage.
  if( mGraphicsController.EnableDepthStencilBuffer( usesDepth, usesStencil ) )
  {
    PrepareRendererPipelines( renderInstructions, usesDepth, usesStencil, bufferIndex );
  }*/

  auto instructionCount = renderInstructions.Count( bufferIndex );

  // Prepare uniform buffers
  if( !mUniformBufferManager )
  {
    mUniformBufferManager.reset( new UniformBufferManager( &mGraphicsController ) );
  }

  uint32_t cpuSize=0;
  uint32_t gpuSize=0;
  ComputeUniformBufferRequirements(scene, bufferIndex, cpuSize, gpuSize);

  mUniformBufferManager->SetCurrentSceneRenderInfo(scene, true);
  mUniformBufferManager->Rollback(scene, true);

  if( instructionCount > 0)
  {
    mUniformBufferManager->GetUniformBufferForScene(scene, renderToFbo, true)->ReSpecify(cpuSize);
    mUniformBufferManager->GetUniformBufferForScene(scene, renderToFbo, false)->ReSpecify(gpuSize);
  }

  auto clippingRect = Rect<int>();
  auto viewportRect = Rect<int>();
  std::vector<Graphics::RenderTarget*> targetsToPresent;
  Rect<int32_t> surfaceRect = scene->GetSurfaceRect();

  for(uint32_t i = 0; i < instructionCount; ++i)
  {
    RenderInstruction& instruction = renderInstructions.At(bufferIndex, i);

    if((renderToFbo && !instruction.mFrameBuffer) || (!renderToFbo && instruction.mFrameBuffer))
    {
      continue; // skip
    }

    Graphics::RenderTarget*           currentRenderTarget = nullptr;
    Graphics::RenderPass*             currentRenderPass   = nullptr;
    std::vector<Graphics::ClearValue> currentClearValues{};

    if(instruction.mFrameBuffer)
    {
      // Ensure graphics framebuffer is created, bind attachments and create render passes
      // Only happens once per framebuffer. If the create fails, e.g. no attachments yet,
      // then don't render to this framebuffer.
      if(!instruction.mFrameBuffer->GetGraphicsObject())
      {
        instruction.mFrameBuffer->PrepareFramebuffer();
        if(!instruction.mFrameBuffer->GetGraphicsObject())
        {
          continue;
        }
      }

      auto& clearValues = instruction.mFrameBuffer->GetGraphicsRenderPassClearValues();

      // Set the clear color for first color attachment
      if(instruction.mIsClearColorSet && !clearValues.empty())
      {
        clearValues[0].color = {
          instruction.mClearColor.r,
          instruction.mClearColor.g,
          instruction.mClearColor.b,
          instruction.mClearColor.a};
      }

      currentClearValues = clearValues;

      auto loadOp = instruction.mIsClearColorSet ? Graphics::AttachmentLoadOp::CLEAR : Graphics::AttachmentLoadOp::LOAD;

      // offscreen buffer
      currentRenderTarget = instruction.mFrameBuffer->GetGraphicsRenderTarget();
      currentRenderPass   = instruction.mFrameBuffer->GetGraphicsRenderPass(loadOp, Graphics::AttachmentStoreOp::STORE);
    }
    else // no framebuffer
    {
      // Copy from surface
      auto clearValues = scene->GetGraphicsRenderPassClearValues();

      if(instruction.mIsClearColorSet)
      {
        clearValues[0].color = {
          instruction.mClearColor.r,
          instruction.mClearColor.g,
          instruction.mClearColor.b,
          instruction.mClearColor.a};
      }

      currentClearValues = clearValues;

      // @todo SceneObject should already have the depth clear / stencil clear in the clearValues array.
      // if the window has a depth/stencil buffer.
      if((depthBufferAvailable == Integration::DepthBufferAvailable::TRUE ||
            stencilBufferAvailable == Integration::StencilBufferAvailable::TRUE) &&
          (currentClearValues.size() <= 1))
      {
        currentClearValues.emplace_back();
        currentClearValues.back().depthStencil.depth   = 0;
        currentClearValues.back().depthStencil.stencil = 0;
      }

      auto loadOp = instruction.mIsClearColorSet ? Graphics::AttachmentLoadOp::CLEAR : Graphics::AttachmentLoadOp::LOAD;

      currentRenderTarget = scene->GetSurfaceRenderTarget();
      currentRenderPass   = scene->GetGraphicsRenderPass(loadOp, Graphics::AttachmentStoreOp::STORE);
    }

    targetsToPresent.emplace_back(currentRenderTarget);

    if(!instruction.mIgnoreRenderToFbo && (instruction.mFrameBuffer != nullptr))
    {
      // Offscreen buffer rendering
      if(instruction.mIsViewportSet)
      {
        // For Viewport the lower-left corner is (0,0)
        const int32_t y = (instruction.mFrameBuffer->GetHeight() - instruction.mViewport.height) - instruction.mViewport.y;
        viewportRect.Set(instruction.mViewport.x, y, instruction.mViewport.width, instruction.mViewport.height);
      }
      else
      {
        viewportRect.Set(0, 0, instruction.mFrameBuffer->GetWidth(), instruction.mFrameBuffer->GetHeight());
      }
    }
    else // No Offscreen frame buffer rendering
    {
      // Check whether a viewport is specified, otherwise the full surface size is used
      if(instruction.mIsViewportSet)
      {
        // For Viewport the lower-left corner is (0,0)
        const int32_t y = (surfaceRect.height - instruction.mViewport.height) - instruction.mViewport.y;
        viewportRect.Set(instruction.mViewport.x, y, instruction.mViewport.width, instruction.mViewport.height);
      }
      else
      {
        viewportRect = surfaceRect;
      }
    }

    // Set surface orientation
    // @todo Inform graphics impl by another route.
    // was: mImpl->currentContext->SetSurfaceOrientation(surfaceOrientation);

    /*** Clear region of framebuffer or surface before drawing ***/
    bool clearFullFrameRect = (surfaceRect == viewportRect);
    if(instruction.mFrameBuffer != nullptr)
    {
      Viewport frameRect(0, 0, instruction.mFrameBuffer->GetWidth(), instruction.mFrameBuffer->GetHeight());
      clearFullFrameRect = (frameRect == viewportRect);
    }

    if(!clippingRect.IsEmpty())
    {
      if(!clippingRect.Intersect(viewportRect))
      {
        DALI_LOG_ERROR("Invalid clipping rect %d %d %d %d\n", clippingRect.x, clippingRect.y, clippingRect.width, clippingRect.height);
        clippingRect = Rect<int>();
      }
      clearFullFrameRect = false;
    }

    Graphics::Rect2D scissorArea{viewportRect.x, viewportRect.y, uint32_t(viewportRect.width), uint32_t(viewportRect.height)};
    if(instruction.mIsClearColorSet)
    {
      if(!clearFullFrameRect)
      {
        if(!clippingRect.IsEmpty())
        {
          scissorArea = {clippingRect.x, clippingRect.y, uint32_t(clippingRect.width), uint32_t(clippingRect.height)};
        }
      }
    }

    // Begin render pass
    mGraphicsCommandBuffer->BeginRenderPass(
      currentRenderPass,
      currentRenderTarget,
      scissorArea,
      currentClearValues);

    mGraphicsCommandBuffer->SetViewport({float(viewportRect.x),
                                         float(viewportRect.y),
                                         float(viewportRect.width),
                                         float(viewportRect.height)});

    RecordInstruction(instruction, renderToFbo,
                      depthBufferAvailable==Integration::DepthBufferAvailable::TRUE,
                      stencilBufferAvailable==Integration::StencilBufferAvailable::TRUE,
                      viewportRect,
                      clippingRect,
                      Uint16Pair(surfaceRect.width, surfaceRect.height),
                      bufferIndex);


    Graphics::SyncObject* syncObject{nullptr};

    // @todo Add render tracker back to instruction (and figure out how to sync!)
    //if(instruction.mRenderTracker && instruction.mFrameBuffer)
    //{
    //syncObject                 = instruction.mRenderTracker->CreateSyncObject(mImpl->graphicsController);
    //instruction.mRenderTracker = nullptr;
    //}
    mGraphicsCommandBuffer->EndRenderPass(syncObject);
  }

  mUniformBufferManager->Flush(nullptr, false);

  /// @todo REMOVE For testing only:
  /// Ensure that the main surface is presented, even if nothing is available to render.
  if(targetsToPresent.empty())
  {
    targetsToPresent.emplace_back(scene->GetSurfaceRenderTarget());
  }

  // Submit command buffers
  Graphics::SubmitInfo submitInfo;
  submitInfo.cmdBuffer.push_back(mGraphicsCommandBuffer.get());
  submitInfo.flags = 0 | Graphics::SubmitFlagBits::FLUSH;
  mGraphicsController.SubmitCommandBuffers(submitInfo);

  // Present render targets
  std::sort(targetsToPresent.begin(), targetsToPresent.end());
  Graphics::RenderTarget* rt = nullptr;
  for(auto& target : targetsToPresent)
  {
    if(target != rt)
    {
      mGraphicsController.PresentRenderTarget(target);
      rt = target;
    }
  }

  mCurrentFrameIndex++;
}

UniformBufferManager& GraphicsAlgorithms::GetUniformBufferManager()
{
  return *mUniformBufferManager;
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
