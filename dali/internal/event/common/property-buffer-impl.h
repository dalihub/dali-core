#ifndef DALI_INTERNAL_PROPERTY_BUFFER_H
#define DALI_INTERNAL_PROPERTY_BUFFER_H

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

// EXTERNAL INCLUDES
#include <utility> // std::pair

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h> // DALI_ASSERT_ALWAYS
#include <dali/public-api/common/intrusive-ptr.h> // Dali::IntrusivePtr
#include <dali/public-api/object/property-map.h> // Dali::Property::Map
#include <dali/devel-api/object/property-buffer.h> // Dali::PropertyBuffer
#include <dali/internal/event/common/connectable.h> // Dali::Internal::Connectable
#include <dali/internal/event/common/object-connector.h> // Dali::Internal::ObjectConnector
#include <dali/internal/event/common/object-impl.h> // Dali::Internal::Object

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class PropertyBuffer;

namespace PropertyBufferMetadata
{
struct Format;
} // namespace PropertyBufferMetadata

} // namespace SceneGraph

class PropertyBuffer;
typedef IntrusivePtr<PropertyBuffer> PropertyBufferPtr;

/**
 * PropertyBuffer is an object that contains an array of structures of values that
 * can be accessed as properties.
 */
class PropertyBuffer : public Object, public Connectable
{
public:

  /**
   * @copydoc PropertBuffer::New()
   */
  static PropertyBufferPtr New();

  /**
   * @copydoc PropertBuffer::SetSize()
   */
  void SetSize( std::size_t size );

  /**
   * @copydoc PropertBuffer::GetSize()
   */
  std::size_t GetSize() const;

  /**
   * @copydoc PropertBuffer::SetData()
   */
  void SetData( const void* data );

  /**
   * @copydoc PropertBuffer::GetPropertyIndex()
   */
  Dali::Property::Index GetPropertyIndex( const std::string name, std::size_t index );

  /**
   * @brief Get the propertyBuffer scene object
   *
   * @return the propertyBuffer scene object
   */
  const SceneGraph::PropertyBuffer* GetPropertyBufferSceneObject() const;

  /**
   * @brief Set the format of the PropertyBuffer
   *
   * @pre Has not been set yet
   *
   * @param[in] format of the PropertyBuffer
   */
  void SetFormat( Dali::Property::Map& format );

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
   * @copydoc Dali::Internal::Connectable::Contnect()
   */
  virtual void Connect();

  /**
   * @copydoc Dali::Internal::Connectable::Disconnect()
   */
  virtual void Disconnect();

protected:
  /**
   * @brief Destructor
   */
  ~PropertyBuffer();

private: // implementation
  /**
   * @brief Default constructor
   */
  PropertyBuffer();

  /**
   * Second stage initialization
   */
  void Initialize();

  /**
   * Update the buffer when the format changes
   */
  void FormatChanged();

  /**
   * Update the buffer when the size changes
   */
  void SizeChanged();

private: // unimplemented methods
  PropertyBuffer( const PropertyBuffer& );
  PropertyBuffer& operator=( const PropertyBuffer& );

private: // data
  SceneGraph::PropertyBuffer* mSceneObject; ///< Update side object

  Property::Map mFormat;  ///< Format of the property buffer
  const SceneGraph::PropertyBufferMetadata::Format* mBufferFormat;  ///< Metadata for the format of the property buffer
  unsigned int mSize; ///< Number of elements in the buffer
  Dali::Vector< char > mBuffer; // Data of the property-buffer

  bool mOnStage;  ///< Flag to know if the object is on stage
};

/**
 * Get the implementation type from a Property::Type
 */
template<Property::Type type> struct PropertyImplementationType
{
  // typedef ... Type; not defined, only support types declared bellow
};
template<> struct PropertyImplementationType< Property::BOOLEAN > { typedef bool Type; };
template<> struct PropertyImplementationType< Property::FLOAT > { typedef float Type; };
template<> struct PropertyImplementationType< Property::INTEGER > { typedef int Type; };
template<> struct PropertyImplementationType< Property::VECTOR2 > { typedef Vector2 Type; };
template<> struct PropertyImplementationType< Property::VECTOR3 > { typedef Vector3 Type; };
template<> struct PropertyImplementationType< Property::VECTOR4 > { typedef Vector4 Type; };
template<> struct PropertyImplementationType< Property::MATRIX3 > { typedef Matrix3 Type; };
template<> struct PropertyImplementationType< Property::MATRIX > { typedef Matrix Type; };
template<> struct PropertyImplementationType< Property::RECTANGLE > { typedef Rect<int> Type; };
template<> struct PropertyImplementationType< Property::ROTATION > { typedef Quaternion Type; };

unsigned int GetPropertyImplementationSize( Property::Type& propertyType );

} // namespace Internal

// Helpers for public-api forwarding methods
inline Internal::PropertyBuffer& GetImplementation(Dali::PropertyBuffer& handle)
{
  DALI_ASSERT_ALWAYS(handle && "PropertyBuffer handle is empty");

  BaseObject& object = handle.GetBaseObject();

  return static_cast<Internal::PropertyBuffer&>(object);
}

inline const Internal::PropertyBuffer& GetImplementation(const Dali::PropertyBuffer& handle)
{
  DALI_ASSERT_ALWAYS(handle && "PropertyBuffer handle is empty");

  const BaseObject& object = handle.GetBaseObject();

  return static_cast<const Internal::PropertyBuffer&>(object);
}

} // namespace Dali

#endif // DALI_INTERNAL_PROPERTY_BUFFER_H
