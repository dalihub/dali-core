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

Sampler::Sampler( const std::string& textureUnitUniformName )
: mMinFilter( Dali::Sampler::DEFAULT ),
  mMagFilter( Dali::Sampler::DEFAULT ),
  mUWrapMode( Dali::Sampler::CLAMP_TO_EDGE ),
  mVWrapMode( Dali::Sampler::CLAMP_TO_EDGE ),
  mAffectsTransparency( true ),
  mTextureUnitUniformName( textureUnitUniformName ),
  mTextureId( 0u ),
  mFullyOpaque(true)
{
}

Sampler::~Sampler()
{
}

void Sampler::SetTextureUnitUniformName( const std::string& textureUnitUniformName )
{
  mTextureUnitUniformName = textureUnitUniformName;
}

void Sampler::SetTexture( BufferIndex bufferIndex, Integration::ResourceId textureId )
{
  if( mTextureId[bufferIndex] != textureId )
  {
    mTextureId.Set( bufferIndex, textureId );
    mConnectionObservers.ConnectionsChanged(*this);
  }
}

void Sampler::SetFilterMode( BufferIndex bufferIndex, FilterMode minFilter, FilterMode magFilter )
{
  mMinFilter.Set(bufferIndex, minFilter);
  mMagFilter.Set(bufferIndex, magFilter);
}

void Sampler::SetWrapMode( BufferIndex bufferIndex, WrapMode uWrap, WrapMode vWrap )
{
}

const std::string& Sampler::GetTextureUnitUniformName() const
{
  return mTextureUnitUniformName;
}

Integration::ResourceId Sampler::GetTextureId( BufferIndex bufferIndex ) const
{
  return mTextureId[bufferIndex];
}

Sampler::FilterMode Sampler::GetMinifyFilterMode( BufferIndex bufferIndex ) const
{
  return static_cast<Sampler::FilterMode>(mMinFilter[bufferIndex]);
}

Sampler::FilterMode Sampler::GetMagnifyFilterMode( BufferIndex bufferIndex ) const
{
  return static_cast<Sampler::FilterMode>(mMagFilter[bufferIndex]);
}

Sampler::WrapMode Sampler::GetUWrapMode( BufferIndex bufferIndex ) const
{
  return static_cast<Sampler::WrapMode>(mUWrapMode[bufferIndex]);
}

Sampler::WrapMode Sampler::GetVWrapMode( BufferIndex bufferIndex ) const
{
  return static_cast<Sampler::WrapMode>(mVWrapMode[bufferIndex]);
}

bool Sampler::AffectsTransparency( BufferIndex bufferIndex ) const
{
  return mAffectsTransparency[bufferIndex] ;
}

void Sampler::SetFullyOpaque( bool fullyOpaque )
{
  mFullyOpaque = fullyOpaque;
}

bool Sampler::IsFullyOpaque( BufferIndex bufferIndex ) const
{
  return mFullyOpaque;
}

void Sampler::ConnectToSceneGraph( SceneController& sceneController, BufferIndex bufferIndex )
{
}

void Sampler::DisconnectFromSceneGraph(SceneController& sceneController, BufferIndex bufferIndex)
{
}

void Sampler::AddConnectionObserver( ConnectionChangePropagator::Observer& observer )
{
  mConnectionObservers.Add(observer);
}

void Sampler::RemoveConnectionObserver( ConnectionChangePropagator::Observer& observer )
{
  mConnectionObservers.Remove(observer);
}

void Sampler::ResetDefaultProperties( BufferIndex bufferIndex )
{
  mTextureId.CopyPrevious( bufferIndex );
  mMinFilter.CopyPrevious( bufferIndex );
  mMagFilter.CopyPrevious( bufferIndex );
  mUWrapMode.CopyPrevious( bufferIndex );
  mVWrapMode.CopyPrevious( bufferIndex );
  mAffectsTransparency.CopyPrevious( bufferIndex );
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
