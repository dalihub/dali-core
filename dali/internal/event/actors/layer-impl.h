#ifndef __DALI_INTERNAL_LAYER_H__
#define __DALI_INTERNAL_LAYER_H__

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
#include <dali/public-api/actors/layer.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/actors/actor-declarations.h>

namespace Dali
{

namespace Internal
{

class LayerList;

namespace SceneGraph
{
class UpdateManager;
class Layer;
}

typedef Dali::ClippingBox ClippingBox;

class Layer : public Actor
{
public:

  /**
   * @copydoc Dali::Layer::ZValue(const Vector3&, float)
   */
  static float ZValue(const Vector3& position, float sortModifier)
  {
    // inlined so we avoid a function call when sorting renderers
    return position.z + sortModifier;
  }

  /**
   * Create a new Layer.
   * @return A smart-pointer to the newly allocated Layer.
   */
  static LayerPtr New();

  /**
   * Create a new root layer; this is typically owned by the stage.
   * @param[in] stage The owning stage.
   * @param[in] layerList The layer will be added to this ordered list.
   * @param[in] manager The update manager to install a root node with.
   * @param[in] systemLevel True if using the SystemOverlay API; see Integration::GetSystemOverlay() for more details.
   * @return A smart-pointer to the newly allocated Actor.
   */
  static LayerPtr NewRoot( Stage& stage, LayerList& layerList, SceneGraph::UpdateManager& manager, bool systemLevel );

  /**
   * @copydoc Dali::Internal::Actor::OnInitialize
   */
  void OnInitialize();

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
  void RaiseAbove( const Internal::Layer& target );

  /**
   * @copydoc Dali::Layer::LowerBelow
   */
  void LowerBelow( const Internal::Layer& target );

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
  void MoveAbove( const Internal::Layer& target );

  /**
   * @copydoc Dali::Layer::MoveAbove
   */
  void MoveBelow( const Internal::Layer& target );

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
   * @copydoc Dali::Layer::SetDepthTestDisabled()
   */
  void SetDepthTestDisabled( bool disable );

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
  void SetTouchConsumed( bool consume );

  /**
   * @copydoc Dali::Layer::IsTouchConsumed()
   */
  bool IsTouchConsumed() const;

  /**
   * @copydoc Dali::Layer::SetHoverConsumed()
   */
  void SetHoverConsumed( bool consume );

  /**
   * @copydoc Dali::Layer::IsHoverConsumed()
   */
  bool IsHoverConsumed() const;

  /**
   * Helper function to get the scene object.
   * This should only be called by Stage
   * @return the scene object for the layer.
   */
  const SceneGraph::Layer& GetSceneLayerOnStage() const;

  /**
   * @copydoc Dali::Internal::Actor::DoAction()
   */
  static bool DoAction(BaseObject* object, const std::string& actionName, const std::vector<Property::Value>& attributes);

public: // Default property extensions from ProxyObject
  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultPropertyCount()
   */
  virtual unsigned int GetDefaultPropertyCount() const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultPropertyIndices()
   */
  virtual void GetDefaultPropertyIndices( Property::IndexContainer& indices ) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultPropertyName()
   */
  virtual const std::string& GetDefaultPropertyName(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultPropertyIndex()
   */
  virtual Property::Index GetDefaultPropertyIndex(const std::string& name) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::IsDefaultPropertyWritable()
   */
  virtual bool IsDefaultPropertyWritable(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::IsDefaultPropertyAnimatable()
   */
  virtual bool IsDefaultPropertyAnimatable(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::IsDefaultPropertyAConstraintInput()
   */
  virtual bool IsDefaultPropertyAConstraintInput( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultPropertyType()
   */
  virtual Property::Type GetDefaultPropertyType(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::SetDefaultProperty()
   */
  virtual void SetDefaultProperty(Property::Index index, const Property::Value& propertyValue);

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultProperty()
   */
  virtual Property::Value GetDefaultProperty( Property::Index index ) const;

protected:

  /**
   * Construct a new layer.
   * @param[in] type Either Actor::LAYER or Actor::ROOT_LAYER if this is the root actor.
   */
  Layer( Actor::DerivedType type );

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Layer();

private: // From Actor

  /**
   * From Actor; create a node to represent the layer in the scene-graph.
   * @return A newly allocated layer node.
   */
  virtual SceneGraph::Node* CreateNode() const;

  /**
   * From Actor.
   */
  virtual void OnStageConnectionInternal();

  /**
   * From Actor.
   */
  virtual void OnStageDisconnectionInternal();

private:

  LayerList* mLayerList; ///< Only valid when layer is on-stage

  // These properties not animatable; the actor side has the most up-to-date values
  ClippingBox mClippingBox;                     ///< The clipping box, in window coordinates
  Dali::Layer::SortFunctionType mSortFunction;  ///< Used to sort semi-transparent geometry

  bool mIsClipping:1;                           ///< True when clipping is enabled
  bool mDepthTestDisabled:1;                    ///< Whether depth test is disabled.
  bool mTouchConsumed:1;                        ///< Whether we should consume touch (including gesture).
  bool mHoverConsumed:1;                        ///< Whether we should consume hover.

  static bool mFirstInstance;
  static DefaultPropertyLookup* mDefaultLayerPropertyLookup; ///< Default properties
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


#endif //__DALI_INTERNAL_LAYER_H__
