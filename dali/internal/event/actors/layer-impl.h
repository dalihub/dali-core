#ifndef DALI_INTERNAL_LAYER_H
#define DALI_INTERNAL_LAYER_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/actors/actor-declarations.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/public-api/actors/layer.h>

namespace Dali
{
namespace Internal
{
class LayerList;

namespace SceneGraph
{
class UpdateManager;
class Layer;
} // namespace SceneGraph

using ClippingBox = Dali::ClippingBox;

class Layer : public Actor, public ConnectionTracker
{
public:
  /**
   * @copydoc Dali::Layer::ZValue(const Vector3&, float)
   *
   * This is the default sorting function.
   * It is useful for 2D user interfaces, and it's used to sort translucent renderers.
   *
   * Only the Z signed distance from the camera is considererd, lower values will be drawn on top.
   *
   * @param[in] position     position of actor in view space
   * @return depth
   */
  static float ZValue(const Vector3& position)
  {
    // inlined so we avoid a function call when sorting renderers
    return position.z;
  }

  /**
   * Create a new Layer.
   * @return A smart-pointer to the newly allocated Layer.
   */
  static LayerPtr New();

  /**
   * Create a new root layer; this is typically owned by the stage.
   * @param[in] layerList The layer will be added to this ordered list.
   * @return A smart-pointer to the newly allocated Actor.
   */
  static LayerPtr NewRoot(LayerList& layerList);

  /**
   * @copydoc Dali::Internal::Actor::OnInitialize
   */
  void OnInitialize() override;

  /**
   * Query the current depth of the layer
   */
  unsigned int GetDepth() const;

  /**
   * @copydoc Dali::Layer::Raise
   */
  void Raise();

  /**
   * @copydoc Dali::Layer::Lower
   */
  void Lower();

  /**
   * @copydoc Dali::Layer::RaiseAbove
   */
  void RaiseAbove(const Internal::Layer& target);

  /**
   * @copydoc Dali::Layer::LowerBelow
   */
  void LowerBelow(const Internal::Layer& target);

  /**
   * @copydoc Dali::Layer::RaiseToTop
   */
  void RaiseToTop();

  /**
   * @copydoc Dali::Layer::LowerToBottom
   */
  void LowerToBottom();

  /**
   * @copydoc Dali::Layer::MoveAbove
   */
  void MoveAbove(const Internal::Layer& target);

  /**
   * @copydoc Dali::Layer::MoveAbove
   */
  void MoveBelow(const Internal::Layer& target);

  /**
   * @copydoc Dali::Layer::SetClipping()
   */
  void SetClipping(bool enabled);

  /**
   * @copydoc Dali::Layer::IsClipping()
   */
  bool IsClipping() const
  {
    return mIsClipping; // Actor-side has most up-to-date value
  }

  /**
   * @copydoc Dali::Layer::SetClippingBox()
   */
  void SetClippingBox(int x, int y, int width, int height);

  /**
   * @copydoc Dali::Layer::GetClippingBox()
   */
  const Dali::ClippingBox& GetClippingBox() const
  {
    return mClippingBox; // Actor-side has most up-to-date value
  }

  /**
   * @copydoc Dali::Layer::SetBehavior()
   */
  void SetBehavior(Dali::Layer::Behavior behavior);

  /**
   * @copydoc Dali::Layer::GetBehavior()
   */
  Dali::Layer::Behavior GetBehavior() const
  {
    return mBehavior;
  }

  /**
   * @copydoc Dali::Layer::SetDepthTestDisabled()
   */
  void SetDepthTestDisabled(bool disable);

  /**
   * @copydoc Dali::Layer::IsDepthTestDisabled()
   */
  bool IsDepthTestDisabled() const;

  /**
   * @copydoc Dali::Layer::SetSortFunction()
   */
  void SetSortFunction(Dali::Layer::SortFunctionType function);

  /**
   * @copydoc Dali::Layer::SetTouchConsumed()
   */
  void SetTouchConsumed(bool consume);

  /**
   * @copydoc Dali::Layer::IsTouchConsumed()
   */
  bool IsTouchConsumed() const;

  /**
   * @copydoc Dali::Layer::SetHoverConsumed()
   */
  void SetHoverConsumed(bool consume);

  /**
   * @copydoc Dali::Layer::IsHoverConsumed()
   */
  bool IsHoverConsumed() const;

  /**
   * Helper function to get the scene object.
   *
   * @return the scene object for the layer.
   */
  const SceneGraph::Layer& GetSceneGraphLayer() const;

  /**
   * @copydoc Dali::Internal::Actor::DoAction()
   */
  static bool DoAction(BaseObject* object, const std::string& actionName, const Property::Map& attributes);

public: // Default property extensions from Object
  /**
   * @copydoc Dali::Internal::Object::SetDefaultProperty()
   */
  void SetDefaultProperty(Property::Index index, const Property::Value& propertyValue) override;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultProperty()
   */
  Property::Value GetDefaultProperty(Property::Index index) const override;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultProperty()
   */
  Property::Value GetDefaultPropertyCurrentValue(Property::Index index) const override;

protected:
  /**
   * Construct a new layer.
   * @param[in] type Either Actor::LAYER or Actor::ROOT_LAYER if this is the root actor.
   * @param[in] layer the scene graph layer
   */
  Layer(Actor::DerivedType type, const SceneGraph::Layer& layer);

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~Layer() override;

private: // From Actor
  /**
   * From Actor.
   */
  void OnSceneConnectionInternal() override;

  /**
   * From Actor.
   */
  void OnSceneDisconnectionInternal() override;

private:
  /**
   * @brief Callback when Layer is touched
   *
   * @param[in] actor Layer touched
   * @param[in] touch Touch information
   * @return True if the touch is consumed.
   */
  bool OnTouched(Dali::Actor actor, const Dali::TouchEvent& touch);

  LayerList* mLayerList; ///< Only valid when layer is on-scene

  // These properties not animatable; the actor side has the most up-to-date values
  ClippingBox                   mClippingBox;  ///< The clipping box, in window coordinates
  Dali::Layer::SortFunctionType mSortFunction; ///< Used to sort semi-transparent geometry

  Dali::Layer::Behavior mBehavior; ///< Behavior of the layer

  bool mIsClipping : 1;        ///< True when clipping is enabled
  bool mDepthTestDisabled : 1; ///< Whether depth test is disabled.
  bool mTouchConsumed : 1;     ///< Whether we should consume touch (including gesture).
  bool mHoverConsumed : 1;     ///< Whether we should consume hover.
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::Layer& GetImplementation(Dali::Layer& layer)
{
  DALI_ASSERT_ALWAYS(layer && "Layer handle is empty");

  BaseObject& handle = layer.GetBaseObject();

  return static_cast<Internal::Layer&>(handle);
}

inline const Internal::Layer& GetImplementation(const Dali::Layer& layer)
{
  DALI_ASSERT_ALWAYS(layer && "Layer handle is empty");

  const BaseObject& handle = layer.GetBaseObject();

  return static_cast<const Internal::Layer&>(handle);
}

} // namespace Dali

#endif // DALI_INTERNAL_LAYER_H
