#ifndef DALI_INTERNAL_RENDERER_H
#define DALI_INTERNAL_RENDERER_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/dali-common.h> // DALI_ASSERT_ALWAYS
#include <dali/public-api/common/intrusive-ptr.h> // Dali::IntrusivePtr
#include <dali/public-api/rendering/renderer.h> // Dali::Renderer
#include <dali/internal/common/blending-options.h>
#include <dali/internal/event/common/object-connector.h> // Dali::Internal::ObjectConnector
#include <dali/internal/event/common/object-impl.h> // Dali::Internal::Object
#include <dali/internal/event/rendering/texture-set-impl.h> // Dali::Internal::TextureSet
#include <dali/internal/event/rendering/geometry-impl.h> // Dali::Internal::Geometry
#include <dali/internal/render/renderers/render-renderer.h> // Dali::Render::Renderer::StencilParameters

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class Renderer;
}

class Renderer;
typedef IntrusivePtr<Renderer> RendererPtr;

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
  void SetGeometry( Geometry& geometry );

  /**
   * @copydoc Dali::Renderer::GetGeometry()
   */
  Geometry* GetGeometry() const;

  /**
   * @copydoc Dali::Renderer::SetTextures()
   */
  void SetTextures( TextureSet& textureSet );

  /**
   * @copydoc Dali::Renderer::GetTextures()
   */
  TextureSet* GetTextures() const;

  /**
   * @copydoc Dali::Renderer::SetShader()
   */
  void SetShader( Shader& shader );

  /**
   * @copydoc Dali::Renderer::GetShader()
   */
  Shader* GetShader() const;

  /**
   * @copydoc Dali::Renderer::SetDepthIndex()
   */
  void SetDepthIndex( int depthIndex );

  /**
   * @copydoc Dali::Renderer::GetDepthIndex()
   */
  int GetDepthIndex() const;

   /**
    * @copydoc Dali::Renderer::SetBlendMode()
    */
   void SetBlendMode( BlendMode::Type mode );

   /**
    * @copydoc Dali::Renderer::GetBlendMode()
    */
   BlendMode::Type GetBlendMode() const;

   /**
    * @copydoc Dali::Renderer::SetBlendFunc()
    */
   void SetBlendFunc( BlendFactor::Type srcFactorRgba, BlendFactor::Type destFactorRgba );

   /**
    * @copydoc Dali::Renderer::SetBlendFunc()
    */
   void SetBlendFunc( BlendFactor::Type srcFactorRgb,   BlendFactor::Type destFactorRgb,
                      BlendFactor::Type srcFactorAlpha, BlendFactor::Type destFactorAlpha );

   /**
    * @copydoc Dali::Renderer::GetBlendFunc()
    */
   void GetBlendFunc( BlendFactor::Type& srcFactorRgb,   BlendFactor::Type& destFactorRgb,
                      BlendFactor::Type& srcFactorAlpha, BlendFactor::Type& destFactorAlpha ) const;

   /**
    * @copydoc Dali::Renderer::SetBlendEquation()
    */
   void SetBlendEquation( BlendEquation::Type equationRgba );

   /**
    * @copydoc Dali::Renderer::SetBlendEquation()
    */
   void SetBlendEquation( BlendEquation::Type equationRgb, BlendEquation::Type equationAlpha );

   /**
    * @copydoc Dali::Renderer::GetBlendEquation()
    */
   void GetBlendEquation( BlendEquation::Type& equationRgb, BlendEquation::Type& equationAlpha ) const;

   /**
    * @copydoc Dali::Renderer::SetIndexedDrawFirstElement
    */
   void SetIndexedDrawFirstElement( size_t firstElement );

   /**
    * @copydoc Dali::Renderer::SetIndexedDrawElementsCount
    */
   void SetIndexedDrawElementsCount( size_t elementsCount );

   /**
    * @brief Set whether the Pre-multiplied Alpha Blending is required
    *
    * @param[in] preMultipled whether alpha is pre-multiplied.
    */
   void EnablePreMultipliedAlpha( bool preMultipled );

   /**
    * @brief Query whether alpha is pre-multiplied.
    *
    * @return True is alpha is pre-multiplied, false otherwise.
    */
   bool IsPreMultipliedAlphaEnabled() const;

   /**
    * @brief Get the scene graph object
    *
    * @return the scene object
    */
   SceneGraph::Renderer* GetRendererSceneObject();

