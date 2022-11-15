/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali/internal/update/manager/render-instruction-processor.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/common/matrix-utils.h>
#include <dali/internal/event/actors/layer-impl.h> // for the default sorting function
#include <dali/internal/render/common/render-instruction-container.h>
#include <dali/internal/render/common/render-instruction.h>
#include <dali/internal/render/common/render-item.h>
#include <dali/internal/render/common/render-tracker.h>
#include <dali/internal/render/renderers/render-renderer.h>
#include <dali/internal/render/shaders/render-shader.h>
#include <dali/internal/update/manager/sorted-layers.h>
#include <dali/internal/update/nodes/scene-graph-layer.h>
#include <dali/internal/update/render-tasks/scene-graph-render-task.h>
#include <dali/internal/update/rendering/scene-graph-texture-set.h>
#include <dali/public-api/actors/layer.h>

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gRenderListLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_RENDER_LISTS");
#endif
} // namespace

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
namespace
{
/**
 * Function which compares render items by shader/textureSet/geometry
 * @param[in] lhs Left hand side item
 * @param[in] rhs Right hand side item
 * @return True if left item is greater than right
 */
inline bool PartialCompareItems(const RenderInstructionProcessor::SortAttributes& lhs,
                                const RenderInstructionProcessor::SortAttributes& rhs)
{
  if(lhs.shader == rhs.shader)
  {
    if(lhs.textureSet == rhs.textureSet)
    {
      return lhs.geometry < rhs.geometry;
    }
    return lhs.textureSet < rhs.textureSet;
  }
  return lhs.shader < rhs.shader;
}

/**
 * Function which sorts render items by depth index then by instance
 * ptrs of shader/textureSet/geometry.
 * @param[in] lhs Left hand side item
 * @param[in] rhs Right hand side item
 * @return True if left item is greater than right
 */
bool CompareItems(const RenderInstructionProcessor::SortAttributes& lhs, const RenderInstructionProcessor::SortAttributes& rhs)
{
  // @todo Consider replacing all these sortAttributes with a single long int that
  // encapsulates the same data (e.g. the middle-order bits of the ptrs).
  if(lhs.renderItem->mDepthIndex == rhs.renderItem->mDepthIndex)
  {
    return PartialCompareItems(lhs, rhs);
  }
  return lhs.renderItem->mDepthIndex < rhs.renderItem->mDepthIndex;
}

/**
 * Function which sorts the render items by Z function, then
 * by instance ptrs of shader / geometry / material.
 * @param[in] lhs Left hand side item
 * @param[in] rhs Right hand side item
 * @return True if left item is greater than right
 */
bool CompareItems3D(const RenderInstructionProcessor::SortAttributes& lhs, const RenderInstructionProcessor::SortAttributes& rhs)
{
  const bool lhsIsOpaque = lhs.renderItem->mIsOpaque;
  if(lhsIsOpaque == rhs.renderItem->mIsOpaque)
  {
    if(lhsIsOpaque)
    {
      // If both RenderItems are opaque, sort using shader, then material then geometry.
      return PartialCompareItems(lhs, rhs);
    }
    else
    {
      // If both RenderItems are transparent, sort using Z, then shader, then material, then geometry.
      if(Equals(lhs.zValue, rhs.zValue))
      {
        return PartialCompareItems(lhs, rhs);
      }
      return lhs.zValue > rhs.zValue;
    }
  }
  else
  {
    return lhsIsOpaque;
  }
}

/**
 * Function which sorts render items by clipping hierarchy, then Z function and instance ptrs of shader / geometry / material.
 * @param[in] lhs Left hand side item
 * @param[in] rhs Right hand side item
 * @return True if left item is greater than right
 */
bool CompareItems3DWithClipping(const RenderInstructionProcessor::SortAttributes& lhs, const RenderInstructionProcessor::SortAttributes& rhs)
{
  // Items must be sorted in order of clipping first, otherwise incorrect clipping regions could be used.
  if(lhs.renderItem->mNode->mClippingSortModifier == rhs.renderItem->mNode->mClippingSortModifier)
  {
    return CompareItems3D(lhs, rhs);
  }

  return lhs.renderItem->mNode->mClippingSortModifier < rhs.renderItem->mNode->mClippingSortModifier;
}

/**
 * Set the update area of the node
 * @param[in] node The node of the renderer
 * @param[in] isLayer3d Whether we are processing a 3D layer or not
 * @param[in,out] nodeWorldMatrix The world matrix of the node
 * @param[in,out] nodeSize The size of the node
 * @param[in,out] nodeUpdateArea The update area of the node
 *
 * @return True if node use it's own UpdateAreaHint, or z transform occured. False if we use nodeUpdateArea equal with Vector4(0, 0, nodeSize.width, nodeSize.height).
 */
inline bool SetNodeUpdateArea(Node* node, bool isLayer3d, Matrix& nodeWorldMatrix, Vector3& nodeSize, Vector4& nodeUpdateArea)
{
  node->GetWorldMatrixAndSize(nodeWorldMatrix, nodeSize);

  if(node->GetUpdateAreaHint() == Vector4::ZERO)
  {
    // RenderItem::CalculateViewportSpaceAABB cannot cope with z transform
    // I don't use item.mModelMatrix.GetTransformComponents() for z transform, would be too slow
    if(!isLayer3d && nodeWorldMatrix.GetZAxis() == Vector3(0.0f, 0.0f, 1.0f))
    {
      nodeUpdateArea = Vector4(0.0f, 0.0f, nodeSize.width, nodeSize.height);
      return false;
    }
    // Keep nodeUpdateArea as Vector4::ZERO, and return true.
    return true;
  }
  else
  {
    nodeUpdateArea = node->GetUpdateAreaHint();
    return true;
  }
}

/**
 * Add a renderer to the list
 * @param updateBufferIndex to read the model matrix from
 * @param renderList to add the item to
 * @param renderable Node-Renderer pair
 * @param viewMatrix used to calculate modelview matrix for the item
 * @param camera The camera used to render
 * @param isLayer3d Whether we are processing a 3D layer or not
 * @param viewportSet Whether the viewport is set or not
 * @param viewport The viewport
 * @param cull Whether frustum culling is enabled or not
 */
inline void AddRendererToRenderList(BufferIndex               updateBufferIndex,
                                    RenderList&               renderList,
                                    Renderable&               renderable,
                                    const Matrix&             viewMatrix,
                                    const SceneGraph::Camera& camera,
                                    bool                      isLayer3d,
                                    bool                      viewportSet,
                                    const Viewport&           viewport,
                                    bool                      cull)
{
  bool    inside(true);
  Node*   node = renderable.mNode;
  Matrix  nodeWorldMatrix(false);
  Vector3 nodeSize;
  Vector4 nodeUpdateArea;
  bool    nodeUpdateAreaSet(false);
  bool    nodeUpdateAreaUseHint(false);
  Matrix  nodeModelViewMatrix(false);
  bool    nodeModelViewMatrixSet(false);

  // Don't cull items which have render callback
  bool hasRenderCallback = (renderable.mRenderer && renderable.mRenderer->GetRenderCallback());

  if(cull && renderable.mRenderer && (hasRenderCallback || (!renderable.mRenderer->GetShader().HintEnabled(Dali::Shader::Hint::MODIFIES_GEOMETRY) && node->GetClippingMode() == ClippingMode::DISABLED)))
  {
    const Vector4& boundingSphere = node->GetBoundingSphere();
    inside                        = (boundingSphere.w > Math::MACHINE_EPSILON_1000) &&
             (camera.CheckSphereInFrustum(updateBufferIndex, Vector3(boundingSphere), boundingSphere.w));

    if(inside && !isLayer3d && viewportSet)
    {
      nodeUpdateAreaUseHint = SetNodeUpdateArea(node, isLayer3d, nodeWorldMatrix, nodeSize, nodeUpdateArea);
      nodeUpdateAreaSet     = true;

      const Vector3& scale = node->GetWorldScale(updateBufferIndex);
      const Vector3& size  = Vector3(nodeUpdateArea.z, nodeUpdateArea.w, 1.0f) * scale;

      if(size.LengthSquared() > Math::MACHINE_EPSILON_1000)
      {
        MatrixUtils::Multiply(nodeModelViewMatrix, nodeWorldMatrix, viewMatrix);
        nodeModelViewMatrixSet = true;

        // Assume actors are at z=0, compute AABB in view space & test rect intersection
        // against z=0 plane boundaries for frustum. (NOT viewport). This should take into account
        // magnification due to FOV etc.
        ClippingBox boundingBox = RenderItem::CalculateTransformSpaceAABB(nodeModelViewMatrix, Vector3(nodeUpdateArea.x, nodeUpdateArea.y, 0.0f), Vector3(nodeUpdateArea.z, nodeUpdateArea.w, 0.0f));
        ClippingBox clippingBox(camera.mLeftClippingPlane, camera.mBottomClippingPlane, camera.mRightClippingPlane - camera.mLeftClippingPlane, fabsf(camera.mBottomClippingPlane - camera.mTopClippingPlane));
        inside = clippingBox.Intersects(boundingBox);
      }
    }
    /*
     * Note, the API Camera::CheckAABBInFrustum() can be used to test if a bounding box is (partially/fully) inside the view frustum.
     */
  }

  if(inside)
  {
    bool skipRender(false);
    bool isOpaque = true;
    if(!hasRenderCallback)
    {
      Renderer::OpacityType opacityType = renderable.mRenderer ? renderable.mRenderer->GetOpacityType(updateBufferIndex, *node) : Renderer::OPAQUE;

      // We can skip render when node is not clipping and transparent
      skipRender = (opacityType == Renderer::TRANSPARENT && node->GetClippingMode() == ClippingMode::DISABLED);

      isOpaque = (opacityType == Renderer::OPAQUE);
    }

    if(!skipRender)
    {
      // Get the next free RenderItem.
      RenderItem& item = renderList.GetNextFreeItem();

      // Get cached values
      auto& partialRenderingData = node->GetPartialRenderingData();

      auto& partialRenderingCacheInfo = node->GetPartialRenderingData().GetCurrentCacheInfo();

      partialRenderingCacheInfo.node       = node;
      partialRenderingCacheInfo.isOpaque   = isOpaque;
      partialRenderingCacheInfo.renderer   = renderable.mRenderer;
      partialRenderingCacheInfo.color      = node->GetWorldColor(updateBufferIndex);
      partialRenderingCacheInfo.depthIndex = node->GetDepthIndex();

      if(DALI_LIKELY(renderable.mRenderer))
      {
        partialRenderingCacheInfo.color.a *= renderable.mRenderer->GetOpacity(updateBufferIndex);
        partialRenderingCacheInfo.textureSet = renderable.mRenderer->GetTextureSet();
      }

      item.mNode     = node;
      item.mIsOpaque = isOpaque;
      item.mColor    = node->GetColor(updateBufferIndex);

      item.mDepthIndex = 0;
      if(!isLayer3d)
      {
        item.mDepthIndex = node->GetDepthIndex();
      }

      if(DALI_LIKELY(renderable.mRenderer))
      {
        item.mRenderer   = &renderable.mRenderer->GetRenderer();
        item.mTextureSet = renderable.mRenderer->GetTextureSet();
        item.mDepthIndex += renderable.mRenderer->GetDepthIndex();

        // Get whether collected map is up to date
        item.mIsUpdated |= renderable.mRenderer->UniformMapUpdated();
      }
      else
      {
        item.mRenderer = nullptr;
      }

      item.mIsUpdated |= isLayer3d;

      if(!nodeUpdateAreaSet)
      {
        nodeUpdateAreaUseHint = SetNodeUpdateArea(node, isLayer3d, nodeWorldMatrix, nodeSize, nodeUpdateArea);
      }

      item.mSize        = nodeSize;
      item.mUpdateArea  = nodeUpdateArea;
      item.mModelMatrix = nodeWorldMatrix;

      // Apply transform informations if node doesn't have update size hint and use VisualRenderer.
      if(!nodeUpdateAreaUseHint && renderable.mRenderer && renderable.mRenderer->GetVisualProperties())
      {
        Vector3 updateSize = renderable.mRenderer->CalculateVisualTransformedUpdateSize(updateBufferIndex, Vector3(item.mUpdateArea.z, item.mUpdateArea.w, 0.0f));
        item.mUpdateArea.z = updateSize.x;
        item.mUpdateArea.w = updateSize.y;
      }

      if(!nodeModelViewMatrixSet)
      {
        MatrixUtils::Multiply(nodeModelViewMatrix, nodeWorldMatrix, viewMatrix);
      }
      item.mModelViewMatrix = nodeModelViewMatrix;

      partialRenderingCacheInfo.matrix              = item.mModelViewMatrix;
      partialRenderingCacheInfo.size                = item.mSize;
      partialRenderingCacheInfo.updatedPositionSize = item.mUpdateArea;

      item.mIsUpdated = partialRenderingData.IsUpdated() || item.mIsUpdated;

      partialRenderingData.mRendered = true;

      partialRenderingData.SwapBuffers();
    }
    else
    {
      // Mark as not rendered
      auto& partialRenderingData     = node->GetPartialRenderingData();
      partialRenderingData.mRendered = false;
    }

    node->SetCulled(updateBufferIndex, false);
  }
  else
  {
    // Mark as not rendered
    auto& partialRenderingData     = node->GetPartialRenderingData();
    partialRenderingData.mRendered = false;

    node->SetCulled(updateBufferIndex, true);
  }
}

/**
 * Add all renderers to the list
 * @param updateBufferIndex to read the model matrix from
 * @param renderList to add the items to
 * @param renderers to render
 * NodeRendererContainer Node-Renderer pairs
 * @param viewMatrix used to calculate modelview matrix for the items
 * @param camera The camera used to render
 * @param isLayer3d Whether we are processing a 3D layer or not
 * @param cull Whether frustum culling is enabled or not
 */
inline void AddRenderersToRenderList(BufferIndex               updateBufferIndex,
                                     RenderList&               renderList,
                                     RenderableContainer&      renderers,
                                     const Matrix&             viewMatrix,
                                     const SceneGraph::Camera& camera,
                                     bool                      isLayer3d,
                                     bool                      viewportSet,
                                     const Viewport&           viewport,
                                     bool                      cull)
{
  DALI_LOG_INFO(gRenderListLogFilter, Debug::Verbose, "AddRenderersToRenderList()\n");

  for(auto&& renderer : renderers)
  {
    AddRendererToRenderList(updateBufferIndex,
                            renderList,
                            renderer,
                            viewMatrix,
                            camera,
                            isLayer3d,
                            viewportSet,
                            viewport,
                            cull);
  }
}

/**
 * Try to reuse cached RenderItems from the RenderList
 * This avoids recalculating the model view matrices in case this part of the scene was static
 * An example case is a toolbar layer that rarely changes or a popup on top of the rest of the stage
 * @param layer that is being processed
 * @param renderList that is cached from frame N-1
 * @param renderables list of renderables
 */
inline bool TryReuseCachedRenderers(Layer&               layer,
                                    RenderList&          renderList,
                                    RenderableContainer& renderables)
{
  bool     retValue        = false;
  uint32_t renderableCount = static_cast<uint32_t>(renderables.Size());
  // Check that the cached list originates from this layer and that the counts match
  if((renderList.GetSourceLayer() == &layer) &&
     (renderList.GetCachedItemCount() == renderableCount))
  {
    // Check that all the same renderers are there. This gives us additional security in avoiding rendering the wrong things.
    // Render list is sorted so at this stage renderers may be in different order.
    // Therefore we check a combined sum of all renderer addresses.
    size_t checkSumNew = 0;
    size_t checkSumOld = 0;
    for(uint32_t index = 0; index < renderableCount; ++index)
    {
      if(DALI_LIKELY(renderables[index].mRenderer))
      {
        const Render::Renderer& renderer = renderables[index].mRenderer->GetRenderer();
        checkSumNew += reinterpret_cast<std::size_t>(&renderer);
      }
      if(DALI_LIKELY(renderList.GetItem(index).mRenderer))
      {
        checkSumOld += reinterpret_cast<std::size_t>(&renderList.GetRenderer(index));
      }
    }
    if(checkSumNew == checkSumOld)
    {
      // tell list to reuse its existing items
      renderList.ReuseCachedItems();
      retValue = true;
    }
  }

  return retValue;
}

inline bool SetupRenderList(RenderableContainer& renderables,
                            Layer&               layer,
                            RenderInstruction&   instruction,
                            bool                 tryReuseRenderList,
                            RenderList**         renderList)
{
  *renderList = &(instruction.GetNextFreeRenderList(renderables.Size()));
  (*renderList)->SetClipping(layer.IsClipping(), layer.GetClippingBox());
  (*renderList)->SetSourceLayer(&layer);

  // Try to reuse cached RenderItems from last time around.
  return (tryReuseRenderList && TryReuseCachedRenderers(layer, **renderList, renderables));
}

} // Anonymous namespace.

