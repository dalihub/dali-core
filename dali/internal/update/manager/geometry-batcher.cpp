/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali/internal/update/manager/geometry-batcher.h>

// INTERNAL INCLUDES
#include <dali/internal/render/common/render-item.h>
#include <dali/internal/render/common/render-tracker.h>
#include <dali/internal/render/common/render-instruction.h>
#include <dali/internal/render/common/render-instruction-container.h>
#include <dali/internal/render/shaders/scene-graph-shader.h>
#include <dali/internal/render/renderers/render-renderer.h>
#include <dali/internal/render/renderers/render-property-buffer.h>
#include <dali/internal/render/renderers/render-geometry.h>
#include <dali/internal/update/rendering/scene-graph-renderer.h>
#include <dali/internal/update/controllers/scene-controller.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/common/math.h>

namespace
{

// helper macros to deal with handles
#define BATCH_LOCAL_INDEX(x) (x&0xFFFF)
#define BATCH_PARENT_INDEX(x) ((x>>16)&0xFFFF)
#define BATCH_INDEX_CREATE( batchParentIndex, batchIndex ) ( ( ( (batchParentIndex)&0xFFFF ) << 16 ) | ( (batchIndex)&0xFFFF ) )

/**
 * The TransformVertexBufferInfo struct
 * Must be filled before transforming vertices
 */
struct TransformVertexBufferData
{
  void*                         destinationPtr;       ///< pointer to the destination vertex buffer
  const void*                   sourcePtr;            ///< pointer to the source vertex buffer
  float*                        transform;            ///< transform relative to batch parent
  const float*                  worldMatrix;          ///< model/world matrix of node being batched
  const float*                  parentInvWorldMatrix; ///< inv world matrix of batch parent
  unsigned                      componentSize;        ///< size of component
  unsigned                      vertexCount;          ///< number of vertices to process
  const float*                  size;                 ///< size of render item
};

/**
 * @brief function transforms vertices from 'source' and writes into 'destination'
 * @param[in,out] data Filled TransformVertexBufferInfo arguments structure
 */
template <typename PositionType >
void TransformVertexBuffer( TransformVertexBufferData& data )
{
  const PositionType* source = reinterpret_cast<const PositionType*>( data.sourcePtr );
  PositionType* destination = reinterpret_cast<PositionType*>( data.destinationPtr );

  size_t componentSize = data.componentSize ? data.componentSize : sizeof( PositionType );
  const void* sourceEnd = (reinterpret_cast<const char*>( source ) + ( data.vertexCount*componentSize ));
  for( ; source < sourceEnd;
       *(reinterpret_cast<char**>( &destination )) += componentSize,
       *(reinterpret_cast<const char**>( &source )) += componentSize
       )
  {
    Dali::Internal::MultiplyVectorBySize( *destination, *source, data.size );
    Dali::Internal::MultiplyVectorByMatrix4( *destination, data.transform, *destination );
  }
}

} //Unnamed namespace

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

/**
 * @brief The VertexDescriptor struct
 * Holds details of vertex format used for batching
 */
struct VertexDescriptor
{
  VertexDescriptor()
    : mVertexComponentSize( 0 ),
      mVertexPositionType( Dali::Property::NONE ),
      mVertexFormat( NULL )
  {
  }

  unsigned int mVertexComponentSize; ///< Vertex component size in bytes
  Dali::Property::Type mVertexPositionType;  ///< Vertex position type ( may be Vector2, Vector3, Vector4 )
  Dali::Internal::Render::PropertyBuffer::Format* mVertexFormat; ///< Vertex format cloned from the very first batched item
};

struct BatchKey
{
  BatchKey()
  : batchParentNode( NULL ),
    shader( NULL ),
    textureSet( NULL ),
    depthIndex( 0 )
  {
  }

  ~BatchKey()
  {
  }

  BatchKey( Node* node )
  {
    MakeFromNode( node );
  }

  inline void MakeFromNode( Node* node )
  {
    Renderer* renderer = node->GetRendererAt( 0 );
    batchParentNode = node->GetBatchParent();
    shader = &renderer->GetShader();
    textureSet = renderer->GetTextures();
    depthIndex = renderer->GetDepthIndex();
  }

  inline bool operator==( const BatchKey& key )
  {
    return batchParentNode == key.batchParentNode && shader == key.shader && textureSet == key.textureSet && depthIndex == key.depthIndex;
  }

  inline bool operator!=( const BatchKey& key )
  {
    return !( *this == key );
  }

  const Node*           batchParentNode;    /// batch parent node that owns batch
  const Shader*         shader;             /// shader associated with the batch
  const TextureSet*     textureSet;         /// texture set used by the batch
  int                   depthIndex;         /// depth index of the batch
};

