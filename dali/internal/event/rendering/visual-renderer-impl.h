#ifndef DALI_INTERNAL_VISUAL_RENDERER_H
#define DALI_INTERNAL_VISUAL_RENDERER_H

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
#include <dali/internal/event/rendering/renderer-impl.h> // Dali::Internal::Renderer
#include <dali/public-api/rendering/visual-renderer.h>   // Dali::VisualRenderer

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class Renderer;
}

class VisualRenderer;
using VisualRendererPtr = IntrusivePtr<VisualRenderer>;

/**
 * VisualRenderer is a Renderer that has additional default properties for toolkit
 */
class VisualRenderer : public Dali::Internal::Renderer
{
public:
  /**
   * Create a new VisualRenderer.
   * @return A smart-pointer to the newly allocated VisualRenderer.
   */
  static VisualRendererPtr New();

  /**
   * @brief Get the scene graph object
   *
   * @return the scene object
   */
  const SceneGraph::Renderer& GetVisualRendererSceneObject() const;

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
   * @copydoc Dali::Internal::Object::GetDefaultPropertyCurrentValue()
   */
  Property::Value GetDefaultPropertyCurrentValue(Property::Index index) const override;

  /**
   * @copydoc Dali::Internal::Object::OnNotifyDefaultPropertyAnimation()
   */
  void OnNotifyDefaultPropertyAnimation(Animation& animation, Property::Index index, const Property::Value& value, Animation::Type animationType) override;

  /**
   * @copydoc Dali::Internal::Object::GetSceneObjectAnimatableProperty()
   */
  const SceneGraph::PropertyBase* GetSceneObjectAnimatableProperty(Property::Index index) const override;

  /**
   * @copydoc Dali::Internal::Object::GetSceneObjectInputProperty()
   */
  const PropertyInputImpl* GetSceneObjectInputProperty(Property::Index index) const override;

public:
  /**
   * @copydoc Dali::VisualRenderer::RegisterVisualTransformUniform()
   */
  void RegisterVisualTransformUniform();

protected: // implementation
  /**
   * @brief Constructor.
   *
   * @param sceneObject the scene graph renderer
   */
  VisualRenderer(const SceneGraph::Renderer* sceneObject);

  /**
   * @brief Retrieves the current value of a default property from the scene-graph.
   * @param[in]  index  The index of the property
   * @param[out] value  Is set with the current scene-graph value of the property
   * @return True if value set, false otherwise.
   */
  bool GetCurrentPropertyValue(Property::Index index, Property::Value& value) const;

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~VisualRenderer() override;

private:
  VisualRenderer(const VisualRenderer&)            = delete;
  VisualRenderer& operator=(const VisualRenderer&) = delete;

public:
  struct VisualPropertyCache
  {
    Vector2 mTransformOffset{Vector2::ZERO};
    Vector2 mTransformSize{Vector2::ONE};
    Vector2 mTransformOrigin{Vector2::ZERO};
    Vector2 mTransformAnchorPoint{Vector2::ZERO};
    Vector4 mTransformOffsetSizeMode{Vector2::ZERO};
    Vector2 mExtraSize{Vector2::ZERO};
    float   mPreMultipliedAlpha{0.0f};
  };

private:
  VisualPropertyCache mPropertyCache;

  bool mUniformMapped : 1;
};

} // namespace Internal

// Helpers for public-api forwarding methods
inline Internal::VisualRenderer& GetImplementation(Dali::VisualRenderer& handle)
{
  DALI_ASSERT_ALWAYS(handle && "VisualRenderer handle is empty");

  BaseObject& object = handle.GetBaseObject();

  return static_cast<Internal::VisualRenderer&>(object);
}

inline const Internal::VisualRenderer& GetImplementation(const Dali::VisualRenderer& handle)
{
  DALI_ASSERT_ALWAYS(handle && "VisualRenderer handle is empty");

  const BaseObject& object = handle.GetBaseObject();

  return static_cast<const Internal::VisualRenderer&>(object);
}

} // namespace Dali

#endif // DALI_INTERNAL_VISUAL_RENDERER_H
