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
#include <dali/internal/event/actor-attachments/mesh-attachment-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/internal/update/node-attachments/scene-graph-mesh-attachment.h>
#include <dali/internal/update/modeling/scene-graph-material.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/update/nodes/node.h>

using Dali::Internal::MeshIPtr;
using Dali::Internal::MaterialIPtr;

namespace Dali
{

namespace Internal
{

MeshAttachmentPtr MeshAttachment::New( Stage& stage, const SceneGraph::Node& parentNode )
{
  MeshAttachmentPtr attachment( new MeshAttachment( stage ) );

  // Transfer object ownership of scene-object to message
  SceneGraph::MeshAttachment* sceneObject = SceneGraph::MeshAttachment::New();
  AttachToNodeMessage( stage.GetUpdateManager(), parentNode, sceneObject );

  // Keep raw pointer for message passing
  attachment->mSceneObject = sceneObject;

  return attachment;
}

MeshAttachment::MeshAttachment( Stage& stage )
: RenderableAttachment( stage ),
  mSceneObject( NULL )
{
}

MeshAttachment::~MeshAttachment()
{
  // Belt and braces - should already have been disconnected from stage
  if ( Stage::IsInstalled() &&
       OnStage() )
  {
    DisconnectMaterial();
  }
}

void MeshAttachment::SetMesh( const MeshIPtr        meshPtr,
                              ResourceId            meshId,
                              const BoneContainer&  bones,
                              const MaterialIPtr    material )
{
  mMesh.mMesh = meshPtr;
  mMesh.mCustomMaterial = 0;

  std::size_t boneCount = bones.size();

  if ( boneCount > 0 )
  {
    // Copy bone names locally in order to perform actor binding later

    // Must keep names in same order (vertices reference into bone matrix array by index)
    for ( BoneContainer::const_iterator iter = bones.begin(); iter != bones.end(); ++iter )
    {
      const Bone& bone = (*iter);
      mMesh.mBoneNames.push_back( bone.GetName() );
    }
  }

  mMesh.mMaterial = material;
  const SceneGraph::Material* materialSceneObject = material->GetSceneObject();

  // sceneObject is being used in a separate thread; queue a message to set
  SetMeshMessage( mStage->GetUpdateInterface(), *mSceneObject, meshId, materialSceneObject, boneCount );
}

void MeshAttachment::SetMesh( const ResourceTicketPtr ticket,
                              const BoneContainer& bones,
                              const MaterialIPtr material )
{
  SetMesh( 0, ticket->GetId(), bones, material );
}

void MeshAttachment::SetMaterial( MaterialIPtr material )
{
  const SceneGraph::Material* materialSceneObject = NULL;

  if ( material )
  {
    // We have a new material.
    if ( OnStage() )
    {
      DisconnectMaterial();

      // connect the new material
      material->Connect();
    }

    mMesh.mCustomMaterial = material;

    materialSceneObject = material->GetSceneObject();
    DALI_ASSERT_DEBUG( materialSceneObject != NULL );
  }
  else
  {
    // We are unsetting the current material, and reverting to the original material
    if ( mMesh.mCustomMaterial )
    {
      if ( OnStage() )
      {
        mMesh.mCustomMaterial->Disconnect();
      }
      mMesh.mCustomMaterial = NULL;
    }

    // connect original material
    DALI_ASSERT_DEBUG( mMesh.mMaterial );

    if ( OnStage() )
    {
      mMesh.mMaterial->Connect();
    }
    materialSceneObject = mMesh.mMaterial->GetSceneObject();
  }

  if ( OnStage() )
  {
    // sceneObject is being used in a separate thread; queue a message to set
    SetMaterialMessage( mStage->GetUpdateInterface(), *mSceneObject, materialSceneObject );
  }
}

Internal::MaterialIPtr MeshAttachment::GetMaterial( ) const
{
  Internal::MaterialIPtr material;

  if ( mMesh.mCustomMaterial )
  {
    material = mMesh.mCustomMaterial;
  }
  else if ( mMesh.mMaterial )
  {
    material = mMesh.mMaterial;
  }
  return material;
}


void MeshAttachment::DisconnectMaterial()
{
  if ( mMesh.mCustomMaterial )
  {
    mMesh.mCustomMaterial->Disconnect();
  }
  else if ( mMesh.mMaterial )
  {
    mMesh.mMaterial->Disconnect();
  }
}

void MeshAttachment::BindBonesToMesh( Internal::ActorPtr rootActor )
{
  size_t boneIdx = 0;
  size_t boneCount = mMesh.mBoneNames.size();

  if ( boneCount > 0 )
  {
    for ( BoneNamesIter boneIter=mMesh.mBoneNames.begin(); boneIter != mMesh.mBoneNames.end(); ++boneIter )
    {
      ActorPtr boneActor = rootActor->FindChildByName( *boneIter );
      if ( boneActor )
      {
        ConnectBoneActor( boneActor, boneIdx, boneCount );
        boneIdx++;
      }
    }
  }
}

void MeshAttachment::ConnectBoneActor( Internal::ActorPtr boneActor,
                                       size_t             boneIdx,
                                       size_t             boneCount )
{
  Connector* connector = new Connector( boneActor, boneIdx, *this );
  mConnectors.PushBack( connector );
  connector->ConnectNode();
}

void MeshAttachment::OnStageConnection2()
{
  // Ensure current materials are staged
  const SceneGraph::MeshAttachment& sceneObject = *mSceneObject;

  const SceneGraph::Material* materialSceneObject = NULL;
  if ( mMesh.mCustomMaterial )
  {
    mMesh.mCustomMaterial->Connect();
    materialSceneObject = mMesh.mCustomMaterial->GetSceneObject();
  }
  else if ( mMesh.mMaterial )
  {
    mMesh.mMaterial->Connect();
    materialSceneObject = mMesh.mMaterial->GetSceneObject();
  }
  DALI_ASSERT_DEBUG( materialSceneObject );

  // And that the scene object has a connection to each material
  SetMaterialMessage( mStage->GetUpdateInterface(), sceneObject, materialSceneObject );

  // Ensure all staged bones are reconnected
  for(ConnectorList::Iterator iter=mConnectors.Begin(); iter != mConnectors.End(); ++iter)
  {
    Connector* connector = (*iter);
    connector->ConnectNode();
  }
}

void MeshAttachment::OnStageDisconnection2()
{
  DisconnectMaterial();
}

const SceneGraph::RenderableAttachment& MeshAttachment::GetSceneObject() const
{
  DALI_ASSERT_DEBUG( mSceneObject != NULL );
  return *mSceneObject;
}

void MeshAttachment::SetBoneNode( SceneGraph::Node* node, size_t boneIdx )
{
  size_t boneCount = mMesh.mBoneNames.size();

  SetBoneNodeMessage( mStage->GetUpdateInterface(), *mSceneObject, node, boneIdx, boneCount );
}

// Helper class for connecting Nodes to the scene-graph MeshAttachment
MeshAttachment::Connector::Connector(
  Internal::ActorPtr boneActor,
  size_t             boneIdx,
  MeshAttachment&    meshAttachment)
: mMeshAttachment(meshAttachment),
  mActor(boneActor.Get()),
  mBoneIdx(boneIdx)
{
  if( mActor )
  {
    mActor->AddObserver( *this );
    ConnectNode();
  }
}

MeshAttachment::Connector::~Connector()
{
  if( mActor)
  {
    mActor->RemoveObserver( *this );
  }
}

void MeshAttachment::Connector::SceneObjectAdded( Object& object )
{
  ConnectNode();
}

void MeshAttachment::Connector::SceneObjectRemoved( Object& object )
{
  ConnectNode();
}

void MeshAttachment::Connector::ObjectDestroyed( Object& object )
{
  mActor = NULL;

  ConnectNode();
}

void MeshAttachment::Connector::ConnectNode()
{
  SceneGraph::Node* theNode( NULL );

  if( mActor != NULL )
  {
    const SceneGraph::PropertyOwner* object = mActor->GetSceneObject();
    if ( NULL != object )
    {
      const SceneGraph::Node* aNode = dynamic_cast< const SceneGraph::Node* >( object );
      if( aNode != NULL )
      {
        theNode = const_cast< SceneGraph::Node* >( aNode );
      }
    }
  }

  mMeshAttachment.SetBoneNode( theNode, mBoneIdx );
}


} // namespace Internal

} // namespace Dali
