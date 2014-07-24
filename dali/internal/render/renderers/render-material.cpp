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
#include <dali/internal/render/renderers/render-material.h>

// INTERNAL INCLUDES
#include <dali/internal/common/internal-constants.h>
#include <dali/internal/render/gl-resources/texture.h>
#include <dali/internal/render/gl-resources/texture-cache.h>
#include <dali/internal/render/shaders/program.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

void RenderMaterialUniforms::ResetCustomUniforms()
{
  for( unsigned int subType = SHADER_DEFAULT; subType < SHADER_SUBTYPE_LAST; ++subType )
  {
    for( unsigned int i = 0; i < mNumberOfCustomUniforms; ++i )
    {
      mCustomUniform[ subType ][ i ].Reset();
    }
  }
}

void RenderMaterialUniforms::SetUniforms( const RenderMaterial& material, Program& program, ShaderSubTypes shaderType )
{
  GLint location = Program::UNIFORM_UNKNOWN;

  location = mCustomUniform[ shaderType ][ 0 ].GetUniformLocation( program, "uMaterial.mOpacity" );
  if( Program::UNIFORM_UNKNOWN != location )
  {
    program.SetUniform1f( location, material.mOpacity );
  }

  location = mCustomUniform[ shaderType ][ 1 ].GetUniformLocation( program, "uMaterial.mShininess" );
  if( Program::UNIFORM_UNKNOWN != location )
  {
    program.SetUniform1f( location, material.mShininess );
  }

  location = mCustomUniform[ shaderType ][ 2 ].GetUniformLocation( program, "uMaterial.mAmbient" );
  if( Program::UNIFORM_UNKNOWN != location )
  {
    const Vector4& color = material.mAmbientColor;
    program.SetUniform4f( location,  color.r, color.g, color.b, color.a );
  }

  location = mCustomUniform[ shaderType ][ 3 ].GetUniformLocation( program, "uMaterial.mDiffuse" );
  if( Program::UNIFORM_UNKNOWN != location )
  {
    const Vector4& color = material.mDiffuseColor;
    program.SetUniform4f( location,  color.r, color.g, color.b, color.a );
  }

  location = mCustomUniform[ shaderType ][ 4 ].GetUniformLocation( program, "uMaterial.mSpecular" );
  if( Program::UNIFORM_UNKNOWN != location )
  {
    const Vector4& color = material.mSpecularColor;
    program.SetUniform4f( location, color.r, color.g, color.b, color.a );
  }

  location = mCustomUniform[ shaderType ][ 5 ].GetUniformLocation( program, "uMaterial.mEmissive" );
  if( Program::UNIFORM_UNKNOWN != location )
  {
    const Vector4& color = material.mEmissiveColor;
    program.SetUniform4f( location, color.r, color.g, color.b, color.a );
  }
}



RenderMaterial::RenderMaterial()
: mTextureCache(NULL),

  mDiffuseTextureId(0),
  mOpacityTextureId(0),
  mNormalMapTextureId(0),

  mDiffuseTexture(NULL),
  mOpacityTexture(NULL),
  mNormalMapTexture(NULL),

  mOpacity(1.0f),
  mShininess(0.5f),

  mAmbientColor(Vector4(0.2f, 0.2f, 0.2f, 1.0f)),
  mDiffuseColor(Vector4(0.8f, 0.8f, 0.8f, 1.0f)),
  mSpecularColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f)),
  mEmissiveColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f))
{
}

RenderMaterial::~RenderMaterial()
{
  if(mDiffuseTextureId > 0)
  {
    mTextureCache->RemoveObserver( mDiffuseTextureId, this );
  }

  if(mOpacityTextureId > 0)
  {
    mTextureCache->RemoveObserver( mOpacityTextureId, this );
  }

  if(mNormalMapTextureId > 0)
  {
    mTextureCache->RemoveObserver( mNormalMapTextureId, this );
  }
}

void RenderMaterial::Initialize(TextureCache& textureCache)
{
  mTextureCache = &textureCache;
}

void RenderMaterial::SetDiffuseTextureId( unsigned int textureId )
{
  DALI_ASSERT_DEBUG( NULL != mTextureCache );

  if(mDiffuseTextureId > 0)
  {
    mTextureCache->RemoveObserver( mDiffuseTextureId, this );
  }

  mDiffuseTextureId = textureId;
  mDiffuseTexture = NULL;

  if(mDiffuseTextureId > 0)
  {
    mTextureCache->AddObserver( textureId, this );
  }
}

