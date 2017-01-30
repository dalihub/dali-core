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
 */

// CLASS HEADER
#include "scene-graph-texture-set.h"

// INTERNAL HEADERS
#include <dali/internal/common/internal-constants.h>
#include <dali/internal/common/memory-pool-object-allocator.h>
#include <dali/internal/update/rendering/scene-graph-renderer.h>

namespace //Unnamed namespace
{
//Memory pool used to allocate new texture sets. Memory used by this pool will be released when shutting down DALi
Dali::Internal::MemoryPoolObjectAllocator<Dali::Internal::SceneGraph::TextureSet> gTextureSetMemoryPool;
}

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

TextureSet* TextureSet::New()
{
  return new ( gTextureSetMemoryPool.AllocateRawThreadSafe() ) TextureSet();
}

TextureSet::TextureSet()
: mSamplers(),
  mRenderers(),
  mHasAlpha( false )
{
}

TextureSet::~TextureSet()
{
}

void TextureSet::operator delete( void* ptr )
{
  gTextureSetMemoryPool.FreeThreadSafe( static_cast<TextureSet*>( ptr ) );
}

void TextureSet::SetSampler( size_t index, Render::Sampler* sampler )
{
  size_t samplerCount( mSamplers.Size() );
  if( samplerCount < index + 1 )
  {
    mSamplers.Resize( index + 1 );
    for( size_t i(samplerCount); i<=index; ++i )
    {
      mSamplers[i] = NULL;
    }
  }

  mSamplers[index] = sampler;
  NotifyChangeToRenderers();
}

void TextureSet::SetTexture( size_t index, Render::NewTexture* texture )
{
  const size_t textureCount( mTextures.Size() );
  if( textureCount < index + 1 )
  {
    mTextures.Resize( index + 1 );

    bool samplerExist = true;
    if( mSamplers.Size() < index + 1 )
    {
      mSamplers.Resize( index + 1 );
      samplerExist = false;
    }

    for( size_t i(textureCount); i<=index; ++i )
    {
      mTextures[i] = 0;

      if( !samplerExist )
      {
        mSamplers[i] = 0;
      }
    }
  }

  mTextures[index] = texture;
  if( texture )
  {
    mHasAlpha |= texture->HasAlphaChannel();
  }

  NotifyChangeToRenderers();
}

bool TextureSet::HasAlpha() const
{
  return mHasAlpha;
}

void TextureSet::AddObserver( Renderer* renderer )
{
  size_t rendererCount( mRenderers.Size() );
  for( size_t i(0); i<rendererCount; ++i )
  {
    if( mRenderers[i] == renderer )
    {
      //Renderer already in the list
      return;
    }
  }

  mRenderers.PushBack( renderer );
}

void TextureSet::RemoveObserver( Renderer* renderer )
{
  size_t rendererCount( mRenderers.Size() );
  for( size_t i(0); i<rendererCount; ++i )
  {
    if( mRenderers[i] == renderer )
    {
      mRenderers.Remove( mRenderers.Begin() + i );
      return;
    }
  }
}

void TextureSet::NotifyChangeToRenderers()
{
  size_t rendererCount = mRenderers.Size();
  for( size_t i(0); i<rendererCount; ++i )
  {
    mRenderers[i]->TextureSetChanged();
  }
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
