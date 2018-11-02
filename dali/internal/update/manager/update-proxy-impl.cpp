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
#include <dali/internal/update/manager/update-proxy-impl.h>

// INTERNAL INCLUDES
#include <dali/internal/update/manager/update-proxy-property-modifier.h>

namespace Dali
{

namespace Internal
{

namespace
{

SceneGraph::Node* FindNodeInSceneGraph( uint32_t id, SceneGraph::Node& node )
{
  SceneGraph::Node* matchingNode = NULL;

  if( node.mId == id )
  {
    matchingNode = &node;
  }
  else
  {
    for( auto&& i : node.GetChildren() )
    {
      matchingNode = FindNodeInSceneGraph( id, *i );
      if( matchingNode )
      {
        break;
      }
    }
  }

  return matchingNode;
}

} // unnamed namespace

UpdateProxy::UpdateProxy( SceneGraph::UpdateManager& updateManager, SceneGraph::TransformManager& transformManager, SceneGraph::Node& rootNode )
: mNodeContainer(),
  mLastCachedIdNodePair( { 0u, NULL } ),
  mCurrentBufferIndex( 0u ),
  mUpdateManager( updateManager ),
  mTransformManager( transformManager ),
  mRootNode( rootNode ),
  mPropertyModifier( nullptr )
{
}

UpdateProxy::~UpdateProxy()
{
}

bool UpdateProxy::GetPosition( uint32_t id, Vector3& position ) const
{
  bool success = false;
  const SceneGraph::Node* node = GetNodeWithId( id );
  if( node )
  {
    position = mTransformManager.GetVector3PropertyValue( node->mTransformId, SceneGraph::TRANSFORM_PROPERTY_POSITION );
    success = true;
  }
  return success;
}

bool UpdateProxy::SetPosition( uint32_t id, const Vector3& position )
{
  bool success = false;
  SceneGraph::Node* node = GetNodeWithId( id );
  if( node )
  {
    mTransformManager.SetVector3PropertyValue( node->mTransformId, SceneGraph::TRANSFORM_PROPERTY_POSITION, position );
    success = true;
  }
  return success;
}

bool UpdateProxy::BakePosition( uint32_t id, const Vector3& position )
{
  bool success = false;
  SceneGraph::Node* node = GetNodeWithId( id );
  if( node )
  {
    mTransformManager.BakeVector3PropertyValue( node->mTransformId, SceneGraph::TRANSFORM_PROPERTY_POSITION, position );
    success = true;
  }
  return success;
}

bool UpdateProxy::GetSize( uint32_t id, Vector3& size ) const
{
  bool success = false;
  const SceneGraph::Node* node = GetNodeWithId( id );
  if( node )
  {
    size = mTransformManager.GetVector3PropertyValue( node->mTransformId, SceneGraph::TRANSFORM_PROPERTY_SIZE );
    success = true;
  }
  return success;
}

bool UpdateProxy::SetSize( uint32_t id, const Vector3& size )
{
  bool success = false;
  SceneGraph::Node* node = GetNodeWithId( id );
  if( node )
  {
    mTransformManager.SetVector3PropertyValue( node->mTransformId, SceneGraph::TRANSFORM_PROPERTY_SIZE, size );
    success = true;
  }
  return success;
}

bool UpdateProxy::BakeSize( uint32_t id, const Vector3& size )
{
  bool success = false;
  SceneGraph::Node* node = GetNodeWithId( id );
  if( node )
  {
    mTransformManager.BakeVector3PropertyValue( node->mTransformId, SceneGraph::TRANSFORM_PROPERTY_SIZE, size );
    success = true;
  }
  return success;
}

bool UpdateProxy::GetPositionAndSize( uint32_t id, Vector3& position, Vector3& size ) const
{
  bool success = false;
  const SceneGraph::Node* node = GetNodeWithId( id );
  if( node )
  {
    position = mTransformManager.GetVector3PropertyValue( node->mTransformId, SceneGraph::TRANSFORM_PROPERTY_POSITION );
    size = mTransformManager.GetVector3PropertyValue( node->mTransformId, SceneGraph::TRANSFORM_PROPERTY_SIZE );
    success = true;
  }
  return success;
}

bool UpdateProxy::GetScale( uint32_t id, Vector3& scale ) const
{
  bool success = false;
  const SceneGraph::Node* node = GetNodeWithId( id );
  if( node )
  {
    scale = mTransformManager.GetVector3PropertyValue( node->mTransformId, SceneGraph::TRANSFORM_PROPERTY_SCALE );
    success = true;
  }

  return success;
}

bool UpdateProxy::SetScale( uint32_t id, const Vector3& scale )
{
  bool success = false;
  SceneGraph::Node* node = GetNodeWithId( id );
  if( node )
  {
    mTransformManager.SetVector3PropertyValue( node->mTransformId, SceneGraph::TRANSFORM_PROPERTY_SCALE, scale );
    success = true;
  }
  return success;
}

bool UpdateProxy::BakeScale( uint32_t id, const Vector3& scale )
{
  bool success = false;
  SceneGraph::Node* node = GetNodeWithId( id );
  if( node )
  {
    mTransformManager.BakeVector3PropertyValue( node->mTransformId, SceneGraph::TRANSFORM_PROPERTY_SCALE, scale );
    success = true;
  }
  return success;
}

bool UpdateProxy::GetColor( uint32_t id, Vector4& color ) const
{
  bool success = false;
  SceneGraph::Node* node = GetNodeWithId( id );
  if( node )
  {
    color = node->mColor.Get( mCurrentBufferIndex );
    success = true;
  }

  return success;
}

bool UpdateProxy::SetColor( uint32_t id, const Vector4& color )
{
  bool success = false;
  SceneGraph::Node* node = GetNodeWithId( id );
  if( node )
  {
    node->mColor.Set( mCurrentBufferIndex, color );
    node->SetDirtyFlag( SceneGraph::NodePropertyFlags::COLOR );
    AddResetter( *node, node->mColor );
    success = true;
  }
  return success;
}

bool UpdateProxy::BakeColor( uint32_t id, const Vector4& color )
{
  bool success = false;
  SceneGraph::Node* node = GetNodeWithId( id );
  if( node )
  {
    node->mColor.Bake( mCurrentBufferIndex, color );
    success = true;
  }
  return success;
}

void UpdateProxy::NodeHierarchyChanged()
{
  mLastCachedIdNodePair = { 0u, NULL };
  mNodeContainer.clear();
  mPropertyModifier.reset();
}

SceneGraph::Node* UpdateProxy::GetNodeWithId( uint32_t id ) const
{
  SceneGraph::Node* node = NULL;

  // Cache the last accessed node so we don't have to traverse
  if( mLastCachedIdNodePair.node && mLastCachedIdNodePair.id == id )
  {
    node = mLastCachedIdNodePair.node;
  }
  else
  {
    // Find node in vector
    for( auto&& pair : mNodeContainer )
    {
      if( pair.id == id )
      {
        node = pair.node;
        mLastCachedIdNodePair = pair;
        break;
      }
    }

    if( ! node )
    {
      // Node not in vector, find in scene-graph
      node = FindNodeInSceneGraph( id, mRootNode );
      if( node )
      {
        mNodeContainer.push_back( { id, node } );
        mLastCachedIdNodePair = *mNodeContainer.rbegin();
      }
    }
  }

  return node;
}

void UpdateProxy::AddResetter( SceneGraph::Node& node, SceneGraph::PropertyBase& propertyBase )
{
  if( ! mPropertyModifier )
  {
    mPropertyModifier = PropertyModifierPtr( new PropertyModifier( mUpdateManager ) );
  }
  mPropertyModifier->AddResetter( node, propertyBase );
}

} // namespace Internal

} // namespace Dali
