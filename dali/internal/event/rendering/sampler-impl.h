#ifndef DALI_INTERNAL_SAMPLER_H
#define DALI_INTERNAL_SAMPLER_H

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
#include <dali/devel-api/rendering/sampler.h> // Dali::Sampler
#include <dali/internal/event/common/connectable.h> // Dali::Internal::Connectable
#include <dali/internal/event/common/object-connector.h> // Dali::Internal::ObjectConnector
#include <dali/internal/event/common/object-impl.h> // Dali::Internal::Object
#include <dali/internal/event/images/image-connector.h> // Dali::Internal::ImageConnector

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class Sampler;
}

class Sampler;
typedef IntrusivePtr<Sampler> SamplerPtr;

/**
 * Sampler is an object that contains an array of structures of values that
 * can be accessed as properties.
 */
class Sampler : public Object, public Connectable
{
public:

  /**
   * Create a new Sampler.
   * @return A smart-pointer to the newly allocated Sampler.
   */
  static SamplerPtr New( const std::string& textureUnitUniformName );

  /**
   * @copydoc Dali::Sampler::SetUniformName()
   */
  void SetTextureUnitUniformName( const std::string& name );

  /**
   * @copydoc Dali::Sampler::GetUniformName()
   */
  const std::string& GetTextureUnitUniformName() const;

  /**
   * @copydoc Dali::Sampler::SetImage()
   */
  void SetImage( ImagePtr& image );

  /**
   * @copydoc Dali::Sampler::GetImage()
   */
  ImagePtr GetImage() const;

  /**
   * @copydoc Dali::Sampler::SetFilterMode()
   */
  void SetFilterMode( Dali::Sampler::FilterMode minFilter, Dali::Sampler::FilterMode magFilter );

  /**
   * @copydoc Dali::Sampler::SetWrapMode()
   */
  void SetWrapMode( Dali::Sampler::WrapMode uWrap, Dali::Sampler::WrapMode vWrap );

  /**
   * @copydoc Dali::Sampler::SetAffectsTransparency()
   */
  void SetAffectsTransparency( bool affectsTransparency );

  /**
   * @brief Get the sampler scene object
   *
   * @return the sampler scene object
   */
  const SceneGraph::Sampler* GetSamplerSceneObject() const;

  /**
   * Retrieve the scene-graph sampler added by this object.
   * @return A pointer to the sampler, or NULL if no sampler has been added to the scene-graph.
   */
  SceneGraph::Sampler* GetSamplerSceneObject();

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

private:
  Sampler();

  /**
   * Second stage initialization
   */
  void Initialize( const std::string& textureUnitUniformName );

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Sampler();

private: // data
  //TODO: MESH_REWORK : change to ObjectConnector
  ImageConnector mImageConnector;
  SceneGraph::Sampler* mSceneObject;
  bool mOnStage;
};

} // namespace Internal

// Helpers for public-api forwarding methods
inline Internal::Sampler& GetImplementation(Dali::Sampler& handle)
{
  DALI_ASSERT_ALWAYS(handle && "Sampler handle is empty");

  BaseObject& object = handle.GetBaseObject();

  return static_cast<Internal::Sampler&>(object);
}

inline const Internal::Sampler& GetImplementation(const Dali::Sampler& handle)
{
  DALI_ASSERT_ALWAYS(handle && "Sampler handle is empty");

  const BaseObject& object = handle.GetBaseObject();

  return static_cast<const Internal::Sampler&>(object);
}

} // namespace Dali

#endif // DALI_INTERNAL_SAMPLER_H