RenderInstructionProcessor::RenderInstructionProcessor()
: mSortingHelper()
{
  // Set up a container of comparators for fast run-time selection.
  mSortComparitors.Reserve(3u);

  mSortComparitors.PushBack(CompareItems);
  mSortComparitors.PushBack(CompareItems3D);
  mSortComparitors.PushBack(CompareItems3DWithClipping);
}

RenderInstructionProcessor::~RenderInstructionProcessor() = default;

inline void RenderInstructionProcessor::SortRenderItems(BufferIndex bufferIndex, RenderList& renderList, Layer& layer, bool respectClippingOrder, bool isOrthographicCamera)
{
  const uint32_t renderableCount = static_cast<uint32_t>(renderList.Count());
  // Reserve space if needed.
  const uint32_t oldcapacity = static_cast<uint32_t>(mSortingHelper.size());
  if(oldcapacity < renderableCount)
  {
    mSortingHelper.reserve(renderableCount);
    // Add real objects (reserve does not construct objects).
    mSortingHelper.insert(mSortingHelper.begin() + oldcapacity,
                          (renderableCount - oldcapacity),
                          RenderInstructionProcessor::SortAttributes());
  }
  else
  {
    // Clear extra elements from helper, does not decrease capability.
    mSortingHelper.resize(renderableCount);
  }

  // Calculate the sorting value, once per item by calling the layers sort function.
  // Using an if and two for-loops rather than if inside for as its better for branch prediction.

  // List of zValue calculating functions.
  const Dali::Layer::SortFunctionType zValueFunctionFromVector3[] = {
    [](const Vector3& position) { return position.z; },
    [](const Vector3& position) { return position.LengthSquared(); },
    layer.GetSortFunction(),
  };

  // Determine whether we need to use zValue as Euclidean distance or translatoin's z value.
  // If layer is LAYER_UI or camera is OrthographicProjection mode, we don't need to calculate
  // renderItem's distance from camera.

  // Here we determine which zValue SortFunctionType (of the 3) to use.
  //   0 is position z value : Default LAYER_UI or Orthographic camera
  //   1 is distance square value : Default LAYER_3D and Perspective camera
  //   2 is user defined function.
  const int zValueFunctionIndex = layer.UsesDefaultSortFunction() ? ((layer.GetBehavior() == Dali::Layer::LAYER_UI || isOrthographicCamera) ? 0 : 1) : 2;

  for(uint32_t index = 0; index < renderableCount; ++index)
  {
    RenderItem& item = renderList.GetItem(index);

    if(DALI_LIKELY(item.mRenderer))
    {
      item.mRenderer->SetSortAttributes(mSortingHelper[index]);
    }

    // texture set
    mSortingHelper[index].textureSet = item.mTextureSet;

    mSortingHelper[index].zValue = zValueFunctionFromVector3[zValueFunctionIndex](item.mModelViewMatrix.GetTranslation3()) - static_cast<float>(item.mDepthIndex);

    // Keep the renderitem pointer in the helper so we can quickly reorder items after sort.
    mSortingHelper[index].renderItem = &item;
  }

  // Here we determine which comparitor (of the 3) to use.
  //   0 is LAYER_UI
  //   1 is LAYER_3D
  //   2 is LAYER_3D + Clipping
  const unsigned int comparitorIndex = layer.GetBehavior() == Dali::Layer::LAYER_3D ? respectClippingOrder ? 2u : 1u : 0u;

  std::stable_sort(mSortingHelper.begin(), mSortingHelper.end(), mSortComparitors[comparitorIndex]);

  // Reorder / re-populate the RenderItems in the RenderList to correct order based on the sortinghelper.
  DALI_LOG_INFO(gRenderListLogFilter, Debug::Verbose, "Sorted Transparent List:\n");
  RenderItemContainer::Iterator renderListIter = renderList.GetContainer().Begin();
  for(uint32_t index = 0; index < renderableCount; ++index, ++renderListIter)
  {
    *renderListIter = mSortingHelper[index].renderItem;
    DALI_LOG_INFO(gRenderListLogFilter, Debug::Verbose, "  sortedList[%d] = %p\n", index, mSortingHelper[index].renderItem->mRenderer);
  }
}

