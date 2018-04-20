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

// CLASS HEADER
#include <dali/internal/update/manager/render-instruction-processor.h>

// INTERNAL INCLUDES
#include <dali/public-api/actors/layer.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/event/actors/layer-impl.h> // for the default sorting function
#include <dali/internal/update/manager/sorted-layers.h>
#include <dali/internal/update/render-tasks/scene-graph-render-task.h>
#include <dali/internal/update/rendering/scene-graph-texture-set.h>
#include <dali/internal/render/common/render-item.h>
#include <dali/internal/render/common/render-tracker.h>
#include <dali/internal/render/common/render-instruction.h>
#include <dali/internal/render/common/render-instruction-container.h>
#include <dali/internal/render/shaders/scene-graph-shader.h>
#include <dali/internal/render/renderers/render-renderer.h>
#include <dali/internal/render/renderers/render-property-buffer.h>
#include <dali/internal/update/nodes/scene-graph-layer.h>

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gRenderListLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_RENDER_LISTS");
#endif
}

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
inline bool PartialCompareItems( const RenderInstructionProcessor::SortAttributes& lhs,
                                 const RenderInstructionProcessor::SortAttributes& rhs )
{
  if( lhs.shader == rhs.shader )
  {
    if( lhs.textureSet == rhs.textureSet )
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
bool CompareItems( const RenderInstructionProcessor::SortAttributes& lhs, const RenderInstructionProcessor::SortAttributes& rhs )
{
  // @todo Consider replacing all these sortAttributes with a single long int that
  // encapsulates the same data (e.g. the middle-order bits of the ptrs).
  if( lhs.renderItem->mDepthIndex == rhs.renderItem->mDepthIndex )
  {
    return PartialCompareItems( lhs, rhs );
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
bool CompareItems3D( const RenderInstructionProcessor::SortAttributes& lhs, const RenderInstructionProcessor::SortAttributes& rhs )
{
  const bool lhsIsOpaque = lhs.renderItem->mIsOpaque;
  if( lhsIsOpaque == rhs.renderItem->mIsOpaque )
  {
    if( lhsIsOpaque )
    {
      // If both RenderItems are opaque, sort using shader, then material then geometry.
      return PartialCompareItems( lhs, rhs );
    }
    else
    {
      // If both RenderItems are transparent, sort using Z, then shader, then material, then geometry.
      if( Equals( lhs.zValue, rhs.zValue ) )
      {
        return PartialCompareItems( lhs, rhs );
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
bool CompareItems3DWithClipping( const RenderInstructionProcessor::SortAttributes& lhs, const RenderInstructionProcessor::SortAttributes& rhs )
{
  // Items must be sorted in order of clipping first, otherwise incorrect clipping regions could be used.
  if( lhs.renderItem->mNode->mClippingSortModifier == rhs.renderItem->mNode->mClippingSortModifier )
  {
    return CompareItems3D( lhs, rhs );
  }

  return lhs.renderItem->mNode->mClippingSortModifier < rhs.renderItem->mNode->mClippingSortModifier;
}

/**
 * Add a renderer to the list
 * @param updateBufferIndex to read the model matrix from
 * @param renderList to add the item to
 * @param renderable Node-Renderer pair
 * @param viewMatrix used to calculate modelview matrix for the item
 * @param camera The camera used to render
 * @param isLayer3d Whether we are processing a 3D layer or not
 * @param cull Whether frustum culling is enabled or not
 */
inline void AddRendererToRenderList( BufferIndex updateBufferIndex,
                                     RenderList& renderList,
                                     Renderable& renderable,
                                     const Matrix& viewMatrix,
                                     SceneGraph::Camera& camera,
                                     bool isLayer3d,
                                     bool cull )
{
  bool inside( true );
  const Node* node = renderable.mNode;

  if( cull && renderable.mRenderer && !renderable.mRenderer->GetShader().HintEnabled( Dali::Shader::Hint::MODIFIES_GEOMETRY ) )
  {
    const Vector4& boundingSphere = node->GetBoundingSphere();
    inside = ( boundingSphere.w > Math::MACHINE_EPSILON_1000 ) &&
             ( camera.CheckSphereInFrustum( updateBufferIndex, Vector3( boundingSphere ), boundingSphere.w ) );
  }

  if( inside )
  {
    Renderer::OpacityType opacityType = renderable.mRenderer ? renderable.mRenderer->GetOpacityType( updateBufferIndex, *renderable.mNode ) : Renderer::OPAQUE;
    if( opacityType != Renderer::TRANSPARENT || node->GetClippingMode() == ClippingMode::CLIP_CHILDREN )
    {
      // Get the next free RenderItem.
      RenderItem& item = renderList.GetNextFreeItem();

      item.mNode = renderable.mNode;
      item.mIsOpaque = ( opacityType == Renderer::OPAQUE );
      item.mDepthIndex = 0;

      if(!isLayer3d)
      {
        item.mDepthIndex = renderable.mNode->GetDepthIndex();
      }

      if( DALI_LIKELY( renderable.mRenderer ) )
      {
        item.mRenderer =   &renderable.mRenderer->GetRenderer();
        item.mTextureSet =  renderable.mRenderer->GetTextures();
        item.mDepthIndex += renderable.mRenderer->GetDepthIndex();
      }
      else
      {
        item.mRenderer = nullptr;
      }

      // Save ModelView matrix onto the item.
      node->GetWorldMatrixAndSize( item.mModelMatrix, item.mSize );

      Matrix::Multiply( item.mModelViewMatrix, item.mModelMatrix, viewMatrix );
    }
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
inline void AddRenderersToRenderList( BufferIndex updateBufferIndex,
                                      RenderList& renderList,
                                      RenderableContainer& renderers,
                                      const Matrix& viewMatrix,
                                      SceneGraph::Camera& camera,
                                      bool isLayer3d,
                                      bool cull )
{
  DALI_LOG_INFO( gRenderListLogFilter, Debug::Verbose, "AddRenderersToRenderList()\n");

  unsigned int rendererCount( renderers.Size() );
  for( unsigned int i(0); i < rendererCount; ++i )
  {
    AddRendererToRenderList( updateBufferIndex,
                             renderList,
                             renderers[i],
                             viewMatrix,
                             camera,
                             isLayer3d,
                             cull );
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
inline bool TryReuseCachedRenderers( Layer& layer,
                                     RenderList& renderList,
                                     RenderableContainer& renderables )
{
  bool retValue = false;
  size_t renderableCount = renderables.Size();
  // Check that the cached list originates from this layer and that the counts match
  if( ( renderList.GetSourceLayer() == &layer )&&
      ( renderList.GetCachedItemCount() == renderableCount ) )
  {
    // Check that all the same renderers are there. This gives us additional security in avoiding rendering the wrong things.
    // Render list is sorted so at this stage renderers may be in different order.
    // Therefore we check a combined sum of all renderer addresses.
    size_t checkSumNew = 0;
    size_t checkSumOld = 0;
    for( size_t index = 0; index < renderableCount; ++index )
    {
      const Render::Renderer& renderer = renderables[index].mRenderer->GetRenderer();
      checkSumNew += size_t( &renderer );
      checkSumOld += size_t( &renderList.GetRenderer( index ) );
    }
    if( checkSumNew == checkSumOld )
    {
      // tell list to reuse its existing items
      renderList.ReuseCachedItems();
      retValue = true;
    }
  }
  return retValue;
}

inline bool SetupRenderList( RenderableContainer& renderables,
                             Layer& layer,
                             RenderInstruction& instruction,
                             bool tryReuseRenderList,
                             RenderList** renderList )
{
  *renderList = &( instruction.GetNextFreeRenderList( renderables.Size() ) );
  ( *renderList )->SetClipping( layer.IsClipping(), layer.GetClippingBox() );
  ( *renderList )->SetSourceLayer( &layer );

  // Try to reuse cached RenderItems from last time around.
  return ( tryReuseRenderList && TryReuseCachedRenderers( layer, **renderList, renderables ) );
}

} // Anonymous namespace.


RenderInstructionProcessor::RenderInstructionProcessor()
: mSortingHelper()
{
  // Set up a container of comparators for fast run-time selection.
  mSortComparitors.Reserve( 3u );

  mSortComparitors.PushBack( CompareItems );
  mSortComparitors.PushBack( CompareItems3D );
  mSortComparitors.PushBack( CompareItems3DWithClipping );
}

RenderInstructionProcessor::~RenderInstructionProcessor()
{
}

inline void RenderInstructionProcessor::SortRenderItems( BufferIndex bufferIndex, RenderList& renderList, Layer& layer, bool respectClippingOrder )
{
  const size_t renderableCount = renderList.Count();
  // Reserve space if needed.
  const unsigned int oldcapacity = mSortingHelper.size();
  if( oldcapacity < renderableCount )
  {
    mSortingHelper.reserve( renderableCount );
    // Add real objects (reserve does not construct objects).
    mSortingHelper.insert( mSortingHelper.begin() + oldcapacity,
                          (renderableCount - oldcapacity),
                          RenderInstructionProcessor::SortAttributes() );
  }
  else
  {
    // Clear extra elements from helper, does not decrease capability.
    mSortingHelper.resize( renderableCount );
  }

  // Calculate the sorting value, once per item by calling the layers sort function.
  // Using an if and two for-loops rather than if inside for as its better for branch prediction.
  if( layer.UsesDefaultSortFunction() )
  {
    for( size_t index = 0; index < renderableCount; ++index )
    {
      RenderItem& item = renderList.GetItem( index );

      if( item.mRenderer )
      {
        item.mRenderer->SetSortAttributes( bufferIndex, mSortingHelper[ index ] );
      }

      // texture set
      mSortingHelper[ index ].textureSet = item.mTextureSet;

      // The default sorting function should get inlined here.
      mSortingHelper[ index ].zValue = Internal::Layer::ZValue( item.mModelViewMatrix.GetTranslation3() ) - item.mDepthIndex;

      // Keep the renderitem pointer in the helper so we can quickly reorder items after sort.
      mSortingHelper[ index ].renderItem = &item;
    }
  }
  else
  {
    const Dali::Layer::SortFunctionType sortFunction = layer.GetSortFunction();
    for( size_t index = 0; index < renderableCount; ++index )
    {
      RenderItem& item = renderList.GetItem( index );

      item.mRenderer->SetSortAttributes( bufferIndex, mSortingHelper[ index ] );

      // texture set
      mSortingHelper[ index ].textureSet = item.mTextureSet;


      mSortingHelper[ index ].zValue = (*sortFunction)( item.mModelViewMatrix.GetTranslation3() ) - item.mDepthIndex;

      // Keep the RenderItem pointer in the helper so we can quickly reorder items after sort.
      mSortingHelper[ index ].renderItem = &item;
    }
  }

  // Here we determine which comparitor (of the 3) to use.
  //   0 is LAYER_UI
  //   1 is LAYER_3D
  //   2 is LAYER_3D + Clipping
  const unsigned int comparitorIndex = layer.GetBehavior() == Dali::Layer::LAYER_3D ? respectClippingOrder ? 2u : 1u : 0u;

  std::stable_sort( mSortingHelper.begin(), mSortingHelper.end(), mSortComparitors[ comparitorIndex ] );

  // Reorder / re-populate the RenderItems in the RenderList to correct order based on the sortinghelper.
  DALI_LOG_INFO( gRenderListLogFilter, Debug::Verbose, "Sorted Transparent List:\n");
  RenderItemContainer::Iterator renderListIter = renderList.GetContainer().Begin();
  for( unsigned int index = 0; index < renderableCount; ++index, ++renderListIter )
  {
    *renderListIter = mSortingHelper[ index ].renderItem;
    DALI_LOG_INFO( gRenderListLogFilter, Debug::Verbose, "  sortedList[%d] = %p\n", index, mSortingHelper[ index ].renderItem->mRenderer);
  }
}

void RenderInstructionProcessor::Prepare( BufferIndex updateBufferIndex,
                                          SortedLayerPointers& sortedLayers,
                                          RenderTask& renderTask,
                                          bool cull,
                                          bool hasClippingNodes,
                                          RenderInstructionContainer& instructions )
{
  // Retrieve the RenderInstruction buffer from the RenderInstructionContainer
  // then populate with instructions.
  RenderInstruction& instruction = instructions.GetNextInstruction( updateBufferIndex );
  renderTask.PrepareRenderInstruction( instruction, updateBufferIndex );
  bool viewMatrixHasNotChanged = !renderTask.ViewMatrixUpdated();
  bool isRenderListAdded = false;

  const Matrix& viewMatrix = renderTask.GetViewMatrix( updateBufferIndex );
  SceneGraph::Camera& camera = renderTask.GetCamera();

  const SortedLayersIter endIter = sortedLayers.end();
  for( SortedLayersIter iter = sortedLayers.begin(); iter != endIter; ++iter )
  {
    Layer& layer = **iter;
    const bool tryReuseRenderList( viewMatrixHasNotChanged && layer.CanReuseRenderers( &renderTask.GetCamera() ) );
    const bool isLayer3D = layer.GetBehavior() == Dali::Layer::LAYER_3D;
    RenderList* renderList = NULL;

    if( !layer.colorRenderables.Empty() )
    {
      RenderableContainer& renderables = layer.colorRenderables;

      if( !SetupRenderList( renderables, layer, instruction, tryReuseRenderList, &renderList ) )
      {
        renderList->SetHasColorRenderItems( true );
        AddRenderersToRenderList( updateBufferIndex,
                                  *renderList,
                                  renderables,
                                  viewMatrix,
                                  camera,
                                  isLayer3D,
                                  cull );

        // We only use the clipping version of the sort comparitor if any clipping nodes exist within the RenderList.
        SortRenderItems( updateBufferIndex, *renderList, layer, hasClippingNodes );
      }

      isRenderListAdded = true;
    }

    if( !layer.overlayRenderables.Empty() )
    {
      RenderableContainer& renderables = layer.overlayRenderables;

      if( !SetupRenderList( renderables, layer, instruction, tryReuseRenderList, &renderList ) )
      {
        renderList->SetHasColorRenderItems( false );
        AddRenderersToRenderList( updateBufferIndex,
                                  *renderList,
                                  renderables,
                                  viewMatrix,
                                  camera,
                                  isLayer3D,
                                  cull );

        // Clipping hierarchy is irrelevant when sorting overlay items, so we specify using the non-clipping version of the sort comparitor.
        SortRenderItems( updateBufferIndex, *renderList, layer, false );
      }

      isRenderListAdded = true;
    }
  }

  // Inform the render instruction that all renderers have been added and this frame is complete.
  instruction.UpdateCompleted();

  if( !isRenderListAdded && !instruction.mIsClearColorSet )
  {
    instructions.DiscardCurrentInstruction( updateBufferIndex );
  }
}

} // SceneGraph

} // Internal

} // Dali
