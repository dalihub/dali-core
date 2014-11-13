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
#include <dali/internal/render/renderers/scene-graph-mesh-renderer.h>

// INTERNAL INCLUDES
#include <dali/public-api/math/matrix3.h>
#include <dali/public-api/math/matrix.h>
#include <dali/internal/common/internal-constants.h>
#include <dali/internal/update/modeling/scene-graph-mesh.h>
#include <dali/internal/update/modeling/scene-graph-material.h>
#include <dali/internal/update/node-attachments/scene-graph-light-attachment.h>
#include <dali/internal/update/controllers/light-controller.h>
#include <dali/internal/update/controllers/scene-controller.h>
#include <dali/internal/render/shaders/program.h>
#include <dali/internal/render/shaders/shader.h>
#include <dali/internal/render/renderers/render-material.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

MeshRenderer* MeshRenderer::New( RenderDataProvider& dataprovider, LightController& lightController )
{
  MeshRenderer* meshRenderer = new MeshRenderer( dataprovider );
  meshRenderer->mLightController = &lightController;

  return meshRenderer;
}

MeshRenderer::MeshRenderer( RenderDataProvider& dataprovider )
: Renderer( dataprovider ),
  mLightController(NULL),
  mAffectedByLighting(true),
  mGeometryType(GEOMETRY_TYPE_TEXTURED_MESH),
  mShaderType(SHADER_DEFAULT)
{
}

void MeshRenderer::ResetCustomUniforms()
{
  for( unsigned int subType = SHADER_DEFAULT; subType < SHADER_SUBTYPE_LAST; ++subType )
  {
    for( unsigned int i = 0; i < mNumberOfCustomUniforms; ++i )
    {
      mCustomUniform[ subType ][ i ].Reset();
    }
  }

  mRenderMaterialUniforms.ResetCustomUniforms();
}

void MeshRenderer::SetAffectedByLighting( bool affectedByLighting )
{
  mAffectedByLighting = affectedByLighting;
}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::GlContextDestroyed()
{
  MeshInfo& meshInfo0 = mMeshInfo[ 0 ];
  Mesh* mesh = meshInfo0.mesh;
  if( mesh )
  {
    mesh->GlContextDestroyed();
  }
  MeshInfo& meshInfo1 = mMeshInfo[ 1 ];
  mesh = meshInfo1.mesh;
  if( mesh )
  {
    mesh->GlContextDestroyed();
  }
}

void MeshRenderer::GlCleanup()
{
  // MeshRenderer does not own any GL resources
}

bool MeshRenderer::RequiresDepthTest() const
{
  return true;
}

bool MeshRenderer::CheckResources()
{
  // Early out if the scene is unlit.
  // There must be at least one light.
  const unsigned int numberOfLights = mLightController->GetNumberOfLights();
  if( 0 == numberOfLights && mAffectedByLighting )
  {
    return false;
  }

  return true;
}

void MeshRenderer::ResolveGeometryTypes( BufferIndex bufferIndex, GeometryType& outType, ShaderSubTypes& outSubType )
{
  MeshInfo& meshInfo = mMeshInfo[bufferIndex];
  Mesh*           mesh     =   meshInfo.mesh;
  RenderMaterial& material = *(meshInfo.material);

  outType = GEOMETRY_TYPE_TEXTURED_MESH;
  if( ! material.HasTexture() )
  {
    outType = GEOMETRY_TYPE_UNTEXTURED_MESH;
  }
  outSubType = SHADER_DEFAULT;

  if( mShader->AreSubtypesRequired( outType ) )
  {
    GLsizei numBoneMatrices = (GLsizei)mesh->GetMeshData(Mesh::RENDER_THREAD).GetBoneCount();

    if( numBoneMatrices )
    {
      if( mesh->GetMeshData(Mesh::RENDER_THREAD).HasColor() )
      {
        outSubType = SHADER_RIGGED_AND_VERTEX_COLOR;
      }
      else if( mAffectedByLighting )
      {
        outSubType = SHADER_RIGGED_AND_LIT;
      }
      else
      {
        outSubType = SHADER_RIGGED_AND_EVENLY_LIT;
      }
    }
    else
    {
      if( mesh->GetMeshData(Mesh::RENDER_THREAD).HasColor() )
      {
        outSubType = SHADER_VERTEX_COLOR;
      }
      else if( ! mAffectedByLighting )
      {
        outSubType = SHADER_EVENLY_LIT;
      } // else default
    }
  }
  if( outType != mGeometryType || outSubType != mShaderType )
  {
    mGeometryType = outType;
    mShaderType = outSubType;
    ResetCustomUniforms();
  }
}

