/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali/internal/update/rendering/scene-graph-geometry.h>

// INTERNAL HEADERS
#include <dali/integration-api/debug.h>
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/update/rendering/scene-graph-property-buffer.h>

#include <dali/graphics-api/graphics-buffer.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

Geometry::Geometry()
: mGraphicsController( nullptr ),
  mIndexBuffer{ nullptr },
  mIndexBufferElementCount( 0 ),
  mGeometryType( Dali::Geometry::TRIANGLES ),
  mIndicesChanged(false),
  mHasBeenUpdated(false),
  mAttributesChanged(true),
  mHasIndexBuffer(false)
{
}

Geometry::~Geometry()
{
}

void Geometry::Initialize( Graphics::Controller& graphics )
{
  mGraphicsController = &graphics;
}

void Geometry::AddPropertyBuffer( SceneGraph::PropertyBuffer* propertyBuffer )
{
  mVertexBuffers.PushBack( propertyBuffer );
  mAttributesChanged = true;
}

void Geometry::SetIndexBuffer( Dali::Vector<unsigned short>& indices )
{
  // set new index buffer
  auto sizeInBytes = uint32_t(indices.Size() * sizeof(indices[0]));
  auto indexBuffer = mGraphicsController->CreateBuffer( mGraphicsController->GetBufferFactory()
                                                        .SetSize( uint32_t(indices.Size() * sizeof(indices[0])) )
                                                        .SetUsageFlags( 0u | Graphics::BufferUsage::INDEX_BUFFER )
  );

  // transfer data
  indexBuffer->Write( indices.begin(), sizeInBytes, 0u );

  mIndexBuffer = std::move(indexBuffer);

  mIndexBufferElementCount = uint32_t(indices.Size());

  mHasIndexBuffer = true;
}

void Geometry::RemovePropertyBuffer( const SceneGraph::PropertyBuffer* propertyBuffer )
{
  size_t bufferCount = mVertexBuffers.Size();
  for( size_t i(0); i<bufferCount; ++i )
  {
    if( propertyBuffer == mVertexBuffers[i] )
    {
      //This will delete the gpu buffer associated to the RenderPropertyBuffer if there is one
      mVertexBuffers.Remove( mVertexBuffers.Begin()+i );
      mAttributesChanged = true;
      break;
    }
  }
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
