/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/internal/update/manager/prepare-render-algorithms.h>

// INTERNAL INCLUDES
#include <dali/internal/update/manager/sorted-layers.h>
#include <dali/internal/update/node-attachments/scene-graph-renderable-attachment.h>
#include <dali/internal/update/nodes/scene-graph-layer.h>
#include <dali/internal/render/renderers/scene-graph-renderer.h>
#include <dali/internal/render/common/render-instruction.h>
#include <dali/internal/render/shaders/scene-graph-shader.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

/******************************************************************************
 ************************** Clear lists from previous update ******************
 ******************************************************************************/

void ClearRenderables( SortedLayerPointers& sortedLayers )
{
  // Clear the lists of renderables from the previous update

  const SortedLayersIter endIter = sortedLayers.end();

  for ( SortedLayersIter iter = sortedLayers.begin(); iter != endIter; ++iter )
  {
    (*iter)->stencilRenderables.clear();
    (*iter)->colorRenderables.clear();
    (*iter)->overlayRenderables.clear();
  }
}

/******************************************************************************
 ***************************** Prepare renderers ******************************
 ******************************************************************************/

void PrepareRenderables( BufferIndex updateBufferIndex, RenderableAttachmentContainer& renderableList )
{
  const RenderableAttachmentIter endIter = renderableList.end();
  for ( RenderableAttachmentIter iter = renderableList.begin(); iter != endIter; ++iter )
  {
    RenderableAttachment& renderable = **iter;
    renderable.DoPrepareRender( updateBufferIndex );
  }
}

void PrepareRenderables( BufferIndex updateBufferIndex, SortedLayerPointers& sortedLayers )
{
  const SortedLayersIter endIter = sortedLayers.end();

  for ( SortedLayersIter iter = sortedLayers.begin(); iter != endIter; ++iter )
  {
    Layer& layer = **iter;

    PrepareRenderables( updateBufferIndex, layer.stencilRenderables );
    PrepareRenderables( updateBufferIndex, layer.colorRenderables );
    PrepareRenderables( updateBufferIndex, layer.overlayRenderables );
  }
}

} // SceneGraph

} // Internal

} // Dali