public: // Default property extensions from Object

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyCount()
   */
  virtual unsigned int GetDefaultPropertyCount() const;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyIndices()
   */
  virtual void GetDefaultPropertyIndices( Property::IndexContainer& indices ) const;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyName()
   */
  virtual const char* GetDefaultPropertyName(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyIndex()
   */
  virtual Property::Index GetDefaultPropertyIndex(const std::string& name) const;

  /**
   * @copydoc Dali::Internal::Object::IsDefaultPropertyWritable()
   */
  virtual bool IsDefaultPropertyWritable(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::Object::IsDefaultPropertyAnimatable()
   */
  virtual bool IsDefaultPropertyAnimatable(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::Object::IsDefaultPropertyAConstraintInput()
   */
  virtual bool IsDefaultPropertyAConstraintInput( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyType()
   */
  virtual Property::Type GetDefaultPropertyType(Property::Index index) const;

  /**
   * @copydoc Dali::Internal::Object::SetDefaultProperty()
   */
  virtual void SetDefaultProperty(Property::Index index, const Property::Value& propertyValue);

  /**
   * @copydoc Dali::Internal::Object::SetSceneGraphProperty()
   */
  virtual void SetSceneGraphProperty( Property::Index index, const PropertyMetadata& entry, const Property::Value& value );

  /**
   * @copydoc Dali::Internal::Object::GetDefaultProperty()
   */
  virtual Property::Value GetDefaultProperty( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::GetPropertyOwner()
   */
  virtual const SceneGraph::PropertyOwner* GetPropertyOwner() const;

  /**
   * @copydoc Dali::Internal::Object::GetSceneObject()
   */
  virtual const SceneGraph::PropertyOwner* GetSceneObject() const;

  /**
   * @copydoc Dali::Internal::Object::GetSceneObjectAnimatableProperty()
   */
  virtual const SceneGraph::PropertyBase* GetSceneObjectAnimatableProperty( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::GetSceneObjectInputProperty()
   */
  virtual const PropertyInputImpl* GetSceneObjectInputProperty( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::GetPropertyComponentIndex()
   */
  virtual int GetPropertyComponentIndex( Property::Index index ) const;

private: // implementation

  /**
   * @brief Default constructor.
   */
  Renderer();

  /**
   * @brief Initializes the Renderer.
   */
  void Initialize();

  /**
   * @brief Sets the blend color.
   * @param[in] blendColor The blend color to set.
   */
  void SetBlendColor( const Vector4& blendColor );

  /**
   * @brief Retrieves the blend-color.
   * @return A const reference to the blend-color
   */
  const Vector4& GetBlendColor() const;

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Renderer();

private: // unimplemented methods
  Renderer( const Renderer& );
  Renderer& operator=( const Renderer& );

private: // data
  SceneGraph::Renderer*               mSceneObject;
  GeometryPtr                         mGeometry;                     ///< Connector that holds the geometry used by this renderer
  TextureSetPtr                       mTextureSet;                   ///< Connector that holds the texture set used by this renderer
  ShaderPtr                           mShader;                       ///< Connector that holds the shader used by this renderer

  int                                 mDepthIndex;

  size_t                              mIndexedDrawFirstElement;      ///< Offset of first element to draw from bound index buffer
  size_t                              mIndexedDrawElementCount;      ///< Number of elements to draw

  Render::Renderer::StencilParameters mStencilParameters;            ///< Struct containing all stencil related options
  BlendingOptions                     mBlendingOptions;              ///< Local copy of blending options bitmask

  DepthFunction::Type                 mDepthFunction:3;              ///< Local copy of the depth function
  FaceCullingMode::Type               mFaceCullingMode:2;            ///< Local copy of the mode of face culling
  BlendMode::Type                     mBlendMode:2;                  ///< Local copy of the mode of blending
  DepthWriteMode::Type                mDepthWriteMode:2;             ///< Local copy of the depth write mode
  DepthTestMode::Type                 mDepthTestMode:2;              ///< Local copy of the depth test mode
  bool                                mPremultipledAlphaEnabled:1;   ///< Flag indicating whether the Pre-multiplied Alpha Blending is required
};

} // namespace Internal

// Helpers for public-api forwarding methods
inline Internal::Renderer& GetImplementation( Dali::Renderer& handle )
{
  DALI_ASSERT_ALWAYS(handle && "Renderer handle is empty");

  BaseObject& object = handle.GetBaseObject();

  return static_cast<Internal::Renderer&>(object);
}

inline const Internal::Renderer& GetImplementation( const Dali::Renderer& handle )
{
  DALI_ASSERT_ALWAYS(handle && "Renderer handle is empty");

  const BaseObject& object = handle.GetBaseObject();

  return static_cast<const Internal::Renderer&>(object);
}

} // namespace Dali

#endif // DALI_INTERNAL_RENDERER_H
