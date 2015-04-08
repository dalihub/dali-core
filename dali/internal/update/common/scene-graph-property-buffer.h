#ifndef DALI_INTERNAL_SCENE_GRAPH_PROPERTY_BUFFER_H
#define DALI_INTERNAL_SCENE_GRAPH_PROPERTY_BUFFER_H

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
 */

#include <dali/internal/common/buffer-index.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/render/data-providers/property-buffer-data-provider.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class SceneController;

namespace PropertyBufferMetadata
{
/**
 * Structure that holds name and size of a component in the PropertyBuffer.
 */
struct Component
{
  std::string name;
  unsigned int accumulatedSize;
};

/**
 * Structure that holds the meta-data of the format of PropertyBuffer.
 */
struct Format
{
  std::vector<Component> components;

  /**
   * @brief Get the offset of a component within an element
   *
   * @pre index must be within 0 and components.Size()-1
   *
   * @return The offset for a component within an element
   */
  unsigned int GetComponentOffset( unsigned int index ) const;

  /**
   * @brief Get the size of an element
   *
   * @return The size of an element
   */
  unsigned int GetElementSize() const;
};

} // PropertyBufferMetadata

class PropertyBuffer : public PropertyOwner, public PropertyBufferDataProvider
{
public:

  /**
   * Constructor
   */
  PropertyBuffer();

  /**
   * Destructor
   */
  virtual ~PropertyBuffer();

  /**
   * Set the format of the buffer
   * @param[in] format The format for the PropertyBuffer
   */
  void SetFormat( PropertyBufferMetadata::Format* format );

  /**
   * Set the data of the PropertyBuffer
   * @param[in] data The new data of the PropertyBuffer
   */
  void SetData( PropertyBufferDataProvider::BufferType* data );

  //TODO:: MESH_REWORK  Remove this, should be a property
  void SetSize( unsigned int size );

  /**
   * Connect the object to the scene graph
   *
   * @param[in] sceneController The scene controller - used for sending messages to render thread
   * @param[in] bufferIndex The current buffer index - used for sending messages to render thread
   */
  void ConnectToSceneGraph( SceneController& sceneController, BufferIndex bufferIndex );

  /**
   * Disconnect the object from the scene graph
   */
  void DisconnectFromSceneGraph( SceneController& sceneController, BufferIndex bufferIndex );

public: // PropertyBufferDataProvider

  /**
   * @copydoc PropertyBufferDataProvider::HasDataChanged( BufferIndex bufferIndex )
   */
  virtual bool HasDataChanged( BufferIndex bufferIndex ) const;

  /**
   * @copydoc PropertyBufferDataProvider::HasDataChanged( BufferIndex bufferIndex )
   */
  virtual unsigned int GetAttributeCount( BufferIndex bufferIndex ) const;

  /**
   * @copydoc PropertyBufferDataProvider::GetAttributeName( BufferIndex bufferIndex, unsigned int index )
   */
  virtual const std::string& GetAttributeName( BufferIndex bufferIndex, unsigned int index ) const;

  /**
   * @copydoc PropertyBufferDataProvider::GetAttributeSize( BufferIndex bufferIndex, unsigned int index )
   */
  virtual size_t GetAttributeSize( BufferIndex bufferIndex, unsigned int index ) const;

  /**
   * @copydoc PropertyBufferDataProvider::GetAttributeOffset( BufferIndex bufferIndex, unsigned int index )
   */
  virtual size_t GetAttributeOffset( BufferIndex bufferIndex, unsigned int index ) const;

  /**
   * @copydoc PropertyBufferDataProvider::GetData( BufferIndex bufferIndex )
   */
  virtual const PropertyBufferDataProvider::BufferType& GetData( BufferIndex bufferIndex ) const;

  /**
   * @copydoc PropertyBufferDataProvider::GetDataSize( BufferIndex bufferIndex )
   */
  virtual size_t GetDataSize( BufferIndex bufferIndex ) const;

  /**
   * @copydoc PropertyBufferDataProvider::GetElementSize( BufferIndex bufferIndex )
   */
  virtual size_t GetElementSize( BufferIndex bufferIndex ) const;

  /**
   * @copydoc PropertyBufferDataProvider::GetElementCount( BufferIndex bufferIndex )
   */
  virtual unsigned int GetElementCount( BufferIndex bufferIndex ) const;

  /**
   * @copydoc PropertyBufferDataProvider::GetGpuBufferId
   */
  virtual unsigned int GetGpuBufferId( BufferIndex bufferIndex ) const;

private:
  OwnerPointer<PropertyBufferMetadata::Format> mFormat; ///< Format of the buffer
  OwnerPointer<PropertyBufferDataProvider::BufferType> mBufferData; ///< Data
  PropertyBufferDataProvider::BufferType* mRenderBufferData;

  //TODO: MESH_REWORK should be double buffered property
  unsigned int mSize; ///< Size of the buffer
};

inline void SetFormatMessage( EventThreadServices& eventThreadServices,
                              const PropertyBuffer& propertyBuffer,
                              PropertyBufferMetadata::Format* format )
{
  typedef MessageValue1< PropertyBuffer, OwnerPointer<PropertyBufferMetadata::Format> > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &propertyBuffer, &PropertyBuffer::SetFormat, format );
}

//TODO:: MESH_REWORK  Remove this, should be a property
inline void SetSizeMessage( EventThreadServices& eventThreadServices,
                            const PropertyBuffer& propertyBuffer,
                            unsigned int size )
{
  typedef MessageValue1< PropertyBuffer, unsigned int > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &propertyBuffer, &PropertyBuffer::SetSize, size );
}

inline void SetDataMessage( EventThreadServices& eventThreadServices,
                            const PropertyBuffer& propertyBuffer,
                            PropertyBuffer::BufferType* data )
{
  typedef MessageValue1< PropertyBuffer, OwnerPointer<PropertyBuffer::BufferType> > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &propertyBuffer, &PropertyBuffer::SetData, data );
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_PROPERTY_BUFFER_H
