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
#include <dali/internal/render/renderers/scene-graph-text-renderer.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/actors/text-actor.h>
#include <dali/internal/common/text-parameters.h>
#include <dali/internal/common/text-vertex-2d.h>
#include <dali/internal/event/text/font-impl.h>
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/render/common/performance-monitor.h>
#include <dali/internal/render/shaders/program.h>
#include <dali/internal/render/common/vertex.h>
#include <dali/internal/render/shaders/shader.h>
#include <dali/internal/render/gl-resources/texture-cache.h>
#include <dali/internal/update/controllers/scene-controller.h>
#include <dali/internal/render/gl-resources/texture.h>

using namespace std;

#if defined(DEBUG_ENABLED)
namespace
{
Debug::Filter* gTextFilter = Debug::Filter::New(Debug::Concise, false, "LOG_SCENE_GRAPH_TEXT_RENDERER");
}
#endif

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

TextRenderer* TextRenderer::New( RenderDataProvider& dataprovider )
{
  return new TextRenderer( dataprovider );
}

TextRenderer::~TextRenderer()
{
  if(mTextureId > 0)
  {
    mTextureCache->RemoveObserver(mTextureId, this);
  }

  GlCleanup();

  delete mTextColor;
}

void TextRenderer::TextureDiscarded( ResourceId textureId )
{
  DALI_ASSERT_DEBUG( mTextureId == textureId || mTextureId == 0 );

  mTextureId = 0;
  mTexture = NULL;
}

void TextRenderer::AllocateTextParameters()
{
  if( !mTextParameters )
  {
    mTextParameters = new TextParameters;
  }
}

void TextRenderer::SetTextureId( ResourceId textureId )
{
  DALI_LOG_INFO( gTextFilter, Debug::General, "TextRenderer::SetTextureId(%d)\n", textureId );

  if(mTextureId > 0)
  {
    mTextureCache->RemoveObserver(mTextureId, this);
  }

  mTextureId = textureId;
  mTexture = NULL;

  if(textureId > 0)
  {
    mTextureCache->AddObserver(textureId, this);
  }
}

void TextRenderer::UpdateIndexBuffer( std::size_t size )
{
  if( size == 0)
  {
    return;
  }
  DALI_ASSERT_DEBUG( (size % 4 == 0) && "Invalid vertex length");

  // @todo need to create a simple gpu-buffer-manager class which allow us
  // to use a single indice buffer for all text-renderers and image renderers (minus 9 patch).

  // number of indices(points) = number of (vertices / 4 ) = number of quads * 6
  // to display the quad as two triangles (each triangle has 3 points).
  // equivalent to 1.5 * number verts, which can be done with bit shifting.
  std::size_t numberIndices =  size + (size >> 1);

  std::size_t numberQuads =  size >> 2;  // quads = verts / 4

  GLushort* indices = new GLushort[ numberIndices  ];

  std::size_t n = 0;
  for( std::size_t i = 0; i < numberQuads; ++i )
  {
    /*
     * 1 --- 2
     * |    /|
     * |  A  |
     * | /   |
     * 0 --- 3
     *
     * Draw 2 triangles with clock wise winding: 0->1->2 and 0->2->3
     */
    std::size_t vertIndex = i << 2;  // vert index = i * 4

    indices[ n++ ] = 0 + vertIndex;
    indices[ n++ ] = 1 + vertIndex;
    indices[ n++ ] = 2 + vertIndex;

    indices[ n++ ] = 0 + vertIndex;
    indices[ n++ ] = 2 + vertIndex;
    indices[ n++ ] = 3 + vertIndex;
  }

  mIndexBuffer->UpdateDataBuffer( numberIndices * sizeof(GLushort),indices );

  delete []indices;
}