bool MeshRenderer::IsOutsideClipSpace( const Matrix& modelMatrix, const Matrix& modelViewProjectionMatrix )
{
  return false; // @todo add implementation
}

void MeshRenderer::DoRender( BufferIndex bufferIndex, Program& program, const Matrix& modelViewMatrix, const Matrix& viewMatrix )
{
  MeshInfo& meshInfo = mMeshInfo[bufferIndex];
  Mesh*           mesh              =    meshInfo.mesh;
  RenderMaterial& material          =  *(meshInfo.material);
  BoneTransforms& boneTransforms    =    meshInfo.boneTransforms;

  if( ! meshInfo.boneTransforms.transforms.empty() )
  {
    ApplyViewToBoneTransforms( meshInfo.boneTransforms, viewMatrix );
  }

  const int stride = sizeof(Dali::MeshData::Vertex);
  Dali::MeshData::Vertex *v = 0;

  mesh->UploadVertexData( *mContext, bufferIndex );
  mesh->BindBuffers( *mContext );

  GLsizei numBoneMatrices = (GLsizei)mesh->GetMeshData(Mesh::RENDER_THREAD).GetBoneCount();

  GLint location       = Program::UNIFORM_UNKNOWN;
  GLint positionLoc    = program.GetAttribLocation(Program::ATTRIB_POSITION);
  GLint texCoordLoc    = Program::ATTRIB_UNKNOWN;
  GLint boneWeightsLoc = Program::ATTRIB_UNKNOWN;
  GLint boneIndicesLoc = Program::ATTRIB_UNKNOWN;
  GLint normalLoc      = Program::ATTRIB_UNKNOWN;
  GLint colorLoc       = Program::ATTRIB_UNKNOWN;

  mContext->VertexAttribPointer( positionLoc, 3, GL_FLOAT, GL_FALSE, stride, &v->x );
  mContext->EnableVertexAttributeArray( positionLoc );

  if( numBoneMatrices > 0 )
  {
    location = mCustomUniform[ mShaderType ][ 0 ].GetUniformLocation( program, "uBoneCount" );
    if( Program::UNIFORM_UNKNOWN != location )
    {
      program.SetUniform1i(location, numBoneMatrices);
    }

    location = mCustomUniform[ mShaderType ][ 1 ].GetUniformLocation( program, "uBoneMatrices" );
    if( Program::UNIFORM_UNKNOWN != location )
    {
      program.SetUniformMatrix4fv(location, numBoneMatrices, boneTransforms.viewTransforms[0].AsFloat());
    }
    if( mesh->GetMeshData(Mesh::RENDER_THREAD).HasNormals() )
    {
      location = mCustomUniform[ mShaderType ][ 2 ].GetUniformLocation( program, "uBoneMatricesIT" );
      if( Program::UNIFORM_UNKNOWN != location )
      {
        program.SetUniformMatrix3fv( location, numBoneMatrices, boneTransforms.inverseTransforms[0].AsFloat() );
      }
    }

    boneWeightsLoc = program.GetAttribLocation( Program::ATTRIB_BONE_WEIGHTS );
    if( Program::ATTRIB_UNKNOWN != boneWeightsLoc )
    {
      mContext->VertexAttribPointer( boneWeightsLoc, 4, GL_FLOAT, GL_FALSE, stride, &v->boneWeights[0] );
      mContext->EnableVertexAttributeArray( boneWeightsLoc );
    }

    boneIndicesLoc = program.GetAttribLocation( Program::ATTRIB_BONE_INDICES );
    if( Program::ATTRIB_UNKNOWN != boneIndicesLoc )
    {
      mContext->VertexAttribPointer( boneIndicesLoc, 4, GL_UNSIGNED_BYTE, GL_FALSE, stride, &v->boneIndices[0] );
      mContext->EnableVertexAttributeArray( boneIndicesLoc );
    }
  }

  if( material.HasTexture() )
  {
    material.BindTextures( program, mSamplerBitfield );
  }
  // Always use UV's - may be being used for another purpose by a custom shader!
  texCoordLoc = program.GetAttribLocation(Program::ATTRIB_TEXCOORD);
  if( Program::ATTRIB_UNKNOWN != texCoordLoc )
  {
    mContext->VertexAttribPointer( texCoordLoc, 2, GL_FLOAT, GL_FALSE, stride, &v->u );
    mContext->EnableVertexAttributeArray( texCoordLoc );
  }

  if( mesh->GetMeshData(Mesh::RENDER_THREAD).HasNormals() )
  {
    normalLoc = program.GetAttribLocation(Program::ATTRIB_NORMAL);
    if( Program::ATTRIB_UNKNOWN != normalLoc )
    {
      mContext->VertexAttribPointer( normalLoc, 3, GL_FLOAT, GL_FALSE, stride, &v->nX );
      mContext->EnableVertexAttributeArray( normalLoc );
    }
  }
  else if( mesh->GetMeshData(Mesh::RENDER_THREAD).HasColor() )  // Normals & color are mutually exclusive
  {
    colorLoc = program.GetAttribLocation(Program::ATTRIB_COLOR);
    if( Program::ATTRIB_UNKNOWN != colorLoc)
    {
      mContext->VertexAttribPointer( colorLoc, 3, GL_FLOAT, GL_FALSE, stride, &v->vertexR );
      mContext->EnableVertexAttributeArray( colorLoc );
    }
  }

  material.SetUniforms( mRenderMaterialUniforms, program, mShaderType );

  if( mAffectedByLighting )
  {
    // Set light parameters
    location = mCustomUniform[ mShaderType ][ 3 ].GetUniformLocation( program, "uNumberOfLights" );
    if( Program::UNIFORM_UNKNOWN != location )
    {
      program.SetUniform1i( location, mLightController->GetNumberOfLights() );
    }

    // Model View IT matrix required for vertex normal lighting calculation
    location = mCustomUniform[ mShaderType ][ 4 ].GetUniformLocation( program, "uModelViewIT" );
    if( Program::UNIFORM_UNKNOWN != location )
    {
      Matrix3 modelViewInverseTranspose = modelViewMatrix;
      modelViewInverseTranspose.Invert();
      modelViewInverseTranspose.Transpose();

      program.SetUniformMatrix3fv( location, 1, modelViewInverseTranspose.AsFloat() );
    }

    // only one active light supported at the moment (due to performance)
    //if( numberOfLights > 0 )
    {
      Vector2 tempVector2;
      Vector3 tempVector3;

      Node& lightNode = mLightController->GetLight( 0 );

      LightAttachment& light = dynamic_cast< LightAttachment& >( lightNode.GetAttachment() );

      location = mCustomUniform[ mShaderType ][ 5 ].GetUniformLocation( program, "uLight0.mType" );
      if( Program::UNIFORM_UNKNOWN != location )
      {
        program.SetUniform1i( location, (GLint)light.GetType() );
      }

      location = mCustomUniform[ mShaderType ][ 6 ].GetUniformLocation( program, "uLight0.mFallOff" );
      if( Program::UNIFORM_UNKNOWN != location )
      {
        tempVector2 = light.GetFallOff();
        program.SetUniform2f( location, tempVector2.x, tempVector2.y );
      }

      location = mCustomUniform[ mShaderType ][ 7 ].GetUniformLocation( program, "uLight0.mSpotAngle" );
      if( Program::UNIFORM_UNKNOWN != location )
      {
        tempVector2 = light.GetSpotAngle();
        program.SetUniform2f( location, tempVector2.x, tempVector2.y );
      }

      location = mCustomUniform[ mShaderType ][ 8 ].GetUniformLocation( program, "uLight0.mLightPos" );
      if( Program::UNIFORM_UNKNOWN != location )
      {
        // light position in eyespace
        Vector3 tempVector3( viewMatrix * Vector4(lightNode.GetWorldPosition(bufferIndex)) );
        program.SetUniform3f( location, tempVector3.x, tempVector3.y, tempVector3.z );
      }

      location = mCustomUniform[ mShaderType ][ 9 ].GetUniformLocation( program, "uLight0.mLightDir" );
      if( Program::UNIFORM_UNKNOWN != location )
      {
        tempVector3 = light.GetDirection();
        tempVector3.Normalize();
        program.SetUniform3f( location, tempVector3.x, tempVector3.y, tempVector3.z );
      }

      location = mCustomUniform[ mShaderType ][ 10 ].GetUniformLocation( program, "uLight0.mAmbient" );
      if( Program::UNIFORM_UNKNOWN != location )
      {
        tempVector3 = light.GetAmbientColor();
        program.SetUniform3f( location, tempVector3.r, tempVector3.g, tempVector3.b );
      }

      location = mCustomUniform[ mShaderType ][ 11 ].GetUniformLocation( program, "uLight0.mDiffuse" );
      if( Program::UNIFORM_UNKNOWN != location )
      {
        tempVector3 = light.GetDiffuseColor();
        program.SetUniform3f( location, tempVector3.r, tempVector3.g, tempVector3.b );
      }

      location = mCustomUniform[ mShaderType ][ 12 ].GetUniformLocation( program, "uLight0.mSpecular" );
      if( Program::UNIFORM_UNKNOWN != location )
      {
        tempVector3 = light.GetSpecularColor();
        program.SetUniform3f( location, tempVector3.r, tempVector3.g, tempVector3.b );
      }
    }
  }

  Dali::MeshData::VertexGeometryType vertexGeometry = mesh->GetMeshData(Mesh::RENDER_THREAD).GetVertexGeometryType();
  switch( vertexGeometry )
  {
    case Dali::MeshData::TRIANGLES:
    {
      mContext->DrawElements(GL_TRIANGLES, mesh->GetFaceIndexCount(Mesh::RENDER_THREAD), GL_UNSIGNED_SHORT, 0);
      DRAW_ELEMENT_RECORD(mesh->GetFaceIndexCount());
      break;
    }
    case Dali::MeshData::LINES:
    {
      mContext->DrawElements(GL_LINES, mesh->GetFaceIndexCount(Mesh::RENDER_THREAD), GL_UNSIGNED_SHORT, 0);
      DRAW_ELEMENT_RECORD(mesh->GetFaceIndexCount());
      break;
    }
    case Dali::MeshData::POINTS:
    {
      mContext->DrawArrays(GL_POINTS, 0, mesh->GetFaceIndexCount(Mesh::RENDER_THREAD) );
      DRAW_ARRAY_RECORD(mesh->GetFaceIndexCount());
      break;
    }
  }

  if( normalLoc != Program::ATTRIB_UNKNOWN )
  {
    mContext->DisableVertexAttributeArray( normalLoc );
  }

  if( colorLoc != Program::ATTRIB_UNKNOWN )
  {
    mContext->DisableVertexAttributeArray( colorLoc );
  }

  mContext->DisableVertexAttributeArray( positionLoc );

  if( texCoordLoc != Program::ATTRIB_UNKNOWN )
  {
    mContext->DisableVertexAttributeArray( texCoordLoc );
  }

  if( boneWeightsLoc != Program::ATTRIB_UNKNOWN )
  {
    mContext->DisableVertexAttributeArray( boneWeightsLoc );
  }

  if( boneIndicesLoc != Program::ATTRIB_UNKNOWN )
  {
    mContext->DisableVertexAttributeArray( boneIndicesLoc );
  }

  return;
}

void MeshRenderer::ApplyViewToBoneTransforms( BoneTransforms& boneTransforms,
                                              const Matrix&   viewMatrix )
{
  size_t boneCount = boneTransforms.transforms.size();

  for( size_t boneIdx=0; boneIdx < boneCount; ++boneIdx )
  {
    Matrix& transform = boneTransforms.transforms[boneIdx];
    Matrix& viewTransform = boneTransforms.viewTransforms[boneIdx];
    Matrix::Multiply(viewTransform, transform, viewMatrix);

    boneTransforms.inverseTransforms[boneIdx] = viewTransform;
    boneTransforms.inverseTransforms[boneIdx].ScaledInverseTranspose();
  }
}


} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
