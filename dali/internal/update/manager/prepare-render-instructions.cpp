/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/internal/update/manager/prepare-render-instructions.h>

// INTERNAL INCLUDES
#include <dali/public-api/shader-effects/shader-effect.h>
#include <dali/public-api/actors/layer.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/event/actors/layer-impl.h> // for the default sorting function
#include <dali/internal/update/nodes/scene-graph-layer.h>
#include <dali/internal/update/manager/sorted-layers.h>
#include <dali/internal/update/render-tasks/scene-graph-render-task.h>
#include <dali/internal/update/rendering/scene-graph-texture-set.h>
#include <dali/internal/update/resources/resource-manager-declarations.h>
#include <dali/internal/render/common/render-item.h>
#include <dali/internal/render/common/render-tracker.h>
#include <dali/internal/render/common/render-instruction.h>
#include <dali/internal/render/common/render-instruction-container.h>
#include <dali/internal/render/shaders/scene-graph-shader.h>
#include <dali/internal/render/renderers/render-renderer.h>
#include <dali/internal/render/renderers/render-property-buffer.h>
#include <dali/internal/update/manager/geometry-batcher.h>

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

/**
 * Add a renderer to the list
 * @param updateBufferIndex to read the model matrix from
 * @param renderList to add the item to
 * @param renderable Node-Renderer pair
 * @param viewMatrix used to calculate modelview matrix for the item
 * @param camera The camera used to render
 * @param geometryBatcher The instance of the geometry batcher
 * @param isLayer3d Whether we are processing a 3D layer or not
 * @param cull Whether frustum culling is enabled or not
 */
