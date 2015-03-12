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

Sampler::Sampler( const std::string& unitName )
: mUnitName( unitName ),
  mMinFilter( Dali::Sampler::DEFAULT ),
  mMagFilter( Dali::Sampler::DEFAULT ),
  mUWrapMode( Dali::Sampler::CLAMP_TO_EDGE ),
  mVWrapMode( Dali::Sampler::CLAMP_TO_EDGE )
{
  mTextureId[0] = 0;
  mTextureId[1] = 0;
}

Sampler::~Sampler()
{
}

void Sampler::SetUnitName( const std::string& unitName )
{
  mUnitName = unitName;
}

void Sampler::SetTexture( BufferIndex bufferIndex, Integration::ResourceId textureId )
{
  mTextureId[bufferIndex] = textureId;

  // @todo MESH_REWORK
  //const BitmapMetadata& metadata  - get it from ResourceManager
  //mBitmapMetadata[bufferIndex] = metadata;
}

void Sampler::SetFilterMode( BufferIndex bufferIndex, FilterMode minFilter, FilterMode magFilter )
{
  mMinFilter[bufferIndex] = minFilter;
  mMagFilter[bufferIndex] = magFilter;
}

void Sampler::SetWrapMode( BufferIndex bufferIndex, WrapMode uWrap, WrapMode vWrap )
{
}

const std::string& Sampler::GetUnitName() const
{
  return mUnitName;
}

Integration::ResourceId Sampler::GetTextureId( BufferIndex bufferIndex ) const
{
  return mTextureId[bufferIndex];
}

Sampler::FilterMode Sampler::GetMinifyFilterMode( BufferIndex bufferIndex ) const
{
  return mMinFilter[bufferIndex];
}

Sampler::FilterMode Sampler::GetMagnifyFilterMode( BufferIndex bufferIndex ) const
{
  return mMagFilter[bufferIndex];
}

Sampler::WrapMode Sampler::GetUWrapMode( BufferIndex bufferIndex ) const
{
  return mUWrapMode[bufferIndex];
}

Sampler::WrapMode Sampler::GetVWrapMode( BufferIndex bufferIndex ) const
{
  return mVWrapMode[bufferIndex];
}

bool Sampler::AffectsTransparency( BufferIndex bufferIndex ) const
{
  return mAffectsTransparency[bufferIndex] ;
}

bool Sampler::IsFullyOpaque( BufferIndex bufferIndex ) const
{
  return mBitmapMetadata[bufferIndex].IsFullyOpaque();
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
