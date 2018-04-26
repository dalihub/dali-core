#ifndef DALI_INTERNAL_PROPERTY_BUFFER_H
#define DALI_INTERNAL_PROPERTY_BUFFER_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/property-map.h> // Dali::Property::Map
#include <dali/public-api/rendering/property-buffer.h> // Dali::Property::Map
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/update/rendering/scene-graph-property-buffer.h>

namespace Dali
{
namespace Internal
{

class PropertyBuffer;
typedef IntrusivePtr<PropertyBuffer> PropertyBufferPtr;

/**
 * PropertyBuffer is an object that contains an array of structures of values that
 * can be accessed as properties.
 */
class PropertyBuffer : public BaseObject
{
public:

  /**
   * @copydoc PropertBuffer::New()
   */
  static PropertyBufferPtr New( Dali::Property::Map& format );

  /**
   * @copydoc PropertBuffer::SetData()
   */
  void SetData( const void* data, std::size_t size );

  /**
   * @copydoc PropertBuffer::GetSize()
   */
  std::size_t GetSize() const;

public: // Default property extensions from Object

  /**
   * @brief Get the render thread side of the PropertyBuffer
   *
   * @return The render thread side of this PropertyBuffer
   */
  const SceneGraph::PropertyBuffer* GetRenderObject() const;

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
  void Initialize( Dali::Property::Map& format );

private: // unimplemented methods
  PropertyBuffer( const PropertyBuffer& );
  PropertyBuffer& operator=( const PropertyBuffer& );

private: // data
  EventThreadServices& mEventThreadServices;    ///<Used to send messages to the render thread via update thread
  SceneGraph::PropertyBuffer* mRenderObject;        ///<Render side object
  unsigned int mBufferFormatSize;
  unsigned int mSize; ///< Number of elements in the buffer
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
inline Internal::PropertyBuffer& GetImplementation( Dali::PropertyBuffer& handle )
{
  DALI_ASSERT_ALWAYS(handle && "PropertyBuffer handle is empty");

  BaseObject& object = handle.GetBaseObject();

  return static_cast<Internal::PropertyBuffer&>(object);
}

inline const Internal::PropertyBuffer& GetImplementation( const Dali::PropertyBuffer& handle )
{
  DALI_ASSERT_ALWAYS(handle && "PropertyBuffer handle is empty");

  const BaseObject& object = handle.GetBaseObject();

  return static_cast<const Internal::PropertyBuffer&>(object);
}

} // namespace Dali

#endif // DALI_INTERNAL_PROPERTY_BUFFER_H
