#ifndef DALI_INTERNAL_SCENE_GRAPH_PROPERTY_BUFFER_DATA_PROVIDER_H
#define DALI_INTERNAL_SCENE_GRAPH_PROPERTY_BUFFER_DATA_PROVIDER_H

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
 */

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/object/property.h>
#include <dali/internal/common/buffer-index.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

class PropertyBufferDataProvider
{
public:

  /**
   * Type for the data contained in the buffer
   */
  typedef Dali::Vector< char > BufferType;

public:
  /**
   * Constructor
   */
  PropertyBufferDataProvider()
  {
  }

  /**
   * Get the number of attributes
   * @param[in] bufferIndex Index to access double buffered values
   * @return the number of attributes
   */
  virtual unsigned int GetAttributeCount( BufferIndex bufferIndex ) const = 0;

  /**
   * Get the name of an attribute
   * @param[in] bufferIndex Index to access double buffered values
   * @param[in] index Index of the attribute
   * @return the name of the attribute
   */
  virtual const std::string& GetAttributeName( BufferIndex bufferIndex, unsigned int index ) const = 0;

  /**
   * Get the byte size of an attribute
   * @param[in] bufferIndex Index to access double buffered values
   * @param[in] index Index of the attribute
   * @return the byte size of the attribute
   */
  virtual size_t GetAttributeSize( BufferIndex bufferIndex, unsigned int index ) const = 0;

  /**
   * Get the type of an attribute
   * @param[in] bufferIndex Index to access double buffered values
   * @param[in] index Index of the attribute
   * @return the type of the attribute
   */
  virtual Property::Type GetAttributeType( BufferIndex bufferIndex, unsigned int index ) const = 0;

  /**
   * Get the byte offset of an attribute
   * @param[in] bufferIndex Index to access double buffered values
   * @param[in] index Index of the attribute
   * @return the byte offset of the attribute
   */
  virtual size_t GetAttributeOffset( BufferIndex bufferIndex, unsigned int index ) const = 0;

  /**
   * Get the property buffer data
   * @param[in] bufferIndex Index to access double buffered values
   * @return the property buffer's data array
   */
  virtual const BufferType& GetData( BufferIndex bufferIndex ) const = 0;

  /**
   * Get the size of the property buffer in bytes
   * @param[in] bufferIndex Index to access double buffered values
   * @return the size in bytes
   */
  virtual size_t GetDataSize( BufferIndex bufferIndex ) const = 0;

  /**
   * Get the size of an element of the buffer in bytes
   * @param[in] bufferIndex Index to access double buffered values
   * @return the element size in bytes
   */
  virtual size_t GetElementSize( BufferIndex bufferIndex ) const = 0;

  /**
   * Get the number of elements
   * @param[in] bufferIndex Index to access double buffered values
   * @return the number of elements
   */
  virtual unsigned int GetElementCount( BufferIndex bufferIndex ) const = 0;

  /**
   * Get Id of the GPU buffer associated with this propertyBuffer
   * @param[in] bufferIndex Index to access double buffered values
   * @return the Id
   */
  virtual unsigned int GetGpuBufferId( BufferIndex bufferIndex ) const = 0;

  /**
   * Checks if data in the PropertyBuffer has changed repect previous frame
   * @param[in] bufferIndex Index to access double buffered values
   * @return true if data has changed, false otherwise
   */
  virtual bool HasDataChanged( BufferIndex bufferIndex ) const = 0;

protected:
  /**
   * No deletion through this interface
   */
  virtual ~PropertyBufferDataProvider()
  {
  }
};

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_PROPERTY_BUFFER_DATA_PROVIDER_H
