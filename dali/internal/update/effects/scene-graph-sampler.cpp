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
 */

// CLASS HEADER
#include "scene-graph-sampler.h"

// EXTERNAL HEADERS


namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

Sampler::Sampler( const std::string& samplerName )
: mUniformName( samplerName ),
  mTextureId( 0 ),
  mMinFilter( Dali::Sampler::DEFAULT ),
  mMagFilter( Dali::Sampler::DEFAULT ),
  mUWrapMode( Dali::Sampler::CLAMP_TO_EDGE ),
  mVWrapMode( Dali::Sampler::CLAMP_TO_EDGE )
{
}

Sampler::~Sampler()
{
}

void Sampler::SetUniformName( const std::string& samplerName )
{
}

void Sampler::SetTextureId( ResourceId textureId )
{
}

void Sampler::SetFilterMode( BufferIndex bufferIndex, FilterMode minFilter, FilterMode magFilter )
{
}

void Sampler::SetWrapMode( BufferIndex bufferIndex, WrapMode uWrap, WrapMode vWrap )
{
}

const std::string& Sampler::GetUniformName()
{
  // @todo MESH_REWORK
  return mUniformName;
}

Integration::ResourceId Sampler::GetTextureId()
{
  // @todo MESH_REWORK
  return mTextureId;
}

Sampler::FilterMode Sampler::GetMinifyFilterMode( BufferIndex bufferIndex )
{
  // @todo MESH_REWORK
  return mMinFilter[bufferIndex];
}

Sampler::FilterMode Sampler::GetMagnifyFilterMode( BufferIndex bufferIndex )
{
  // @todo MESH_REWORK
  return mMagFilter[bufferIndex];
}

Sampler::WrapMode Sampler::GetUWrapMode( BufferIndex bufferIndex )
{
  // @todo MESH_REWORK
  return mUWrapMode[bufferIndex];
}

Sampler::WrapMode Sampler::GetVWrapMode( BufferIndex bufferIndex )
{
  // @todo MESH_REWORK
  return mVWrapMode[bufferIndex];
}

bool Sampler::IsFullyOpaque()
{
  return true; // @todo MESH_REWORK - check the actual image. For the moment, pretend it's opaque
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
