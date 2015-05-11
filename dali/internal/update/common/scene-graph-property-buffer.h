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
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/update/common/double-buffered-property.h>
#include <dali/internal/update/common/property-owner.h>
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
  unsigned int offset;
  unsigned int size;
};

/**
 * Structure that holds the meta-data of the format of PropertyBuffer.
 */
struct Format
{
  std::vector<Component> components;
  unsigned int size;
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
   * @brief Set the format of the buffer
   *
   * This function takes ownership of the pointer
   *
   * @param[in] format The format for the PropertyBuffer
   */
  void SetFormat( PropertyBufferMetadata::Format* format );

  /**
   * @brief Set the data of the PropertyBuffer
   *
   * This function takes ownership of the pointer
   *
   * @param[in] bufferIndex Index for double buffered values
   * @param[in] data The new data of the PropertyBuffer
   */
  void SetData( BufferIndex bufferIndex, PropertyBufferDataProvider::BufferType* data );

  /**
   * @brief Set the number of elements
   *
   * @param[in] bufferIndex Index for double buffered values
   * @param[in] size The number of elements
   */
  void SetSize( BufferIndex bufferIndex, unsigned int size );

  /**
   * @brief Connect the object to the scene graph
   *
   * @param[in] sceneController The scene controller - used for sending messages to render thread
   * @param[in] bufferIndex The current buffer index - used for sending messages to render thread
   */
  void ConnectToSceneGraph( SceneController& sceneController, BufferIndex bufferIndex );

  /**
   * @brief Disconnect the object from the scene graph
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

protected: // From PropertyOwner
  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyOwner::ResetDefaultProperties()
   */
  virtual void ResetDefaultProperties( BufferIndex updateBufferIndex );

private:
  OwnerPointer<PropertyBufferMetadata::Format> mFormat; ///< Format of the buffer

  DoubleBuffered< OwnerPointer<PropertyBufferDataProvider::BufferType> > mBufferData; ///< Data
  DoubleBuffered< bool > mDataChanged; ///< Flag to know if data has changed in a frame

  DoubleBufferedProperty<unsigned int>  mSize; ///< Number of Elements in the buffer
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

inline void SetSizeMessage( EventThreadServices& eventThreadServices,
                            const PropertyBuffer& propertyBuffer,
                            unsigned int size )
{
  typedef MessageDoubleBuffered1 < PropertyBuffer, unsigned int > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &propertyBuffer, &PropertyBuffer::SetSize, size );
}

inline void SetDataMessage( EventThreadServices& eventThreadServices,
                            const PropertyBuffer& propertyBuffer,
                            PropertyBuffer::BufferType* data )
{
  typedef MessageDoubleBuffered1< PropertyBuffer, OwnerPointer<PropertyBuffer::BufferType> > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &propertyBuffer, &PropertyBuffer::SetData, data );
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_PROPERTY_BUFFER_H
