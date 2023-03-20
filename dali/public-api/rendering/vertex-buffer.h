#ifndef DALI_VERTEX_BUFFER_H
#define DALI_VERTEX_BUFFER_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <cstddef> // std::size_t
#include <string>  // std::string

// INTERNAL INCLUDES
#include <dali/public-api/object/handle.h>       // Dali::Handle
#include <dali/public-api/object/property-map.h> // Dali::Property::Map

namespace Dali
{
/**
 * @addtogroup dali_core_rendering_effects
 * @{
 */

namespace Internal DALI_INTERNAL
{
class VertexBuffer;
}

/**
 * @brief VertexBuffer is a handle to an object that contains a buffer of structured data.
 *
 * VertexBuffers can be used to provide data to Geometry objects.
 *
 * Example:
 *
 *  const float halfQuadSize = .5f;
 *  struct TexturedQuadVertex { Vector2 position; Vector2 textureCoordinates; };
 *  TexturedQuadVertex texturedQuadVertexData[4] = {
 *    { Vector2(-halfQuadSize, -halfQuadSize), Vector2(0.f, 0.f) },
 *    { Vector2( halfQuadSize, -halfQuadSize), Vector2(1.f, 0.f) },
 *    { Vector2(-halfQuadSize,  halfQuadSize), Vector2(0.f, 1.f) },
 *    { Vector2( halfQuadSize,  halfQuadSize), Vector2(1.f, 1.f) } };
 *
 *  Property::Map texturedQuadVertexFormat;
 *  texturedQuadVertexFormat["aPosition"] = Property::VECTOR2;
 *  texturedQuadVertexFormat["aTexCoord"] = Property::VECTOR2;
 *  VertexBuffer texturedQuadVertices = VertexBuffer::New( texturedQuadVertexFormat );
 *  texturedQuadVertices.SetData( texturedQuadVertexData, 4 );
 *
 *  // Create indices
 *  uint32_t indexData[6] = { 0, 3, 1, 0, 2, 3 };
 *
 *  // Create the geometry object
 *  Geometry texturedQuadGeometry = Geometry::New();
 *  texturedQuadGeometry.AddVertexBuffer( texturedQuadVertices );
 *  texturedQuadGeometry.SetIndexBuffer( indexData, sizeof(indexData)/sizeof(indexData[0] );
 *
 * @SINCE_1_1.43
 */
class DALI_CORE_API VertexBuffer : public BaseHandle
{
public:
  /**
   * @brief Creates a VertexBuffer.
   *
   * @SINCE_1_9.27
   * @param[in] bufferFormat Map of names and types that describes the components of the buffer
   * @return Handle to a newly allocated VertexBuffer
   */
  static VertexBuffer New(Dali::Property::Map& bufferFormat);

  /**
   * @brief Default constructor, creates an empty handle.
   *
   * @SINCE_1_9.27
   */
  VertexBuffer();

  /**
   * @brief Destructor.
   *
   * @SINCE_1_9.27
   */
  ~VertexBuffer();

  /**
   * @brief Copy constructor, creates a new handle to the same object.
   *
   * @SINCE_1_9.27
   * @param[in] handle Handle to an object
   */
  VertexBuffer(const VertexBuffer& handle);

  /**
   * @brief Downcasts to a property buffer handle.
   * If not, a property buffer the returned property buffer handle is left uninitialized.
   *
   * @SINCE_1_9.27
   * @param[in] handle Handle to an object
   * @return Property buffer handle or an uninitialized handle
   */
  static VertexBuffer DownCast(BaseHandle handle);

  /**
   * @brief Assignment operator, changes this handle to point at the same object.
   *
   * @SINCE_1_9.27
   * @param[in] handle Handle to an object
   * @return Reference to the assigned object
   */
  VertexBuffer& operator=(const VertexBuffer& handle);

  /**
   * @brief Move constructor.
   *
   * @SINCE_1_9.27
   * @param[in] rhs A reference to the moved handle
   */
  VertexBuffer(VertexBuffer&& rhs) noexcept;

  /**
   * @brief Move assignment operator.
   *
   * @SINCE_1_9.27
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this handle
   */
  VertexBuffer& operator=(VertexBuffer&& rhs) noexcept;

  /**
   * @brief Updates the whole buffer information.
   *
   * This function expects a pointer to an array of structures with the same
   * format that was given in the construction, and the number of elements to
   * be the same as the size of the buffer.
   *
   * If the initial structure was: { { "position", VECTOR3}, { "uv", VECTOR2 } }
   * and a size of 10 elements, this function should be called with a pointer equivalent to:
   * <pre>
   * struct Vertex {
   *   Dali::Vector3 position;
   *   Dali::Vector2 uv;
   * };
   * Vertex vertices[ 10 ] = { ... };
   * vertexBuffer.SetData( vertices );
   * </pre>
   *
   * @SINCE_1_9.27
   * @param[in] data A pointer to the data that will be copied to the buffer
   * @param[in] size Number of elements to expand or contract the buffer
   */
  void SetData(const void* data, std::size_t size);

  /**
   * @brief Gets the number of elements in the buffer.
   *
   * @SINCE_1_9.27
   * @return Number of elements to expand or contract the buffer
   */
  std::size_t GetSize() const;

public:
  /**
   * @brief The constructor.
   * @note  Not intended for application developers.
   * @SINCE_1_9.27
   * @param[in] pointer A pointer to a newly allocated VertexBuffer
   */
  explicit DALI_INTERNAL VertexBuffer(Internal::VertexBuffer* pointer);
};

/**
 * @}
 */
} // namespace Dali

#endif // DALI_VERTEX_BUFFER_H
