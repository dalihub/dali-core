#ifndef DALI_INTERNAL_RENDERER_H
#define DALI_INTERNAL_RENDERER_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/internal/common/blending-options.h>
#include <dali/internal/event/common/object-connector.h>    // Dali::Internal::ObjectConnector
#include <dali/internal/event/common/object-impl.h>         // Dali::Internal::Object
#include <dali/internal/event/rendering/geometry-impl.h>    // Dali::Internal::Geometry
#include <dali/internal/event/rendering/texture-set-impl.h> // Dali::Internal::TextureSet
#include <dali/internal/render/renderers/render-renderer.h> // Dali::Render::Renderer::StencilParameters
#include <dali/public-api/common/dali-common.h>             // DALI_ASSERT_ALWAYS
#include <dali/public-api/common/intrusive-ptr.h>           // Dali::IntrusivePtr
#include <dali/public-api/rendering/renderer.h>             // Dali::Renderer

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class Renderer;
}

class Renderer;
using RendererPtr = IntrusivePtr<Renderer>;

/**
 * Renderer is an object that can be used to show content by combining a Geometry with a shader and textures.
 */
class Renderer : public Object
{
public:
  /**
   * Create a new Renderer.
   * @return A smart-pointer to the newly allocated Renderer.
   */
  static RendererPtr New();

  /**
   * @copydoc Dali::Renderer::SetGeometry()
   */
  void SetGeometry(Geometry& geometry);

  /**
   * @copydoc Dali::Renderer::GetGeometry()
   */
  GeometryPtr GetGeometry() const;

  /**
   * @copydoc Dali::Renderer::SetTextures()
   */
  void SetTextures(TextureSet& textureSet);

  /**
   * @copydoc Dali::Renderer::GetTextures()
   */
  TextureSetPtr GetTextures() const;

  /**
   * @copydoc Dali::Renderer::SetShader()
   */
  void SetShader(Shader& shader);

  /**
   * @copydoc Dali::Renderer::GetShader()
   */
  ShaderPtr GetShader() const;

  /**
   * @copydoc Dali::Renderer::SetDepthIndex()
   */
  void SetDepthIndex(int32_t depthIndex);

  /**
   * @copydoc Dali::Renderer::GetDepthIndex()
   */
  int32_t GetDepthIndex() const;

  /**
   * @copydoc Dali::Renderer::SetBlendMode()
   */
  void SetBlendMode(BlendMode::Type mode);

  /**
   * @copydoc Dali::Renderer::GetBlendMode()
   */
  BlendMode::Type GetBlendMode() const;

  /**
   * @copydoc Dali::Renderer::SetBlendFunc()
   */
  void SetBlendFunc(BlendFactor::Type srcFactorRgba, BlendFactor::Type destFactorRgba);

  /**
   * @copydoc Dali::Renderer::SetBlendFunc()
   */
  void SetBlendFunc(BlendFactor::Type srcFactorRgb, BlendFactor::Type destFactorRgb, BlendFactor::Type srcFactorAlpha, BlendFactor::Type destFactorAlpha);

  /**
   * @copydoc Dali::Renderer::GetBlendFunc()
   */
  void GetBlendFunc(BlendFactor::Type& srcFactorRgb, BlendFactor::Type& destFactorRgb, BlendFactor::Type& srcFactorAlpha, BlendFactor::Type& destFactorAlpha) const;

  /**
   * @brief Set same Blend Equation for the RGB and alpha
   */
  void SetBlendEquation(DevelBlendEquation::Type equationRgba);

  /**
   * @brief Set Blend Equation separately for the RGB and alpha
   */
  void SetBlendEquation(DevelBlendEquation::Type equationRgb, DevelBlendEquation::Type equationAlpha);

  /**
   * @brief Get Blend Equation of rgb and alpha
   */
  void GetBlendEquation(DevelBlendEquation::Type& equationRgb, DevelBlendEquation::Type& equationAlpha) const;

  /**
   * @copydoc Dali::Renderer::SetIndexedDrawFirstElement
   */
  void SetIndexedDrawFirstElement(uint32_t firstElement);

  /**
   * @copydoc Dali::Renderer::SetIndexedDrawElementsCount
   */
  void SetIndexedDrawElementsCount(uint32_t elementsCount);

  /**
   * @brief Set whether the Pre-multiplied Alpha Blending is required
   *
   * @param[in] preMultipled whether alpha is pre-multiplied.
   */
  void EnablePreMultipliedAlpha(bool preMultipled);

