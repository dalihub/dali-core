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
 *
 */

// CLASS HEADER
#include <dali/internal/update/nodes/node.h>

// INTERNAL INCLUDES
#include <dali/internal/common/internal-constants.h>
#include <dali/internal/common/memory-pool-object-allocator.h>
#include <dali/internal/update/common/discard-queue.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/constants.h>

namespace //Unnamed namespace
{
//Memory pool used to allocate new nodes. Memory used by this pool will be released when process dies
// or DALI library is unloaded
Dali::Internal::MemoryPoolObjectAllocator<Dali::Internal::SceneGraph::Node> gNodeMemoryPool;
#ifdef DEBUG_ENABLED
// keep track of nodes created / deleted, to ensure we have 0 when the process exits or DALi library is unloaded
int32_t gNodeCount =0;

// Called when the process is about to exit, Node count should be zero at this point.
void __attribute__ ((destructor)) ShutDown(void)
{
DALI_ASSERT_DEBUG( (gNodeCount == 0) && "Node memory leak");
}
#endif
}

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

const PositionInheritanceMode Node::DEFAULT_POSITION_INHERITANCE_MODE( INHERIT_PARENT_POSITION );
const ColorMode Node::DEFAULT_COLOR_MODE( USE_OWN_MULTIPLY_PARENT_ALPHA );

Node* Node::New( uint32_t id )
{
  return new ( gNodeMemoryPool.AllocateRawThreadSafe() ) Node( id );
}

void Node::Delete( Node* node )
{
  // check we have a node not a layer
  if( !node->mIsLayer )
  {
    // Manually call the destructor
    node->~Node();

    // Mark the memory it used as free in the memory pool
    gNodeMemoryPool.FreeThreadSafe( node );
  }
  else
  {
    // not in the pool, just delete it.
    delete node;
  }
}

Node::Node( uint32_t id )
: mTransformManager( NULL ),
  mTransformId( INVALID_TRANSFORM_ID ),
  mParentOrigin( TRANSFORM_PROPERTY_PARENT_ORIGIN ),
  mAnchorPoint( TRANSFORM_PROPERTY_ANCHOR_POINT ),
  mSize( TRANSFORM_PROPERTY_SIZE ),                                               // Zero initialized by default
  mPosition( TRANSFORM_PROPERTY_POSITION ),                                       // Zero initialized by default
  mOrientation(),                                                                 // Initialized to identity by default
  mScale( TRANSFORM_PROPERTY_SCALE ),
  mVisible( true ),
  mCulled( false ),
  mColor( Color::WHITE ),
  mWorldPosition( TRANSFORM_PROPERTY_WORLD_POSITION, Vector3( 0.0f,0.0f,0.0f ) ), // Zero initialized by default
  mWorldScale( TRANSFORM_PROPERTY_WORLD_SCALE, Vector3( 1.0f,1.0f,1.0f ) ),
  mWorldOrientation(),                                                            // Initialized to identity by default
  mWorldMatrix(),
  mWorldColor( Color::WHITE ),
  mClippingSortModifier( 0u ),
  mId( id ),
  mParent( NULL ),
  mExclusiveRenderTask( NULL ),
  mChildren(),
  mClippingDepth( 0u ),
  mScissorDepth( 0u ),
  mDepthIndex( 0u ),
  mDirtyFlags( NodePropertyFlags::ALL ),
  mRegenerateUniformMap( 0 ),
  mDrawMode( DrawMode::NORMAL ),
  mColorMode( DEFAULT_COLOR_MODE ),
  mClippingMode( ClippingMode::DISABLED ),
  mIsRoot( false ),
  mIsLayer( false ),
  mPositionUsesAnchorPoint( true )
{
#ifdef DEBUG_ENABLED
  gNodeCount++;
#endif
}

Node::~Node()
{
  if( mTransformId != INVALID_TRANSFORM_ID )
  {
    mTransformManager->RemoveTransform(mTransformId);
  }

#ifdef DEBUG_ENABLED
  gNodeCount--;
#endif
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

  //Set whether the position should use the anchor point
  transformManager->SetPositionUsesAnchorPoint( mTransformId, mPositionUsesAnchorPoint );
}

void Node::SetRoot(bool isRoot)
{
  DALI_ASSERT_DEBUG(!isRoot || mParent == NULL); // Root nodes cannot have a parent

  mIsRoot = isRoot;
}

void Node::AddUniformMapping( OwnerPointer< UniformPropertyMapping >& map )
{
  PropertyOwner::AddUniformMapping( map );
  mRegenerateUniformMap = 2;
}

void Node::RemoveUniformMapping( const std::string& uniformName )
{
  PropertyOwner::RemoveUniformMapping( uniformName );
  mRegenerateUniformMap = 2;
}

bool Node::GetUniformMapChanged( BufferIndex bufferIndex ) const
{
  return mRegenerateUniformMap > 0;
}

void Node::PrepareRender( BufferIndex bufferIndex )
{
  // Each renderer will query back to find out if the node's map has been updated
  for(auto renderer : mRenderer )
  {
    renderer->UpdateUniformMap( bufferIndex, *this );
  }

  if( mRegenerateUniformMap > 0 )
  {
    mRegenerateUniformMap--;
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
  // If it is the first renderer added, make sure the world transform will be calculated
  // in the next update as world transform is not computed if node has no renderers.
  if( mRenderer.Empty() )
  {
    mDirtyFlags |= NodePropertyFlags::TRANSFORM;
  }
  else
  {
    // Check that it has not been already added.
    for( auto&& existingRenderer : mRenderer )
    {
      if( existingRenderer == renderer )
      {
        // Renderer is already in the list.
        return;
      }
    }
  }

  mRenderer.PushBack( renderer );
}

void Node::RemoveRenderer( Renderer* renderer )
{
  RendererContainer::SizeType rendererCount( mRenderer.Size() );
  for( RendererContainer::SizeType i = 0; i < rendererCount; ++i )
  {
    if( mRenderer[i] == renderer )
    {
      mRenderer.Erase( mRenderer.Begin()+i);
      return;
    }
  }
}

NodePropertyFlags Node::GetDirtyFlags() const
{
  // get initial dirty flags, they are reset ResetDefaultProperties, but setters may have made the node dirty already
  NodePropertyFlags flags = mDirtyFlags;

  // Check whether the visible property has changed
  if ( !mVisible.IsClean() )
  {
    flags |= NodePropertyFlags::VISIBLE;
  }

  // Check whether the color property has changed
  if ( !mColor.IsClean() )
  {
    flags |= NodePropertyFlags::COLOR;
  }

  return flags;
}

NodePropertyFlags Node::GetInheritedDirtyFlags( NodePropertyFlags parentFlags ) const
{
  // Size is not inherited. VisibleFlag is inherited
  static const NodePropertyFlags InheritedDirtyFlags = NodePropertyFlags::TRANSFORM | NodePropertyFlags::VISIBLE | NodePropertyFlags::COLOR;
  using UnderlyingType = typename std::underlying_type<NodePropertyFlags>::type;

  return static_cast<NodePropertyFlags>( static_cast<UnderlyingType>( mDirtyFlags ) |
                                         ( static_cast<UnderlyingType>( parentFlags ) & static_cast<UnderlyingType>( InheritedDirtyFlags ) ) );
}

void Node::ResetDirtyFlags( BufferIndex updateBufferIndex )
{
  mDirtyFlags = NodePropertyFlags::NOTHING;
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
