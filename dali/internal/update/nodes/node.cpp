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
#include <dali/internal/update/nodes/node.h>

// INTERNAL INCLUDES
#include <dali/internal/common/internal-constants.h>
#include <dali/internal/common/memory-pool-object-allocator.h>
#include <dali/internal/update/common/discard-queue.h>
#include <dali/internal/update/manager/geometry-batcher.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/constants.h>

namespace //Unnamed namespace
{
//Memory pool used to allocate new nodes. Memory used by this pool will be released when shutting down DALi
Dali::Internal::MemoryPoolObjectAllocator<Dali::Internal::SceneGraph::Node> gNodeMemoryPool;
}

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

const PositionInheritanceMode Node::DEFAULT_POSITION_INHERITANCE_MODE( INHERIT_PARENT_POSITION );
const ColorMode Node::DEFAULT_COLOR_MODE( USE_OWN_MULTIPLY_PARENT_ALPHA );

Node* Node::New()
{
  return new ( gNodeMemoryPool.AllocateRawThreadSafe() ) Node();
}

Node::Node()
: mTransformManager( NULL ),
  mTransformId( INVALID_TRANSFORM_ID ),
  mParentOrigin( TRANSFORM_PROPERTY_PARENT_ORIGIN ),
  mAnchorPoint( TRANSFORM_PROPERTY_ANCHOR_POINT ),
  mSize( TRANSFORM_PROPERTY_SIZE ),                                               // Zero initialized by default
  mPosition( TRANSFORM_PROPERTY_POSITION ),                                       // Zero initialized by default
  mOrientation(),                                                                 // Initialized to identity by default
  mScale( TRANSFORM_PROPERTY_SCALE ),
  mVisible( true ),
  mColor( Color::WHITE ),
  mWorldPosition( TRANSFORM_PROPERTY_WORLD_POSITION, Vector3( 0.0f,0.0f,0.0f ) ), // Zero initialized by default
  mWorldScale( TRANSFORM_PROPERTY_WORLD_SCALE, Vector3( 1.0f,1.0f,1.0f ) ),
  mWorldOrientation(),                                                            // Initialized to identity by default
  mWorldMatrix(),
  mWorldColor( Color::WHITE ),
  mGeometryBatcher( NULL ),
  mBatchIndex( BATCH_NULL_HANDLE ),
  mClippingSortModifier( 0u ),
  mIsBatchParent( false ),
  mParent( NULL ),
  mBatchParent( NULL ),
  mExclusiveRenderTask( NULL ),
  mChildren(),
  mClippingDepth( 0u ),
  mDepthIndex( 0u ),
  mRegenerateUniformMap( 0 ),
  mDirtyFlags( AllFlags ),
  mDrawMode( DrawMode::NORMAL ),
  mColorMode( DEFAULT_COLOR_MODE ),
  mClippingMode( ClippingMode::DISABLED ),
  mIsRoot( false )
{
  mUniformMapChanged[0] = 0u;
  mUniformMapChanged[1] = 0u;
}

Node::~Node()
{
  if( mTransformId != INVALID_TRANSFORM_ID )
  {
    mTransformManager->RemoveTransform(mTransformId);
  }
}

void Node::operator delete( void* ptr )
{
  gNodeMemoryPool.FreeThreadSafe( static_cast<Node*>( ptr ) );
}

void Node::OnDestroy()
{
  // Animators, Constraints etc. should be disconnected from the child's properties.
  PropertyOwner::Destroy();
}

void Node::CreateTransform( SceneGraph::TransformManager* transformManager )
{
  //Create a new transform
  mTransformManager = transformManager;
  mTransformId = transformManager->CreateTransform();

  //Initialize all the animatable properties
  mPosition.Initialize( transformManager, mTransformId );
  mScale.Initialize( transformManager, mTransformId );
  mOrientation.Initialize( transformManager, mTransformId );
  mSize.Initialize( transformManager, mTransformId );
  mParentOrigin.Initialize( transformManager, mTransformId );
  mAnchorPoint.Initialize( transformManager, mTransformId );

  //Initialize all the input properties
  mWorldPosition.Initialize( transformManager, mTransformId );
  mWorldScale.Initialize( transformManager, mTransformId );
  mWorldOrientation.Initialize( transformManager, mTransformId );
  mWorldMatrix.Initialize( transformManager, mTransformId );
}

void Node::SetRoot(bool isRoot)
{
  DALI_ASSERT_DEBUG(!isRoot || mParent == NULL); // Root nodes cannot have a parent

  mIsRoot = isRoot;
}

void Node::AddUniformMapping( UniformPropertyMapping* map )
{
  PropertyOwner::AddUniformMapping( map );
  mRegenerateUniformMap = 2;
}

void Node::RemoveUniformMapping( const std::string& uniformName )
{
  PropertyOwner::RemoveUniformMapping( uniformName );
  mRegenerateUniformMap = 2;
}

void Node::PrepareRender( BufferIndex bufferIndex )
{
  if(mRegenerateUniformMap != 0 )
  {
    if( mRegenerateUniformMap == 2 )
    {
      CollectedUniformMap& localMap = mCollectedUniformMap[ bufferIndex ];
      localMap.Resize(0);

      for( unsigned int i=0, count=mUniformMaps.Count(); i<count; ++i )
      {
        localMap.PushBack( &mUniformMaps[i] );
      }
    }
    else if( mRegenerateUniformMap == 1 )
    {
      CollectedUniformMap& localMap = mCollectedUniformMap[ bufferIndex ];
      CollectedUniformMap& oldMap = mCollectedUniformMap[ 1-bufferIndex ];

      localMap.Resize( oldMap.Count() );

      unsigned int index=0;
      for( CollectedUniformMap::Iterator iter = oldMap.Begin(), end = oldMap.End() ; iter != end ; ++iter, ++index )
      {
        localMap[index] = *iter;
      }
    }
    --mRegenerateUniformMap;
    mUniformMapChanged[bufferIndex] = 1u;
  }
}

