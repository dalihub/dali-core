#ifndef DALI_INTERNAL_RENDER_VERTEX_BUFFER_H
#define DALI_INTERNAL_RENDER_VERTEX_BUFFER_H

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

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/property.h>

#include <dali/graphics-api/graphics-types.h>
#include <dali/internal/common/const-string.h>
#include <dali/internal/common/owner-pointer.h>
#include <dali/internal/render/renderers/gpu-buffer.h>

namespace Dali
{
namespace Internal
{
namespace Render
{
class VertexBuffer
{
public:
  struct Component
  {
    ConstString    name;
    uint32_t       offset;
    uint32_t       size;
    Property::Type type;
  };

  /**
   * Structure that holds the meta-data of the format of VertexBuffer.
   */
  struct Format
  {
    std::vector<Component> components;
    uint32_t               size;
  };

  /**
   * @brief Default constructor
   */
  VertexBuffer();

  /**
   * @brief Destructor
   */
  ~VertexBuffer();

  /**
   * @brief Set the format of the buffer
   *
   * This function takes ownership of the pointer
   *
   * @param[in] format The format for the VertexBuffer
   */
  void SetFormat(VertexBuffer::Format* format);

  /**
   * @brief Set the data of the VertexBuffer
   *
   * This function takes ownership of the pointer
   * @param[in] data The new data of the VertexBuffer
   * @param[in] size The new size of the buffer
   */
  void SetData(Dali::Vector<uint8_t>* data, uint32_t size);

  /**
   * Perform the upload of the buffer only when required
   * @param[in] graphicsController The controller
   * @return True if all data are valid so update success. False if some data are invalid.
   */
  bool Update(Graphics::Controller& graphicsController);

  /**
   * Get the number of attributes present in the buffer
   * @return The number of attributes stored in this buffer
   */
  [[nodiscard]] inline uint32_t GetAttributeCount() const
  {
    DALI_ASSERT_DEBUG(mFormat && "Format should be set ");
    return static_cast<uint32_t>(mFormat->components.size());
  }

  /**
   * Retrieve the i-essim attribute name
   * @param[in] index The index of the attribute
   * @return The name of the attribute
   */
  [[nodiscard]] inline ConstString GetAttributeName(uint32_t index) const
  {
    DALI_ASSERT_DEBUG(mFormat && "Format should be set ");
    return mFormat->components[index].name;
  }

  /**
   * Retrieve the size of the buffer in bytes
   * @return The total size of the buffer
   */
  [[nodiscard]] inline uint32_t GetDataSize() const
  {
    DALI_ASSERT_DEBUG(mFormat && "Format should be set ");
    return mFormat->size * mSize;
  }

  /**
   * Retrieve the size of one element of the buffer
   * @return The size of one element
   */
  [[nodiscard]] inline uint32_t GetElementSize() const
  {
    DALI_ASSERT_DEBUG(mFormat && "Format should be set ");
    return mFormat->size;
  }

  /**
   * Retrieve the number of elements in the buffer
   * @return The total number of elements
   */
  [[nodiscard]] inline uint32_t GetElementCount() const
  {
    return mSize;
  }

  [[nodiscard]] inline const VertexBuffer::Format* GetFormat() const
  {
    return mFormat.Get();
  }

  [[nodiscard]] inline const GpuBuffer* GetGpuBuffer() const
  {
    return mGpuBuffer.Get();
  }

  [[nodiscard]] inline bool IsDataChanged() const
  {
    return mDataChanged;
  }

private:
  OwnerPointer<VertexBuffer::Format>   mFormat;    ///< Format of the buffer
  OwnerPointer<Dali::Vector<uint8_t> > mData;      ///< Data
  OwnerPointer<GpuBuffer>              mGpuBuffer; ///< Pointer to the GpuBuffer associated with this RenderVertexBuffer

  uint32_t mSize;        ///< Number of Elements in the buffer
  bool     mDataChanged; ///< Flag to know if data has changed in a frame
};

} // namespace Render

} // namespace Internal

} // namespace Dali

#endif //  DALI_INTERNAL_RENDER_VERTEX_BUFFER_H
