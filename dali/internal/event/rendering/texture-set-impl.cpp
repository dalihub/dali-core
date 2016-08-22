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
#include <dali/internal/event/rendering/texture-set-impl.h> // Dali::Internal::TextureSet

// INTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
#include <dali/internal/event/common/object-impl-helper.h> // Dali::Internal::ObjectHelper
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

void TextureSet::SetImage( size_t index, ImagePtr image )
{
  if( !mNewTextures.empty() )
  {
    DALI_LOG_ERROR( "Error: Cannot mix images and textures in the same TextureSet");
    return;
  }

  size_t textureCount( mImages.size() );
  if( index < textureCount )
  {
    if( mImages[index] && mOnStage )
    {
      mImages[index]->Disconnect();
    }
  }
  else
  {
    mImages.resize(index + 1);
    mSamplers.resize(index + 1);
    for( size_t i(textureCount); i<=index; ++i )
    {
      mImages[i] = NULL;
      mSamplers[i] = NULL;
    }
  }
  mImages[index] = image;


  if( image )
  {
    if( mOnStage )
    {
      image->Connect();
    }
    SceneGraph::SetImageMessage( mEventThreadServices, *mSceneObject, index, image->GetResourceId() );
  }
  else
  {
    SceneGraph::SetImageMessage( mEventThreadServices, *mSceneObject, index, Integration::InvalidResourceId );
  }
}

void TextureSet::SetTexture( size_t index, NewTexturePtr texture )
{
  if( !mImages.empty() )
  {
    DALI_LOG_ERROR( "Error: Cannot mix images and textures in the same texture set");
    return;
  }

  size_t textureCount( mNewTextures.size() );
  if( index >= textureCount )
  {
    mNewTextures.resize(index + 1);
    mSamplers.resize(index + 1);
    for( size_t i(textureCount); i<=index; ++i )
    {
      mNewTextures[i] = NULL;
      mSamplers[i] = NULL;
    }
  }

  mNewTextures[index]= texture;

  Render::NewTexture* renderTexture(0);
  if( texture )
  {
    renderTexture = texture->GetRenderObject();
  }

  SceneGraph::SetTextureMessage( mEventThreadServices, *mSceneObject, index, renderTexture );
}

Image* TextureSet::GetImage( size_t index ) const
{
  Image* result(0);
  if( index < mImages.size() )
  {
    result = mImages[index].Get();
  }
  else
  {
    DALI_LOG_ERROR( "Error: Invalid index to TextureSet::GetImage");
  }

  return result;
}

NewTexture* TextureSet::GetTexture( size_t index ) const
{
  NewTexture* result(0);
  if( index < mNewTextures.size() )
  {
    result = mNewTextures[index].Get();
  }
  else
  {
    DALI_LOG_ERROR( "Error: Invalid index to TextureSet::GetTexture");
  }

  return result;
}

void TextureSet::SetSampler( size_t index, SamplerPtr sampler )
{
  size_t samplerCount( mSamplers.size() );
  if( samplerCount < index + 1  )
  {
    mSamplers.resize( index + 1 );
    mNewTextures.resize( index + 1 );
    mImages.resize( index + 1 );
    for( size_t i(samplerCount); i<=index; ++i )
    {
      mImages[i] = NULL;
      mSamplers[i] = NULL;
      mNewTextures[i] = NULL;
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

Sampler* TextureSet::GetSampler( size_t index ) const
{
  Sampler* result(0);
  if( index < mSamplers.size() )
  {
    result = mSamplers[index].Get();
  }
  else
  {
    DALI_LOG_ERROR( "Error: Invalid index to TextureSet::GetSampler");
  }

  return result;
}

size_t TextureSet::GetTextureCount() const
{
  return mImages.size();
}

const SceneGraph::TextureSet* TextureSet::GetTextureSetSceneObject() const
{
  return mSceneObject;
}

bool TextureSet::OnStage() const
{
  return mOnStage;
}

void TextureSet::Connect()
{
  mOnStage = true;

  for( size_t i(0); i<mImages.size(); ++i )
  {
    if( mImages[i] )
    {
      mImages[i]->Connect();
      SceneGraph::SetImageMessage( mEventThreadServices, *mSceneObject, i, mImages[i]->GetResourceId() );
    }
    else
    {
      SceneGraph::SetImageMessage( mEventThreadServices, *mSceneObject, i, Integration::InvalidResourceId );
    }
  }
}

void TextureSet::Disconnect()
{
  for( size_t i(0); i<mImages.size(); ++i )
  {
    if( mImages[i] )
    {
      mImages[i]->Disconnect();
    }
  }

  mOnStage = false;
}

TextureSet::TextureSet()
:mEventThreadServices( *Stage::GetCurrent() ),
 mSceneObject( NULL ),
 mOnStage( false )
{
}

void TextureSet::Initialize()
{
  SceneGraph::UpdateManager& updateManager = mEventThreadServices.GetUpdateManager();

  mSceneObject = SceneGraph::TextureSet::New();
  AddTextureSetMessage( updateManager, *mSceneObject );
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
