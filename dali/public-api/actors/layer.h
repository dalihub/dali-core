#ifndef DALI_LAYER_H
#define DALI_LAYER_H

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
 * layer is at depth zero. The stage provides a default layer for it's children (see Stage::GetRootLayer()).
 *
 * Layered actors inherit position etc. as normal, but are drawn in an order determined
 * by the layers. In case of LAYER_3D, the depth buffer is cleared before each layer is rendered unless depth
 * test is disabled or there's no need for it based on the layers contents;
 * actors in lower layers cannot obscure actors in higher layers.
 *
 * A layer has either LAYER_2D or LAYER_3D mode. LAYER_2D has better performance,
 * the depth test is disabled, and a child actor hides its parent actor.
 * LAYER_3D uses the depth test, thus a close actor hides a farther one.
 * LAYER_2D is the default mode and recommended for general cases.
 * See Layer::Behavior and SetBehavior() for more information.
 *
 * Layer is a type of Actor, thus can have parent or children actors.
 * A layer influences rendering of its all descendant actors,
 * until another layer appears in the actor tree and manages its own subtree.
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
   * @brief Enumeration for the instance of properties belonging to the Layer class.
   *
   * Properties additional to Actor.
   * @SINCE_1_0.0
   */
  struct Property
  {
    /**
     * @brief Enumeration for the instance of properties belonging to the Layer class.
     *
     * Properties additional to Actor.
     * @SINCE_1_0.0
     */
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
     * @DEPRECATED_1_1.45, use LAYER_UI instead
     * @brief UI control rendering mode.
     * @SINCE_1_0.0
     * @see LAYER_UI
     */
    LAYER_2D,

    /**
     * @brief UI control rendering mode (default mode).
     *
     * This mode is designed for UI controls. In this mode renderer order will be respective to tree hierarchy of Actors.
     * This mode is expected to have better performance than LAYER_3D as renderers can be sorted more efficiently.
     *
     * For the following actor tree the rendering order will be A first, then B & C and finally D,E,F regardless of their
     * Z positions.
     * Rendering order between siblings, such as D, E & F or B & C, is determined based on the renderers depth index.
     * In UI we don't normally expect overlap. If you have two overlapped actors, make them parent-child to guarantee order of all renderers.
     *
     * @code
     *
     *     Layer1 (parent)
     *       |
     *       A
     *      / \
     *     B   C
     *    / \   \
     *   D   E   F
     *
     * @endcode
     *
     * @SINCE_1_1.45
     */
    LAYER_UI = LAYER_2D,

    /**
     * @brief Layer will use depth test.
     *
     * When using this mode, depth test will be used. A depth clear will happen for each layer,
     * which means actors in a layer "above" other layers will be rendered in front of actors in
     * those layers regardless of their Z positions (see Layer::Raise() and Layer::Lower()).
     * Opaque renderers are drawn first and write to the depth buffer.
     * Then transparent renderers are drawn with depth test enabled but depth write switched off.
     * Transparent renderers are drawn based on their distance from the camera.
     * A renderers DEPTH_INDEX property is used to offset the distance to the camera when ordering transparent renderers.
     * This is useful if you want to define the draw order of two or more transparent renderers that are
     * equal distance from the camera.
     * Unlike LAYER_UI, parent-child relationship does not affect rendering order at all.
     *
     * @SINCE_1_0.0
     */
    LAYER_3D

  };

  /**
   * @brief Enumeration for TREE_DEPTH_MULTIPLIER is used by the rendering sorting algorithm to decide which actors to render first.
   * @SINCE_1_0.0
   */
  enum TreeDepthMultiplier
  {
    TREE_DEPTH_MULTIPLIER = 10000,
  };
  /**
   * @brief The sort function type.
   *
   * @SINCE_1_0.0
   * @param[in] position This is the actor translation from camera
   */
  typedef float (*SortFunctionType)( const Vector3& position );

  /**
   * @brief Creates an empty Layer handle.
   *
   * This can be initialized with Layer::New(...).
   * @SINCE_1_0.0
   */
  Layer();

  /**
   * @brief Creates a Layer object.
   *
   * @SINCE_1_0.0
   * @return A handle to a newly allocated Layer
   */
  static Layer New();

  /**
   * @brief Downcasts a handle to Layer handle.
   *
   * If handle points to a Layer, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   * @SINCE_1_0.0
   * @param[in] handle Handle to an object
   * @return Handle to a Layer or an uninitialized handle
   */
  static Layer DownCast( BaseHandle handle );

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~Layer();

  /**
   * @brief Copy constructor.
   *
   * @SINCE_1_0.0
   * @param[in] copy The actor to copy
   */
  Layer(const Layer& copy);

  /**
   * @brief Assignment operator.
   *
   * @SINCE_1_0.0
   * @param[in] rhs The actor to copy
   * @return A reference to this
   */
  Layer& operator=(const Layer& rhs);

  /**
   * @brief Queries the depth of the layer.
   *
   * 0 is the bottom most layer, higher number is on top.
   * @SINCE_1_0.0
   * @return The current depth of the layer
   * @pre Layer is on the stage.
   * If layer is not added to the stage, returns 0.
   */
  unsigned int GetDepth() const;

  /**
   * @brief Increments the depth of the layer.
   *
   * @SINCE_1_0.0
   * @pre Layer is on the stage.
   */
  void Raise();

  /**
   * @brief Decrements the depth of the layer.
   *
   * @SINCE_1_0.0
   * @pre Layer is on the stage.
   */
  void Lower();

  /**
   * @brief Ensures the layers depth is greater than the target layer.
   *
   * If the layer already is above the target layer, its depth is not changed.
   * If the layer was below target, its new depth will be immediately above target.
   * @SINCE_1_0.0
   * @param target Layer to get above of
   * @pre Layer is on the stage.
   * @pre Target layer is on the stage.
   * @note All layers between this layer and target get new depth values.
   */
  void RaiseAbove( Layer target );

  /**
   * @brief Ensures the layers depth is less than the target layer.
   *
   * If the layer already is below the target layer, its depth is not changed.
   * If the layer was above target, its new depth will be immediately below target.
   * @SINCE_1_0.0
   * @param target Layer to get below of
   * @pre Layer is on the stage.
   * @pre Target layer is on the stage.
   * @note All layers between this layer and target get new depth values.
   */
  void LowerBelow( Layer target );

  /**
   * @brief Raises the layer to the top.
   * @SINCE_1_0.0
   * @pre Layer is on the stage.
   */
  void RaiseToTop();

  /**
   * @brief Lowers the layer to the bottom.
   * @SINCE_1_0.0
   * @pre layer is on the stage.
   */
  void LowerToBottom();

  /**
   * @brief Moves the layer directly above the given layer.
   *
   * After the call, this layers depth will be immediately above target.
   * @SINCE_1_0.0
   * @param target Layer to get on top of
   * @pre Layer is on the stage.
   * @pre Target layer is on the stage.
   * @note All layers between this layer and target get new depth values.
   */
  void MoveAbove( Layer target );

  /**
   * @brief Moves the layer directly below the given layer.
   *
   * After the call, this layers depth will be immediately below target.
   * @SINCE_1_0.0
   * @param target Layer to get below of
   * @pre Layer is on the stage.
   * @pre Target layer is on the stage.
   * @note All layers between this layer and target get new depth values.
   */
  void MoveBelow( Layer target );

  /**
   * @brief Sets the behavior of the layer.
   *
   * @SINCE_1_0.0
   * @param[in] behavior The desired behavior
   */
  void SetBehavior( Behavior behavior );

  /**
   * @brief Gets the behavior of the layer.
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
   * @param[in] enabled True if clipping is enabled
   *
   * @note When clipping is enabled, the default clipping box is empty (0,0,0,0), which means everything is clipped.
   */
  void SetClipping(bool enabled);

  /**
   * @brief Queries whether clipping is enabled for a layer.
   * @SINCE_1_0.0
   * @return True if clipping is enabled
   */
  bool IsClipping() const;

  /**
   * @brief Sets the clipping box of a layer, in window coordinates.
   *
   * The contents of the layer will not be visible outside this box, when clipping is
   * enabled. The default clipping box is empty (0,0,0,0) which means everything is clipped.
   * You can only do rectangular clipping using this API in window coordinates.
   * @SINCE_1_0.0
   * @param[in] x The X-coordinate of the top-left corner of the box
   * @param[in] y The Y-coordinate of the top-left corner of the box
   * @param[in] width The width of the box
   * @param[in] height The height of the box
   */
  void SetClippingBox(int x, int y, int width, int height);

  /**
   * @brief Sets the clipping box of a layer in window coordinates.
   *
   * The contents of the layer will not be visible outside this box when clipping is
   * enabled. The default clipping box is empty (0,0,0,0).
   * @SINCE_1_0.0
   * @param[in] box The clipping box
   */
  void SetClippingBox(ClippingBox box);

  /**
   * @brief Retrieves the clipping box of a layer in window coordinates.
   *
   * @SINCE_1_0.0
   * @return The clipping box
   */
  ClippingBox GetClippingBox() const;

  // Depth test

  /**
   * @brief Whether to disable the depth test.
   *
   * By default a layer enables depth test if there is more than one opaque actor or if there is one opaque actor and one, or more, transparent actors in LAYER_3D mode.
   * However, it's possible to disable the depth test by calling this method.
   *
   * @SINCE_1_0.0
   * @param[in] disable \e True disables depth test. \e false sets the default behavior
   */
  void SetDepthTestDisabled( bool disable );

  /**
   * @brief Retrieves whether depth test is disabled.
   *
   * @SINCE_1_0.0
   * @return \e True if depth test is disabled
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
   * @param[in] function The sort function pointer
   * @note If the sort function returns a low number, the actor with the data will be
   * drawn in front of an actor whose data yields a high value from the sort function.
   *
   * @note All child layers use the same sort function. If a child layer is added to this
   * layer, then the sort function used by the child layer will also be the same.
   *
  */
  void SetSortFunction( SortFunctionType function );

  /**
   * @brief This allows the user to specify whether this layer should consume touch (including gestures).
   *
   * If set, any layers behind this layer will not be hit-test.
   *
   * @SINCE_1_0.0
   * @param[in] consume Whether the layer should consume touch (including gestures)
   */
  void SetTouchConsumed( bool consume );

  /**
   * @brief Retrieves whether the layer consumes touch (including gestures).
   *
   * @SINCE_1_0.0
   * @return @c True if consuming touch, @c false otherwise
   */
  bool IsTouchConsumed() const;

  /**
   * @brief This allows the user to specify whether this layer should consume hover.
   *
   * If set, any layers behind this layer will not be hit-test.
   *
   * @SINCE_1_0.0
   * @param[in] consume Whether the layer should consume hover
   */
  void SetHoverConsumed( bool consume );

  /**
   * @brief Retrieves whether the layer consumes hover.
   *
   * @SINCE_1_0.0
   * @return @c True if consuming hover, @c false otherwise
   */
  bool IsHoverConsumed() const;

public: // Not intended for application developers

  /**
   * @internal
   * @brief This constructor is used by Layer::New() methods.
   *
   * @SINCE_1_0.0
   * @param[in] Layer A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL Layer(Internal::Layer* Layer);
};

/**
 * @}
 */
} // namespace Dali

#endif // DALI_LAYER_H
