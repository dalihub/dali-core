/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/internal/update/nodes/partial-rendering-data.h>
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
  if(lhs.depthIndex == rhs.depthIndex)
  {
    return PartialCompareItems(lhs, rhs);
  }
  return lhs.depthIndex < rhs.depthIndex;
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
      if(lhs.renderItem->mDepthIndex != rhs.renderItem->mDepthIndex)
      {
        return lhs.renderItem->mDepthIndex < rhs.renderItem->mDepthIndex;
      }

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
 * Add a renderer to the list
 * @param updateBufferIndex to read the model matrix from
 * @param renderPass render pass for this render instruction
 * @param renderList to add the item to
 * @param renderable Node-Renderer pair
 * @param viewMatrix used to calculate modelview matrix for the item
 * @param viewMatrixChanged Whether view matrix chagned for this time or not.
 * @param camera The camera used to render
 * @param isLayer3d Whether we are processing a 3D layer or not
 * @param viewportSet Whether the viewport is set or not
 * @param viewport The viewport
 * @param cullingEnabled Whether frustum culling is enabled or not
 * @param stopperNode Marker node that stops rendering(must be rendered)
 */
inline void AddRendererToRenderList(BufferIndex               updateBufferIndex,
                                    uint32_t                  renderPass,
                                    RenderList&               renderList,
                                    Renderable&               renderable,
                                    const Matrix&             viewMatrix,
                                    bool                      viewMatrixChanged,
                                    const SceneGraph::Camera& camera,
                                    bool                      isLayer3d,
                                    bool                      viewportSet,
                                    const Viewport&           viewport,
                                    bool                      cullingEnabled,
                                    Node*                     stopperNode)
{
  bool  inside(true);
  Node* node = renderable.mNode;

  bool nodePartialRenderingDataUpdateChecked(false);

  Matrix nodeModelViewMatrix(false);
  bool   nodeModelViewMatrixSet(false);

  auto& nodePartialRenderingData = node->GetPartialRenderingData();

  const bool rendererExist(renderable.mRenderer && DALI_LIKELY(renderable.mRenderer->IsRenderable()));

  // Don't cull items which have render callback
  bool hasRenderCallback = (rendererExist && renderable.mRenderer->GetRenderCallback());

  auto requiredInsideCheck = [&]()
  {
    if(node != stopperNode &&
       cullingEnabled &&
       !hasRenderCallback &&
       node->GetClippingMode() == ClippingMode::DISABLED &&
       rendererExist)
    {
      const auto& shaderData = renderable.mRenderer->GetShader().GetShaderData(renderPass);
      return (shaderData && !shaderData->HintEnabled(Dali::Shader::Hint::MODIFIES_GEOMETRY));
    }
    return false;
  };

  if(requiredInsideCheck())
  {
    const Vector4& boundingSphere = node->GetBoundingSphere();

    inside = (boundingSphere.w > Math::MACHINE_EPSILON_1000) &&
             (camera.CheckSphereInFrustum(updateBufferIndex, Vector3(boundingSphere), boundingSphere.w));

    if(inside && !isLayer3d && viewportSet)
    {
      node->UpdatePartialRenderingData(updateBufferIndex, isLayer3d);

      const Vector4& nodeUpdateArea = nodePartialRenderingData.mNodeInfomations.updatedPositionSize;
      const Vector2& nodeScaleXY    = nodePartialRenderingData.mNodeInfomations.modelMatrix.GetScaleXY();

      nodePartialRenderingDataUpdateChecked = true;

      const Vector2& sizeXY = Vector2(nodeUpdateArea.z * nodeScaleXY.x, nodeUpdateArea.w * nodeScaleXY.y);

      if(sizeXY.LengthSquared() > Math::MACHINE_EPSILON_1000)
      {
        MatrixUtils::MultiplyTransformMatrix(nodeModelViewMatrix, nodePartialRenderingData.mNodeInfomations.modelMatrix, viewMatrix);
        nodeModelViewMatrixSet = true;

        // Assume actors are at z=0, compute AABB in view space & test rect intersection
        // against z=0 plane boundaries for frustum. (NOT viewport). This should take into account
        // magnification due to FOV etc.

        // TODO : Below logic might need to refactor it.
        // If camera is Perspective, we need to calculate clipping box by FoV. Currently, we just believe default camera setup OrthographicSize well.
        //  - If then, It must use math calculate like tan(fov) internally. So, we might need calculate it only one times, and cache.
        ClippingBox boundingBox = RenderItem::CalculateTransformSpaceAABB(nodeModelViewMatrix, Vector3(nodeUpdateArea.x, nodeUpdateArea.y, 0.0f), Vector3(nodeUpdateArea.z, nodeUpdateArea.w, 0.0f));
        ClippingBox clippingBox = camera.GetOrthographicClippingBox(updateBufferIndex);

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
      const bool isVisualRendererUnder3D = (isLayer3d && !!(renderable.mRenderer && renderable.mRenderer->GetVisualProperties()));

      const Renderer::OpacityType opacityType = rendererExist ? (isVisualRendererUnder3D ? Renderer::TRANSLUCENT : renderable.mRenderer->GetOpacityType(updateBufferIndex, renderPass, *node)) : Renderer::OPAQUE;

      // We can skip render when node is not clipping and transparent
      // We must not skip when node is a stopper
      skipRender = (opacityType == Renderer::TRANSPARENT &&
                    node->GetClippingMode() == ClippingMode::DISABLED &&
                    node != stopperNode);

      isOpaque = (opacityType == Renderer::OPAQUE);
    }

    if(!skipRender)
    {
      // Get the next free RenderItem.
      RenderItem& item = renderList.GetNextFreeItem();

      item.mNode       = static_cast<const Node*>(node);
      item.mIsOpaque   = isOpaque;
      item.mDepthIndex = isLayer3d ? 0 : node->GetDepthIndex();

      if(DALI_LIKELY(rendererExist))
      {
        item.mRenderer   = renderable.mRenderer->GetRenderer();
        item.mTextureSet = renderable.mRenderer->GetTextureSet();
        item.mDepthIndex += renderable.mRenderer->GetDepthIndex();
      }
      else
      {
        item.mRenderer = Render::RendererKey{};
      }

      item.mIsUpdated = (viewMatrixChanged || isLayer3d);

      if(!nodePartialRenderingDataUpdateChecked)
      {
        node->UpdatePartialRenderingData(updateBufferIndex, isLayer3d);
      }

      if(!nodeModelViewMatrixSet)
      {
        MatrixUtils::MultiplyTransformMatrix(nodeModelViewMatrix, nodePartialRenderingData.mNodeInfomations.modelMatrix, viewMatrix);
      }

      item.mModelViewMatrix = std::move(nodeModelViewMatrix);

      item.mIsUpdated = item.mIsUpdated || nodePartialRenderingData.mUpdated;
    }

    node->SetCulled(updateBufferIndex, false);
  }
  else
  {
    node->SetCulled(updateBufferIndex, true);
  }
}

/**
 * Add all renderers to the list
 * @param updateBufferIndex to read the model matrix from
 * @param renderPass render pass for this render instruction
 * @param renderList to add the items to
 * @param renderers to render NodeRendererContainer Node-Renderer pairs
 * @param viewMatrix used to calculate modelview matrix for the items
 * @param viewMatrixChanged Whether view matrix chagned for this time or not.
 * @param camera The camera used to render
 * @param isLayer3d Whether we are processing a 3D layer or not
 * @param viewportSet Whether the viewport is set or not
 * @param viewport The viewport
 * @param cullingEnabled Whether frustum culling is enabled or not
 * @param stopperNode Marker node that stops rendering(must be rendered)
 */
inline void AddRenderersToRenderList(BufferIndex               updateBufferIndex,
                                     uint32_t                  renderPass,
                                     RenderList&               renderList,
                                     RenderableContainer&      renderers,
                                     const Matrix&             viewMatrix,
                                     bool                      viewMatrixChanged,
                                     const SceneGraph::Camera& camera,
                                     bool                      isLayer3d,
                                     bool                      viewportSet,
                                     const Viewport&           viewport,
                                     bool                      cullingEnabled,
                                     Node*                     stopperNode)
{
  DALI_LOG_INFO(gRenderListLogFilter, Debug::Verbose, "AddRenderersToRenderList()\n");

  for(auto&& renderer : renderers)
  {
    AddRendererToRenderList(updateBufferIndex,
                            renderPass,
                            renderList,
                            renderer,
                            viewMatrix,
                            viewMatrixChanged,
                            camera,
                            isLayer3d,
                            viewportSet,
                            viewport,
                            cullingEnabled,
                            stopperNode);
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
    //@todo just use keys, don't deref.
    for(uint32_t index = 0; index < renderableCount; ++index)
    {
      if(DALI_LIKELY(renderables[index].mRenderer))
      {
        Render::RendererKey renderer = renderables[index].mRenderer->GetRenderer();
        checkSumNew += renderer.Value();
      }
      if(DALI_LIKELY(renderList.GetItem(index).mRenderer))
      {
        checkSumOld += renderList.GetItem(index).mRenderer.Value();
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
    [](const Vector3& position)
    { return position.z; },
    [](const Vector3& position)
    { return position.LengthSquared(); },
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

  // Here we determine which comparitor (of the 3) to use.
  //   0 is LAYER_UI
  //   1 is LAYER_3D
  //   2 is LAYER_3D + Clipping
  const int comparitorIndex = layer.GetBehavior() == Dali::Layer::LAYER_3D ? respectClippingOrder ? 2u : 1u : 0u;

  bool needToSort = false;

  for(uint32_t index = 0; index < renderableCount; ++index)
  {
    RenderItemKey itemKey = renderList.GetItemKey(index);
    RenderItem&   item    = *itemKey.Get();

    if(DALI_LIKELY(item.mRenderer))
    {
      item.mRenderer->SetSortAttributes(mSortingHelper[index]);
    }

    // texture set
    mSortingHelper[index].textureSet = item.mTextureSet;

    if(comparitorIndex == 0)
    {
      // If we are under LAYER_UI, We don't need to get zValue and renderer sort attributes.
      // Since all render items well-sorted by draw order normally.
      mSortingHelper[index].depthIndex = item.mDepthIndex;
    }
    else
    {
      mSortingHelper[index].zValue = zValueFunctionFromVector3[zValueFunctionIndex](item.mModelViewMatrix.GetTranslation3()) - static_cast<float>(item.mDepthIndex);
    }

    // Keep the renderitem pointer in the helper so we can quickly reorder items after sort.
    mSortingHelper[index].renderItem = itemKey;

    if(!needToSort && index > 0u)
    {
      // Check if we need to sort the list.
      // We only need to sort if the depth index is not the same as the previous item.
      // This is a fast way of checking if we need to sort.
      if(mSortComparitors[comparitorIndex](mSortingHelper[index], mSortingHelper[index - 1u]))
      {
        needToSort = true;
      }
    }
  }

  // If we don't need to sort, we can skip the sort.
  if(needToSort)
  {
    std::stable_sort(mSortingHelper.begin(), mSortingHelper.end(), mSortComparitors[comparitorIndex]);

    // Reorder / re-populate the RenderItems in the RenderList to correct order based on the sortinghelper.
    DALI_LOG_INFO(gRenderListLogFilter, Debug::Verbose, "Sorted Transparent List:\n");
    RenderItemContainer::Iterator renderListIter = renderList.GetContainer().Begin();
    for(uint32_t index = 0; index < renderableCount; ++index, ++renderListIter)
    {
      *renderListIter = mSortingHelper[index].renderItem;
      DALI_LOG_INFO(gRenderListLogFilter, Debug::Verbose, "  sortedList[%d] = node : %x renderer : %x\n", index, mSortingHelper[index].renderItem->mNode, mSortingHelper[index].renderItem->mRenderer.Get());
    }
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
  const bool                isOrthographicCamera = camera.mProjectionMode[0] == Dali::Camera::ProjectionMode::ORTHOGRAPHIC_PROJECTION;

  Viewport viewport;
  bool     viewportSet = renderTask.QueryViewport(updateBufferIndex, viewport);

  Node* stopperNode = renderTask.GetStopperNode();

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
                                 instruction.mRenderPassTag,
                                 *renderList,
                                 renderables,
                                 viewMatrix,
                                 !viewMatrixHasNotChanged,
                                 camera,
                                 isLayer3D,
                                 viewportSet,
                                 viewport,
                                 cull,
                                 stopperNode);

        // We only use the clipping version of the sort comparitor if any clipping nodes exist within the RenderList.
        SortRenderItems(updateBufferIndex, *renderList, layer, hasClippingNodes, isOrthographicCamera);
      }
      else
      {
        renderList->SetHasColorRenderItems(true);
      }

      isRenderListAdded = true;

      if(stopperNode && renderList->RenderUntil(stopperNode))
      {
        break;
      }
    }

    if(!layer.overlayRenderables.Empty())
    {
      RenderableContainer& renderables = layer.overlayRenderables;

      if(!SetupRenderList(renderables, layer, instruction, tryReuseRenderList, &renderList))
      {
        renderList->SetHasColorRenderItems(false);
        AddRenderersToRenderList(updateBufferIndex,
                                 instruction.mRenderPassTag,
                                 *renderList,
                                 renderables,
                                 viewMatrix,
                                 !viewMatrixHasNotChanged,
                                 camera,
                                 isLayer3D,
                                 viewportSet,
                                 viewport,
                                 cull,
                                 stopperNode);

        // Clipping hierarchy is irrelevant when sorting overlay items, so we specify using the non-clipping version of the sort comparitor.
        SortRenderItems(updateBufferIndex, *renderList, layer, false, isOrthographicCamera);
      }
      else
      {
        renderList->SetHasColorRenderItems(false);
      }

      isRenderListAdded = true;

      if(stopperNode && renderList->RenderUntil(stopperNode))
      {
        break;
      }
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
