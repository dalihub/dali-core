//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// CLASS HEADER
#include <dali/internal/update/node-attachments/scene-graph-mesh-attachment.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/integration-api/debug.h>

#include <dali/internal/common/internal-constants.h>

#include <dali/internal/event/images/image-impl.h>
#include <dali/internal/event/modeling/mesh-impl.h>
#include <dali/internal/event/modeling/material-impl.h>

#include <dali/internal/update/common/discard-queue.h>
#include <dali/internal/update/resources/resource-manager.h>
#include <dali/internal/update/controllers/render-message-dispatcher.h>
#include <dali/internal/update/controllers/scene-controller.h>
#include <dali/internal/update/modeling/scene-graph-mesh.h>
#include <dali/internal/update/modeling/scene-graph-material.h>
#include <dali/internal/update/nodes/node.h>

#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/render/renderers/scene-graph-mesh-renderer.h>
#include <dali/internal/render/shaders/program.h>
#include <dali/internal/render/shaders/shader.h>


using namespace std;

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

class Material;

MeshAttachment* MeshAttachment::New()
{
  return new MeshAttachment();
}

MeshAttachment::MeshAttachment()
: RenderableAttachment( true ), // scale enabled
  mRenderer( NULL ),
  mAffectedByLighting(true),
  mLastLightingState(true),
  mLightInitialized(false)
{
}

void MeshAttachment::PropertyOwnerDestroyed( PropertyOwner& owner )
{
  // Bone node has been destroyed.
}

void MeshAttachment::ConnectToSceneGraph2( BufferIndex updateBufferIndex )
{
  DALI_ASSERT_DEBUG( NULL != mSceneController );

  // Create main renderer, passing ownership to the render-thread
  mRenderer = MeshRenderer::New( *mParent, mSceneController->GetLightController() );
  mSceneController->GetRenderMessageDispatcher().AddRenderer( *mRenderer );
}

void MeshAttachment::OnDestroy2()
{
  DALI_ASSERT_DEBUG( NULL != mSceneController );

  // Request deletion in the next Render
  mSceneController->GetRenderMessageDispatcher().RemoveRenderer( *mRenderer );
  mRenderer = NULL;
}

MeshAttachment::~MeshAttachment()
{
  BoneNodeContainer& boneNodes = mMesh.boneNodes;
  for( BoneNodeContainer::iterator iter = boneNodes.begin(); iter != boneNodes.end(); ++iter )
  {
    Node* node = (*iter);
    if( node != NULL )
    {
      node->RemoveObserver( *this );
    }
  }

}

Renderer& MeshAttachment::GetRenderer()
{
  return *mRenderer;
}

const Renderer& MeshAttachment::GetRenderer() const
{
  return *mRenderer;
}

void MeshAttachment::ShaderChanged( BufferIndex updateBufferIndex )
{
  DALI_ASSERT_DEBUG(mSceneController);
  Shader* shader = GetParent().GetInheritedShader();

  {
    typedef MessageValue1< Renderer, Shader* > DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

    // Construct message in the mRenderer queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( mRenderer, &Renderer::SetShader, shader );
  }

  {
    typedef Message< MeshRenderer > DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

    // Construct message in the mRenderer queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( mRenderer, &MeshRenderer::ResetCustomUniforms );
  }
}

void MeshAttachment::SizeChanged( BufferIndex updateBufferIndex )
{
  SetRecalculateScaleForSize();
}

void MeshAttachment::SetMesh( ResourceId meshId, const SceneGraph::Material* material, std::size_t boneCount )
{
  mMesh.meshResourceId = meshId;
  mMesh.material = material;
  mMesh.boneCount = boneCount;

  if( mMesh.boneCount > 0 )
  {
    mMesh.boneNodes.resize(mMesh.boneCount, NULL);
  }
}

void MeshAttachment::SetMaterial( const SceneGraph::Material* material )
{
  mMesh.material = material;
}

void MeshAttachment::SetAffectedByLighting( bool affectedByLighting )
{
  mAffectedByLighting = affectedByLighting;
}

void MeshAttachment::SetBoneNode( Node* node, size_t boneIdx, size_t boneCount )
{
  DALI_ASSERT_DEBUG(boneIdx < mMesh.boneNodes.size());

  // Observe bone nodes (this ensures that their world matrix will be calculated)
  if(mMesh.boneNodes[boneIdx] != NULL)
  {
    mMesh.boneNodes[boneIdx]->RemoveObserver( *this );
  }

  mMesh.boneNodes[boneIdx] = node;
  if( node != NULL )
  {
    mMesh.boneNodes[boneIdx]->AddObserver( *this );
  }
}

