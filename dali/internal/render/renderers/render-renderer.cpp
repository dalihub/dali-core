/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/internal/render/renderers/render-renderer.h>


// INTERNAL INCLUDES
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/render/shaders/scene-graph-shader.h>
#include <dali/internal/render/shaders/program.h>
#include <dali/internal/render/data-providers/node-data-provider.h>
#include <dali/public-api/actors/blending.h>
#include <dali/internal/common/image-sampler.h>
#include <dali/internal/render/renderers/render-new-renderer.h>

namespace Dali
{

namespace Internal
{

namespace
{

static Matrix gModelViewProjectionMatrix( false ); ///< a shared matrix to calculate the MVP matrix, dont want to store it in object to reduce storage overhead
static Matrix3 gNormalMatrix; ///< a shared matrix to calculate normal matrix, dont want to store it in object to reduce storage overhead

/**
 * Helper to set view and projection matrices once per program
 * @param program to set the matrices to
 * @param modelMatrix to set
 * @param viewMatrix to set
 * @param projectionMatrix to set
 * @param modelViewMatrix to set
 * @param modelViewProjectionMatrix to set
 */
inline void SetMatrices( Program& program,
                         const Matrix& modelMatrix,
                         const Matrix& viewMatrix,
                         const Matrix& projectionMatrix,
                         const Matrix& modelViewMatrix,
                         const Matrix& modelViewProjectionMatrix )
{
  GLint loc = program.GetUniformLocation(Program::UNIFORM_MODEL_MATRIX);
  if( Program::UNIFORM_UNKNOWN != loc )
  {
    program.SetUniformMatrix4fv( loc, 1, modelMatrix.AsFloat() );
  }
  loc = program.GetUniformLocation( Program::UNIFORM_VIEW_MATRIX );
  if( Program::UNIFORM_UNKNOWN != loc )
  {
    if( program.GetViewMatrix() != &viewMatrix )
    {
      program.SetViewMatrix( &viewMatrix );
      program.SetUniformMatrix4fv( loc, 1, viewMatrix.AsFloat() );
    }
  }
  // set projection matrix if program has not yet received it this frame or if it is dirty
  loc = program.GetUniformLocation( Program::UNIFORM_PROJECTION_MATRIX );
  if( Program::UNIFORM_UNKNOWN != loc )
  {
    if( program.GetProjectionMatrix() != &projectionMatrix )
    {
      program.SetProjectionMatrix( &projectionMatrix );
      program.SetUniformMatrix4fv( loc, 1, projectionMatrix.AsFloat() );
    }
  }
  loc = program.GetUniformLocation(Program::UNIFORM_MODELVIEW_MATRIX);
  if( Program::UNIFORM_UNKNOWN != loc )
  {
    program.SetUniformMatrix4fv( loc, 1, modelViewMatrix.AsFloat() );
  }

  loc = program.GetUniformLocation( Program::UNIFORM_MVP_MATRIX );
  if( Program::UNIFORM_UNKNOWN != loc )
  {
    program.SetUniformMatrix4fv( loc, 1, modelViewProjectionMatrix.AsFloat() );
  }

  loc = program.GetUniformLocation( Program::UNIFORM_NORMAL_MATRIX );
  if( Program::UNIFORM_UNKNOWN != loc )
  {
    gNormalMatrix = modelViewMatrix;
    gNormalMatrix.Invert();
    gNormalMatrix.Transpose();
    program.SetUniformMatrix3fv( loc, 1, gNormalMatrix.AsFloat() );
  }
}

}

namespace Render
{

void Renderer::Initialize( Context& context, SceneGraph::TextureCache& textureCache )
{
  mContext = &context;
  mTextureCache = &textureCache;
}

Renderer::~Renderer()
{
}

void Renderer::SetShader( SceneGraph::Shader* shader )
{
  mShader = shader;
}

void Renderer::SetCullFace( CullFaceMode mode )
{
  DALI_ASSERT_DEBUG(mode >= CullNone && mode <= CullFrontAndBack);
  mCullFaceMode = mode;
}

void Renderer::SetSampler( unsigned int samplerBitfield )
{
  mSamplerBitfield = samplerBitfield;
}

void Renderer::Render( Context& context,
                       SceneGraph::TextureCache& textureCache,
                       BufferIndex bufferIndex,
                       const SceneGraph::NodeDataProvider& node,
                       SceneGraph::Shader& defaultShader,
                       const Matrix& modelViewMatrix,
                       const Matrix& viewMatrix,
                       const Matrix& projectionMatrix,
                       float frametime,
                       bool cull,
                       bool blend)
{
  // @todo MESH_REWORK Fix when merging! :D

  NewRenderer* renderer = dynamic_cast<NewRenderer*>(this);

  if( renderer )
  {
    // Get the shader from the material:
    mShader = &renderer->mRenderDataProvider->GetShader();
  }

  // if mShader is NULL it means we're set to default
  if( !mShader )
  {
    mShader = &defaultShader;
  }

  if( !CheckResources() )
  {
    // CheckResources() is overriden in derived classes.
    // Prevents modify the GL state if resources are not ready and nothing is to be rendered.
    return;
  }

  // Calculate the MVP matrix first so we can do the culling test
  const Matrix& modelMatrix = node.GetModelMatrix( bufferIndex );
  Matrix::Multiply( gModelViewProjectionMatrix, modelViewMatrix, projectionMatrix );

  // Get the program to use:
  Program* program = mShader->GetProgram();
  if( !program )
  {
    // if program is NULL it means this is a custom shader with non matching geometry type so we need to use default shaders program
    program = defaultShader.GetProgram();
    DALI_ASSERT_DEBUG( program && "Default shader should always have a program available." );
    if( !program )
    {
      DALI_LOG_ERROR( "Failed to get program for shader at address %p.", (void*) &*mShader );
      return;
    }

  }

  // Check culling (does not need the program to be in use)
  if( cull && ! program->ModifiesGeometry() )
  {
    if( IsOutsideClipSpace( context, gModelViewProjectionMatrix ) )
    {
      // don't do any further gl state changes as this renderer is not visible
      return;
    }
  }

  // Take the program into use so we can send uniforms to it
  program->Use();

  DoSetCullFaceMode( context, bufferIndex );

  DoSetBlending( context, bufferIndex, blend );

  // Ignore missing uniforms - custom shaders and flat color shaders don't have SAMPLER
  // set projection and view matrix if program has not yet received them yet this frame
  SetMatrices( *program, modelMatrix, viewMatrix, projectionMatrix, modelViewMatrix, gModelViewProjectionMatrix );

  // set color uniform
  GLint loc = program->GetUniformLocation( Program::UNIFORM_COLOR );
  if( Program::UNIFORM_UNKNOWN != loc )
  {
    const Vector4& color = node.GetRenderColor( bufferIndex );
    program->SetUniform4f( loc, color.r, color.g, color.b, color.a );
  }
  loc = program->GetUniformLocation(Program::UNIFORM_TIME_DELTA);
  if( Program::UNIFORM_UNKNOWN != loc )
  {
    program->SetUniform1f( loc, frametime );
  }

  //@todo MESH_REWORK Remove after removing ImageRenderer
  DoSetUniforms(context, bufferIndex, mShader, program );

  // subclass rendering and actual draw call
  DoRender( context, textureCache, node, bufferIndex, *program, modelViewMatrix, viewMatrix );
}

void Renderer::SetSortAttributes( SceneGraph::RendererWithSortAttributes& sortAttributes ) const
{
  sortAttributes.shader = mShader;
  sortAttributes.material = NULL;
  sortAttributes.geometry = NULL;
}

// can be overridden by deriving class
void Renderer::DoSetUniforms(Context& context, BufferIndex bufferIndex, SceneGraph::Shader* shader, Program* program )
{
  shader->SetUniforms( context, *program, bufferIndex );
}

// can be overridden by deriving class
void Renderer::DoSetCullFaceMode(Context& context, BufferIndex bufferIndex )
{
  // Set face culling mode
  context.CullFace( mCullFaceMode );
}

Renderer::Renderer()
: mContext(NULL),
  mTextureCache( NULL ),
  mShader( NULL ),
  mSamplerBitfield( ImageSampler::PackBitfield( FilterMode::DEFAULT, FilterMode::DEFAULT ) ),
  mCullFaceMode( CullNone )
{
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
