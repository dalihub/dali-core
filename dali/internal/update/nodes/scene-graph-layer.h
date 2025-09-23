#ifndef DALI_INTERNAL_SCENE_GRAPH_LAYER_H
#define DALI_INTERNAL_SCENE_GRAPH_LAYER_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/public-api/actors/layer.h>

namespace Dali
{
namespace Internal
{
// value types used by messages
template<>
struct ParameterType<Dali::Layer::SortFunctionType>
: public BasicType<Dali::Layer::SortFunctionType>
{
};
template<>
struct ParameterType<Dali::Layer::Behavior>
: public BasicType<Dali::Layer::Behavior>
{
};

namespace SceneGraph
{
class Camera;

/**
 * Pair of node-renderer
 */
struct Renderable
{
  Renderable() = default;

  Renderable(Node* node, RendererKey renderer)
  : mNode(node),
    mRenderer(renderer)
  {
  }

  Node*       mNode{nullptr};
  RendererKey mRenderer{};
};

} // namespace SceneGraph
} // Namespace Internal

/// Enable Renderable to be used as a trivial type in Dali::Vector.
template<>
struct TypeTraits<Internal::SceneGraph::Renderable> : public Dali::BasicTypes<Internal::SceneGraph::Renderable>
{
  enum
  {
    IS_TRIVIAL_TYPE = true
  };
};

namespace Internal
{
namespace SceneGraph
{
using RenderableContainer = Dali::Vector<Renderable>;

/**
 * Layers have a "depth" relative to all other layers in the scene-graph.
 * Non-layer child nodes are considered part of the layer.
 *
 * Layers are rendered separately, and by default the depth buffer is cleared before each layer is rendered.
 * Objects in higher layers, are rendered after (in front of) objects in lower layers.
 */
class Layer : public Node
{
public:
  using SortFunctionType = Dali::Layer::SortFunctionType;

  // Creation methods

  /**
   * Construct a new Layer.
   * @return A smart-pointer to a newly allocated Node
   */
  static SceneGraph::Layer* New();

  /**
   * Virtual destructor
   */
  ~Layer() override;

  /**
   * From Node, to convert a node to a layer.
   * @return The layer.
   */
  Layer* GetLayer() override
  {
    return this;
  }

  /**
   * Sets the sort-function of a layer.
   * @param [in] function The new sort-function.
   */
  void SetSortFunction(Dali::Layer::SortFunctionType function);

  /**
   * Retrieve the function used to sort semi-transparent geometry in this layer.
   * @return The sort function.
   */
  Dali::Layer::SortFunctionType GetSortFunction() const
  {
    return mSortFunction;
  }

  /**
   * Sets whether clipping is enabled for a layer.
   * @param [in] enabled True if clipping is enabled.
   */
  void SetClipping(bool enabled);

  /**
   * Query whether clipping is enabled for a layer.
   * @return True if clipping is enabled.
   */
  bool IsClipping() const
  {
    return mIsClipping;
  }

  /**
   * Sets the clipping box of a layer, in window coordinates.
   * The contents of the layer will not be visible outside this box, when clipping is
   * enabled. The default clipping box is empty (0,0,0,0).
   * @param [in] box The clipping box
   */
  void SetClippingBox(const ClippingBox& box);

  /**
   * Retrieves the clipping box of a layer, in window coordinates.
   * @return The clipping box
   */
  const ClippingBox& GetClippingBox() const
  {
    return mClippingBox;
  }

  /**
   * Sets the behavior of the layer
   * @param [in] behavior The behavior of the layer
   */
  void SetBehavior(Dali::Layer::Behavior behavior);

  /**
   * Retrieves the behavior of the layer.
   * @return The behavior
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
   * Enables the reuse of the model view matrices of all renderers for this layer
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] value to set
   */
  void SetReuseRenderers(BufferIndex updateBufferIndex, bool value)
  {
    mAllChildTransformsClean[updateBufferIndex] = value;
  }

  /**
   * Get the reuse of the model view matrices of all renderers for this layer is enabled.
   * @param[in] updateBufferIndex The current update buffer index.
   * @return Whether all child transform was clean or not.
   */
  bool GetReuseRenderers(BufferIndex updateBufferIndex) const
  {
    return mAllChildTransformsClean[updateBufferIndex];
  }

