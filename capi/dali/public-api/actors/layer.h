#ifndef __DALI_LAYER_H__
#define __DALI_LAYER_H__

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

/**
 * @addtogroup CAPI_DALI_ACTORS_MODULE
 * @{
 */

// INTERNAL INCLUDES
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/images/frame-buffer-image.h>

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{
class Layer;
}

/**
 * @brief Rectangle describing area on screen that a layer can draw to.
 *
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
 */
class DALI_IMPORT_API Layer : public Actor
{
public:

  // Default Properties additional to Actor
  static const Property::Index CLIPPING_ENABLE; ///< name "clipping-enable",  type BOOLEAN
  static const Property::Index CLIPPING_BOX;    ///< name "clipping-box",     type RECTANGLE

  // Action Names
  static const char* const ACTION_RAISE;           ///< name "raise"
  static const char* const ACTION_LOWER;           ///< name "lower"
  static const char* const ACTION_RAISE_TO_TOP;    ///< name "raise-to-top"
  static const char* const ACTION_LOWER_TO_BOTTOM; ///< name "lower-to-bottom"

  /**
   * @brief The sort function type.
   *
   * The position value is the actor translation from camera.
   * The sortModifier is the user value that can be used to sort coplanar actors/nodes. This value is
   * the one set by calling RenderableActor::SetSortModifier().
   *
   * A high return value means that the actor will be positioned further away by the sort algorithm.
   * @see RenderableActor::SetSortModifier
   */
  typedef float (*SortFunctionType)(const Vector3& position, float sortModifier);

  /**
   * @brief Create an empty Layer handle.
   *
   * This can be initialised with Layer::New(...)
   */
  Layer();

  /**
   * @brief Create a Layer object.
   *
   * @return A handle to a newly allocated Layer
   */
  static Layer New();

  /**
   * @brief Downcast an Object handle to Layer.
   *
   * If handle points to a Layer the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   * @param[in] handle to An object
   * @return handle to a Layer or an uninitialized handle
   */
  static Layer DownCast( BaseHandle handle );

  /**
   * @brief Virtual destructor.
   *
   * Dali::Object derived classes typically do not contain member data.
   */
  virtual ~Layer();

  /**
   * @copydoc Dali::BaseHandle::operator=
   */
  using BaseHandle::operator=;

  /**
   * @brief Query the depth of the layer
   *
   * 0 is bottom most layer, higher number is on top
   * @pre layer is on the stage
   * If layer is not added to the stage, returns 0.
   * @return the current depth of the layer.
   */
  unsigned int GetDepth() const;

  /**
   * @brief Increment the depth of the layer.
   *
   * @pre layer is on the stage
   */
  void Raise();

  /**
   * @brief Decrement the depth of the layer.
   *
   * @pre layer is on the stage
   */
  void Lower();

  /**
   * @brief Ensures the layers depth is greater than the target layer.
   *
   * If the layer already is above target layer its depth is not changed
   * If the layer was below target, its new depth will be immediately above target
   * Note! All layers between this layer and target get new depth values
   * @pre layer is on the stage
   * @pre target layer is on the stage
   * @param target layer to get above of
   */
  void RaiseAbove( Layer target );

  /**
   * @brief Ensures the layers depth is less than the target layer.
   *
   * If the layer already is below the layer its depth is not changed
   * If the layer was above target, its new depth will be immediately below target
   * Note! All layers between this layer and target get new depth values
   * @pre layer is on the stage
   * @pre target layer is on the stage
   * @param target layer to get below of
   */
  void LowerBelow( Layer target );

  /**
   * @brief Raises the layer to the top.
   * @pre layer is on the stage
   */
  void RaiseToTop();

  /**
   * @brief Lowers the layer to the bottom.
   * @pre layer is on the stage
   */
  void LowerToBottom();

  /**
   * @brief Moves the layer directly above the given layer.
   *
   * After the call this layers depth will be immediately above target
   * Note! All layers between this layer and target get new depth values
   * @pre layer is on the stage
   * @pre target layer is on the stage
   * @param target layer to get on top of
   */
  void MoveAbove( Layer target );

