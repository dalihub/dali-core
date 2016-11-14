#ifndef __DALI_INTERNAL_SHADER_EFFECT_H__
#define __DALI_INTERNAL_SHADER_EFFECT_H__

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
#include <dali/public-api/images/image.h>
#include <dali/devel-api/shader-effects/shader-effect.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/effects/shader-declarations.h>
#include <dali/internal/event/common/object-impl.h>
#include <dali/internal/event/rendering/shader-impl.h>

namespace Dali
{

namespace Internal
{

/**
 * An abstract base class for a shader effect object.
 * The corresponding scene-graph object is a collection of shader programs,
 * which can apply the same effect to different geometry types.
 */
class ShaderEffect : public Object
{
public:
  typedef Dali::ShaderEffect::UniformCoordinateType UniformCoordinateType;

  /**
   * Create a new ShaderEffect with no programs
   * @param hints GeometryHints to define the geometry of the rendered object
   * @return A smart-pointer to a newly allocated shader effect.
   */
  static ShaderEffectPtr New( Dali::ShaderEffect::GeometryHints hints = Dali::ShaderEffect::HINT_NONE );

  /**
   * @copydoc Dali::ShaderEffect::SetEffectImage
   */
  void SetEffectImage( Dali::Image image );

  /**
   * @copydoc Dali::ShaderEffect::SetUniform( const std::string& name, Property::Value value, UniformCoordinateType uniformCoordinateType )
   */
  void SetUniform( const std::string& name,
                   Property::Value value,
                   UniformCoordinateType uniformCoordinateType );

  /**
   * Add a GeometryType specific default program to this ShaderEffect
   * @param[in] geometryType    The GeometryType rendered by the shader program
   * @param[in] vertexSource    The source code for the vertex shader
   * @param[in] fragmentSource  The source code for the fragment shader
   */
  void SetPrograms( const std::string& vertexSource, const std::string& fragmentSource );

  /**
   * Add a default program to this ShaderEffect.
   * This overload allows the optional prefixing for both the vertex and fragment shader.
   * A useful prefix may be shader \#defines for conditional compilation.
   * @param[in] vertexPrefix    The prefix source code for the vertex shader
   * @param[in] fragmentPrefix  The prefix source code for the fragment shader
   * @param[in] vertexSource    The source code for the vertex shader
   * @param[in] fragmentSource  The source code for the fragment shader
   */
  void SetPrograms( const std::string& vertexPrefix, const std::string& fragmentPrefix,
                    const std::string& vertexSource, const std::string& fragmentSource );

  /**
   * @brief Notify ShaderEffect that it's being used by an Actor.
   *
   * @param[in] actor The Actor that is connecting to this ShaderEffect
   */
  void Connect( ActorPtr actor );

  /**
   * @brief Notify ShaderEffect that an Actor is no longer using it.
   *
   * @param[in] actor The Actor that is disconnecting from this ShaderEffect
   */
  void Disconnect( ActorPtr actor );

public:

  /**
   * Returns the shader for this ShaderEffect
   *
   * @return Returns the shader for this ShaderEffect
   */
  ShaderPtr GetShader() const { return mShader; }

  /**
   * Returns the geometry grid size.
   *
   * @param[in] size The pixel area size.
   * @return Returns the geometry grid size
   */
  Vector2 GetGridSize( const Vector2& size );

  /**
   * Returns the effect image for this ShaderEffect
   *
   * @return Returns the effect image for this ShaderEffect
   */
  Dali::Image GetEffectImage() const { return mEffectImage; }

public: //  override property functions from Object

  /**
   * @copydoc Dali::Handle::GetPropertyCount()
   */
  virtual unsigned int GetPropertyCount() const;

  /**
   * @copydoc Dali::Handle::GetPropertyName()
   */
  virtual std::string GetPropertyName( Property::Index index ) const;

  /**
   * @copydoc Dali::Handle::GetPropertyIndex()
   */
  virtual Property::Index GetPropertyIndex( const std::string& name ) const;

  /**
   * @copydoc Dali::Handle::IsPropertyWritable()
   */
  virtual bool IsPropertyWritable( Property::Index index ) const;

  /**
   * @copydoc Dali::Handle::IsPropertyAnimatable()
   */
  virtual bool IsPropertyAnimatable( Property::Index index ) const;

