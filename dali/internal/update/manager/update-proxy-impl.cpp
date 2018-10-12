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
  const Dali::Matrix& matrix = GetWorldMatrix( id );
  return matrix.GetTranslation3();
}

void UpdateProxy::SetPosition( unsigned int id, const Vector3& position )
{
  const SceneGraph::Node* node = GetNodeWithId( id );
  if( node )
  {
    Matrix& matrix = mTransformManager.GetWorldMatrix( node->mTransformId );
    matrix.SetTranslation( position );
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

void UpdateProxy::GetPositionAndSize( unsigned int id, Vector3& position, Vector3& size ) const
{
  Matrix worldMatrix( false );
  GetWorldMatrixAndSize( id, worldMatrix, size );
  position = worldMatrix.GetTranslation3();
}

Vector4 UpdateProxy::GetWorldColor( unsigned int id ) const
{
  SceneGraph::Node* node = GetNodeWithId( id );
  if( node )
  {
    return node->mWorldColor.Get( mCurrentBufferIndex );
  }

  return Vector4::ZERO;
}

void UpdateProxy::SetWorldColor( unsigned int id, const Vector4& color ) const
{
  SceneGraph::Node* node = GetNodeWithId( id );
  if( node )
  {
    Vector4& currentColor = node->mWorldColor.Get( mCurrentBufferIndex );
    currentColor = color;
  }
}

void UpdateProxy::GetWorldMatrixAndSize( unsigned int id, Matrix& worldMatrix, Vector3& size ) const
{
  const SceneGraph::Node* node = GetNodeWithId( id );
  if( node )
  {
    mTransformManager.GetWorldMatrixAndSize( node->mTransformId, worldMatrix, size );
  }
}

const Matrix& UpdateProxy::GetWorldMatrix( unsigned int id ) const
{
  const SceneGraph::Node* node = GetNodeWithId( id );
  if( node )
  {
    return mTransformManager.GetWorldMatrix( node->mTransformId );
  }

  return Matrix::IDENTITY;
}

void UpdateProxy::SetWorldMatrix( unsigned int id, const Matrix& worldMatrix )
{
  SceneGraph::Node* node = GetNodeWithId( id );
  if( node )
  {
    Matrix& currentMatrix = mTransformManager.GetWorldMatrix( node->mTransformId );
    currentMatrix = worldMatrix;
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