void Node::ConnectChild( Node* childNode )
{
  DALI_ASSERT_ALWAYS( this != childNode );
  DALI_ASSERT_ALWAYS( IsRoot() || NULL != mParent ); // Parent should be connected first
  DALI_ASSERT_ALWAYS( !childNode->IsRoot() && NULL == childNode->GetParent() ); // Child should be disconnected

  childNode->SetParent( *this );

  // Everything should be reinherited when reconnected to scene-graph
  childNode->SetAllDirtyFlags();

  // Add the node to the end of the child list.
  mChildren.PushBack( childNode );

  // Inform property observers of new connection
  childNode->ConnectToSceneGraph();
}

void Node::DisconnectChild( BufferIndex updateBufferIndex, Node& childNode )
{
  DALI_ASSERT_ALWAYS( this != &childNode );
  DALI_ASSERT_ALWAYS( childNode.GetParent() == this );

  // Find the childNode and remove it
  Node* found( NULL );

  const NodeIter endIter = mChildren.End();
  for ( NodeIter iter = mChildren.Begin(); iter != endIter; ++iter )
  {
    Node* current = *iter;
    if ( current == &childNode )
    {
      found = current;
      mChildren.Erase( iter ); // order matters here
      break; // iter is no longer valid
    }
  }
  DALI_ASSERT_ALWAYS( NULL != found );

  found->RecursiveDisconnectFromSceneGraph( updateBufferIndex );
}

void Node::AddRenderer( Renderer* renderer )
{
  // Check that it has not been already added.
  unsigned int rendererCount( mRenderer.Size() );
  for( unsigned int i(0); i < rendererCount; ++i )
  {
    if( mRenderer[i] == renderer )
    {
      // Renderer is already in the list.
      return;
    }
  }

  // If it is the first renderer added, make sure the world transform will be calculated
  // in the next update as world transform is not computed if node has no renderers.
  if( rendererCount == 0 )
  {
    mDirtyFlags |= TransformFlag;
  }

  mRenderer.PushBack( renderer );

  // Tell geometry batcher if should batch the child
  if( mRenderer.Size() == 1 && mRenderer[0]->mBatchingEnabled )
  {
    mGeometryBatcher->AddNode( this );
  }
}

void Node::RemoveRenderer( Renderer* renderer )
{
  unsigned int rendererCount( mRenderer.Size() );
  for( unsigned int i(0); i<rendererCount; ++i )
  {
    if( mRenderer[i] == renderer )
    {
      mRenderer.Erase( mRenderer.Begin()+i);
      return;
    }
  }
}

int Node::GetDirtyFlags() const
{
  // get initial dirty flags, they are reset ResetDefaultProperties, but setters may have made the node dirty already
  int flags = mDirtyFlags;

  // Check whether the visible property has changed
  if ( !mVisible.IsClean() )
  {
    flags |= VisibleFlag;
  }

  // Check whether the color property has changed
  if ( !mColor.IsClean() )
  {
    flags |= ColorFlag;
  }

  return flags;
}

void Node::ResetDefaultProperties( BufferIndex updateBufferIndex )
{
  mVisible.ResetToBaseValue( updateBufferIndex );
  mColor.ResetToBaseValue( updateBufferIndex );

  mDirtyFlags = NothingFlag;
}

void Node::SetParent( Node& parentNode )
{
  DALI_ASSERT_ALWAYS(this != &parentNode);
  DALI_ASSERT_ALWAYS(!mIsRoot);
  DALI_ASSERT_ALWAYS(mParent == NULL);

  mParent = &parentNode;

  if( mTransformId != INVALID_TRANSFORM_ID )
  {
    mTransformManager->SetParent( mTransformId, parentNode.GetTransformId() );
  }
}

void Node::SetBatchParent( Node* batchParentNode )
{
  DALI_ASSERT_ALWAYS(!mIsRoot);
  mBatchParent = batchParentNode;
}

void Node::SetIsBatchParent( bool enabled )
{
  if( mIsBatchParent != enabled )
  {
    mIsBatchParent = enabled;

    if( enabled )
    {
      mGeometryBatcher->AddBatchParent( this );
    }
    else
    {
      mGeometryBatcher->RemoveBatchParent( this );
    }
  }
}

void Node::RecursiveDisconnectFromSceneGraph( BufferIndex updateBufferIndex )
{
  DALI_ASSERT_ALWAYS(!mIsRoot);
  DALI_ASSERT_ALWAYS(mParent != NULL);

  const NodeIter endIter = mChildren.End();
  for ( NodeIter iter = mChildren.Begin(); iter != endIter; ++iter )
  {
    (*iter)->RecursiveDisconnectFromSceneGraph( updateBufferIndex );
  }

  // Animators, Constraints etc. should be disconnected from the child's properties.
  PropertyOwner::DisconnectFromSceneGraph( updateBufferIndex );

  // Remove back-pointer to parent
  mParent = NULL;

  // Remove all child pointers
  mChildren.Clear();

  if( mTransformId != INVALID_TRANSFORM_ID )
  {
    mTransformManager->SetParent( mTransformId, INVALID_TRANSFORM_ID );
  }
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
