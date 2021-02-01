#ifndef DALI_INTERNAL_RENDER_GEOMETRY_H
#define DALI_INTERNAL_RENDER_GEOMETRY_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
 */

// INTERNAL INCLUDES
#include <dali/devel-api/common/owner-container.h>
#include <dali/integration-api/gl-abstraction.h>
#include <dali/integration-api/gl-defines.h>
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/common/owner-pointer.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/rendering/geometry.h>

namespace Dali
{
namespace Internal
{
class Context;
class Program;
class GpuBuffer;

namespace Render
{
class VertexBuffer;

/**
 * This class encapsulates the GPU buffers. It is used to upload vertex data
 * to it's GPU buffers, to bind all the buffers and to setup/teardown vertex attribute
 * bindings
 */
class Geometry
{
public:
  using Type = Dali::Geometry::Type;

  Geometry();

  /**
   * Destructor
   */
  ~Geometry();

  /**
   * Called on Gl Context created
   */
  void GlContextCreated(Context& context);

  /**
   * Called on Gl Context destroyed.
   */
  void GlContextDestroyed();

  /**
   * Adds a property buffer to the geometry
   * @param[in] dataProvider The VertexBuffer data provider
   */
  void AddVertexBuffer(Render::VertexBuffer* vertexBuffer);

  /**
   * Set the data for the index buffer to be used by the geometry
   * @param[in] indices A vector containing the indices
   */
  void SetIndexBuffer(Dali::Vector<uint16_t>& indices);

  /**
   * Removes a VertexBuffer from the geometry
   * @param[in] vertexBuffer The property buffer to be removed
   */
  void RemoveVertexBuffer(const Render::VertexBuffer* vertexBuffer);

  /**
   * Gets the attribute locations on the shader for the attributes defined in the geometry RenderBuffers
   * @param[out] attributeLocation The vector where the attributes locations will be stored
   * @param[in] program The program
   * @param[in] bufferIndex The current buffer index
   */
  void GetAttributeLocationFromProgram(Vector<GLint>& attributeLocation, Program& program, BufferIndex bufferIndex) const;

  /**
   * Called from RenderManager to notify the geometry that current rendering pass has finished.
   */
  void OnRenderFinished();

  /**
   * Check if the attributes for the geometry have changed
   * @return True if vertex buffers have been added or removed since last frame, false otherwise
   */
  bool AttributesChanged() const
  {
    return mAttributesChanged;
  }

  /**
   * Sets the geometry type
   * @param[in] type The new geometry type
   */
  void SetType(Type type)
  {
    mGeometryType = type;
  }

  /**
   * Upload the geometry if it has changed
   * @param[in] context The GL context
   */
  void Upload(Context& context);

  /**
   * Set up the attributes and perform the Draw call corresponding to the geometry type
   * @param[in] context The GL context
   * @param[in] bufferIndex The current buffer index
   * @param[in] attributeLocation The location for the attributes in the shader
   * @param[in] elementBufferOffset The index of first element to draw if index buffer bound
   * @param[in] elementBufferCount Number of elements to draw if index buffer bound, uses whole buffer when 0
   */
  void Draw(Context&       context,
            BufferIndex    bufferIndex,
            Vector<GLint>& attributeLocation,
            uint32_t       elementBufferOffset,
            uint32_t       elementBufferCount);

private:
  // VertexBuffers
  Vector<Render::VertexBuffer*> mVertexBuffers;

  Dali::Vector<uint16_t>  mIndices;
  OwnerPointer<GpuBuffer> mIndexBuffer;
  Type                    mGeometryType;

  // Booleans
  bool mIndicesChanged : 1;
  bool mHasBeenUpdated : 1;
  bool mAttributesChanged : 1;
};

} // namespace Render
} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_RENDER_GEOMETRY_H
