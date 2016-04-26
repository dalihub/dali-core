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
#include <dali/devel-api/rendering/texture-set.h> // Dali::Internal::Render::TextureSet
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
  size_t textureCount( mTextures.size() );
  if( index < textureCount )
  {
    if( mTextures[index].image && mOnStage )
    {
      mTextures[index].image->Disconnect();
    }
  }
  else
  {
    mTextures.resize(index + 1);
    for( size_t i(textureCount); i<=index; ++i )
    {
      mTextures[i].image = NULL;
      mTextures[i].sampler = NULL;
    }
  }
  mTextures[index].image = image;


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

Image* TextureSet::GetImage( size_t index ) const
{
  Image* result(0);
  if( index < mTextures.size() )
  {
    result = mTextures[index].image.Get();
  }
  else
  {
    DALI_LOG_ERROR( "Error: Invalid index to TextureSet::GetImage");
  }

  return result;
}

void TextureSet::SetSampler( size_t index, SamplerPtr sampler )
{
  size_t textureCount( mTextures.size() );
  if( textureCount < index + 1  )
  {
    mTextures.resize(index + 1);
    for( size_t i(textureCount); i<=index; ++i )
    {
      mTextures[i].image = NULL;
      mTextures[i].sampler = NULL;
    }
  }

  mTextures[index].sampler = sampler;

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
  if( index < mTextures.size() )
  {
    result = mTextures[index].sampler.Get();
  }
  else
  {
    DALI_LOG_ERROR( "Error: Invalid index to TextureSet::GetSampler");
  }

  return result;
}

size_t TextureSet::GetTextureCount() const
{
  return mTextures.size();
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

  for( size_t i(0); i<mTextures.size(); ++i )
  {
    if( mTextures[i].image )
    {
      mTextures[i].image->Connect();
      SceneGraph::SetImageMessage( mEventThreadServices, *mSceneObject, i, mTextures[i].image->GetResourceId() );
    }
    else
    {
      SceneGraph::SetImageMessage( mEventThreadServices, *mSceneObject, i, Integration::InvalidResourceId );
    }
  }
}

void TextureSet::Disconnect()
{
  for( size_t i(0); i<mTextures.size(); ++i )
  {
    if( mTextures[i].image )
    {
      mTextures[i].image->Disconnect();
    }
  }

  mOnStage = false;
}

TextureSet::TextureSet()
:mEventThreadServices( *Stage::GetCurrent() ),
 mSceneObject( NULL ),
 mTextures(),
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
