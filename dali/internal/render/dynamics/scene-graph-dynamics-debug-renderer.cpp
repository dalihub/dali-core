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
#include "scene-graph-dynamics-debug-renderer.h"

// INTERNAL HEADERS
#include <dali/integration-api/debug.h>
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/render/shaders/program.h>
#include <dali/internal/render/shaders/shader.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

DynamicsDebugRenderer::DynamicsDebugRenderer(const Shader& debugShader)
: mBufferName(0),
  mShader(const_cast<Shader&>(debugShader)),
  mContext(NULL),
  mBufferIndex(0),
  mViewMatrix(Matrix::IDENTITY),
  mProjectionMatrix(Matrix::IDENTITY),
  mNumberOfPoints(0)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);
}

DynamicsDebugRenderer::~DynamicsDebugRenderer()
{
}

void DynamicsDebugRenderer::Initialize(Context& context)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);

  mContext = &context;
}

void DynamicsDebugRenderer::UpdateMatrices( BufferIndex bufferIndex, const Matrix& projectionMatrix, const Matrix& viewMatrix )
{
  mProjectionMatrix = projectionMatrix;
  mViewMatrix = viewMatrix;
  mBufferIndex = bufferIndex;
}

void DynamicsDebugRenderer::UpdateBuffer( const Integration::DynamicsDebugVertexContainer& vertices )
{
  if( NULL != mContext )
  {
    if( !mBuffer )
    {
      mBuffer = new GpuBuffer(*mContext, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
    }

    if( vertices.size() > 1 )
    {
      mBuffer->UpdateDataBuffer( vertices.size() * sizeof(vertices[0]), vertices.data() );
    }
    mNumberOfPoints = vertices.size();
  }
}

void DynamicsDebugRenderer::Render()
{
  if( mBuffer && mNumberOfPoints )
  {
    Integration::DynamicsDebugVertex* vertex = 0;

    Program& program = mShader.Apply(*mContext, mBufferIndex, GEOMETRY_TYPE_IMAGE, Matrix::IDENTITY, Matrix::IDENTITY, mViewMatrix, mProjectionMatrix, Color::WHITE, SHADER_DEFAULT);
    const GLint positionLoc = program.GetAttribLocation(Program::ATTRIB_POSITION);
    const GLint colorLoc = program.GetAttribLocation(Program::ATTRIB_COLOR);
    mBuffer->Bind();
    mContext->VertexAttribPointer(positionLoc, 3, GL_FLOAT, false, sizeof(Integration::DynamicsDebugVertex), (void*)&vertex->position);
    mContext->VertexAttribPointer(colorLoc, 4, GL_FLOAT, false, sizeof(Integration::DynamicsDebugVertex), (void*)&vertex->color);
    mContext->EnableVertexAttributeArray(positionLoc);
    mContext->EnableVertexAttributeArray(colorLoc);
    mContext->DrawArrays( GL_LINES, 0, mNumberOfPoints );
    mContext->DisableVertexAttributeArray(positionLoc);
    mContext->DisableVertexAttributeArray(colorLoc);
    mContext->BindArrayBuffer( 0 );
  }
}


} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
