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

namespace Dali
{

namespace Internal
{

namespace
{

SceneGraph::Node* FindNodeInSceneGraph( unsigned int id, SceneGraph::Node& node )
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

UpdateProxy::UpdateProxy( SceneGraph::TransformManager& transformManager, SceneGraph::Node& rootNode )
: mNodeContainer(),
  mLastCachedIdNodePair( { 0u, NULL } ),
  mCurrentBufferIndex( 0u ),
  mTransformManager( transformManager ),
  mRootNode( rootNode )
{
}

UpdateProxy::~UpdateProxy()
{
}

Vector3 UpdateProxy::GetPosition( unsigned int id ) const
{
  Vector3 position;
  const SceneGraph::Node* node = GetNodeWithId( id );
  if( node )
  {
    position = mTransformManager.GetVector3PropertyValue( node->mTransformId, SceneGraph::TRANSFORM_PROPERTY_POSITION );
  }

  return position;
}

void UpdateProxy::SetPosition( unsigned int id, const Vector3& position )
{
  SceneGraph::Node* node = GetNodeWithId( id );
  if( node )
  {
    mTransformManager.SetVector3PropertyValue( node->mTransformId, SceneGraph::TRANSFORM_PROPERTY_POSITION, position );
  }
}

void UpdateProxy::BakePosition( unsigned int id, const Vector3& position )
{
  SceneGraph::Node* node = GetNodeWithId( id );
  if( node )
  {
    mTransformManager.BakeVector3PropertyValue( node->mTransformId, SceneGraph::TRANSFORM_PROPERTY_POSITION, position );
  }
}

const Vector3& UpdateProxy::GetSize( unsigned int id ) const
{
  const SceneGraph::Node* node = GetNodeWithId( id );
  if( node )
  {
    return mTransformManager.GetVector3PropertyValue( node->mTransformId, SceneGraph::TRANSFORM_PROPERTY_SIZE );
  }

  return Vector3::ZERO;
}

void UpdateProxy::SetSize( unsigned int id, const Vector3& size )
{
  SceneGraph::Node* node = GetNodeWithId( id );
  if( node )
  {
    mTransformManager.SetVector3PropertyValue( node->mTransformId, SceneGraph::TRANSFORM_PROPERTY_SIZE, size );
  }
}

void UpdateProxy::BakeSize( unsigned int id, const Vector3& size )
{
  SceneGraph::Node* node = GetNodeWithId( id );
  if( node )
  {
    mTransformManager.BakeVector3PropertyValue( node->mTransformId, SceneGraph::TRANSFORM_PROPERTY_SIZE, size );
  }
}

void UpdateProxy::GetPositionAndSize( unsigned int id, Vector3& position, Vector3& size ) const
{
  const SceneGraph::Node* node = GetNodeWithId( id );
  if( node )
  {
    position = mTransformManager.GetVector3PropertyValue( node->mTransformId, SceneGraph::TRANSFORM_PROPERTY_POSITION );
    size = mTransformManager.GetVector3PropertyValue( node->mTransformId, SceneGraph::TRANSFORM_PROPERTY_SIZE );
  }
}

Vector3 UpdateProxy::GetScale( unsigned int id ) const
{
  Vector3 scale;
  const SceneGraph::Node* node = GetNodeWithId( id );
  if( node )
  {
    scale = mTransformManager.GetVector3PropertyValue( node->mTransformId, SceneGraph::TRANSFORM_PROPERTY_SCALE );
  }

  return scale;
}

void UpdateProxy::SetScale( unsigned int id, const Vector3& scale )
{
  SceneGraph::Node* node = GetNodeWithId( id );
  if( node )
  {
    mTransformManager.SetVector3PropertyValue( node->mTransformId, SceneGraph::TRANSFORM_PROPERTY_SCALE, scale );
  }
}

void UpdateProxy::BakeScale( unsigned int id, const Vector3& scale )
{
  SceneGraph::Node* node = GetNodeWithId( id );
  if( node )
  {
    mTransformManager.BakeVector3PropertyValue( node->mTransformId, SceneGraph::TRANSFORM_PROPERTY_SCALE, scale );
  }
}

Vector4 UpdateProxy::GetColor( unsigned int id ) const
{
  SceneGraph::Node* node = GetNodeWithId( id );
  if( node )
  {
    return node->mColor.Get( mCurrentBufferIndex );
  }

  return Vector4::ZERO;
}

void UpdateProxy::SetColor( unsigned int id, const Vector4& color ) const
{
  SceneGraph::Node* node = GetNodeWithId( id );
  if( node )
  {
    node->mColor.Set( mCurrentBufferIndex, color );
  }
}

void UpdateProxy::BakeColor( unsigned int id, const Vector4& color ) const
{
  SceneGraph::Node* node = GetNodeWithId( id );
  if( node )
  {
    node->mColor.Bake( mCurrentBufferIndex, color );
  }
}

SceneGraph::Node* UpdateProxy::GetNodeWithId( unsigned int id ) const
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

} // namespace Internal

} // namespace Dali
