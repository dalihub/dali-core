#ifndef DALI_INTERNAL_VERTEX_BUFFER_H
#define DALI_INTERNAL_VERTEX_BUFFER_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/dali-common.h>   // DALI_ASSERT_ALWAYS
#include <dali/public-api/common/intrusive-ptr.h> // Dali::IntrusivePtr
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/property-map.h>     // Dali::Property::Map
#include <dali/public-api/rendering/vertex-buffer.h> // Dali::VertexBuffer

#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/render/renderers/render-vertex-buffer.h>

namespace Dali
{
class VertexBuffer;
namespace Internal
{
class VertexBuffer;
using VertexBufferPtr = IntrusivePtr<VertexBuffer>;

/**
 * VertexBuffer is an object that contains an array of structures of values that
 * can be accessed as properties.
 */
class VertexBuffer : public BaseObject
{
public:
  /**
   * @copydoc Dali::VertexBuffer::New()
   */
  static VertexBufferPtr New(Dali::Property::Map& format);

  /**
   * @copydoc Dali::VertexBuffer::SetData()
   */
  void SetData(const void* data, uint32_t size);

  /**
   * @copydoc Dali::VertexBuffer::GetSize()
   */
  uint32_t GetSize() const;

  /**
   * @copydoc Dali::VertexBuffer::SetDivisor()
   */
  void SetDivisor(uint32_t divisor);

  /**
   * @copydoc Dali::VertexBuffer::GetDivisor()
   */
  uint32_t GetDivisor() const;

  /**
   * @copydoc Dali::VertexBuffer::SetVertexBufferUpdateCallbackMessage()
   */
  void SetVertexBufferUpdateCallback(VertexBufferUpdateCallback& callback);

  /**
   * @copydoc Dali::VertexBuffer::ClearVertexBufferUpdateCallback()
   */
  void ClearVertexBufferUpdateCallback();

public: // Default property extensions from Object
  /**
   * @brief Get the render thread side of the VertexBuffer
   *
   * @return The render thread side of this VertexBuffer
   */
  const Render::VertexBuffer* GetRenderObject() const;

protected:
  /**
   * @brief Destructor
   */
  ~VertexBuffer() override;

private: // implementation
  /**
   * @brief Default constructor
   */
  VertexBuffer();

  /**
   * Second stage initialization
   */
  void Initialize(Dali::Property::Map& format);

private: // unimplemented methods
  VertexBuffer(const VertexBuffer&);
  VertexBuffer& operator=(const VertexBuffer&);

private:                                                            // data
  EventThreadServices&        mEventThreadServices;                 ///<Used to send messages to the render thread via update thread
  Render::VertexBuffer*       mRenderObject{nullptr};               ///<Render side object
  VertexBufferUpdateCallback* mVertexBufferUpdateCallback{nullptr}; ///<Vertex buffer update callback pointer
  uint32_t                    mBufferFormatSize{0};
  uint32_t                    mSize{0};    ///< Number of elements in the buffer
  uint32_t                    mDivisor{0}; ///< How many elements to skip in instanced draw
};

/**
 * Get the implementation type from a Property::Type
 */
template<Property::Type type>
struct PropertyImplementationType
{
  // typedef ... Type; not defined, only support types declared below
};
template<>
struct PropertyImplementationType<Property::BOOLEAN>
{
  using Type = bool;
};
template<>
struct PropertyImplementationType<Property::FLOAT>
{
  using Type = float;
};
template<>
struct PropertyImplementationType<Property::INTEGER>
{
  using Type = int;
};
template<>
struct PropertyImplementationType<Property::VECTOR2>
{
  using Type = Vector2;
};
template<>
struct PropertyImplementationType<Property::VECTOR3>
{
  using Type = Vector3;
};
template<>
struct PropertyImplementationType<Property::VECTOR4>
{
  using Type = Vector4;
};
template<>
struct PropertyImplementationType<Property::MATRIX3>
{
  using Type = Matrix3;
};
template<>
struct PropertyImplementationType<Property::MATRIX>
{
  using Type = Matrix;
};
template<>
struct PropertyImplementationType<Property::RECTANGLE>
{
  using Type = Rect<int>;
};
template<>
struct PropertyImplementationType<Property::ROTATION>
{
  using Type = Quaternion;
};

uint32_t GetPropertyImplementationSize(Property::Type& propertyType);

} // namespace Internal

// Helpers for public-api forwarding methods
inline Internal::VertexBuffer& GetImplementation(Dali::VertexBuffer& handle)
{
  DALI_ASSERT_ALWAYS(handle && "VertexBuffer handle is empty");

  BaseObject& object = handle.GetBaseObject();

  return static_cast<Internal::VertexBuffer&>(object);
}

inline const Internal::VertexBuffer& GetImplementation(const Dali::VertexBuffer& handle)
{
  DALI_ASSERT_ALWAYS(handle && "VertexBuffer handle is empty");

  const BaseObject& object = handle.GetBaseObject();

  return static_cast<const Internal::VertexBuffer&>(object);
}

} // namespace Dali

#endif // DALI_INTERNAL_VERTEX_BUFFER_H