void RenderInstructionProcessor::Prepare(BufferIndex                 updateBufferIndex,
                                         SortedLayerPointers&        sortedLayers,
                                         RenderTask&                 renderTask,
                                         bool                        cull,
                                         bool                        hasClippingNodes,
                                         RenderInstructionContainer& instructions)
{
  // Retrieve the RenderInstruction buffer from the RenderInstructionContainer
  // then populate with instructions.
  RenderInstruction& instruction             = renderTask.PrepareRenderInstruction(updateBufferIndex);
  bool               viewMatrixHasNotChanged = !renderTask.ViewMatrixUpdated();
  bool               isRenderListAdded       = false;
  bool               isRootLayerDirty        = false;

  const Matrix&             viewMatrix           = renderTask.GetViewMatrix(updateBufferIndex);
  const SceneGraph::Camera& camera               = renderTask.GetCamera();
  const bool                isOrthographicCamera = camera.mProjectionMode == Dali::Camera::ProjectionMode::ORTHOGRAPHIC_PROJECTION;

  Viewport viewport;
  bool     viewportSet = renderTask.QueryViewport(updateBufferIndex, viewport);

  const SortedLayersIter endIter = sortedLayers.end();
  for(SortedLayersIter iter = sortedLayers.begin(); iter != endIter; ++iter)
  {
    Layer&      layer = **iter;
    const bool  tryReuseRenderList(viewMatrixHasNotChanged && layer.CanReuseRenderers(&camera));
    const bool  isLayer3D  = layer.GetBehavior() == Dali::Layer::LAYER_3D;
    RenderList* renderList = nullptr;

    if(layer.IsRoot() && (layer.GetDirtyFlags() != NodePropertyFlags::NOTHING))
    {
      // If root-layer & dirty, i.e. a property has changed or a child has been deleted, then we need to ensure we render once more
      isRootLayerDirty = true;
    }

    if(!layer.colorRenderables.Empty())
    {
      RenderableContainer& renderables = layer.colorRenderables;

      if(!SetupRenderList(renderables, layer, instruction, tryReuseRenderList, &renderList))
      {
        renderList->SetHasColorRenderItems(true);
        AddRenderersToRenderList(updateBufferIndex,
                                 *renderList,
                                 renderables,
                                 viewMatrix,
                                 camera,
                                 isLayer3D,
                                 viewportSet,
                                 viewport,
                                 cull);

        // We only use the clipping version of the sort comparitor if any clipping nodes exist within the RenderList.
        SortRenderItems(updateBufferIndex, *renderList, layer, hasClippingNodes, isOrthographicCamera);
      }

      isRenderListAdded = true;
    }

    if(!layer.overlayRenderables.Empty())
    {
      RenderableContainer& renderables = layer.overlayRenderables;

      if(!SetupRenderList(renderables, layer, instruction, tryReuseRenderList, &renderList))
      {
        renderList->SetHasColorRenderItems(false);
        AddRenderersToRenderList(updateBufferIndex,
                                 *renderList,
                                 renderables,
                                 viewMatrix,
                                 camera,
                                 isLayer3D,
                                 viewportSet,
                                 viewport,
                                 cull);

        // Clipping hierarchy is irrelevant when sorting overlay items, so we specify using the non-clipping version of the sort comparitor.
        SortRenderItems(updateBufferIndex, *renderList, layer, false, isOrthographicCamera);
      }

      isRenderListAdded = true;
    }
  }

  // Inform the render instruction that all renderers have been added and this frame is complete.
  instruction.UpdateCompleted();

  if(isRenderListAdded || instruction.mIsClearColorSet || isRootLayerDirty)
  {
    instructions.PushBack(updateBufferIndex, &instruction);
  }
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
