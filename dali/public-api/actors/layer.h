#ifndef __DALI_LAYER_H__
#define __DALI_LAYER_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/images/frame-buffer-image.h>

namespace Dali
{
/**
 * @addtogroup dali_core_actors
 * @{
 */

namespace Internal DALI_INTERNAL
{
class Layer;
}

/**
 * @brief Rectangle describing area on screen that a layer can draw to.
 *
 * @SINCE_1_0.0
 * @see Dali::Layer::SetClippingBox()
 */
typedef Rect<int> ClippingBox;

/**
 * @brief Layers provide a mechanism for overlaying groups of actors on top of each other.
 *
 * When added to the stage, a layer can be ordered relative to other layers. The bottom
 * layer is at depth zero. The stage provides a default layer for it's children.
 *
 * Layered actors inherit position etc. as normal, but are drawn in an order determined
 * by the layers. The depth buffer is cleared before each layer is rendered unless depth
 * test is disabled or there's no need for it based on the layers contents;
 * actors in lower layers cannot obscure actors in higher layers.
 *
 * If depth test is disabled, there is no performance overhead from clearing the depth buffer.
 *
 * Actions
 * | %Action Name    | %Layer method called |
 * |-----------------|----------------------|
 * | raise           | @ref Raise()         |
 * | lower           | @ref Lower()         |
 * | raiseToTop      | @ref RaiseToTop()    |
 * | lowerToBottom   | @ref LowerToBottom() |
 * @SINCE_1_0.0
 */
class DALI_IMPORT_API Layer : public Actor
{
public:

  /**
   * @brief An enumeration of properties belonging to the Layer class.
   * Properties additional to Actor.
   * @SINCE_1_0.0
   */
  struct Property
  {
    enum
    {
      CLIPPING_ENABLE = DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX, ///< name "clippingEnable",   type bool @SINCE_1_0.0
      CLIPPING_BOX,                                                 ///< name "clippingBox",      type Rect<int> @SINCE_1_0.0
      BEHAVIOR,                                                     ///< name "behavior",         type String @SINCE_1_0.0
    };
  };

  /**
   * @brief Enumeration for the behavior of the layer.
   *
   * Check each value to see how it affects the layer.
   * @SINCE_1_0.0
   */
  enum Behavior
  {
    /**
     * @brief Layer doesn't make use of the depth test.
     *
     * This mode is expected to have better performance than the 3D mode.
     * When using this mode any ordering would be with respect to depthIndex property of Renderers.
     * @SINCE_1_0.0
     */
    LAYER_2D,

    /**
     * @brief Layer will use depth test and do several clears.
     *
     * When using this mode depth depth test will be used. A depth clear will happen for each distinct
     * depth-index value in the layer, opaque renderers are drawn first and write to the depth buffer.
     * Then transparent renderers are drawn with depth test enabled but depth write switched off.
     * @SINCE_1_0.0
     */
    LAYER_3D,
  };

  /*
   * TREE_DEPTH_MULTIPLIER is used by the rendering sorting algorithm to decide which actors to render first.
   * For 2D layers, this value will be multiplied to the actor depth in the tree and added to the depth index
   * to obtain the value which will be used for ordering
   */
  enum TreeDepthMultiplier
  {
    TREE_DEPTH_MULTIPLIER = 10000,
  };
  /**
   * @brief The sort function type.
   *
   * @SINCE_1_0.0
   * @param[in] position this is the actor translation from camera.
   */
  typedef float (*SortFunctionType)( const Vector3& position );

  /**
   * @brief Create an empty Layer handle.
   *
   * This can be initialised with Layer::New(...)
   * @SINCE_1_0.0
   */
  Layer();

  /**
   * @brief Create a Layer object.
   *
   * @SINCE_1_0.0
   * @return A handle to a newly allocated Layer
   */
  static Layer New();

