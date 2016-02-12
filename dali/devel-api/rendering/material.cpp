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
#include <dali/devel-api/rendering/material.h>  // Dali::Material

// INTERNAL INCLUDES
#include <dali/internal/event/rendering/material-impl.h> // Dali::Internal::Material
#include <dali/internal/event/rendering/sampler-impl.h> // Dali::Internal::Sampler
#include <dali/internal/event/rendering/shader-impl.h> // Dali::Internal::Shader

namespace Dali
{

Material Material::New( Shader shader )
{
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

int Material::AddTexture( Image image, const std::string& uniformName, Sampler sampler)
{
  int index( -1 );
  if( image )
  {
    Internal::ImagePtr imagePtr(&GetImplementation( image ));
    Internal::SamplerPtr samplerPtr(0);
    if( sampler )
    {
      samplerPtr = &GetImplementation( sampler );
    }

    index = GetImplementation(*this).AddTexture( imagePtr, uniformName, samplerPtr );
  }
  else
  {
    DALI_LOG_ERROR("Error adding invalid texture %s to material", uniformName.c_str() );
  }
  return index;
}

void Material::RemoveTexture( size_t index )
{
  GetImplementation(*this).RemoveTexture( index );
}

void Material::SetTextureImage( size_t index, Image image )
{
  if( image )
  {
    Internal::Image* imagePtr(0);
    imagePtr = &GetImplementation( image );
    GetImplementation(*this).SetTextureImage( index, imagePtr );
  }
}

void Material::SetTextureSampler( size_t index, Sampler sampler )
{
  Internal::Sampler* samplerPtr(0);
  if( sampler )
  {
    samplerPtr = &GetImplementation( sampler );
  }

  GetImplementation(*this).SetTextureSampler( index, samplerPtr );
}

Sampler Material::GetTextureSampler( size_t index ) const
{
  Internal::Sampler* samplerPtr( GetImplementation(*this).GetTextureSampler( index ) );
  return Dali::Sampler( samplerPtr );
}

void Material::SetTextureUniformName( size_t index, const std::string& uniformName )
{
  GetImplementation(*this).SetTextureUniformName( index, uniformName );
}

int Material::GetTextureIndex( const std::string& uniformName ) const
{
  return GetImplementation(*this).GetTextureIndex( uniformName );
}

Image Material::GetTexture( const std::string& uniformName ) const
{
  Internal::Image* imagePtr( GetImplementation(*this).GetTexture( uniformName ) );
  return Dali::Image( imagePtr );
}

Image Material::GetTexture( size_t index ) const
{
  Internal::Image* imagePtr( GetImplementation(*this).GetTexture( index ) );
  return Dali::Image( imagePtr );
}

std::size_t Material::GetNumberOfTextures() const
{
  return GetImplementation(*this).GetNumberOfTextures();
}


Material::Material( Internal::Material* pointer )
: Handle( pointer )
{
}

} //namespace Dali
