#ifndef DALI_INTERNAL_SCENE_GRAPH_PROPERTY_BUFFER_H
#define DALI_INTERNAL_SCENE_GRAPH_PROPERTY_BUFFER_H

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
#include <dali/public-api/actors/sampling.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/rendering/sampler.h>
#include <dali/graphics-api/graphics-api-accessor.h>
#include <dali/graphics-api/graphics-api-buffer.h>
#include <dali/integration-api/graphics/graphics.h>
#include <dali/internal/common/message.h>
#include <dali/internal/common/owner-pointer.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/update/rendering/scene-graph-sampler.h>

namespace Dali
{
namespace Graphics
{
namespace API
{
class Controller;
} // API
} // Graphics

namespace Internal
{
namespace SceneGraph
{

class PropertyBuffer
{
public:

  struct Component
  {
    std::string     name;
    unsigned int    offset;
    unsigned int    size;
    Property::Type  type;
  };

  /**
   * Structure that holds the meta-data of the format of PropertyBuffer.
   */
  struct Format
  {
    std::vector<Component> components;
    unsigned int           size;
  };

  /**
   * @brief Default constructor
   */
  PropertyBuffer();

  /**
   * @brief Destructor
   */
  ~PropertyBuffer();

  /**
   * Initialize the shader object with the Graphics API when added to UpdateManager
   *
   * @param[in] graphics The Graphics API
   */
  void Initialize( Integration::Graphics::Graphics& graphics );

  /**
   * @brief Set the format of the buffer
   *
   * This function takes ownership of the pointer
   *
   * @param[in] format The format for the PropertyBuffer
   */
  void SetFormat( OwnerPointer<PropertyBuffer::Format>& format );


  /**
   * @brief Set the data of the PropertyBuffer
   *
   * This function takes ownership of the pointer
   * @param[in] data The new data of the PropertyBuffer
   * @param[in] size The new size of the buffer
   */
  void SetData( OwnerPointer< Dali::Vector<char> >& data, size_t size );

  /**
   * @brief Set the number of elements
   * @param[in] size The number of elements
   */
  void SetSize( unsigned int size );

  /**
   * Perform the upload of the buffer only when required
   * @param[in] controller
   * @return
   */
  bool Update( Dali::Graphics::API::Controller& controller );

  /**
   * Get the number of attributes present in the buffer
   * @return The number of attributes stored in this buffer
   */
  inline unsigned int GetAttributeCount() const
  {
    DALI_ASSERT_DEBUG( mFormat && "Format should be set ");
    return mFormat->components.size();
  }

  /**
   * Retrieve the i-essim attribute name
   * @param[in] index The index of the attribute
   * @return The name of the attribute
   */
  inline const std::string& GetAttributeName(unsigned int index ) const
  {
    DALI_ASSERT_DEBUG( mFormat && "Format should be set ");
    return mFormat->components[index].name;
  }

  /**
   * Retrieve the size of the buffer in bytes
   * @return The total size of the buffer
   */
  inline std::size_t GetDataSize() const
  {
    DALI_ASSERT_DEBUG( mFormat && "Format should be set ");
    return mFormat->size * mSize;
  }

  /**
   * Retrieve the size of one element of the buffer
   * @return The size of one element
   */
  inline std::size_t GetElementSize() const
  {
    DALI_ASSERT_DEBUG( mFormat && "Format should be set ");
    return mFormat->size;
  }

  /**
   * Retrieve the number of elements in the buffer
   * @return The total number of elements
   */
  inline unsigned int GetElementCount() const
  {
    return mSize;
  }

  inline const PropertyBuffer::Format* GetFormat() const
  {
    return mFormat.Get();
  }
  inline Graphics::API::Accessor<Graphics::API::Buffer> GetGfxObject() const
  {
    return mGraphicsBuffer;
  }

  void SetUsage( Graphics::API::Buffer::UsageHint usage );

private:
  Integration::Graphics::Graphics*        mGraphics;  ///< Graphics interface object
  OwnerPointer< PropertyBuffer::Format >  mFormat;    ///< Format of the buffer
  OwnerPointer< Dali::Vector< char > >    mData;      ///< Data

  size_t mSize;       ///< Number of Elements in the buffer
  bool mDataChanged;  ///< Flag to know if data has changed in a frame

  // GRAPHICS
  Graphics::API::Accessor<Graphics::API::Buffer> mGraphicsBuffer;
  Graphics::API::Buffer::UsageHint mGraphicsBufferUsage;
};


inline void SetPropertyBufferFormatMessage( EventThreadServices& eventThreadServices, SceneGraph::PropertyBuffer& propertyBuffer, OwnerPointer< SceneGraph::PropertyBuffer::Format>& format )
{
  // Message has ownership of PropertyBuffer::Format while in transit from event -> update
  typedef MessageValue1< SceneGraph::PropertyBuffer, OwnerPointer< SceneGraph::PropertyBuffer::Format> > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ), false );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &propertyBuffer, &PropertyBuffer::SetFormat, format );
}

inline void SetPropertyBufferDataMessage( EventThreadServices& eventThreadServices, SceneGraph::PropertyBuffer& propertyBuffer, OwnerPointer< Vector<char> >& data, size_t size )
{
  // Message has ownership of PropertyBuffer data while in transit from event -> update
  typedef MessageValue2< SceneGraph::PropertyBuffer, OwnerPointer< Vector<char> >, size_t  > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ), false );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &propertyBuffer, &PropertyBuffer::SetData, data, size );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali


#endif //  DALI_INTERNAL_SCENE_GRAPH_PROPERTY_BUFFER_H