  /**
   * @brief Query whether alpha is pre-multiplied.
   *
   * @return True is alpha is pre-multiplied, false otherwise.
   */
  bool IsPreMultipliedAlphaEnabled() const;

  /**
   * @brief Query whether Blend Equation Advanced is used in this renderer
   *
   * @return True is Blend Equation Advanced is used, false otherwise.
   */
  bool IsAdvancedBlendEquationApplied() const;

  /**
   * @brief Get the scene graph object
   *
   * @return the scene object
   */
  const SceneGraph::Renderer& GetRendererSceneObject() const;

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

  /**
   * @brief Adds a draw command to the Renderer.
   * DrawCommands override Renderer's default behaviour.
   *
   * @param[in] command Valid reference to a DrawCommand objects
   */
  void AddDrawCommand(const Dali::DevelRenderer::DrawCommand& command);

  /**
   * @copydoc Dali::Renderer::SetRenderCallback()
   */
  void SetRenderCallback(RenderCallback* callback);

protected: // implementation
  /**
   * @brief Constructor.
   *
   * @param sceneObject the scene graph renderer
   */
  Renderer(const SceneGraph::Renderer* sceneObject);

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~Renderer() override;

private:
  Renderer(const Renderer&) = delete;            ///< Deleted copy constructor
  Renderer& operator=(const Renderer&) = delete; ///< Deleted assignment operator

  /**
   * @brief Retrieves the cached event side value of a default property.
   * @param[in]  index  The index of the property
   * @param[out] value  Is set with the cached value of the property if found.
   * @return True if value set, false otherwise.
   */
  bool GetCachedPropertyValue(Property::Index index, Property::Value& value) const;

  /**
   * @brief Retrieves the current value of a default property from the scene-graph.
   * @param[in]  index  The index of the property
   * @param[out] value  Is set with the current scene-graph value of the property
   * @return True if value set, false otherwise.
   */
  bool GetCurrentPropertyValue(Property::Index index, Property::Value& value) const;

  /**
   * @brief Sets the blend color.
   * @param[in] blendColor The blend color to set.
   */
  void SetBlendColor(const Vector4& blendColor);

  /**
   * @brief Retrieves the blend-color.
   * @return A const reference to the blend-color
   */
  const Vector4& GetBlendColor() const;

protected:
  GeometryPtr   mGeometry;   ///< Intrusive pointer to the geometry used by this renderer
  TextureSetPtr mTextureSet; ///< Intrusive pointer to the texture set used by this renderer
  ShaderPtr     mShader;     ///< Intrusive pointer to the shader used by this renderer

  int32_t mDepthIndex;

  uint32_t mIndexedDrawFirstElement; ///< Offset of first element to draw from bound index buffer
  uint32_t mIndexedDrawElementCount; ///< Number of elements to draw

  Render::Renderer::StencilParameters mStencilParameters; ///< Struct containing all stencil related options
  BlendingOptions                     mBlendingOptions;   ///< Local copy of blending options bitmask

  float                          mOpacity;                      ///< Local copy of the opacity
  DepthFunction::Type            mDepthFunction : 4;            ///< Local copy of the depth function
  FaceCullingMode::Type          mFaceCullingMode : 3;          ///< Local copy of the mode of face culling
  BlendMode::Type                mBlendMode : 3;                ///< Local copy of the mode of blending
  DepthWriteMode::Type           mDepthWriteMode : 3;           ///< Local copy of the depth write mode
  DepthTestMode::Type            mDepthTestMode : 3;            ///< Local copy of the depth test mode
  DevelRenderer::Rendering::Type mRenderingBehavior : 2;        ///< The rendering behavior
  bool                           mPremultipledAlphaEnabled : 1; ///< Flag indicating whether the Pre-multiplied Alpha Blending is required

private:
  std::vector<Dali::DevelRenderer::DrawCommand> mDrawCommands; ///< list of draw commands
};

} // namespace Internal

// Helpers for public-api forwarding methods
inline Internal::Renderer& GetImplementation(Dali::Renderer& handle)
{
  DALI_ASSERT_ALWAYS(handle && "Renderer handle is empty");

  BaseObject& object = handle.GetBaseObject();

  return static_cast<Internal::Renderer&>(object);
}

inline const Internal::Renderer& GetImplementation(const Dali::Renderer& handle)
{
  DALI_ASSERT_ALWAYS(handle && "Renderer handle is empty");

  const BaseObject& object = handle.GetBaseObject();

  return static_cast<const Internal::Renderer&>(object);
}

} // namespace Dali

#endif // DALI_INTERNAL_RENDERER_H
