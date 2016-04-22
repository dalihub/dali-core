#ifndef DALI_INTERNAL_TEXTURE_SET_H
#define DALI_INTERNAL_TEXTURE_SET_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h> // std::vector

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h> // DALI_ASSERT_ALWAYS
#include <dali/public-api/common/intrusive-ptr.h> // Dali::IntrusivePtr
#include <dali/devel-api/rendering/texture-set.h> // Dali::TextureSet
#include <dali/internal/event/common/connectable.h> // Dali::Internal::Connectable
#include <dali/internal/event/common/object-connector.h> // Dali::Internal::ObjectConnector
#include <dali/internal/event/common/object-impl.h> // Dali::Internal::Object
#include <dali/internal/event/common/property-buffer-impl.h> // Dali::Internal::PropertyBuffer
#include <dali/internal/event/rendering/sampler-impl.h> // Dali::Internal::Sampler
#include <dali/internal/event/rendering/shader-impl.h> // Dali::Internal::Shader

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class TextureSet;
}


class TextureSet;
typedef IntrusivePtr<TextureSet> TextureSetPtr;

/**
 * TextureSet is an object that holds all the textures used by a renderer
 */
class TextureSet : public Object, public Connectable
{
public:

  /**
   * @copydoc Dali::TextureSet::New()
   */
  static TextureSetPtr New();

  /**
   * @copydoc Dali::TextureSet::SetImage()
   */
  void SetImage( size_t index, ImagePtr image );

  /**
   * @copydoc Dali::TextureSet::GetImage()
   */
  Image* GetImage( size_t index ) const;

  /**
   * @copydoc Dali::TextureSet::SetSampler()
   */
  void SetSampler( size_t index, SamplerPtr sampler );

  /**
   * @copydoc Dali::TextureSet::GetSampler()
   */
  Sampler* GetSampler( size_t index ) const;

  /**
   * @copydoc Dali::TextureSet::GetTextureCount()
   */
  size_t GetTextureCount() const;

 /**
   * @brief Get the TextureSet scene object
   *
   * @return the texture set scene object
   */
  const SceneGraph::TextureSet* GetTextureSetSceneObject() const;

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

  struct Texture
  {
    Texture()
    :image(NULL),
     sampler( NULL )
    {}

    ImagePtr    image;
    SamplerPtr  sampler;
  };

  TextureSet();

  /**
   * Second stage initialization
   */
  void Initialize();

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~TextureSet();

private: // unimplemented methods
  TextureSet( const TextureSet& );
  TextureSet& operator=( const TextureSet& );

private: // Data

  SceneGraph::TextureSet* mSceneObject;
  std::vector<Texture> mTextures;
  bool mOnStage;

};

} // namespace Internal

// Helpers for public-api forwarding methods
inline Internal::TextureSet& GetImplementation( Dali::TextureSet& handle )
{
  DALI_ASSERT_ALWAYS(handle && "TextureSet handle is empty");

  BaseObject& object = handle.GetBaseObject();

  return static_cast<Internal::TextureSet&>(object);
}

inline const Internal::TextureSet& GetImplementation( const Dali::TextureSet& handle )
{
  DALI_ASSERT_ALWAYS(handle && "TextureSet handle is empty");

  const BaseObject& object = handle.GetBaseObject();

  return static_cast<const Internal::TextureSet&>(object);
}

} // namespace Dali

#endif // DALI_INTERNAL_TEXTURE_SET_H