/**
 * @brief The Batch struct
 * Stores details of single batch
 */
struct Batch
{
  Batch( const BatchKey& key, Render::Geometry* batchGeometry = NULL )
   : batchKey( key ),
     geometry( batchGeometry ),
     renderedFrame( 0 ),
     dirty( true )
  {}


  BatchKey                batchKey;       /// Unique batch key
  Vector<unsigned short>  indices;        /// index buffer per batch
  Render::Geometry*       geometry;       /// Batch geometry
  size_t                  renderedFrame;  /// Flag used to determine if batch has already rendered during a frame
  bool                    dirty;          /// 'dirty' flag per batch
};

typedef std::vector<Batch> BatchList;

/**
 * @brief The BatchParent struct
 * Stores list of children of single batch parent
 */
struct BatchParent
{
  Node*                   node;             /// Pointer to a node which is a parent for batch(es)
  Vector<Node*>           batchedChildren;  /// List of batchable children
  BatchList               batches;          /// List of batches which belong to this parent
  Render::PropertyBuffer* vertexBuffer;     /// Vertex buffer shared by all batches for this parent
  bool                    needsUpdate;      /// Flag indicating if batches should be updated
};

struct Impl
{
  Impl()
  : batchParents(),
    updateManager( NULL ),
    currentFrame( 0 )
  {
  }

  int GetBatchKeyIndex( size_t batchParentIndex, const BatchKey& key )
  {
    BatchParent& batchParent = batchParents[ batchParentIndex ];
    for( size_t j = 0; j < batchParent.batches.size(); ++j )
    {
      if( batchParent.batches[j].batchKey == key )
      {
        return BATCH_INDEX_CREATE( batchParentIndex, j );
      }
    }
    return -1;
  }

  std::vector<BatchParent>          batchParents; /// non-trivial type, hence std::vector
  UpdateManager*                    updateManager;
  size_t                            currentFrame;
};

GeometryBatcher::GeometryBatcher() :
  mImpl( NULL )
{
  mImpl = new Impl();
}

void GeometryBatcher::SetUpdateManager( UpdateManager* updateManager )
{
  mImpl->updateManager = updateManager;
}

GeometryBatcher::~GeometryBatcher()
{
  delete mImpl;
}

bool GeometryBatcher::CloneVertexFormat( const Render::Geometry* sourceGeometry, VertexDescriptor& vertexDescriptor )
{
  const Render::Geometry* geometry = sourceGeometry;
  const Render::PropertyBuffer::Format* format = geometry->GetPropertyBuffer( 0 )->GetFormat();

  if( !format )
  {
    return false;
  }

  Render::PropertyBuffer::Format* clonedVertexFormat = new Render::PropertyBuffer::Format( *format );
  Render::PropertyBuffer::Component& firstComponent = clonedVertexFormat->components[0];

  vertexDescriptor.mVertexPositionType = firstComponent.type;
  vertexDescriptor.mVertexComponentSize = clonedVertexFormat->size;
  vertexDescriptor.mVertexFormat = clonedVertexFormat;

  return true;
}

