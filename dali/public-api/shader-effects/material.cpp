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
#include <dali/public-api/shader-effects/material.h>  // Dali::Material

// INTERNAL INCLUDES
#include <dali/internal/event/effects/material-impl.h> // Dali::Internal::Material
#include <dali/internal/event/effects/sampler-impl.h> // Dali::Internal::Sampler
#include <dali/internal/event/effects/shader-impl.h> // Dali::Internal::Shader

namespace Dali
{

Material Material::New( Shader shader )
{
  // TODO: MESH_REWORK
  Internal::MaterialPtr material = Internal::Material::New();
  material->SetShader( GetImplementation(shader) );

  return Material( material.Get() );
}

Material::Material()
{
}

Material::~Material()
{
}

Material::Material( const Material& handle )
: Handle( handle )
{
}

Material Material::DownCast( BaseHandle handle )
{
  return Material( dynamic_cast<Dali::Internal::Material*>(handle.GetObjectPtr()));
}

Material& Material::operator=( const Material& handle )
{
  BaseHandle::operator=( handle );
  return *this;
}

void Material::SetShader( Shader& shader )
{
  DALI_ASSERT_ALWAYS( shader && "Shader handle is uninitialized" );
  GetImplementation(*this).SetShader( GetImplementation( shader ) );
}

Shader Material::GetShader() const
{
  Internal::Shader* shaderPtr( GetImplementation(*this).GetShader() );
  return Dali::Shader( shaderPtr );
}

void Material::AddSampler( Sampler& sampler )
{
  DALI_ASSERT_ALWAYS( sampler && "Sampler handle is uninitialized" );
  GetImplementation(*this).AddSampler( GetImplementation( sampler ) );
}

std::size_t Material::GetNumberOfSamplers() const
{
  return GetImplementation(*this).GetNumberOfSamplers();
}

void Material::RemoveSampler( std::size_t index )
{
  GetImplementation(*this).RemoveSampler( index );
}

Sampler Material::GetSamplerAt( unsigned int index ) const
{
  Internal::Sampler* samplerPtr( GetImplementation(*this).GetSamplerAt(index) );
  return Dali::Sampler( samplerPtr );
}

void Material::SetFaceCullingMode( FaceCullingMode cullingMode )
{
  GetImplementation(*this).SetFaceCullingMode( cullingMode );
}

void Material::SetBlendMode( BlendingMode::Type mode )
{
  GetImplementation(*this).SetBlendMode( mode );
}

BlendingMode::Type Material::GetBlendMode() const
{
  return GetImplementation(*this).GetBlendMode();
}

void Material::SetBlendFunc( BlendingFactor::Type srcFactorRgba,
                             BlendingFactor::Type destFactorRgba )
{
  GetImplementation(*this).SetBlendFunc( srcFactorRgba, destFactorRgba );
}

void Material::SetBlendFunc( BlendingFactor::Type srcFactorRgb,
                             BlendingFactor::Type destFactorRgb,
                             BlendingFactor::Type srcFactorAlpha,
                             BlendingFactor::Type destFactorAlpha )
{
  GetImplementation(*this).SetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
}

void Material::GetBlendFunc( BlendingFactor::Type& srcFactorRgb,
                             BlendingFactor::Type& destFactorRgb,
                             BlendingFactor::Type& srcFactorAlpha,
                             BlendingFactor::Type& destFactorAlpha ) const
{
  GetImplementation(*this).GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
}

void Material::SetBlendEquation( BlendingEquation::Type equationRgba )
{
  GetImplementation(*this).SetBlendEquation( equationRgba );
}

void Material::SetBlendEquation( BlendingEquation::Type equationRgb,
                                 BlendingEquation::Type equationAlpha )
{
  GetImplementation(*this).SetBlendEquation( equationRgb, equationAlpha );
}

void Material::GetBlendEquation( BlendingEquation::Type& equationRgb,
                                 BlendingEquation::Type& equationAlpha ) const
{
  GetImplementation(*this).GetBlendEquation( equationRgb, equationAlpha );
}

void Material::SetBlendColor( const Vector4& color )
{
  GetImplementation(*this).SetBlendColor( color );
}

const Vector4& Material::GetBlendColor() const
{
  return GetImplementation(*this).GetBlendColor();
}

Material::Material( Internal::Material* pointer )
: Handle( pointer )
{
}

} //namespace Dali