  /**
   * @brief Moves the layer directly below the given layer.
   *
   * After the call this layers depth will be immediately below target
   * Note! All layers between this layer and target get new depth values
   * @pre layer is on the stage
   * @pre target layer is on the stage
   * @param target layer to get below of
   */
  void MoveBelow( Layer target );

  /**
   * @brief Sets whether clipping is enabled for a layer.
   *
   * Clipping is initially disabled; see also SetClippingBox().
   * @param [in] enabled True if clipping is enabled.
   */
  void SetClipping(bool enabled);

  /**
   * @brief Query whether clipping is enabled for a layer.
   * @return True if clipping is enabled.
   */
  bool IsClipping() const;

  /**
   * @brief Sets the clipping box of a layer, in window coordinates.
   *
   * The contents of the layer will not be visible outside this box, when clipping is
   * enabled. The default clipping box is empty (0,0,0,0).
   * This has the limitation that it only applies to rectangles on a window.
   * For other kinds of clipping, @see Dali::Actor::SetDrawMode().
   * @param [in] x The X-coordinate of the lower-left corner.
   * @param [in] y The Y-coordinate of the lower-left corner.
   * @param [in] width  The width of the box.
   * @param [in] height The height of the box.
   */
  void SetClippingBox(int x, int y, int width, int height);

  /**
   * @brief Sets the clipping box of a layer, in window coordinates.
   *
   * The contents of the layer will not be visible outside this box, when clipping is
   * enabled. The default clipping box is empty (0,0,0,0).
   * @param [in] box The clipping box
   */
  void SetClippingBox(ClippingBox box);

  /**
   * @brief Retrieves the clipping box of a layer, in window coordinates.
   *
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
   * @param[in] disable \e true disables depth test. \e false sets the default behaviour.
   */
  void SetDepthTestDisabled( bool disable );

  /**
   * @brief Retrieves whether depth test is disabled.
   *
   * @return \e true if depth test is disabled.
   */
  bool IsDepthTestDisabled() const;

  // Sorting

  /**
   * @brief This sort function sorts translucent actors according to the Z-value in view space.
   *
   * This is useful for 2D user interfaces.
   *
   * This is the default sorting function.
   *
   * We return a negative z value as in our translation, a low z means that it should
   * be sorted further away and a high z means that it should be closer.
   * @param[in] position     position of actor in view space
   * @param[in] sortModifier additional sort modifer
   * @return depth
   */
  static float ZValue(const Vector3& position, float sortModifier);

  /**
   * @brief This allows the user to specify the sort function that the layer should use.
   *
   * The sort function is used to determine the order in which the actors are drawn
   * and input is processed on the actors in the layer.
   *
   * A function of the following type should be used:
   * @code
   *  float YourSortFunction(const Vector3& position, float sortModifier);
   * @endcode
   *
   * @note If the sort function returns a low number, the actor the data applies to will be
   * drawn in front of an actor whose data yields a high value from the sort function.
   *
   * @note All child layers use the same sort function.  If a child layer is added to this
   * layer then the sort function used by the child layer will also be the same.
   *
   * @param[in]  function  The sort function pointer
  */
  void SetSortFunction( SortFunctionType function );

  /**
   * @brief This allows the user to specify whether this layer should consume touch (including gestures).
   *
   * If set, any layers behind this layer will not be hit-test.
   *
   * @param[in]  consume  Whether the layer should consume touch (including gestures).
   */
  void SetTouchConsumed( bool consume );

  /**
   * @brief Retrieves whether the layer consumes touch (including gestures).
   *
   * @return true if consuming touch, false otherwise.
   */
  bool IsTouchConsumed() const;

public: // Not intended for application developers

  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @param [in] Layer A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL Layer(Internal::Layer* Layer);
};

} // namespace Dali

/**
 * @}
 */
#endif //__DALI_LAYER_H__