void TextRenderer::SetVertexData( TextVertexBuffer* vertexData )
{
  DALI_LOG_INFO( gTextFilter, Debug::General, "TextRenderer::SetVertexData(this: %p, vertexData: %p)\n", this, vertexData );

  if( !vertexData )
  {
    DALI_ASSERT_DEBUG( 0 && "No vertex data");  // vertex data structure is required even for empty strings
    return;
  }
  if( vertexData->mVertices.size() > 0 )
  {
    SetTextureId(vertexData->mTextureId);

    if ( !mVertexBuffer )
    {
      mVertexBuffer = new GpuBuffer( *mContext, GpuBuffer::ARRAY_BUFFER, GpuBuffer::DYNAMIC_DRAW );
    }

    if ( !mIndexBuffer )
    {
      mIndexBuffer = new GpuBuffer( *mContext, GpuBuffer::ELEMENT_ARRAY_BUFFER, GpuBuffer::STATIC_DRAW );
    }

    mVertexBuffer->UpdateDataBuffer( vertexData->mVertices.size() * sizeof(TextVertex2D), &vertexData->mVertices[0] );

    UpdateIndexBuffer( vertexData->mVertices.size() ); // Used in DoRender()

    // Get inverted text size, as this is faster for the shader to operate on,
    // and shader won't throw any errors performing a multiplication rather than a divide by zero
    // on a bad size value.
    mInvTextSize = vertexData->mVertexMax;
    mInvTextSize.x = mInvTextSize.x > Math::MACHINE_EPSILON_1 ? 1.0f / mInvTextSize.x : 1.0f;
    mInvTextSize.y = mInvTextSize.y > Math::MACHINE_EPSILON_1 ? 1.0f / mInvTextSize.y : 1.0f;
  }
  else
  {
    // no text to display, delete the GPU buffers, this will stop anything rendering.
    mVertexBuffer.Reset();
    mIndexBuffer.Reset();
  }
  // vertex data no longer required.
  delete vertexData;
}

void TextRenderer::SetFontSize( float pixelSize )
{
  mPixelSize = pixelSize;
}

void TextRenderer::SetGradient( const Vector4& color, const Vector2& startPoint, const Vector2& endPoint )
{
  AllocateTextParameters();
  mTextParameters->SetGradient( color, startPoint, endPoint );
}

void TextRenderer::SetTextColor( const Vector4& color )
{
  if( NULL == mTextColor )
  {
    mTextColor = new Vector4( color );
  }
  else
  {
    *mTextColor = color;
  }
}

void TextRenderer::SetOutline( const bool enable, const Vector4& color, const Vector2& params )
{
  AllocateTextParameters();

  mTextParameters->SetOutline( enable, color, params );
}

void TextRenderer::SetGlow( const bool enable, const Vector4& color, const float params )
{
  AllocateTextParameters();

  mTextParameters->SetGlow( enable, color, params );
}

void TextRenderer::SetDropShadow( const bool enable, const Vector4& color, const Vector2& offset, const float size )
{
  AllocateTextParameters();

  mTextParameters->SetShadow( enable, color, offset, size );
}

void TextRenderer::SetSmoothEdge( float params )
{
  mSmoothing = params;
}

void TextRenderer::GlContextDestroyed()
{
  if( mVertexBuffer )
  {
    mVertexBuffer->GlContextDestroyed();
  }
  if( mIndexBuffer )
  {
    mIndexBuffer->GlContextDestroyed();
  }
}

void TextRenderer::GlCleanup()
{
  mVertexBuffer.Reset();
  mIndexBuffer.Reset();
}

bool TextRenderer::RequiresDepthTest() const
{
  return false;
}

bool TextRenderer::CheckResources()
{
  if ( ! ( mVertexBuffer && mIndexBuffer ) )
  {
    // This character has no geometry, must be a white space
    return true;
  }

  if( !mVertexBuffer->BufferIsValid() )
  {
    return false;
  }

  if( mTexture == NULL )
  {
    mTexture = mTextureCache->GetTexture( mTextureId );

    if( mTexture == NULL )
    {
      // texture atlas hasn't been created yet
      return false;
    }
  }
  if( mTexture->GetTextureId() == 0 )
  {
    return false;
  }

  return true;
}