void GeometryBatcher::Update( BufferIndex bufferIndex )
{
  if( !mImpl->batchParents.empty() )
  {
    std::vector<BatchParent>::iterator iter = mImpl->batchParents.begin();
    std::vector<BatchParent>::iterator end = mImpl->batchParents.end();

    // for each Batch Parent
    for( size_t batchParentIndex = 0; iter != end; ++iter, ++batchParentIndex )
    {
      BatchParent& batchParentData = *iter;
      // Skip update if batch parent doesn't need it
      if( !batchParentData.needsUpdate )
      {
        continue;
      }

      Node* batchParentNode = batchParentData.node;

      // Skip if batch parent doesn't have batched children
      size_t size = batchParentData.batchedChildren.Size();
      if( !size )
      {
        batchParentData.needsUpdate = false;
        continue;
      }

      bool batchingFailed( false );
      uint32_t batchKeyIndex( BATCH_NULL_HANDLE );

      BatchKey oldKey;
      BatchKey key;
      VertexDescriptor vertexDescriptor;

      // Destination vertex buffer per batch parent
      Vector<char>& vertexBufferDest = *( new Vector<char>() );
      Render::PropertyBuffer* batchVertexBuffer = new Render::PropertyBuffer();

      size_t currentElementIndex = 0;

      Matrix invWorldMatrix( batchParentNode->GetWorldMatrix( bufferIndex ) );
      invWorldMatrix.Invert();

      // For each batched child of this batch parent...
      for( size_t i = 0; i < size; ++i )
      {
        Node* node = batchParentData.batchedChildren[i];

        const SceneGraph::Renderer* renderer = node->GetRendererAt( 0 );

        // Geometry
        const Render::Geometry* geometry = &renderer->GetGeometry();

        // Generate batch key
        key.MakeFromNode( node );

        // format of first property buffer
        const Render::PropertyBuffer* vertexBuffer = geometry->GetPropertyBuffer( 0 );

        // Geometry of the node may not be ready, in that case we discard whole batch
        if( !vertexBuffer || ( !vertexDescriptor.mVertexFormat && !CloneVertexFormat( geometry, vertexDescriptor ) ) )
        {
          batchingFailed = true;
          break;
        }

        // Instantiate new batch
        if( oldKey != key )
        {
          oldKey = key;
          batchKeyIndex = mImpl->GetBatchKeyIndex( batchParentIndex, key );

          if( batchKeyIndex == BATCH_NULL_HANDLE )
          {
            // Create new batch geometry
            Render::Geometry* newGeometry = new Render::Geometry();

            Batch batch( key, newGeometry );

            // push new batch
            batchParentData.batches.push_back( batch );

            // rebuild handle
            batchKeyIndex = BATCH_INDEX_CREATE( batchParentIndex, batchParentData.batches.size()-1 );

            // Vertex buffer may be set before it's filled with data
            newGeometry->AddPropertyBuffer( batchVertexBuffer );

            // Register geometry with update manager
            mImpl->updateManager->AddGeometry( newGeometry );
          }
        }

        // Tell node which batch it belongs to
        node->mBatchIndex = batchKeyIndex;

        uint32_t localIndex = BATCH_LOCAL_INDEX( batchKeyIndex );

        if( !batchParentData.batches[ localIndex ].dirty )
        {
          continue;
        }

        const uint32_t vertexBufferSize = vertexBuffer->GetDataSize();
        const char* vertexDataSource = &vertexBuffer->GetData()[ 0 ];

        uint32_t currentSize = vertexBufferDest.Size();
        vertexBufferDest.Resize( currentSize + vertexBufferSize );
        char* vertexDataDest = &vertexBufferDest[ currentSize ];

        // copy data as they are
        std::copy( vertexDataSource, vertexDataSource + vertexBufferSize, vertexDataDest );

        // transform node
        const Matrix& worldMatrix = node->GetWorldMatrix( bufferIndex );

        // vertex count
        const unsigned int sourceVertexCount = vertexBufferSize / vertexDescriptor.mVertexComponentSize;

        // compute transform for the node
        TransformVertexBufferData transformParameters;
        transformParameters.destinationPtr = vertexDataDest;
        transformParameters.sourcePtr = vertexDataSource;

        // perform transformation
        Matrix transformMatrix;
        Dali::Internal::MultiplyMatrices( transformMatrix.AsFloat(), worldMatrix.AsFloat(), invWorldMatrix.AsFloat() );
        transformParameters.transform = transformMatrix.AsFloat();
        transformParameters.componentSize = vertexDescriptor.mVertexComponentSize;
        transformParameters.vertexCount = sourceVertexCount;
        transformParameters.size = node->GetSize( bufferIndex ).AsFloat();

        // Perform vertex transform based on the vertex format
        switch( vertexDescriptor.mVertexPositionType )
        {
          case Dali::Property::VECTOR2:
          {
            TransformVertexBuffer<Vec2>( transformParameters );
            break;
          }
          case Dali::Property::VECTOR3:
          {
            TransformVertexBuffer<Vec3>( transformParameters );
            break;
          }
          case Dali::Property::VECTOR4:
          {
            TransformVertexBuffer<Vec4>( transformParameters );
            break;
          }
          default:
          {
            DALI_ASSERT_ALWAYS( true && "Incorrect vertex format! Use Vector2, Vector3 or Vector4 as position!" );
          }
        }

        // update index buffer
        Batch& batch = batchParentData.batches[ localIndex ];
        uint32_t currentIndexOffset = batch.indices.Size();
        batch.indices.Resize( batch.indices.Size() + sourceVertexCount );
        for( size_t k = 0; k < sourceVertexCount; ++k )
        {
          size_t index = currentElementIndex + k;
          batch.indices[k + currentIndexOffset] = (unsigned short)index;
        }

        currentElementIndex += sourceVertexCount;
      }

      if( batchingFailed )
      {
        delete &vertexBufferDest;
        delete batchVertexBuffer;
        continue;
      }

      // Add shared property buffer
      mImpl->updateManager->AddPropertyBuffer( batchVertexBuffer );
      batchVertexBuffer->SetFormat( vertexDescriptor.mVertexFormat );
      batchVertexBuffer->SetData( &vertexBufferDest, vertexBufferDest.Size()/vertexDescriptor.mVertexComponentSize );

      batchParentData.needsUpdate = false;
      batchParentData.vertexBuffer = batchVertexBuffer;

      // Update index buffers for all batches own by that batch parent
      std::vector<Batch>::iterator iter = batchParentData.batches.begin();
      std::vector<Batch>::iterator end = batchParentData.batches.end();
      for( ; iter != end; ++iter )
      {
        Batch& batch = (*iter);
        batch.geometry->SetIndexBuffer( batch.indices );
        batch.dirty = false;
      }
    }
  }
  ++mImpl->currentFrame;
}

