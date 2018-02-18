#ifndef DALI_INTERNAL_GEOMETRY_H
#define DALI_INTERNAL_GEOMETRY_H

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

// EXTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h> // std::vector

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h> // DALI_ASSERT_ALWAYS
#include <dali/public-api/common/intrusive-ptr.h> // Dali::IntrusivePtr
#include <dali/public-api/rendering/geometry.h> // Dali::Geometry
#include <dali/internal/event/common/connectable.h> // Dali::Internal::Connectable
#include <dali/internal/event/common/object-connector.h> // Dali::Internal::ObjectConnector
#include <dali/internal/event/common/object-impl.h> // Dali::Internal::Object
#include <dali/internal/event/common/property-buffer-impl.h> // Dali::Internal::PropertyBuffer

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class Geometry;
}

class Geometry;
typedef IntrusivePtr<Geometry> GeometryPtr;

/**
 * Geometry is an object that contains an array of structures of values that
 * can be accessed as properties.
 */
class Geometry : public BaseObject
{
public:

  /**
   * Create a new Geometry.
   * @return A smart-pointer to the newly allocated Geometry.
   */
  static GeometryPtr New();

  /**
   * @copydoc Dali::Geometry::AddVertexBuffer()
   */
  std::size_t AddVertexBuffer( PropertyBuffer& vertexBuffer );

  /**
   * @copydoc Dali::Geometry::GetNumberOfVertexBuffers()
   */
  std::size_t GetNumberOfVertexBuffers() const;

  /**
   * @copydoc Dali::Geometry::RemoveVertexBuffer()
   */
  void RemoveVertexBuffer( std::size_t index );

  /**
   * @copydoc Dali::Geometry::SetIndexBuffer()
   */
  void SetIndexBuffer( const unsigned short* indices, size_t count );

  /**
   * @copydoc Dali::Geometry::SetType()
   */
  void SetType( Dali::Geometry::Type geometryType );

  /**
   * @copydoc Dali::Geometry::GetType()
   */
  Dali::Geometry::Type GetType() const;

private: // implementation

  /**
   * Constructor
   */
  Geometry();

  /**
   * Second stage initialization of the Geometry
   */
  void Initialize();

protected:

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Geometry();

private: // unimplemented methods
  Geometry( const Geometry& );
  Geometry& operator=( const Geometry& );

private: // data

  EventThreadServices& mEventThreadServices;    ///<Used to send messages to the render thread via update thread
  //TODO: Render::Geometry* mRenderObject;

  std::vector<PropertyBufferPtr> mVertexBuffers; ///< Vector of intrusive pointers to vertex buffers
  Dali::Geometry::Type mType;      ///< Geometry type (cached)
};

} // namespace Internal

// Helpers for public-api forwarding methods
inline Internal::Geometry& GetImplementation(Dali::Geometry& handle)
{
  DALI_ASSERT_ALWAYS(handle && "Geometry handle is empty");

  BaseObject& object = handle.GetBaseObject();

  return static_cast<Internal::Geometry&>(object);
}

inline const Internal::Geometry& GetImplementation(const Dali::Geometry& handle)
{
  DALI_ASSERT_ALWAYS(handle && "Geometry handle is empty");

  const BaseObject& object = handle.GetBaseObject();

  return static_cast<const Internal::Geometry&>(object);
}

} // namespace Dali

#endif // DALI_INTERNAL_GEOMETRY_H
