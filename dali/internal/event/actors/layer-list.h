#ifndef __DALI_INTERNAL_LAYER_LIST_H__
#define __DALI_INTERNAL_LAYER_LIST_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>

namespace Dali
{

namespace Internal
{

class Layer;
class Stage;

/**
 * An ordered list of layers.
 * Layers are not owned by the LayerList; each layer is responsible for registering & unregistering.
 * This is used by the Stage, to keep track of layer depths.
 * A separate LayerList is maintained for actors added via the SystemOverlay::Add().
 */
class LayerList
{
public:

  /**
   * Create a new list of layers.
   * @param[in] stage A reference to the stage.
   * @param[in] systemLevel True if the layers are added via the SystemOverlay API.
   */
  static LayerList* New( Stage& stage, bool systemLevel );

  /**
   * Non-virtual destructor; not suitable as a base class.
   */
  ~LayerList();

  /**
   * Query the number of layers.
   * @return The number of layers.
   */
  unsigned int GetLayerCount() const;

  /**
   * Retrieve the layer at a specified depth.
   * @pre depth is less than layer count; see GetLayerCount().
   * @param[in] depth The depth.
   * @return The layer found at the given depth.
   */
  Layer* GetLayer( unsigned int depth ) const;

  /**
   * Gets the depth of a given layer
   * @param layer which depth to check
   */
  unsigned int GetDepth( const Layer* layer ) const;

  /**
   * Register a layer with the stage.
   * The stage determines the relative depth of each layer.
   */
  void RegisterLayer( Layer& layer );

  /**
   * Unregister a layer from the stage
   */
  void UnregisterLayer(Layer& layer);

  /**
   * Increment the depth of the layer inside the stage
   * @pre layer is on stage
   */
  void RaiseLayer(Layer& layer);

  /**
   * Decrement the depth of the layer inside the stage
   * @pre layer is on stage
   */
  void LowerLayer(Layer& layer);

  /**
   * Raises the layer to the top of the stage
   * @pre layer is on stage
   * @param layer to move
   */
  void RaiseLayerToTop( const Layer& layer );

  /**
   * Lowers the layer to the bottom of the stage
   * @pre layer is on stage
   * @param layer to move
   */
  void LowerLayerToBottom( const Layer& layer );

  /**
   * Moves the layer above the target layer on the stage
   * @pre layer is on stage
   * @pre target is on stage
   * @param layer to move
   * @param target to move above of
   */
  void MoveLayerAbove( const Layer& layer, const Layer& target );

  /**
   * Moves the layer below the target layer on the stage
   * @pre layer is on stage
   * @pre target is on stage
   * @param layer to move
   * @param target to move below of
   */
  void MoveLayerBelow( const Layer& layer, const Layer& target );

private:

  /**
   * Protected constructor; see also LayerList::New().
   * @param[in] stage A reference to the stage.
   * @param[in] systemLevel True if the layers are added via the SystemOverlay API.
   */
  LayerList( Stage& stage, bool systemLevel );

  /**
   * A private helper method to set the depth for each layer.
   * Layers have depth which is the same as their ordinal number in the stage container
   * This method propagates any changes in the layer depths onto the scene graph side
   */
  void SetLayerDepths();

private:

  Stage& mStage;

  bool mIsSystemLevel; ///< True if the layers are added via the SystemOverlay API.

  typedef std::vector<Layer*> LayerContainer;

  // Layers are not owned by the LayerList.
  // Each layer is responsible for registering & unregistering before the end of its life-time.
  LayerContainer mLayers;
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_LAYER_LIST_H__