  /**
   * Checks if it is ok to reuse renderers. Renderers can be reused if ModelView transform for all the renderers
   * has not changed from previous use.
   * @param[in] camera A pointer to the camera that we want to use to render the list.
   * @return True if all children transforms have been clean for two consecutive frames and the camera we are going
   * to use is the same than the one used before ( Otherwise View transform will be different )
   *
   */
  bool CanReuseRenderers(const Camera* camera)
  {
    bool bReturn(mAllChildTransformsClean[0] && mAllChildTransformsClean[1] && camera == mLastCamera);
    mLastCamera = camera;

    return bReturn;
  }

  /**
   * @return True if default sort function is used
   */
  bool UsesDefaultSortFunction()
  {
    return mIsDefaultSortFunction;
  }

  /**
   * Clears all the renderable lists
   */
  void ClearRenderables();

private:
  /**
   * Private constructor.
   * See also Layer::New()
   */
  Layer();

  // Delete copy and move
  Layer(const Layer&)                = delete;
  Layer(Layer&&)                     = delete;
  Layer& operator=(const Layer& rhs) = delete;
  Layer& operator=(Layer&& rhs)      = delete;

public: // For update-algorithms
  RenderableContainer colorRenderables;
  RenderableContainer overlayRenderables;

private:
  SortFunctionType mSortFunction; ///< Used to sort semi-transparent geometry

  ClippingBox   mClippingBox; ///< The clipping box, in window coordinates
  const Camera* mLastCamera;  ///< Pointer to the last camera that has rendered the layer

  Dali::Layer::Behavior mBehavior; ///< The behavior of the layer

  bool mAllChildTransformsClean[2]; ///< True if all child nodes transforms are clean,
                                    ///  double buffered as we need two clean frames before we can reuse N-1 for N+1
                                    ///  this allows us to cache render items when layer is "static"
  bool mIsClipping : 1;             ///< True when clipping is enabled
  bool mDepthTestDisabled : 1;      ///< Whether depth test is disabled.
  bool mIsDefaultSortFunction : 1;  ///< whether the default depth sort function is used
};

// Messages for Layer

/**
 * Create a message to set the sort-function of a layer
 * @param[in] layer The layer
 * @param[in] function The new sort-function.
 */
inline void SetSortFunctionMessage(EventThreadServices& eventThreadServices, const Layer& layer, Dali::Layer::SortFunctionType function)
{
  using LocalType = MessageValue1<Layer, Dali::Layer::SortFunctionType>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&layer, &Layer::SetSortFunction, function);
}

/**
 * Create a message for enabling/disabling layer clipping
 * @param[in] layer The layer
 * @param[in] enabled True if clipping is enabled
 */
inline void SetClippingMessage(EventThreadServices& eventThreadServices, const Layer& layer, bool enabled)
{
  using LocalType = MessageValue1<Layer, bool>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&layer, &Layer::SetClipping, enabled);
}

/**
 * Create a message to set the clipping box of a layer
 * @param[in] layer The layer
 * @param[in] clippingbox The clipping box
 */
inline void SetClippingBoxMessage(EventThreadServices& eventThreadServices, const Layer& layer, const Dali::ClippingBox& clippingbox)
{
  using LocalType = MessageValue1<Layer, Dali::ClippingBox>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&layer, &Layer::SetClippingBox, clippingbox);
}

/**
 * Create a message to set the behavior of a layer
 * @param[in] layer The layer
 * @param[in] behavior The behavior
 */
inline void SetBehaviorMessage(EventThreadServices&  eventThreadServices,
                               const Layer&          layer,
                               Dali::Layer::Behavior behavior)
{
  using LocalType = MessageValue1<Layer, Dali::Layer::Behavior>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&layer, &Layer::SetBehavior, behavior);
}

/**
 * Create a message for disabling/enabling depth test.
 *
 * @see Dali::Layer::SetDepthTestDisabled().
 *
 * @param[in] layer The layer
 * @param[in] disable \e true disables depth test. \e false sets the default behavior.
 */
inline void SetDepthTestDisabledMessage(EventThreadServices& eventThreadServices, const Layer& layer, bool disable)
{
  using LocalType = MessageValue1<Layer, bool>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&layer, &Layer::SetDepthTestDisabled, disable);
}

} // namespace SceneGraph

// Template specialisation for OwnerPointer<Layer>, because delete is protected
template<>
inline void OwnerPointer<Dali::Internal::SceneGraph::Layer>::Reset()
{
  if(mObject != nullptr)
  {
    Dali::Internal::SceneGraph::Node::Delete(mObject);
    mObject = nullptr;
  }
}
} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_LAYER_H
