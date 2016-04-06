#ifndef DALI_INTERNAL_RENDERER_H
#define DALI_INTERNAL_RENDERER_H

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
#include <dali/public-api/common/dali-common.h> // DALI_ASSERT_ALWAYS
#include <dali/public-api/common/intrusive-ptr.h> // Dali::IntrusivePtr
#include <dali/devel-api/rendering/renderer.h> // Dali::Renderer
#include <dali/internal/common/blending-options.h>
#include <dali/internal/event/common/connectable.h> // Dali::Internal::Connectable
#include <dali/internal/event/common/object-connector.h> // Dali::Internal::ObjectConnector
#include <dali/internal/event/common/object-impl.h> // Dali::Internal::Object
#include <dali/internal/event/rendering/texture-set-impl.h> // Dali::Internal::TextureSet
#include <dali/internal/event/rendering/geometry-impl.h> // Dali::Internal::Geometry

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
    * @copydoc Dali::Renderer::SetFaceCullingMode()
    */
   void SetFaceCullingMode( Dali::Renderer::FaceCullingMode cullingMode );

   /**
    * @copydoc Dali::Renderer::GetFaceCullingMode()
    */
   Dali::Renderer::FaceCullingMode GetFaceCullingMode();

   /**
    * @copydoc Dali::Renderer::SetBlendMode()
    */
   void SetBlendMode( BlendingMode::Type mode );

   /**
    * @copydoc Dali::Renderer::GetBlendMode()
    */
   BlendingMode::Type GetBlendMode() const;

   /**
    * @copydoc Dali::Renderer::SetBlendFunc()
    */
   void SetBlendFunc( BlendingFactor::Type srcFactorRgba, BlendingFactor::Type destFactorRgba );

   /**
    * @copydoc Dali::Renderer::SetBlendFunc()
    */
   void SetBlendFunc( BlendingFactor::Type srcFactorRgb,   BlendingFactor::Type destFactorRgb,
                      BlendingFactor::Type srcFactorAlpha, BlendingFactor::Type destFactorAlpha );

   /**
    * @copydoc Dali::Renderer::GetBlendFunc()
    */
   void GetBlendFunc( BlendingFactor::Type& srcFactorRgb,   BlendingFactor::Type& destFactorRgb,
                      BlendingFactor::Type& srcFactorAlpha, BlendingFactor::Type& destFactorAlpha ) const;

   /**
    * @copydoc Dali::Renderer::SetBlendEquation()
    */
   void SetBlendEquation( BlendingEquation::Type equationRgba );

   /**
    * @copydoc Dali::Renderer::SetBlendEquation()
    */
   void SetBlendEquation( BlendingEquation::Type equationRgb, BlendingEquation::Type equationAlpha );

   /**
    * @copydoc Dali::Renderer::GetBlendEquation()
    */
   void GetBlendEquation( BlendingEquation::Type& equationRgb, BlendingEquation::Type& equationAlpha ) const;

   /**
    * @copydoc Dali::Renderer::SetBlendColor()
    */
   void SetBlendColor( const Vector4& color );

   /**
    * @copydoc Dali::Renderer::GetBlendColor()
    */
   Vector4 GetBlendColor() const;

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
   * @brief Get the scene graph object ( the node attachment )
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

public: // Functions from Connectable
  /**
   * @copydoc Dali::Internal::Connectable::OnStage()
   */
  virtual bool OnStage() const;

  /**
   * @copydoc Dali::Internal::Connectable::Connect()
   */
  virtual void Connect();

  /**
   * @copydoc Dali::Internal::Connectable::Disconnect()
   */
  virtual void Disconnect();

private: // implementation
  Renderer();

  void Initialize();

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Renderer();

private: // unimplemented methods
  Renderer( const Renderer& );
  Renderer& operator=( const Renderer& );

private: // data
  SceneGraph::Renderer* mSceneObject;
  Vector4* mBlendColor;                         ///< Local copy of blend color, pointer only as its rarely used
  ObjectConnector<Geometry> mGeometryConnector; ///< Connector that holds the geometry used by this renderer
  ObjectConnector<TextureSet> mTextureSetConnector; ///< Connector that holds the texture set used by this renderer
  IntrusivePtr<Shader> mShader;                 ///< Connector that holds the shader used by this renderer

  int mDepthIndex;
  int mOnStageCount;

  Dali::Renderer::FaceCullingMode mFaceCullingMode; ///< Local copy of face culling mode
  BlendingMode::Type mBlendingMode;                 ///< Local copy of blending mode
  BlendingOptions mBlendingOptions;                 ///< Local copy of blending options bitmask
  bool mPremultipledAlphaEnabled;                   ///< Flag indicating whether the Pre-multiplied Alpha Blending is required

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