void RenderMaterial::SetOpacityTextureId( unsigned int textureId )
{
  DALI_ASSERT_DEBUG( NULL != mTextureCache );

  if(mOpacityTextureId > 0)
  {
    mTextureCache->RemoveObserver( mOpacityTextureId, this );
  }

  mOpacityTextureId = textureId;
  mOpacityTexture = NULL;

  if(mOpacityTextureId > 0)
  {
    mTextureCache->AddObserver( textureId, this );
  }
}

void RenderMaterial::SetNormalMapTextureId( unsigned int textureId )
{
  DALI_ASSERT_DEBUG( NULL != mTextureCache );

  if(mNormalMapTextureId > 0)
  {
    mTextureCache->RemoveObserver( mNormalMapTextureId, this );
  }

  mNormalMapTextureId = textureId;
  mNormalMapTexture = NULL;

  if(mNormalMapTextureId > 0)
  {
    mTextureCache->AddObserver( textureId, this );
  }
}

void RenderMaterial::SetOpacity(float opacity)
{
  mOpacity = opacity;
}

void RenderMaterial::SetShininess(float shininess)
{
  mShininess = shininess;
}

void RenderMaterial::SetAmbientColor( const Vector4& color )
{
  mAmbientColor = color;
}

void RenderMaterial::SetDiffuseColor( const Vector4& color )
{
  mDiffuseColor = color;
}

void RenderMaterial::SetSpecularColor( const Vector4& color )
{
  mSpecularColor = color;
}

void RenderMaterial::SetEmissiveColor( const Vector4& color )
{
  mEmissiveColor = color;
}

bool RenderMaterial::HasTexture() const
{
  return( mDiffuseTextureId != 0 );
}

void RenderMaterial::SetUniforms( RenderMaterialUniforms& uniforms, Program& program, ShaderSubTypes shaderType ) const
{
  uniforms.SetUniforms( *this, program, shaderType );
}

void RenderMaterial::BindTexture( Program& program, ResourceId id, Texture* texture, unsigned int textureUnit, Program::UniformType samplerIndex ) const
{
  DALI_ASSERT_DEBUG( NULL != mTextureCache );

  if( texture != NULL )
  {
    mTextureCache->BindTexture( texture, id, GL_TEXTURE_2D, GL_TEXTURE0 + textureUnit );
    // Set sampler uniforms for textures
    GLint samplerLoc = program.GetUniformLocation( samplerIndex );
    if( -1 != samplerLoc )
    {
      program.SetUniform1i( samplerLoc, textureUnit );
    }

    GLint location = program.GetUniformLocation(Program::UNIFORM_CUSTOM_TEXTURE_COORDS);
    if( Program::UNIFORM_UNKNOWN != location )
    {
      UvRect uvs;
      texture->GetTextureCoordinates(uvs);

      // Account for UV mapping on non power of 2 textures
      program.SetUniform4f(location, uvs.u0, uvs.v0, uvs.u2-uvs.u0, uvs.v2-uvs.v0);
    }
  }
}

void RenderMaterial::BindTextures( Program& program )
{
  DALI_ASSERT_DEBUG( NULL != mTextureCache );

  if( mDiffuseTexture == NULL )
  {
    if( mDiffuseTextureId > 0 )
    {
      mDiffuseTexture = mTextureCache->GetTexture( mDiffuseTextureId );
    }
  }

  if( mOpacityTexture == NULL )
  {
    if( mOpacityTextureId > 0 )
    {
      mOpacityTexture = mTextureCache->GetTexture( mOpacityTextureId );
    }
  }

  if( mNormalMapTexture == NULL )
  {
    if( mNormalMapTextureId > 0 )
    {
      mNormalMapTexture = mTextureCache->GetTexture( mNormalMapTextureId );
    }
  }

  BindTexture( program, mDiffuseTextureId,mDiffuseTexture, 0, Program::UNIFORM_SAMPLER );
  // GL_TEXTURE1 is used by shader effect texture
  BindTexture( program, mOpacityTextureId, mOpacityTexture, 2, Program::UNIFORM_SAMPLER_OPACITY );
  BindTexture( program, mNormalMapTextureId, mNormalMapTexture, 3, Program::UNIFORM_SAMPLER_NORMAL_MAP );
}

void RenderMaterial::TextureDiscarded( unsigned int textureId )
{
  if( mDiffuseTextureId == textureId )
  {
    mDiffuseTextureId = 0;
    mDiffuseTexture = NULL;
  }

  if( mOpacityTextureId == textureId )
  {
    mOpacityTextureId = 0;
    mOpacityTexture = NULL;
  }

  if( mNormalMapTextureId == textureId )
  {
    mNormalMapTextureId = 0;
    mNormalMapTexture = NULL;
  }
}



} // SceneGraph
} // Internal
} // Dali