void MeshAttachment::CalculateBoneTransforms( BufferIndex               updateBufferIndex,
                                              const Mesh*               mesh,
                                              const BoneNodeContainer&  boneNodes,
                                              BoneTransforms&           boneTransforms )
{
  size_t boneCount = std::min( mesh->GetMeshData(Mesh::UPDATE_THREAD).GetBoneCount(), MAX_NUMBER_OF_BONES_PER_MESH );

  const BoneContainer& bones = mesh->GetMeshData(Mesh::UPDATE_THREAD).GetBones();

  for(size_t boneIdx=0; boneIdx < boneCount; ++boneIdx)
  {
    const Matrix&  boneMatrix = bones.at(boneIdx).GetOffsetMatrix();
    const Node*    boneNode   = boneNodes[boneIdx];

    if ( boneNode != NULL )
    {
      Matrix& transform = boneTransforms.transforms[boneIdx];
      Matrix::Multiply(transform, boneMatrix, boneNode->GetWorldMatrix( updateBufferIndex ) );
      // These transform matrices are modified once per update per renderer.
      // The correct view matrix will be applied in the render thread to the viewTransforms
      // list once per renderer per render task
    }
  }
}

/*
 * Go through each mesh resource and material resource. If any of the resources aren't yet ready,
 * stop and return false.
 */
bool MeshAttachment::DoPrepareResources( BufferIndex updateBufferIndex, ResourceManager& resourceManager )
{
  bool ready = false;
  mFinishedResourceAcquisition = false;

  if ( !mMesh.mesh )
  {
    SceneGraph::Mesh* mesh(resourceManager.GetMesh(mMesh.meshResourceId));
    mMesh.mesh = mesh;
  }

  if ( mMesh.mesh && mMesh.mesh->HasGeometry( Mesh::UPDATE_THREAD ) )
  {
    const SceneGraph::Material* material = mMesh.material;
    ready = material->AreResourcesReady();
    mFinishedResourceAcquisition = ready;
  }

  return ready;
}

void MeshAttachment::DoPrepareRender( BufferIndex updateBufferIndex )
{
  DALI_ASSERT_DEBUG(mSceneController);
  RenderQueue& renderQueue = mSceneController->GetRenderQueue();

  if( !mLightInitialized || ( mAffectedByLighting != mLastLightingState ) )
  {
    mLastLightingState = mAffectedByLighting;
    mLightInitialized = true;

    typedef MessageValue1< MeshRenderer, bool > DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = renderQueue.ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( mRenderer, &MeshRenderer::SetAffectedByLighting, mAffectedByLighting );
  }

  // Provide renderer with ModelIT matrix, mesh, material, and bone transforms for the next frame
  MeshRenderer::MeshInfo& meshInfo = mRenderer->GetMeshInfo( updateBufferIndex );

  Mesh*           meshPtr  = mMesh.mesh;
  RenderMaterial* material = mMesh.material->GetRenderMaterial();

  meshInfo.mesh     = meshPtr;
  meshInfo.material = material;

  if ( material )
  {
    std::size_t boneCount = mMesh.boneNodes.size();

    if ( boneCount > 0 )
    {
      if( meshInfo.boneTransforms.transforms.size() != boneCount )
      {
        // Reallocate bone transforms if the number of bones has changed.
        // ( Prefer allocation in Update thread rather than in render thread )
        meshInfo.boneTransforms.transforms.resize( boneCount );
        meshInfo.boneTransforms.viewTransforms.resize( boneCount );
        meshInfo.boneTransforms.inverseTransforms.resize( boneCount );
      }

      CalculateBoneTransforms( updateBufferIndex, meshPtr, mMesh.boneNodes, meshInfo.boneTransforms );
    }
  }
}

bool MeshAttachment::IsFullyOpaque( BufferIndex updateBufferIndex )
{
  bool fullyOpaque = true; // good default, since transparency is evil.

  /**
   * Fully opaque when:
   *   1) The material is opaque
   *   2) The inherited color is not transparent nor semi-transparent
   *   3) The inherited shader does not blend
   */

  const SceneGraph::Material* material = mMesh.material;

  if ( material && !material->IsOpaque() )
  {
    fullyOpaque = false;
  }

  if( mParent != NULL )
  {
    if( fullyOpaque )
    {
      fullyOpaque = ( mParent->GetWorldColor(updateBufferIndex).a >= FULLY_OPAQUE );
    }

    if ( fullyOpaque )
    {
      Shader* shader = mParent->GetInheritedShader();
      if( shader != NULL )
      {
        fullyOpaque = (shader->GetGeometryHints() != Dali::ShaderEffect::HINT_BLENDING );
      }
    }
  }
  return fullyOpaque;
}

void MeshAttachment::DoGetScaleForSize( const Vector3& nodeSize, Vector3& scaling )
{
  scaling = Vector3::ONE;
  // Check whether mesh data should be scaled to requiredSize
  if( NULL != mMesh.mesh )
  {
    const MeshData& meshData = mMesh.mesh->GetMeshData( Mesh::UPDATE_THREAD );

    if( meshData.IsScalingRequired() )
    {
      scaling = nodeSize;
    }
  }
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