  /**
   * @brief Downcast an Object handle to Layer.
   *
   * If handle points to a Layer the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   * @SINCE_1_0.0
   * @param[in] handle to An object
   * @return handle to a Layer or an uninitialized handle
   */
  static Layer DownCast( BaseHandle handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~Layer();

  /**
   * @brief Copy constructor
   *
   * @SINCE_1_0.0
   * @param [in] copy The actor to copy.
   */
  Layer(const Layer& copy);

  /**
   * @brief Assignment operator
   *
   * @SINCE_1_0.0
   * @param [in] rhs The actor to copy.
   */
  Layer& operator=(const Layer& rhs);

  /**
   * @brief Query the depth of the layer
   *
   * 0 is bottom most layer, higher number is on top
   * @SINCE_1_0.0
   * @return the current depth of the layer.
   * @pre layer is on the stage
   * If layer is not added to the stage, returns 0.
   */
  unsigned int GetDepth() const;

  /**
   * @brief Increment the depth of the layer.
   *
   * @SINCE_1_0.0
   * @pre layer is on the stage
   */
  void Raise();

  /**
   * @brief Decrement the depth of the layer.
   *
   * @SINCE_1_0.0
   * @pre layer is on the stage
   */
  void Lower();

  /**
   * @brief Ensures the layers depth is greater than the target layer.
   *
   * If the layer already is above target layer its depth is not changed
   * If the layer was below target, its new depth will be immediately above target
   * Note! All layers between this layer and target get new depth values
   * @SINCE_1_0.0
   * @param target layer to get above of
   * @pre layer is on the stage
   * @pre target layer is on the stage
   */
  void RaiseAbove( Layer target );

  /**
   * @brief Ensures the layers depth is less than the target layer.
   *
   * If the layer already is below the layer its depth is not changed
   * If the layer was above target, its new depth will be immediately below target
   * Note! All layers between this layer and target get new depth values
   * @SINCE_1_0.0
   * @param target layer to get below of
   * @pre layer is on the stage
   * @pre target layer is on the stage
   */
  void LowerBelow( Layer target );

  /**
   * @brief Raises the layer to the top.
   * @SINCE_1_0.0
   * @pre layer is on the stage
   */
  void RaiseToTop();

  /**
   * @brief Lowers the layer to the bottom.
   * @SINCE_1_0.0
   * @pre layer is on the stage
   */
  void LowerToBottom();

  /**
   * @brief Moves the layer directly above the given layer.
   *
   * After the call this layers depth will be immediately above target
   * Note! All layers between this layer and target get new depth values
   * @SINCE_1_0.0
   * @param target layer to get on top of
   * @pre layer is on the stage
   * @pre target layer is on the stage
   */
  void MoveAbove( Layer target );

  /**
   * @brief Moves the layer directly below the given layer.
   *
   * After the call this layers depth will be immediately below target
   * Note! All layers between this layer and target get new depth values
   * @SINCE_1_0.0
   * @param target layer to get below of
   * @pre layer is on the stage
   * @pre target layer is on the stage
   */
  void MoveBelow( Layer target );

  /**
   * @brief Set the behavior of the layer
   *
   * @SINCE_1_0.0
   * @param[in] behavior The desired behavior
   */
  void SetBehavior( Behavior behavior );

  /**
   * @brief Get the behavior of the layer
   *
   * @SINCE_1_0.0
   * @return The behavior of the layer
   */
  Behavior GetBehavior() const;

  /**
   * @brief Sets whether clipping is enabled for a layer.
   *
   * Clipping is initially disabled; see also SetClippingBox().
   * @SINCE_1_0.0
   * @param [in] enabled True if clipping is enabled.
   *
   * @note When clipping is enabled, the default clipping box is empty (0,0,0,0) which means everything is clipped.
   */
  void SetClipping(bool enabled);

  /**
   * @brief Query whether clipping is enabled for a layer.
   * @SINCE_1_0.0
   * @return True if clipping is enabled.
   */
  bool IsClipping() const;

  /**
   * @brief Sets the clipping box of a layer, in window coordinates.
   *
   * The contents of the layer will not be visible outside this box, when clipping is
   * enabled. The default clipping box is empty (0,0,0,0) which means everything is clipped.
   * You can only do rectangular clipping using this API in window coordinates.
   * For other kinds of clipping, @see Dali::Actor::SetDrawMode().
   * @SINCE_1_0.0
   * @param [in] x The X-coordinate of the top-left corner of the box.
   * @param [in] y The Y-coordinate of the top-left corner of the box.
   * @param [in] width  The width of the box.
   * @param [in] height The height of the box.
   */
  void SetClippingBox(int x, int y, int width, int height);

  /**
   * @brief Sets the clipping box of a layer, in window coordinates.
   *
   * The contents of the layer will not be visible outside this box, when clipping is
   * enabled. The default clipping box is empty (0,0,0,0).
   * @SINCE_1_0.0
   * @param [in] box The clipping box
   */
  void SetClippingBox(ClippingBox box);

  /**
   * @brief Retrieves the clipping box of a layer, in window coordinates.
   *
   * @SINCE_1_0.0
   * @return The clipping box
   */
  ClippingBox GetClippingBox() const;

  // Depth test

  /**
   * @brief Whether to disable the depth test.
   *
   * By default a layer enables depth test if there is more than one opaque actor or if there is one opaque actor and one, or more, transparent actors.
   * However, it's possible to disable the depth test by calling this method.
   *
   * @SINCE_1_0.0
   * @param[in] disable \e true disables depth test. \e false sets the default behavior.
   */
  void SetDepthTestDisabled( bool disable );

  /**
   * @brief Retrieves whether depth test is disabled.
   *
   * @SINCE_1_0.0
   * @return \e true if depth test is disabled.
   */
  bool IsDepthTestDisabled() const;

  // Sorting

  /**
   * @brief This allows the user to specify the sort function that the layer should use.
   *
   * The sort function is used to determine the order in which the actors are drawn
   * and input is processed on the actors in the layer.
   *
   * A function of the following type should be used:
   * @code
   *  float YourSortFunction(const Vector3& position);
   * @endcode
   *
   * @SINCE_1_0.0
   * @param[in]  function  The sort function pointer
   * @note If the sort function returns a low number, the actor the data applies to will be
   * drawn in front of an actor whose data yields a high value from the sort function.
   *
   * @note All child layers use the same sort function.  If a child layer is added to this
   * layer then the sort function used by the child layer will also be the same.
   *
  */
  void SetSortFunction( SortFunctionType function );

  /**
   * @brief This allows the user to specify whether this layer should consume touch (including gestures).
   *
   * If set, any layers behind this layer will not be hit-test.
   *
   * @SINCE_1_0.0
   * @param[in]  consume  Whether the layer should consume touch (including gestures).
   */
  void SetTouchConsumed( bool consume );

  /**
   * @brief Retrieves whether the layer consumes touch (including gestures).
   *
   * @SINCE_1_0.0
   * @return true if consuming touch, false otherwise.
   */
  bool IsTouchConsumed() const;

  /**
   * @brief This allows the user to specify whether this layer should consume hover.
   *
   * If set, any layers behind this layer will not be hit-test.
   *
   * @SINCE_1_0.0
   * @param[in]  consume  Whether the layer should consume hover.
   */
  void SetHoverConsumed( bool consume );

  /**
   * @brief Retrieves whether the layer consumes hover.
   *
   * @SINCE_1_0.0
   * @return true if consuming hover, false otherwise.
   */
  bool IsHoverConsumed() const;

public: // Not intended for application developers

  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @SINCE_1_0.0
   * @param [in] Layer A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL Layer(Internal::Layer* Layer);
};

/**
 * @}
 */
} // namespace Dali

#endif //__DALI_LAYER_H__
