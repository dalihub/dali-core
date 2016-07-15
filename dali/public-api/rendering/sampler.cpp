/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/rendering/sampler.h>          // Dali::Sampler

// INTERNAL INCLUDES
#include <dali/internal/event/rendering/sampler-impl.h> // Dali::Internal::Sampler

namespace Dali
{

Sampler Sampler::New()
{
  Internal::SamplerPtr sampler = Internal::Sampler::New();
  return Sampler( sampler.Get() );
}

Sampler::Sampler()
{
}

Sampler::~Sampler()
{
}

Sampler::Sampler( const Sampler& handle )
: BaseHandle( handle )
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


void Sampler::SetFilterMode( FilterMode::Type minFilter, FilterMode::Type magFilter )
{
  GetImplementation(*this).SetFilterMode( minFilter, magFilter );
}

void Sampler::SetWrapMode( WrapMode::Type uWrap, WrapMode::Type vWrap )
{
  GetImplementation(*this).SetWrapMode( uWrap, uWrap, vWrap );
}

void Sampler::SetWrapMode( WrapMode::Type rWrap, WrapMode::Type sWrap, WrapMode::Type tWrap )
{
  GetImplementation(*this).SetWrapMode( rWrap, sWrap, tWrap );
}

Sampler::Sampler(Internal::Sampler* pointer)
: BaseHandle( pointer )
{
}

} //namespace Dali