  /**
   * @copydoc Dali::Handle::IsPropertyAConstraintInput()
   */
  virtual bool IsPropertyAConstraintInput( Property::Index index ) const;

  /**
   * @copydoc Dali::Handle::GetPropertyType()
   */
  virtual Property::Type GetPropertyType( Property::Index index ) const;

  /**
   * @copydoc Dali::Handle::SetProperty()
   */
  virtual void SetProperty( Property::Index index, const Property::Value& propertyValue );

  /**
   * @copydoc Dali::Handle::GetProperty()
   */
  virtual Property::Value GetProperty( Property::Index index ) const;

  /**
   * @copydoc Dali::Handle::GetPropertyIndices()
   */
  virtual void GetPropertyIndices( Property::IndexContainer& indices ) const;

  /**
   * @copydoc Dali::Handle::RegisterProperty()
   */
  virtual Property::Index RegisterProperty( const std::string& name, const Property::Value& propertyValue );

  /**
   * @copydoc Dali::Handle::RegisterProperty(std::string name, Property::Value propertyValue, Property::AccessMode accessMode)
   */
  virtual Property::Index RegisterProperty( const std::string& name, const Property::Value& propertyValue, Property::AccessMode accessMode );

  /**
   * @copydoc Dali::Handle::AddPropertyNotification()
   */
  virtual Dali::PropertyNotification AddPropertyNotification( Property::Index index,
                                                              int componentIndex,
                                                              const Dali::PropertyCondition& condition );

  /**
   * @copydoc Dali::Handle::RemovePropertyNotification()
   */
  virtual void RemovePropertyNotification( Dali::PropertyNotification propertyNotification );

  /**
   * @copydoc Dali::Handle::RemovePropertyNotifications()
   */
  virtual void RemovePropertyNotifications();

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
  virtual const char* GetDefaultPropertyName( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyIndex()
   */
  virtual Property::Index GetDefaultPropertyIndex( const std::string& name ) const;

  /**
   * @copydoc Dali::Internal::Object::IsDefaultPropertyWritable()
   */
  virtual bool IsDefaultPropertyWritable( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::IsDefaultPropertyAnimatable()
   */
  virtual bool IsDefaultPropertyAnimatable( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::IsDefaultPropertyAConstraintInput()
   */
  virtual bool IsDefaultPropertyAConstraintInput( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyType()
   */
  virtual Property::Type GetDefaultPropertyType( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::Object::SetDefaultProperty()
   */
  virtual void SetDefaultProperty( Property::Index index, const Property::Value& propertyValue );

  /**
   * @copydoc Dali::Internal::Object::GetDefaultProperty()
   */
  virtual Property::Value GetDefaultProperty( Property::Index index ) const;

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

protected:

  /**
   * Protected constructor.
   * @param[in] hints Geometry hints
   */
  ShaderEffect( Dali::ShaderEffect::GeometryHints hints );

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ShaderEffect();

private:

  // No copying allowed, thus these are undefined
  ShaderEffect( const ShaderEffect& );
  ShaderEffect& operator=( const ShaderEffect& rhs );

private: // Data
  std::vector< ActorPtr > mConnectedActors;               ///< The array of actors that are currently connected to this ShaderEffect
  ShaderPtr               mShader;                        ///< The shader pointer
  Dali::Image             mEffectImage;                   ///< The Client-side handle to the effect image
  float                   mGridDensity;                  ///< The grid denisty
  Dali::ShaderEffect::GeometryHints  mGeometryHints;     ///< shader geometry hints for building the geometry

};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::ShaderEffect& GetImplementation(Dali::ShaderEffect& effect)
{
  DALI_ASSERT_ALWAYS(effect && "ShaderEffect handle is empty");

  BaseObject& handle = effect.GetBaseObject();

  return static_cast<Internal::ShaderEffect&>(handle);
}

inline const Internal::ShaderEffect& GetImplementation(const Dali::ShaderEffect& effect)
{
  DALI_ASSERT_ALWAYS(effect && "ShaderEffect handle is empty");

  const BaseObject& handle = effect.GetBaseObject();

  return static_cast<const Internal::ShaderEffect&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_SHADER_EFFECT_H__
