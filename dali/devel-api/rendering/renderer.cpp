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
#include <dali/devel-api/rendering/renderer.h>  // Dali::Renderer

// INTERNAL INCLUDES
#include <dali/internal/event/rendering/renderer-impl.h> // Dali::Internal::Renderer

namespace Dali
{

Renderer Renderer::New( Geometry& geometry, Shader& shader )
{
  Internal::RendererPtr renderer = Internal::Renderer::New(  );
  renderer->SetGeometry( GetImplementation(geometry) );
  renderer->SetShader( GetImplementation(shader) );
  return Renderer( renderer.Get() );
}

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

Renderer::Renderer( const Renderer& handle )
: Handle( handle )
{
}

Renderer Renderer::DownCast( BaseHandle handle )
{
  return Renderer( dynamic_cast<Dali::Internal::Renderer*>(handle.GetObjectPtr()));
}

Renderer& Renderer::operator=( const Renderer& handle )
{
  BaseHandle::operator=( handle );
  return *this;
}

void Renderer::SetGeometry( Geometry& geometry )
{
  DALI_ASSERT_ALWAYS( geometry && "Geometry handle not initialized" );
  GetImplementation(*this).SetGeometry( GetImplementation(geometry) );
}

Geometry Renderer::GetGeometry() const
{
  Internal::Geometry* geometryPtr( GetImplementation(*this).GetGeometry() );
  return Dali::Geometry( geometryPtr );
}

void Renderer::SetTextures( TextureSet& textureSet )
{
  DALI_ASSERT_ALWAYS( textureSet && "TextureSet handle not initialized" );
  GetImplementation(*this).SetTextures( GetImplementation(textureSet) );
}

TextureSet Renderer::GetTextures() const
{
  Internal::TextureSet* textureSet( GetImplementation(*this).GetTextures() );
  return Dali::TextureSet( textureSet );
}

void Renderer::SetShader( Shader& shader )
{
  DALI_ASSERT_ALWAYS( shader && "Shader handle not initialized" );
  GetImplementation(*this).SetShader( GetImplementation(shader) );
}

Shader Renderer::GetShader() const
{
  Internal::Shader* shaderPtr( GetImplementation(*this).GetShader() );
  return Dali::Shader( shaderPtr );
}

void Renderer::SetBlendFunc( BlendingFactor::Type srcFactorRgba,
                             BlendingFactor::Type destFactorRgba )
{
  GetImplementation(*this).SetBlendFunc( srcFactorRgba, destFactorRgba );
}

void Renderer::SetBlendFunc( BlendingFactor::Type srcFactorRgb,
                             BlendingFactor::Type destFactorRgb,
                             BlendingFactor::Type srcFactorAlpha,
                             BlendingFactor::Type destFactorAlpha )
{
  GetImplementation(*this).SetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
}

void Renderer::GetBlendFunc( BlendingFactor::Type& srcFactorRgb,
                             BlendingFactor::Type& destFactorRgb,
                             BlendingFactor::Type& srcFactorAlpha,
                             BlendingFactor::Type& destFactorAlpha ) const
{
  GetImplementation(*this).GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
}

void Renderer::SetBlendEquation( BlendingEquation::Type equationRgba )
{
  GetImplementation(*this).SetBlendEquation( equationRgba );
}

void Renderer::SetBlendEquation( BlendingEquation::Type equationRgb,
                                 BlendingEquation::Type equationAlpha )
{
  GetImplementation(*this).SetBlendEquation( equationRgb, equationAlpha );
}

void Renderer::GetBlendEquation( BlendingEquation::Type& equationRgb,
                                 BlendingEquation::Type& equationAlpha ) const
{
  GetImplementation(*this).GetBlendEquation( equationRgb, equationAlpha );
}

Renderer::Renderer( Internal::Renderer* pointer )
: Handle( pointer )
{
}

} //namespace Dali
