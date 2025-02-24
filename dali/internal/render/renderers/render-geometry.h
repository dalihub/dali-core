#ifndef DALI_INTERNAL_RENDER_GEOMETRY_H
#define DALI_INTERNAL_RENDER_GEOMETRY_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <unordered_map>

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

  /**
   * Observer to determine when the geometry is no longer present
   */
  class LifecycleObserver
  {
  public:
    enum NotifyReturnType
    {
      STOP_OBSERVING,
      KEEP_OBSERVING,
    };

  public:
    /**
     * Called shortly if the geometry indices or vertex buffers are changed.
     * @return NotifyReturnType::STOP_OBSERVING if we will not observe this object after this called
     *         NotifyReturnType::KEEP_OBSERVING if we will observe this object after this called.
     */
    virtual NotifyReturnType GeometryBufferChanged(const Geometry* geometry) = 0;

    /**
     * Called shortly before the geometry is destroyed.
     */
    virtual void GeometryDestroyed(const Geometry* geometry) = 0;

  protected:
    /**
     * Virtual destructor, no deletion through this interface
     */
    virtual ~LifecycleObserver() = default;
  };

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
  [[maybe_unused]] [[nodiscard]] bool Updated() const
  {
    return mUpdated;
  }

  /**
   * Sets the geometry type
   * @param[in] type The new geometry type
   */
  void SetType(Type type)
  {
    mGeometryType = type;

    mUpdated = true;
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
   * @param[in] bufferIndex The current buffer index
   * @param[in] elementBufferOffset The index of first element to draw if index buffer bound
   * @param[in] elementBufferCount Number of elements to draw if index buffer bound, uses whole buffer when 0
   * @return true if the draw command was issued, false otherwise
   */
  bool Draw(Graphics::Controller&    graphicsController,
            Graphics::CommandBuffer& commandBuffer,
            uint32_t                 elementBufferOffset,
            uint32_t                 elementBufferCount);

  /**
   * Allows Geometry to track the life-cycle of this object.
   * Note that we allow to observe lifecycle multiple times.
   * But GeometryDestroyed callback will be called only one time.
   * @param[in] observer The observer to add.
   */
  void AddLifecycleObserver(LifecycleObserver& observer)
  {
    DALI_ASSERT_ALWAYS(!mObserverNotifying && "Cannot add observer while notifying Geometry::LifecycleObservers");

    auto iter = mLifecycleObservers.find(&observer);
    if(iter != mLifecycleObservers.end())
    {
      // Increase the number of observer count
      ++(iter->second);
    }
    else
    {
      mLifecycleObservers.insert({&observer, 1u});
    }
  }

  /**
   * The Geometry no longer needs to track the life-cycle of this object.
   * @param[in] observer The observer that to remove.
   */
  void RemoveLifecycleObserver(LifecycleObserver& observer)
  {
    DALI_ASSERT_ALWAYS(!mObserverNotifying && "Cannot remove observer while notifying Geometry::LifecycleObservers");

    auto iter = mLifecycleObservers.find(&observer);
    DALI_ASSERT_ALWAYS(iter != mLifecycleObservers.end());

    if(--(iter->second) == 0u)
    {
      mLifecycleObservers.erase(iter);
    }
  }

private:
  using LifecycleObserverContainer = std::unordered_map<LifecycleObserver*, uint32_t>; ///< Lifecycle observers container. We allow to add same observer multiple times.
                                                                                       ///< Key is a pointer to observer, value is the number of observer added.
  LifecycleObserverContainer mLifecycleObservers;

  // VertexBuffers
  Vector<Render::VertexBuffer*> mVertexBuffers;

  Uint16ContainerType     mIndices;
  OwnerPointer<GpuBuffer> mIndexBuffer;
  IndexType               mIndexType;
  Type                    mGeometryType;

  // Booleans
  bool mIndicesChanged : 1;
  bool mHasBeenUploaded : 1;
  bool mUpdated : 1; ///< Flag to know if data has changed in a frame. Value fixed after Upload() call, and reset as false after OnRenderFinished

  bool mObserverNotifying : 1; ///< Safety guard flag to ensure that the LifecycleObserver not be added or deleted while observing.
};

} // namespace Render
} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_RENDER_GEOMETRY_H