inline void AddRendererToRenderList( BufferIndex updateBufferIndex,
                                     RenderList& renderList,
                                     Renderable& renderable,
                                     const Matrix& viewMatrix,
                                     SceneGraph::Camera& camera,
                                     GeometryBatcher& geometryBatcher,
                                     bool isLayer3d,
                                     bool cull )
{
  // discard renderable early if it belongs to the batch which has been consumed in during frame
  Node* renderableNode = renderable.mNode;
  const bool batchingValid( renderable.mRenderer->IsBatchingEnabled() && renderableNode->mBatchIndex != BATCH_NULL_HANDLE );
  if( batchingValid && geometryBatcher.HasRendered( renderableNode->mBatchIndex ) )
  {
    return;
  }

  bool inside( true );
  const Node* batchParentNode = renderable.mNode->GetBatchParent();
  const Node* node = ( renderable.mRenderer->IsBatchingEnabled() && batchParentNode ) ?
        batchParentNode : renderableNode;

  if ( cull && !renderable.mRenderer->GetShader().HintEnabled( Dali::Shader::Hint::MODIFIES_GEOMETRY ) )
  {
    const Vector4& boundingSphere = node->GetBoundingSphere();
    inside = (boundingSphere.w > Math::MACHINE_EPSILON_1000) &&
             (camera.CheckSphereInFrustum( updateBufferIndex, Vector3(boundingSphere), boundingSphere.w ) );
  }

  if( inside )
  {
    if( batchingValid )
    {
      geometryBatcher.SetRendered( renderableNode->mBatchIndex );
    }

    Renderer::Opacity opacity = renderable.mRenderer->GetOpacity( updateBufferIndex, *renderable.mNode );
    if( opacity != Renderer::TRANSPARENT )
    {
      // Get the next free RenderItem
      RenderItem& item = renderList.GetNextFreeItem();
      item.mRenderer = &renderable.mRenderer->GetRenderer();
      item.mNode = renderable.mNode;
      item.mIsOpaque = (opacity == Renderer::OPAQUE);

      if( isLayer3d )
      {
        item.mDepthIndex = renderable.mRenderer->GetDepthIndex();
      }
      else
      {
        item.mDepthIndex = renderable.mRenderer->GetDepthIndex() + static_cast<int>( renderable.mNode->GetDepth() ) * Dali::Layer::TREE_DEPTH_MULTIPLIER;
      }
      // save MV matrix onto the item
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
 * @param geometryBatcher The instance of the geometry batcher
 * @param isLayer3d Whether we are processing a 3D layer or not
 * @param cull Whether frustum culling is enabled or not
 */
inline void AddRenderersToRenderList( BufferIndex updateBufferIndex,
                                      RenderList& renderList,
                                      RenderableContainer& renderers,
                                      const Matrix& viewMatrix,
                                      SceneGraph::Camera& camera,
                                      GeometryBatcher* geometryBatcher,
                                      bool isLayer3d,
                                      bool cull)
{
  DALI_LOG_INFO( gRenderListLogFilter, Debug::Verbose, "AddRenderersToRenderList()\n");

  unsigned int rendererCount( renderers.Size() );
  for( unsigned int i(0); i<rendererCount; ++i )
  {
    AddRendererToRenderList( updateBufferIndex, renderList, renderers[i], viewMatrix, camera, *geometryBatcher, isLayer3d, cull );
  }
}

/**
 * Try to reuse cached renderitems from the renderlist
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
  // check that the cached list originates from this layer and that the counts match
  if( ( renderList.GetSourceLayer() == &layer )&&
      ( renderList.GetCachedItemCount() == renderableCount ) )
  {
    // check that all the same renderers are there. This gives us additional security in avoiding rendering the wrong things
    // Render list is sorted so at this stage renderers may be in different order
    // therefore we check a combined sum of all renderer addresses
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

/**
 * Function which sorts render items by depth index then by instance
 * ptrs of shader/texture/geometry.
 * @param lhs item
 * @param rhs item
 * @return true if left item is greater than right
 */
bool CompareItems( const RendererWithSortAttributes& lhs, const RendererWithSortAttributes& rhs )
{
  if( lhs.renderItem->mDepthIndex == rhs.renderItem->mDepthIndex )
  {
    if( lhs.shader == rhs.shader )
    {
      if( lhs.textureResourceId == rhs.textureResourceId )
      {
        return lhs.geometry < rhs.geometry;
      }
      return lhs.textureResourceId < rhs.textureResourceId;
    }
    return lhs.shader < rhs.shader;
  }
  return lhs.renderItem->mDepthIndex < rhs.renderItem->mDepthIndex;
}
/**
 * Function which sorts the render items by Z function, then
 * by instance ptrs of shader/texture/geometry.
 * @param lhs item
 * @param rhs item
 * @return true if left item is greater than right
 */
bool CompareItems3D( const RendererWithSortAttributes& lhs, const RendererWithSortAttributes& rhs )
{
  bool lhsIsOpaque = lhs.renderItem->mIsOpaque;
  if( lhsIsOpaque ==  rhs.renderItem->mIsOpaque )
  {
    if( lhsIsOpaque )
    {
      //If both RenderItems are opaque, sort using shader, then texture then geometry
      if( lhs.shader == rhs.shader )
      {
        if( lhs.textureResourceId == rhs.textureResourceId )
        {
          return lhs.geometry < rhs.geometry;
        }
        return lhs.textureResourceId < rhs.textureResourceId;
      }
      return lhs.shader < rhs.shader;
    }
    else
    {
      //If both RenderItems are transparent, sort using z,then shader, then texture, then geometry
      if( Equals(lhs.zValue, rhs.zValue) )
      {
        if( lhs.shader == rhs.shader )
        {
          if( lhs.textureResourceId == rhs.textureResourceId )
          {
            return lhs.geometry < rhs.geometry;
          }
          return lhs.textureResourceId < rhs.textureResourceId;
        }
        return lhs.shader < rhs.shader;
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
 * Sort render items
 * @param bufferIndex The buffer to read from
 * @param renderList to sort
 * @param layer where the renderers are from
 * @param sortingHelper to use for sorting the renderitems (to avoid reallocating)
 */
inline void SortRenderItems( BufferIndex bufferIndex, RenderList& renderList, Layer& layer, RendererSortingHelper& sortingHelper )
{
  const size_t renderableCount = renderList.Count();
  // reserve space if needed
  const unsigned int oldcapacity = sortingHelper.size();
  if( oldcapacity < renderableCount )
  {
    sortingHelper.reserve( renderableCount );
    // add real objects (reserve does not construct objects)
    sortingHelper.insert( sortingHelper.begin() + oldcapacity,
                          (renderableCount - oldcapacity),
                          RendererWithSortAttributes() );
  }
  else
  {
    // clear extra elements from helper, does not decrease capability
    sortingHelper.resize( renderableCount );
  }

  // calculate the sorting value, once per item by calling the layers sort function
  // Using an if and two for-loops rather than if inside for as its better for branch prediction
  if( layer.UsesDefaultSortFunction() )
  {
    for( size_t index = 0; index < renderableCount; ++index )
    {
      RenderItem& item = renderList.GetItem( index );

      item.mRenderer->SetSortAttributes( bufferIndex, sortingHelper[ index ] );

      // the default sorting function should get inlined here
      sortingHelper[ index ].zValue = Internal::Layer::ZValue( item.mModelViewMatrix.GetTranslation3() ) - item.mDepthIndex;

      // keep the renderitem pointer in the helper so we can quickly reorder items after sort
      sortingHelper[ index ].renderItem = &item;
    }
  }
  else
  {
    const Dali::Layer::SortFunctionType sortFunction = layer.GetSortFunction();
    for( size_t index = 0; index < renderableCount; ++index )
    {
      RenderItem& item = renderList.GetItem( index );

      item.mRenderer->SetSortAttributes( bufferIndex, sortingHelper[ index ] );
      sortingHelper[ index ].zValue = (*sortFunction)( item.mModelViewMatrix.GetTranslation3() ) - item.mDepthIndex;

      // keep the renderitem pointer in the helper so we can quickly reorder items after sort
      sortingHelper[ index ].renderItem = &item;
    }
  }

  if( layer.GetBehavior() ==  Dali::Layer::LAYER_3D)
  {
    // sort the renderers back to front, Z Axis point from near plane to far plane
    std::stable_sort( sortingHelper.begin(), sortingHelper.end(), CompareItems3D );
  }
  else
  {
    // sort the renderers based on DepthIndex
    std::stable_sort( sortingHelper.begin(), sortingHelper.end(), CompareItems );
  }

  // reorder/repopulate the renderitems in renderlist to correct order based on sortinghelper
  DALI_LOG_INFO( gRenderListLogFilter, Debug::Verbose, "Sorted Transparent List:\n");
  RenderItemContainer::Iterator renderListIter = renderList.GetContainer().Begin();
  for( unsigned int index = 0; index < renderableCount; ++index, ++renderListIter )
  {
    *renderListIter = sortingHelper[ index ].renderItem;
    DALI_LOG_INFO( gRenderListLogFilter, Debug::Verbose, "  sortedList[%d] = %p\n", index, sortingHelper[ index ].renderItem->mRenderer);
  }
}

/**
 * Add color renderers from the layer onto the next free render list
 * @param updateBufferIndex to use
 * @param layer to get the renderers from
 * @param viewmatrix for the camera from rendertask
 * @param camera to use the view frustum
 * @param stencilRenderablesExist is true if there are stencil renderers on this layer
 * @param instruction to fill in
 * @param sortingHelper to use for sorting the renderitems (to avoid reallocating)
 * @param geometryBatcher the instance of the geometry batcher
 * @param tryReuseRenderList whether to try to reuse the cached items from the instruction
 * @param cull Whether frustum culling is enabled or not
 */
inline void AddColorRenderers( BufferIndex updateBufferIndex,
                               Layer& layer,
                               const Matrix& viewMatrix,
                               SceneGraph::Camera& camera,
                               bool stencilRenderablesExist,
                               RenderInstruction& instruction,
                               RendererSortingHelper& sortingHelper,
                               GeometryBatcher& geometryBatcher,
                               bool tryReuseRenderList,
                               bool cull)
{
  RenderList& renderList = instruction.GetNextFreeRenderList( layer.colorRenderables.Size() );
  renderList.SetClipping( layer.IsClipping(), layer.GetClippingBox() );
  renderList.SetSourceLayer( &layer );
  renderList.SetHasColorRenderItems( true );

  // try to reuse cached renderitems from last time around
  if( tryReuseRenderList )
  {
    if( TryReuseCachedRenderers( layer, renderList, layer.colorRenderables ) )
    {
      return;
    }
  }

  AddRenderersToRenderList( updateBufferIndex, renderList, layer.colorRenderables, viewMatrix, camera, &geometryBatcher, layer.GetBehavior() == Dali::Layer::LAYER_3D, cull );
  SortRenderItems( updateBufferIndex, renderList, layer, sortingHelper );

  // Setup the render flags for stencil.
  renderList.ClearFlags();
  if( stencilRenderablesExist )
  {
    // Note: SetFlags does not overwrite, it ORs, so ClearFlags() is also required.
    renderList.SetFlags( RenderList::STENCIL_BUFFER_ENABLED );
  }
}

/**
 * Add overlay renderers from the layer onto the next free render list
 * @param updateBufferIndex to use
 * @param layer to get the renderers from
 * @param viewmatrix for the camera from rendertask
 * @param camera to use
 * @param stencilRenderablesExist is true if there are stencil renderers on this layer
 * @param instruction to fill in
 * @param tryReuseRenderList whether to try to reuse the cached items from the instruction
 * @param cull Whether frustum culling is enabled or not
 */
inline void AddOverlayRenderers( BufferIndex updateBufferIndex,
                                 Layer& layer,
                                 const Matrix& viewMatrix,
                                 SceneGraph::Camera& camera,
                                 bool stencilRenderablesExist,
                                 RenderInstruction& instruction,
                                 RendererSortingHelper& sortingHelper,
                                 bool tryReuseRenderList,
                                 bool cull )
{
  RenderList& overlayRenderList = instruction.GetNextFreeRenderList( layer.overlayRenderables.Size() );
  overlayRenderList.SetClipping( layer.IsClipping(), layer.GetClippingBox() );
  overlayRenderList.SetSourceLayer( &layer );
  overlayRenderList.SetHasColorRenderItems( false );

  //Set render flags
  overlayRenderList.ClearFlags();
  if(stencilRenderablesExist)
  {
    overlayRenderList.SetFlags(RenderList::STENCIL_BUFFER_ENABLED);
  }

  // try to reuse cached renderitems from last time around
  if( tryReuseRenderList )
  {
    if( TryReuseCachedRenderers( layer, overlayRenderList, layer.overlayRenderables ) )
    {
      return;
    }
  }
  AddRenderersToRenderList( updateBufferIndex, overlayRenderList, layer.overlayRenderables, viewMatrix, camera, NULL, layer.GetBehavior() == Dali::Layer::LAYER_3D, cull );
  SortRenderItems( updateBufferIndex, overlayRenderList, layer, sortingHelper );
}

/**
 * Add stencil renderers from the layer onto the next free render list
 * @param updateBufferIndex to use
 * @param layer to get the renderers from
 * @param viewmatrix for the camera from rendertask
 * @param camera to use
 * @param instruction to fill in
 * @param tryReuseRenderList whether to try to reuse the cached items from the instruction
 * @param cull Whether frustum culling is enabled or not
 */
inline void AddStencilRenderers( BufferIndex updateBufferIndex,
                                 Layer& layer,
                                 const Matrix& viewMatrix,
                                 SceneGraph::Camera& camera,
                                 RenderInstruction& instruction,
                                 bool tryReuseRenderList,
                                 bool cull )
{
  RenderList& stencilRenderList = instruction.GetNextFreeRenderList( layer.stencilRenderables.Size() );
  stencilRenderList.SetClipping( layer.IsClipping(), layer.GetClippingBox() );
  stencilRenderList.SetSourceLayer( &layer );
  stencilRenderList.SetHasColorRenderItems( false );

  //Set render flags
  stencilRenderList.ClearFlags();
  stencilRenderList.SetFlags(RenderList::STENCIL_CLEAR | RenderList::STENCIL_WRITE | RenderList::STENCIL_BUFFER_ENABLED );

  // try to reuse cached renderitems from last time around
  if( tryReuseRenderList )
  {
    if( TryReuseCachedRenderers( layer, stencilRenderList, layer.stencilRenderables ) )
    {
      return;
    }
  }
  AddRenderersToRenderList( updateBufferIndex, stencilRenderList, layer.stencilRenderables, viewMatrix, camera, NULL, layer.GetBehavior() == Dali::Layer::LAYER_3D, cull );
}

void PrepareRenderInstruction( BufferIndex updateBufferIndex,
                               SortedLayerPointers& sortedLayers,
                               RenderTask& renderTask,
                               RendererSortingHelper& sortingHelper,
                               bool cull,
                               RenderInstructionContainer& instructions,
                               GeometryBatcher& geometryBatcher )
{
  // Retrieve the RenderInstruction buffer from the RenderInstructionContainer
  // then populate with instructions.
  RenderInstruction& instruction = instructions.GetNextInstruction( updateBufferIndex );
  renderTask.PrepareRenderInstruction( instruction, updateBufferIndex );
  bool viewMatrixHasNotChanged = !renderTask.ViewMatrixUpdated();

  const Matrix& viewMatrix = renderTask.GetViewMatrix( updateBufferIndex );
  SceneGraph::Camera& camera = renderTask.GetCamera();

  const SortedLayersIter endIter = sortedLayers.end();
  for ( SortedLayersIter iter = sortedLayers.begin(); iter != endIter; ++iter )
  {
    Layer& layer = **iter;

    const bool stencilRenderablesExist( !layer.stencilRenderables.Empty() );
    const bool colorRenderablesExist( !layer.colorRenderables.Empty() );
    const bool overlayRenderablesExist( !layer.overlayRenderables.Empty() );
    const bool tryReuseRenderList( viewMatrixHasNotChanged && layer.CanReuseRenderers( &renderTask.GetCamera() ) );

    // Ignore stencils if there's nothing to test
    if( stencilRenderablesExist &&
        ( colorRenderablesExist || overlayRenderablesExist ) )
    {
      AddStencilRenderers( updateBufferIndex, layer, viewMatrix, camera, instruction, tryReuseRenderList, cull );
    }

    if ( colorRenderablesExist )
    {
      AddColorRenderers( updateBufferIndex,
                         layer,
                         viewMatrix,
                         camera,
                         stencilRenderablesExist,
                         instruction,
                         sortingHelper,
                         geometryBatcher,
                         tryReuseRenderList,
                         cull );
    }

    if ( overlayRenderablesExist )
    {
      AddOverlayRenderers( updateBufferIndex, layer, viewMatrix, camera, stencilRenderablesExist,
                           instruction, sortingHelper, tryReuseRenderList, cull );
    }
  }

  // inform the render instruction that all renderers have been added and this frame is complete
  instruction.UpdateCompleted();
}

} // SceneGraph

} // Internal

} // Dali