void GeometryBatcher::AddBatchParent( Node* node )
{
  BatchParent batchParent;
  batchParent.node = node;
  batchParent.needsUpdate = true;
  batchParent.batchedChildren.Clear();

  mImpl->batchParents.push_back( batchParent );
}

void GeometryBatcher::RemoveBatchParent( Node* node )
{
  for( size_t i = 0; i < mImpl->batchParents.size(); ++i )
  {
    BatchParent& batchParent = mImpl->batchParents[i];
    if( node == batchParent.node )
    {
      // tell children they're not batched anymore
      Vector<Node*>::Iterator iter = batchParent.batchedChildren.Begin();
      Vector<Node*>::Iterator end = batchParent.batchedChildren.End();
      for( ; iter != end; ++iter )
      {
        Node* child = *iter;
        child->mBatchIndex = BATCH_NULL_HANDLE;
      }

      // delete all resources that belongs to the batch parent
      for( size_t j = 0; j < batchParent.batches.size(); ++j )
      {
        Batch& batch = batchParent.batches[j];
        mImpl->updateManager->RemoveGeometry( batch.geometry );
      }

      // delete main vertex buffer
      mImpl->updateManager->RemovePropertyBuffer( batchParent.vertexBuffer );

      return;
    }
  }
}

void GeometryBatcher::AddNode( Node* node )
{
  // look for batch parent
  Node* currentNode = node->GetParent();
  Node* batchParent = NULL;
  while( currentNode )
  {
    if( currentNode->mIsBatchParent )
    {
      batchParent = currentNode;
    }
    currentNode = currentNode->GetParent();
  }

  if( batchParent )
  {
    // find batch parent
    for( size_t i = 0; i < mImpl->batchParents.size(); ++i )
    {
      if( mImpl->batchParents[i].node == batchParent )
      {
        mImpl->batchParents[i].batchedChildren.PushBack( node );
        node->SetBatchParent( batchParent );
        mImpl->batchParents[i].needsUpdate = true;
        break;
      }
    }
  }
}

void GeometryBatcher::RemoveNode( Node* node )
{
  if( node->mBatchIndex == BATCH_NULL_HANDLE )
  {
    return;
  }

  uint32_t parentIndex = BATCH_PARENT_INDEX( node->mBatchIndex );

  BatchParent& batchParent = mImpl->batchParents[ parentIndex ];

  // delete all batches from batch parent
  for( size_t i = 0; i < batchParent.batches.size(); ++i )
  {
    Batch& batch = batchParent.batches[ i ];

    // delete geometry
    mImpl->updateManager->RemoveGeometry( batch.geometry );
  }

  batchParent.batches.clear();

  // for all children reset batch index to BATCH_NULL_HANDLE
  for( size_t i = 0; i < batchParent.batchedChildren.Size(); )
  {
    Node* child = batchParent.batchedChildren[i];

    if( node == child )
    {
      batchParent.batchedChildren.Erase( batchParent.batchedChildren.Begin() + i );
    }
    else
    {
      child->mBatchIndex = BATCH_NULL_HANDLE;
      ++i;
    }
  }

  mImpl->updateManager->RemovePropertyBuffer( batchParent.vertexBuffer );
  batchParent.needsUpdate = true;
}

bool GeometryBatcher::HasRendered( uint32_t batchIndex )
{
  return mImpl->batchParents[ BATCH_PARENT_INDEX( batchIndex ) ].batches[ BATCH_LOCAL_INDEX( batchIndex ) ].renderedFrame == mImpl->currentFrame;
}

void GeometryBatcher::SetRendered( uint32_t batchIndex )
{
  mImpl->batchParents[ BATCH_PARENT_INDEX( batchIndex ) ].batches[ BATCH_LOCAL_INDEX( batchIndex ) ].renderedFrame = mImpl->currentFrame;
}

Render::Geometry* GeometryBatcher::GetGeometry( uint32_t batchIndex )
{
  return mImpl->batchParents[ BATCH_PARENT_INDEX( batchIndex) ].batches[ BATCH_LOCAL_INDEX( batchIndex ) ].geometry;
}

} // namespace SceneGraph

} // namespace Internal


} // namespace Dali