void TextRenderer::DoRender( BufferIndex bufferIndex, const Matrix& modelViewMatrix, const Matrix& modelMatrix, const Matrix& viewMatrix, const Matrix& projectionMatrix, const Vector4& color )
{
  if ( ! ( mVertexBuffer && mIndexBuffer ) )
  {
    // This character has no geometry, must be a white space
    return;
  }

  DALI_ASSERT_DEBUG( NULL != mTexture && "TextRenderer::DoRender. mTexture == NULL." );

  DALI_LOG_INFO( gTextFilter, Debug::General, "TextRenderer::DoRender(this: %p) textureId:%d\n", this, mTextureId );

  // If we have a color gradient, then we cannot use the default shader.
  ShaderSubTypes shaderType( SHADER_DEFAULT );
  if( mTextParameters )
  {
    if( mTextParameters->IsOutlineEnabled() )
    {
      if( mTextParameters->IsGlowEnabled() )
      {
        shaderType = SHADER_GRADIENT_OUTLINE_GLOW;
      }
      else
      {
        shaderType = SHADER_GRADIENT_OUTLINE;
      }
    }
    else if( mTextParameters->IsGlowEnabled() )
    {
      shaderType = SHADER_GRADIENT_GLOW;
    }
    else if( mTextParameters->IsDropShadowEnabled() )
    {
      shaderType = SHADER_GRADIENT_SHADOW;
    }
    else
    {
      shaderType = SHADER_GRADIENT;
    }
  }

  // Apply shader effect specific program and common uniforms
  Program& program = mShader->Apply( *mContext, bufferIndex, GEOMETRY_TYPE_TEXT, modelMatrix, viewMatrix, modelViewMatrix, projectionMatrix, color, shaderType );

  // Set sampler uniform
  GLint samplerLoc = program.GetUniformLocation( Program::UNIFORM_SAMPLER );
  if( Program::UNIFORM_UNKNOWN != samplerLoc )
  {
    // set the uniform
    program.SetUniform1i( samplerLoc, 0 );
  }

  const float SMOOTHING_ADJUSTMENT( 12.0f );
  const float SMOOTHING_ADJUSTMENT_PIXEL_SIZE( 32.0f );

  float smoothWidth = SMOOTHING_ADJUSTMENT / mPixelSize;
  float smoothing = mSmoothing;

  const int smoothingLoc = program.GetUniformLocation( Program::UNIFORM_SMOOTHING );
  if( Program::UNIFORM_UNKNOWN != smoothingLoc )
  {
    smoothWidth = min( min(mSmoothing, 1.0f - mSmoothing), smoothWidth );

    if( mPixelSize < SMOOTHING_ADJUSTMENT_PIXEL_SIZE )
    {
      smoothing *= Lerp( mPixelSize / SMOOTHING_ADJUSTMENT_PIXEL_SIZE, 0.5f, 1.0f );
    }

    program.SetUniform2f( smoothingLoc, std::max(0.0f, smoothing - smoothWidth), std::min(1.0f, smoothing + smoothWidth) );
  }

  if( mTextParameters )
  {
    if( mTextParameters->IsOutlineEnabled() )
    {
      const int outlineLoc = program.GetUniformLocation( Program::UNIFORM_OUTLINE );
      const int outlineColorLoc = program.GetUniformLocation( Program::UNIFORM_OUTLINE_COLOR );

      if( Program::UNIFORM_UNKNOWN != outlineLoc && Program::UNIFORM_UNKNOWN != outlineColorLoc )
      {
        const Vector2& outline = mTextParameters->GetOutlineThickness();
        const Vector4& outlineColor = mTextParameters->GetOutlineColor();
        float outlineWidth = outline[1] + smoothWidth;
        float outlineStart = outline[0];
        float outlineEnd = min( 1.0f, outlineStart + outlineWidth );

        program.SetUniform2f(outlineLoc, outlineStart, outlineEnd);
        program.SetUniform4f(outlineColorLoc, outlineColor.r, outlineColor.g, outlineColor.b, outlineColor.a);
      }
    }

    if( mTextParameters->IsGlowEnabled() )
    {
      const int glowLoc = program.GetUniformLocation( Program::UNIFORM_GLOW );
      const int glowColorLoc = program.GetUniformLocation( Program::UNIFORM_GLOW_COLOR );

      if( Program::UNIFORM_UNKNOWN != glowLoc && Program::UNIFORM_UNKNOWN != glowColorLoc )
      {
        // if mGlow is > mSmoothing we get an inverted glyph, so clamp the value
        program.SetUniform1f(glowLoc, std::min(mTextParameters->GetGlowIntensity(), mSmoothing));
        const Vector4& glowColor = mTextParameters->GetGlowColor();
        program.SetUniform4f(glowColorLoc, glowColor.r, glowColor.g, glowColor.b, glowColor.a);
      }
    }

    if( mTextParameters->IsDropShadowEnabled() )
    {
      const int shadowLoc = program.GetUniformLocation( Program::UNIFORM_SHADOW );
      const int shadowColorLoc = program.GetUniformLocation( Program::UNIFORM_SHADOW_COLOR );
      const int shadowSmoothingLoc = program.GetUniformLocation( Program::UNIFORM_SHADOW_SMOOTHING );

      if( Program::UNIFORM_UNKNOWN != shadowLoc && Program::UNIFORM_UNKNOWN != shadowColorLoc && Program::UNIFORM_UNKNOWN != shadowSmoothingLoc )
      {
        // convert shadow offset from tile to atlas coordinates
        const Vector2& offset( mTextParameters->GetDropShadowOffset() / mTexture->GetWidth());
        float shadowSmoothing = std::max(0.0f, smoothing - mTextParameters->GetDropShadowSize() );
        program.SetUniform2f(shadowLoc, offset.x, offset.y);
        const Vector4& dropShadowColor = mTextParameters->GetDropShadowColor();
        program.SetUniform4f(shadowColorLoc, dropShadowColor.r, dropShadowColor.g, dropShadowColor.b, dropShadowColor.a);
        program.SetUniform2f( shadowSmoothingLoc, std::max(0.0f, shadowSmoothing - smoothWidth), std::min(1.0f, shadowSmoothing + smoothWidth) );
      }
    }
  }

  // Set the text color uniform
  const int textColorLoc = program.GetUniformLocation( Program::UNIFORM_TEXT_COLOR );
  if( Program::UNIFORM_UNKNOWN != textColorLoc )
  {
    Vector4 textColor( (NULL != mTextColor) ? *mTextColor : TextStyle::DEFAULT_TEXT_COLOR );

    program.SetUniform4f(textColorLoc, textColor.r, textColor.g, textColor.b, textColor.a);
  }

  // All shaders except default shader require the uGradientLine.zw uniform to be set
  // at the very least. (setting it to vec2(0.0, 0.0) will disable gradient)
  if( shaderType != SHADER_DEFAULT )
  {
    Vector2 projection( Vector2::ZERO );
    Vector2 startPoint( TextStyle::DEFAULT_GRADIENT_START_POINT );

    if( mTextParameters )
    {
      startPoint = mTextParameters->GetGradientStartPoint();
      projection = mTextParameters->GetGradientEndPoint() - startPoint;
      if( mTextParameters->IsGradientEnabled() ) // same as: mGradientEndPoint != mGradientStartPoint
      {
        projection /= projection.LengthSquared();

        // For valid gradients Gradient Color and Text Size information must be set.
        const int gradientColorLoc = program.GetUniformLocation( Program::UNIFORM_GRADIENT_COLOR );
        const int textSizeLoc = program.GetUniformLocation( Program::UNIFORM_INVERSE_TEXT_SIZE );

        if( Program::UNIFORM_UNKNOWN != gradientColorLoc && Program::UNIFORM_UNKNOWN != textSizeLoc )
        {
          const Vector4& color = mTextParameters->GetGradientColor();
          program.SetUniform4f( gradientColorLoc, color.r, color.g, color.b, color.a );
          program.SetUniform2f( textSizeLoc, mInvTextSize.width, mInvTextSize.height );
        }
      }
    }

    // If we don't have a gradient present (mGradientEnabled) but the shader requires
    // gradient information (gradientRequired), then we set
    // uGradientLine.zw = vec2(0.0, 0.0) to force vColor = uColor in the expression.
    // If we do have a gradient present, then we set up all information.
    const int gradientLineLoc = program.GetUniformLocation( Program::UNIFORM_GRADIENT_LINE );
    if( Program::UNIFORM_UNKNOWN != gradientLineLoc )
    {
      program.SetUniform4f( gradientLineLoc,
                            startPoint.x - 0.5f,
                            startPoint.y - 0.5f,
                            projection.x,
                            projection.y );
    }
  }

  const int positionLoc = program.GetAttribLocation(Program::ATTRIB_POSITION);
  const int texCoordLoc = program.GetAttribLocation(Program::ATTRIB_TEXCOORD);

  mTexture->Bind(GL_TEXTURE_2D, GL_TEXTURE0);

  mContext->EnableVertexAttributeArray( positionLoc );
  mContext->EnableVertexAttributeArray( texCoordLoc );

  // bind the buffers
  DALI_ASSERT_DEBUG( mVertexBuffer->BufferIsValid() );
  mVertexBuffer->Bind();
  DALI_ASSERT_DEBUG( mIndexBuffer->BufferIsValid() );
  mIndexBuffer->Bind();

  TextVertex2D* v = 0;
  mContext->VertexAttribPointer(positionLoc,  2, GL_FLOAT, GL_FALSE, sizeof(TextVertex2D), &v->mX);
  mContext->VertexAttribPointer(texCoordLoc,  4, GL_FLOAT, GL_FALSE, sizeof(TextVertex2D), &v->mU);

  const GLsizei indexCount = mIndexBuffer->GetBufferSize() / sizeof(GLushort); // compiler will optimize this to >> if possible
  mContext->DrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, (void *) 0);
  DRAW_ELEMENT_RECORD( indexCount );

  mContext->DisableVertexAttributeArray( positionLoc );
  mContext->DisableVertexAttributeArray( texCoordLoc );
}

TextRenderer::TextRenderer( RenderDataProvider& dataprovider )
: Renderer( dataprovider ),
  mTexture( NULL ),
  mTextColor( NULL ),
  mVertexBuffer(),
  mIndexBuffer(),
  mTextParameters(),
  mInvTextSize(),
  mTextureId( 0 ),
  mSmoothing( Dali::TextStyle::DEFAULT_SMOOTH_EDGE_DISTANCE_FIELD ),
  mPixelSize(0.0f)
{
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
