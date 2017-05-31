#ifndef DALI_INTERNAL_SCENE_GRAPH_RENDER_INSTRUCTION_PROCESSOR_H
#define DALI_INTERNAL_SCENE_GRAPH_RENDER_INSTRUCTION_PROCESSOR_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/update/manager/sorted-layers.h>
#include <dali/public-api/common/dali-vector.h>

namespace Dali
{

namespace Internal
{

namespace Render
{
class Geometry;
}

namespace SceneGraph
{

class RenderTracker;
struct RenderItem;
class Shader;
struct RenderList;
class RenderTask;
class RenderInstructionContainer;


/**
 * @brief This class handles the sorting and preparation of Renderers for each layer.
 */
class RenderInstructionProcessor
{
public:

  /**
   * @brief Constructor.
   */
  RenderInstructionProcessor();

  /**
   * @brief Destructor.
   */
  ~RenderInstructionProcessor();

  /**
   * @brief Structure to store information for sorting the renderers.
   * (Note, depthIndex is stored within the renderItem).
   */
  struct SortAttributes
  {
    SortAttributes()
    : renderItem( NULL ),
      shader( NULL ),
      textureSet( NULL ),
      geometry( NULL ),
      zValue( 0.0f )
    {
    }

    RenderItem*             renderItem;        ///< The render item that is being sorted (includes depth index)
    const Shader*           shader;            ///< The shader instance
    const void*             textureSet;        ///< The textureSet instance
    const Render::Geometry* geometry;          ///< The geometry instance
    float                   zValue;            ///< The Z value of the given renderer (either distance from camera, or a custom calculated value)
  };


  /**
   * @brief Sorts and prepares the list of opaque/transparent Renderers for each layer.
   * Whilst iterating through each layer, update the RenderItems ModelView matrices
   *
   * The opaque and transparent render lists are sorted first by depth
   * index, then by Z (for transparent only), then by shader
   * and geometry. The render algorithm should then work through both
   * lists simultaneously, working through opaque then transparent
   * items at each depth index, resetting the flags appropriately.
   *
   * @param[in]  updateBufferIndex The current update buffer index.
   * @param[in]  sortedLayers      The layers containing lists of opaque/transparent renderables.
   * @param[in]  renderTask        The rendering task information.
   * @param[in]  cull              Whether frustum culling is enabled or not
   * @param[in]  hasClippingNodes  Whether any clipping nodes exist within this layer, to optimize sorting if not
   * @param[out] instructions      The rendering instructions for the next frame.
   */
  void Prepare( BufferIndex updateBufferIndex,
                SortedLayerPointers& sortedLayers,
                RenderTask& renderTask,
                bool cull,
                bool hasClippingNodes,
                RenderInstructionContainer& instructions );

private:

  /**
   * Undefine copy and assignment operators.
   */
  RenderInstructionProcessor( const RenderInstructionProcessor& renderInstructionProcessor );             ///< No definition
  RenderInstructionProcessor& operator=( const RenderInstructionProcessor& renderInstructionProcessor );  ///< No definition

private:

  /**
   * @brief Sort render items
   * @param bufferIndex The buffer to read from
   * @param renderList to sort
   * @param layer where the Renderers are from
   * @param respectClippingOrder Sort with the correct clipping hierarchy.
   */
  inline void SortRenderItems( BufferIndex bufferIndex, RenderList& renderList, Layer& layer, bool respectClippingOrder );

  /// Sort comparitor function pointer type.
  typedef bool ( *ComparitorPointer )( const SortAttributes& lhs, const SortAttributes& rhs );
  typedef std::vector< SortAttributes > SortingHelper;

  Dali::Vector< ComparitorPointer > mSortComparitors;       ///< Contains all sort comparitors, used for quick look-up
  RenderInstructionProcessor::SortingHelper mSortingHelper; ///< Helper used to sort Renderers

};


} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_RENDER_INSTRUCTION_PROCESSOR_H
