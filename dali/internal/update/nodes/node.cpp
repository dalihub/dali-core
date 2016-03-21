/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/internal/update/node-attachments/node-attachment.h>
#include <dali/internal/update/common/discard-queue.h>
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
: mParentOrigin( ParentOrigin::DEFAULT ),
  mAnchorPoint( AnchorPoint::DEFAULT ),
  mSize(),     // zero initialized by default
  mPosition(), // zero initialized by default
  mOrientation(), // initialized to identity by default
  mScale( Vector3::ONE ),
  mVisible( true ),
  mColor( Color::WHITE ),
  mWorldPosition(), // zero initialized by default
  mWorldOrientation(), // initialized to identity by default
  mWorldScale( Vector3::ONE ),
  mWorldMatrix(),
  mWorldColor( Color::WHITE ),
  mParent( NULL ),
  mExclusiveRenderTask( NULL ),
  mAttachment( NULL ),
  mChildren(),
  mRegenerateUniformMap( 0 ),
  mDepth(0u),
  mDirtyFlags(AllFlags),
  mIsRoot( false ),
  mInheritPosition( true ),
  mInheritOrientation( true ),
  mInheritScale( true ),
  mDrawMode( DrawMode::NORMAL ),
  mPositionInheritanceMode( DEFAULT_POSITION_INHERITANCE_MODE ),
  mColorMode( DEFAULT_COLOR_MODE )
{
  mUniformMapChanged[0] = 0u;
  mUniformMapChanged[1] = 0u;
}

Node::~Node()
{
}

void Node::operator delete( void* ptr )
{
  gNodeMemoryPool.FreeThreadSafe( static_cast<Node*>( ptr ) );
}

void Node::OnDestroy()
{
  // Node attachments should be notified about the disconnection.
  if ( mAttachment )
  {
    mAttachment->OnDestroy();
  }

  // Animators, Constraints etc. should be disconnected from the child's properties.
  PropertyOwner::Destroy();
}

void Node::Attach( NodeAttachment& object )
{
  DALI_ASSERT_DEBUG(!mAttachment);

  object.SetParent(*this);

  mAttachment = &object;
  SetAllDirtyFlags();

  mAttachment->ConnectedToSceneGraph();
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

  // Inform child node attachment that the node has been added to the stage
  if( childNode->mAttachment )
  {
    childNode->mAttachment->ConnectedToSceneGraph();
  }
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
  const bool sizeFlag = mSize.IsClean();

  if ( !(flags & TransformFlag) )
  {
    // Check whether the transform related properties have changed
    if( !sizeFlag            ||
        !mPosition.IsClean() ||
        !mOrientation.IsClean() ||
        !mScale.IsClean()    ||
        mParentOrigin.InputChanged() || // parent origin and anchor point rarely change
        mAnchorPoint.InputChanged() )
    {
      flags |= TransformFlag;
    }
  }

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

  // Check whether the size property has changed
  if ( !sizeFlag )
  {
    flags |= SizeFlag;
   }

  return flags;
}

void Node::ResetDefaultProperties( BufferIndex updateBufferIndex )
{
  // clear dirty flags in parent origin & anchor point
  mParentOrigin.Clear();
  mAnchorPoint.Clear();
  // Reset default properties
  mSize.ResetToBaseValue( updateBufferIndex );
  mPosition.ResetToBaseValue( updateBufferIndex );
  mOrientation.ResetToBaseValue( updateBufferIndex );
  mScale.ResetToBaseValue( updateBufferIndex );
  mVisible.ResetToBaseValue( updateBufferIndex );
  mColor.ResetToBaseValue( updateBufferIndex );

  mDirtyFlags = NothingFlag;
}

void Node::SetParent(Node& parentNode)
{
  DALI_ASSERT_ALWAYS(this != &parentNode);
  DALI_ASSERT_ALWAYS(!mIsRoot);
  DALI_ASSERT_ALWAYS(mParent == NULL);

  mParent = &parentNode;
  mDepth = mParent->GetDepth() + 1u;
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
  mDepth = 0u;

  // Remove all child pointers
  mChildren.Clear();

  // Inform child node attachment that the node has been removed from the stage
  if( mAttachment )
  {
    mAttachment->DisconnectedFromSceneGraph();
  }

}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
