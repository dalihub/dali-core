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
#include <dali/internal/update/modeling/scene-graph-mesh.h>

// INTERNAL INCLUDES
#include <dali/internal/render/common/post-process-resource-dispatcher.h>
#include <dali/internal/render/gl-resources/context.h>

using namespace std;

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

Mesh::Mesh( ResourceId id,
            PostProcessResourceDispatcher& postProcessResourceDispatcher,
            MeshData* meshData )
: mResourceId ( id ),
  mUpdateMeshData(meshData),
  mRenderMeshData(meshData),
  mRefreshVertexBuffer(true),
  mVertexBuffer(NULL),
  mIndicesBuffer(NULL),
  mNumberOfVertices(0u),
  mNumberOfFaces(0u),
  mPostProcessResourceDispatcher(postProcessResourceDispatcher)
{
}

Mesh::~Mesh()
{
}

void Mesh::SetMeshData(MeshData* meshData)
{
  mUpdateMeshData = meshData;
}

MeshData& Mesh::GetMeshData( Mesh::ThreadBuffer threadBuffer )
{
  return const_cast<MeshData&>(static_cast<const Mesh*>(this)->GetMeshData(threadBuffer));
}

const MeshData& Mesh::GetMeshData( Mesh::ThreadBuffer threadBuffer ) const
{
  MeshData* meshDataPtr = NULL;

  switch(threadBuffer)
  {
    case Mesh::UPDATE_THREAD:
    {
      meshDataPtr = mUpdateMeshData;
    }
    break;

    case Mesh::RENDER_THREAD:
    {
      meshDataPtr = &(*mRenderMeshData);
    }
    break;
  }

  DALI_ASSERT_DEBUG( meshDataPtr );
  return *meshDataPtr;
}

void Mesh::MeshDataUpdated( BufferIndex bufferIndex, Mesh::ThreadBuffer threadBuffer, MeshData* meshData )
{
  if ( threadBuffer == Mesh::RENDER_THREAD )
  {
    // Called from a message, the old MeshData will be release and the new one is saved.
    mRenderMeshData = meshData;
  }
  else
  {
    // Dynamics and animatable meshes don't create new mesh data
    DALI_ASSERT_DEBUG( threadBuffer == Mesh::UPDATE_THREAD );
    DALI_ASSERT_DEBUG( meshData == NULL );
  }

  mRefreshVertexBuffer = true;
}

void Mesh::UploadVertexData( Context& context, BufferIndex renderBufferIndex )
{
  // Short-circuit if nothing has changed
  if ( !mRefreshVertexBuffer )
  {
    return;
  }

  const MeshData::VertexContainer& vertices = mRenderMeshData->GetVertices();

  DALI_ASSERT_DEBUG( !vertices.empty() );
  if ( !mVertexBuffer )
  {
    mVertexBuffer = new GpuBuffer(context,GpuBuffer::ARRAY_BUFFER,GpuBuffer::STATIC_DRAW);
  }
  DALI_ASSERT_DEBUG(mVertexBuffer);

  mVertexBuffer->UpdateDataBuffer( vertices.size() * sizeof(MeshData::Vertex), &vertices.at(0) );
  mNumberOfVertices = mRenderMeshData->GetVertexCount();

  if ( size_t numberOfIndices = mRenderMeshData->GetFaceIndexCount() )
  {
    const MeshData::FaceIndices& faces = mRenderMeshData->GetFaces();
    DALI_ASSERT_DEBUG(!faces.empty());

    if ( !mIndicesBuffer )
    {
      mIndicesBuffer = new GpuBuffer(context,GpuBuffer::ELEMENT_ARRAY_BUFFER,GpuBuffer::STATIC_DRAW);
    }

    mIndicesBuffer->UpdateDataBuffer( numberOfIndices * sizeof(GLushort), &(faces.at(0)) );
    mNumberOfFaces = mRenderMeshData->GetFaceCount();
  }

  // Note, dispatcher should only be used in Render Thread (as should the rest of this method!)
  ResourcePostProcessRequest ppRequest( mResourceId, ResourcePostProcessRequest::UPLOADED );
  mPostProcessResourceDispatcher.DispatchPostProcessRequest(ppRequest);

  mRenderMeshData->Discard();
  mRefreshVertexBuffer = false;
}

void Mesh::BindBuffers(Context& context)
{
  // Short-circuit if nothing has changed
  if ( !mVertexBuffer )
  {
    return;
  }

  DALI_ASSERT_DEBUG( mIndicesBuffer || mRenderMeshData->GetVertexGeometryType() == Dali::MeshData::POINTS );

  mVertexBuffer->Bind();

  if( mIndicesBuffer)
  {
    mIndicesBuffer->Bind();
  }
}

void Mesh::GlCleanup()
{
  mVertexBuffer = NULL;
  mIndicesBuffer = NULL;
}

size_t Mesh::GetFaceIndexCount( ThreadBuffer threadBuffer ) const
{
  DALI_ASSERT_DEBUG( threadBuffer == Mesh::RENDER_THREAD );
  size_t faceCount= 0;
  switch( GetMeshData(threadBuffer).GetVertexGeometryType() )
  {
    case Dali::MeshData::POINTS:
      faceCount = mNumberOfVertices;
      break;
    case Dali::MeshData::LINES:
      faceCount = mNumberOfFaces*2;
      break;
    case Dali::MeshData::TRIANGLES:
      faceCount = mNumberOfFaces*3;
      break;
  }

  return faceCount;
}

bool Mesh::HasGeometry( ThreadBuffer threadBuffer ) const
{
  return GetMeshData(threadBuffer).GetVertexCount() > 0;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
