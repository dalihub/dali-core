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
#include <dali/devel-api/rendering/sampler.h>  // Dali::Sampler

// INTERNAL INCLUDES
#include <dali/internal/event/rendering/sampler-impl.h> // Dali::Internal::Sampler

namespace Dali
{

Sampler Sampler::New( Image& image, const std::string& textureUnitUniformName )
{
  Internal::SamplerPtr sampler = Internal::Sampler::New(textureUnitUniformName);
  Internal::ImagePtr imagePtr = &GetImplementation( image );
  sampler->SetImage( imagePtr );
  return Sampler( sampler.Get() );
}

Sampler::Sampler()
{
}

Sampler::~Sampler()
{
}

Sampler::Sampler( const Sampler& handle )
: Handle( handle )
{
}

Sampler Sampler::DownCast( BaseHandle handle )
{
  return Sampler( dynamic_cast<Dali::Internal::Sampler*>(handle.GetObjectPtr()));
}

Sampler& Sampler::operator=( const Sampler& handle )
{
  BaseHandle::operator=( handle );
  return *this;
}

void Sampler::SetUniformName( const std::string& name )
{
  GetImplementation(*this).SetTextureUnitUniformName( name );
}

const std::string& Sampler::GetUniformName() const
{
  return GetImplementation(*this).GetTextureUnitUniformName();
}

void Sampler::SetImage( Image& image )
{
  Internal::ImagePtr imagePtr = &GetImplementation( image );
  GetImplementation(*this).SetImage( imagePtr );
}

Image Sampler::GetImage() const
{
  Internal::ImagePtr imagePtr( GetImplementation(*this).GetImage() );
  return Dali::Image( imagePtr.Get() );
}

void Sampler::SetFilterMode( FilterMode minFilter, FilterMode magFilter )
{
  GetImplementation(*this).SetFilterMode( minFilter, magFilter );
}

void Sampler::SetWrapMode( WrapMode uWrap, WrapMode vWrap )
{
  GetImplementation(*this).SetWrapMode( uWrap, vWrap );
}

void Sampler::SetAffectsTransparency( bool affectsTransparency )
{
  GetImplementation(*this).SetAffectsTransparency( affectsTransparency );
}

Sampler::Sampler(Internal::Sampler* pointer)
: Handle( pointer )
{
}

} //namespace Dali
