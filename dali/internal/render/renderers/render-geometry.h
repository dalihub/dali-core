#ifndef DALI_INTERNAL_RENDER_GEOMETRY_H
#define DALI_INTERNAL_RENDER_GEOMETRY_H

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
 */

// INTERNAL INCLUDES
#include <dali/devel-api/common/owner-container.h>
#include <dali/graphics-api/graphics-controller.h>
#include <dali/graphics-api/graphics-types.h>
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/common/owner-pointer.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/rendering/geometry.h>

namespace Dali
{
namespace Internal
{
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
  using Type      = Dali::Geometry::Type;
  using IndexType = Dali::Graphics::Format;

  using Uint16ContainerType = Dali::Vector<uint16_t>;
  using Uint32ContainerType = Dali::Vector<uint32_t>;

  Geometry();

  /**
   * Destructor
   */
  ~Geometry();

  /**
   * Adds a property buffer to the geometry
   * @param[in] dataProvider The VertexBuffer data provider
   */
  void AddVertexBuffer(Render::VertexBuffer* vertexBuffer);

  /**
   * Set the data for the index buffer to be used by the geometry
   * @param[in] indices A vector containing the indices
   */
  void SetIndexBuffer(Uint16ContainerType& indices);

  /**
   * Set the data for the index buffer to be used by the geometry
   * @param[in] indices A vector containing the indices
   */
  void SetIndexBuffer(Uint32ContainerType& indices);

  /**
   * Removes a VertexBuffer from the geometry
   * @param[in] vertexBuffer The property buffer to be removed
   */
  void RemoveVertexBuffer(const Render::VertexBuffer* vertexBuffer);

  /**
   * Get the vertex buffers
   * @return the list of vertex buffers
   */
  [[nodiscard]] const Vector<Render::VertexBuffer*>& GetVertexBuffers() const;

  /**
   * Called from RenderManager to notify the geometry that current rendering pass has finished.
   */
  void OnRenderFinished();

  /**
   * Check if the attributes for the geometry have changed
   * @return True if vertex buffers have been added or removed since last frame, false otherwise
   */
  [[maybe_unused]] [[nodiscard]] bool AttributesChanged() const
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
   * @return the topology of this geometry
   */
  [[nodiscard]] Graphics::PrimitiveTopology GetTopology() const;

  /**
   * Upload the geometry if it has changed
   */
  void Upload(Graphics::Controller& graphicsController);

  /**
   * Set up the attributes and perform the Draw call corresponding to the geometry type.
   *
   * @param[in] graphicsController The graphics controller
   * @param[in,out] commandBuffer The current command buffer queue
   * @param[in] elementBufferOffset The index of first element to draw if index buffer bound
   * @param[in] elementBufferCount Number of elements to draw if index buffer bound, uses whole buffer when 0
   * @return true if the draw command was issued, false otherwise
   */
  bool Draw(Graphics::Controller&    graphicsController,
            Graphics::CommandBuffer& commandBuffer,
            uint32_t                 elementBufferOffset,
            uint32_t                 elementBufferCount);

  /**
   * @brief Set up the attributes bind commaneds
   *
   * @param[in,out] commandBuffer The current command buffer queue
   * @return true if the bind command was issued, false otherwise
   */
  bool BindVertexAttributes(Graphics::CommandBuffer& commandBuffer);

private:
  // VertexBuffers
  Vector<Render::VertexBuffer*> mVertexBuffers;

  Uint16ContainerType     mIndices;
  OwnerPointer<GpuBuffer> mIndexBuffer;
  IndexType               mIndexType;
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
