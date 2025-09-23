#ifndef DALI_INTERNAL_DECORATED_VISUAL_RENDERER_H
#define DALI_INTERNAL_DECORATED_VISUAL_RENDERER_H

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
#include <dali/internal/event/rendering/visual-renderer-impl.h>  // Dali::Internal::VisualRenderer
#include <dali/public-api/rendering/decorated-visual-renderer.h> // Dali::DecoratedVisualRenderer

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class Renderer;
}

class DecoratedVisualRenderer;
using DecoratedVisualRendererPtr = IntrusivePtr<DecoratedVisualRenderer>;

/**
 * DecoratedVisualRenderer is a VisualRenderer that has additional default properties for toolkit
 */
class DecoratedVisualRenderer : public VisualRenderer
{
public:
  /**
   * Create a new DecoratedVisualRenderer.
   * @return A smart-pointer to the newly allocated DecoratedVisualRenderer.
   */
  static DecoratedVisualRendererPtr New();

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
   * @copydoc Dali::DecoratedVisualRenderer::RegisterCornerRadiusUniform()
   */
  void RegisterCornerRadiusUniform();

  /**
   * @copydoc Dali::DecoratedVisualRenderer::RegisterCornerSquarenessUniform()
   */
  void RegisterCornerSquarenessUniform();

  /**
   * @copydoc Dali::DecoratedVisualRenderer::RegisterBorderlineUniform()
   */
  void RegisterBorderlineUniform();

  /**
   * @copydoc Dali::DecoratedVisualRenderer::RegisterBlurRadiusUniform()
   */
  void RegisterBlurRadiusUniform();

protected: // implementation
  /**
   * @brief Constructor.
   *
   * @param sceneObject the scene graph renderer
   */
  DecoratedVisualRenderer(const SceneGraph::Renderer* sceneObject);

  /**
   * @brief Retrieves the current value of a default property from the scene-graph.
   * @param[in]  index  The index of the property
   * @param[out] value  Is set with the current scene-graph value of the property
   * @return True if value set, false otherwise.
   */
  bool GetCurrentPropertyValue(Property::Index index, Property::Value& value) const;

  /**
   * @brief Add the uniforms for DecoratedVisualRendererUseType.
   * If that flag didn't appended yet, It will add the uniforms map
   *
   * @param[in] newFlag one of DecoratedVisualRendererUseType
   */
  void AddUniformFlag(uint8_t newAddFlag);

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~DecoratedVisualRenderer() override;

private:
  DecoratedVisualRenderer(const DecoratedVisualRenderer&)            = delete;
  DecoratedVisualRenderer& operator=(const DecoratedVisualRenderer&) = delete;

public:
  enum DecoratedVisualRendererUseType
  {
    DECORATED_VISUAL_RENDERER_USE_CORNER_RADIUS     = 1 << 0,
    DECORATED_VISUAL_RENDERER_USE_CORNER_SQUARENESS = 1 << 1,
    DECORATED_VISUAL_RENDERER_USE_BORDERLINE        = 1 << 2,
    DECORATED_VISUAL_RENDERER_USE_BLUR_RADIUS       = 1 << 3,
  };

  struct DecoratedVisualPropertyCache
  {
    Vector4 mCornerRadius{Vector4::ZERO};
    Vector4 mCornerSquareness{Vector4::ZERO};
    float   mCornerRadiusPolicy{1.0f};
    float   mBorderlineWidth{0.0f};
    Vector4 mBorderlineColor{Color::BLACK};
    float   mBorderlineOffset{0.0f};
    float   mBlurRadius{0.0f};
  };

private:
  DecoratedVisualPropertyCache mDecoratedPropertyCache;

  uint8_t mAddUniformFlag : 5;
};

} // namespace Internal

// Helpers for public-api forwarding methods
inline Internal::DecoratedVisualRenderer& GetImplementation(Dali::DecoratedVisualRenderer& handle)
{
  DALI_ASSERT_ALWAYS(handle && "DecoratedVisualRenderer handle is empty");

  BaseObject& object = handle.GetBaseObject();

  return static_cast<Internal::DecoratedVisualRenderer&>(object);
}

inline const Internal::DecoratedVisualRenderer& GetImplementation(const Dali::DecoratedVisualRenderer& handle)
{
  DALI_ASSERT_ALWAYS(handle && "DecoratedVisualRenderer handle is empty");

  const BaseObject& object = handle.GetBaseObject();

  return static_cast<const Internal::DecoratedVisualRenderer&>(object);
}

} // namespace Dali

#endif // DALI_INTERNAL_DECORATED_VISUAL_RENDERER_H
