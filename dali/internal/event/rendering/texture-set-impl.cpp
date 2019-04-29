/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/rendering/texture-set-impl.h> // Dali::Internal::TextureSet

// INTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/update/rendering/scene-graph-texture-set.h>

namespace Dali
{
namespace Internal
{

TextureSetPtr TextureSet::New()
{
  TextureSetPtr textureSet( new TextureSet() );
  textureSet->Initialize();
  return textureSet;
}

void TextureSet::SetTexture( uint32_t index, TexturePtr texture )
{
  uint32_t textureCount = static_cast<uint32_t>( mTextures.size() );
  if( index >= textureCount )
  {
    mTextures.resize(index + 1);

    bool samplerExist = true;
    if( mSamplers.size() < index + 1 )
    {
      mSamplers.resize( index + 1 );
      samplerExist = false;
    }

    for( uint32_t i(textureCount); i<=index; ++i )
    {
      mTextures[i] = NULL;

      if( !samplerExist )
      {
        mSamplers[i] = NULL;
      }
    }
  }

  mTextures[index]= texture;

  Render::Texture* renderTexture(0);
  if( texture )
  {
    renderTexture = texture->GetRenderObject();
  }

  SceneGraph::SetTextureMessage( mEventThreadServices, *mSceneObject, index, renderTexture );
}

Texture* TextureSet::GetTexture( uint32_t index ) const
{
  Texture* result(0);
  if( index < mTextures.size() )
  {
    result = mTextures[index].Get();
  }
  else
  {
    DALI_LOG_ERROR( "Error: Invalid index to TextureSet::GetTexture\n");
  }

  return result;
}

void TextureSet::SetImage( uint32_t index, ImagePtr image )
{
  Texture* texture( NULL );
  if( image )
  {
    uint32_t textureCount = static_cast<uint32_t>( mImages.size() );
    if( index >= textureCount )
    {
      mImages.resize(index + 1);
    }
    mImages[index] = image;

    texture = image->GetTexture();
  }

  SetTexture( index, texture );
}

Image* TextureSet::GetImage( uint32_t index ) const
{
  Image* result( NULL );
  if( index < mImages.size() )
  {
    result = mImages[index].Get();
  }
  else
  {
    DALI_LOG_ERROR( "Error: Invalid index to TextureSet::GetImage\n");
  }

  return result;
}

void TextureSet::SetSampler( uint32_t index, SamplerPtr sampler )
{
  uint32_t samplerCount = static_cast<uint32_t>( mSamplers.size() );
  if( samplerCount < index + 1  )
  {
    mSamplers.resize( index + 1 );
    for( uint32_t i = samplerCount; i<=index; ++i )
    {
      mSamplers[i] = NULL;
    }
  }

  mSamplers[index] = sampler;

  Render::Sampler* renderSampler(0);
  if( sampler )
  {
    renderSampler = sampler->GetSamplerRenderObject();
  }

  SceneGraph::SetSamplerMessage( mEventThreadServices, *mSceneObject, index, renderSampler );
}

Sampler* TextureSet::GetSampler( uint32_t index ) const
{
  Sampler* result(0);
  if( index < mSamplers.size() )
  {
    result = mSamplers[index].Get();
  }
  else
  {
    DALI_LOG_ERROR( "Error: Invalid index to TextureSet::GetSampler\n");
  }

  return result;
}

uint32_t TextureSet::GetTextureCount() const
{
  return static_cast<uint32_t>( mTextures.size() );
}

const SceneGraph::TextureSet* TextureSet::GetTextureSetSceneObject() const
{
  return mSceneObject;
}

TextureSet::TextureSet()
:mEventThreadServices( EventThreadServices::Get() ),
 mSceneObject( NULL )
{
}

void TextureSet::Initialize()
{
  SceneGraph::UpdateManager& updateManager = mEventThreadServices.GetUpdateManager();

  mSceneObject = SceneGraph::TextureSet::New();
  OwnerPointer< SceneGraph::TextureSet > transferOwnership( mSceneObject );
  AddTextureSetMessage( updateManager, transferOwnership );
}

TextureSet::~TextureSet()
{
  if( EventThreadServices::IsCoreRunning() )
  {
    SceneGraph::UpdateManager& updateManager = mEventThreadServices.GetUpdateManager();
    RemoveTextureSetMessage( updateManager, *mSceneObject );
  }
}

} // namespace Internal
} // namespace Dali
