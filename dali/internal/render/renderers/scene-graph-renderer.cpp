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
#include <dali/internal/render/renderers/scene-graph-renderer.h>
#include <dali/internal/render/renderers/scene-graph-renderer-declarations.h>

// INTERNAL INCLUDES
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/render/shaders/shader.h>
#include <dali/internal/render/renderers/render-data-provider.h>
#include <dali/public-api/actors/blending.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

void Renderer::Initialize( Context& context, TextureCache& textureCache )
{
  mContext = &context;
  mTextureCache = &textureCache;
}

Renderer::~Renderer()
{
}

void Renderer::SetShader( Shader* shader )
{
  mShader = shader;
}

void Renderer::SetUseBlend( bool useBlend )
{
  mUseBlend = useBlend;
}

void Renderer::SetBlendingOptions( unsigned int options )
{
  mBlendingOptions.SetBitmask( options );
}

void Renderer::SetBlendColor( const Vector4& color )
{
  mBlendingOptions.SetBlendColor( color );
}

void Renderer::SetCullFace( CullFaceMode mode )
{
  DALI_ASSERT_DEBUG(mode >= CullNone && mode <= CullFrontAndBack);
  mCullFaceMode = mode;
}

void Renderer::Render( BufferIndex bufferIndex,
                       const Matrix& modelViewMatrix,
                       const Matrix& viewMatrix,
                       const Matrix& projectionMatrix,
                       float frametime )
{
  DALI_ASSERT_DEBUG( mContext && "Renderer::Render. Renderer not initialised!! (mContext == NULL)." );
  DALI_ASSERT_DEBUG( mShader && "Renderer::Render. Shader not set!!" );

  if( !CheckResources() )
  {
    // CheckResources() is overriden in derived classes.
    // Prevents modify the GL state if resources are not ready and nothing is to be rendered.
    return;
  }

  // Enables/disables blending mode.
  mContext->SetBlend( mUseBlend );

  // Set face culling mode
  mContext->CullFace( mCullFaceMode );

  // Set the blend color
  const Vector4* const customColor = mBlendingOptions.GetBlendColor();
  if( customColor )
  {
    mContext->SetCustomBlendColor( *customColor );
  }
  else
  {
    mContext->SetDefaultBlendColor();
  }

  // Set blend source & destination factors
  mContext->BlendFuncSeparate( mBlendingOptions.GetBlendSrcFactorRgb(),
                               mBlendingOptions.GetBlendDestFactorRgb(),
                               mBlendingOptions.GetBlendSrcFactorAlpha(),
                               mBlendingOptions.GetBlendDestFactorAlpha() );

  // Set blend equations
  mContext->BlendEquationSeparate( mBlendingOptions.GetBlendEquationRgb(),
                                   mBlendingOptions.GetBlendEquationAlpha() );

  mShader->SetFrameTime( frametime );

  const Matrix& modelMatrix = mDataProvider.GetModelMatrix( bufferIndex );
  const Vector4& color = mDataProvider.GetRenderColor( bufferIndex );

  // Call to over ridden method in the child class
  // TODO, once MeshRenderer is fixed to render only one mesh, move mShader.Apply here
  // and we can greatly reduce these parameters. Also then derived renderers can be passed the Program&
  DoRender( bufferIndex, modelViewMatrix, modelMatrix, viewMatrix, projectionMatrix, color );
}

Renderer::Renderer( RenderDataProvider& dataprovider )
: mDataProvider( dataprovider ),
  mContext( NULL ),
  mTextureCache( NULL ),
  mShader( NULL ),
  mUseBlend( false ),
  mCullFaceMode( CullNone )
{
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
