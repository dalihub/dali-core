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

//EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
#include <dali/devel-api/rendering/texture-set.h> // Dali::Internal::Render::TextureSet
#include <dali/internal/event/common/object-impl-helper.h> // Dali::Internal::ObjectHelper
#include <dali/internal/event/common/property-helper.h> // DALI_PROPERTY_TABLE_BEGIN, DALI_PROPERTY, DALI_PROPERTY_TABLE_END
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/update/rendering/scene-graph-texture-set.h>

namespace Dali
{
namespace Internal
{

namespace
{

const ObjectImplHelper<0> TEXTURE_SET_IMPL = { NULL };

BaseHandle Create()
{
  return Dali::BaseHandle();
}

TypeRegistration mType( typeid( Dali::TextureSet ), typeid( Dali::Handle ), Create );

} // unnamed namespace

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
    SceneGraph::SetImageMessage( GetEventThreadServices(), *mSceneObject, index, image->GetResourceId() );
  }
  else
  {
    SceneGraph::SetImageMessage( GetEventThreadServices(), *mSceneObject, index, Integration::InvalidResourceId );
  }
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

  SceneGraph::SetSamplerMessage( GetEventThreadServices(), *mSceneObject, index, renderSampler );
}

ImagePtr TextureSet::GetImage( size_t index )
{
  ImagePtr result(0);
  if( index < mTextures.size() )
  {
    result = mTextures[index].image;
  }
  else
  {
    DALI_LOG_ERROR( "Error: Invalid index to TextureSet::GetImage");
  }

  return result;
}

const SceneGraph::TextureSet* TextureSet::GetTextureSetSceneObject() const
{
  return mSceneObject;
}

unsigned int TextureSet::GetDefaultPropertyCount() const
{
  return TEXTURE_SET_IMPL.GetDefaultPropertyCount();
}

void TextureSet::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  TEXTURE_SET_IMPL.GetDefaultPropertyIndices( indices );
}

const char* TextureSet::GetDefaultPropertyName(Property::Index index) const
{
  return TEXTURE_SET_IMPL.GetDefaultPropertyName( index );
}

Property::Index TextureSet::GetDefaultPropertyIndex( const std::string& name ) const
{
  return TEXTURE_SET_IMPL.GetDefaultPropertyIndex( name );
}

bool TextureSet::IsDefaultPropertyWritable( Property::Index index ) const
{
  return TEXTURE_SET_IMPL.IsDefaultPropertyWritable( index );
}

bool TextureSet::IsDefaultPropertyAnimatable( Property::Index index ) const
{
  return TEXTURE_SET_IMPL.IsDefaultPropertyAnimatable( index );
}

bool TextureSet::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  return TEXTURE_SET_IMPL.IsDefaultPropertyAConstraintInput( index );
}

Property::Type TextureSet::GetDefaultPropertyType( Property::Index index ) const
{
  return TEXTURE_SET_IMPL.GetDefaultPropertyType( index );
}

void TextureSet::SetDefaultProperty( Property::Index index,
                                   const Property::Value& propertyValue )
{
}

void TextureSet::SetSceneGraphProperty( Property::Index index,
                                      const PropertyMetadata& entry,
                                      const Property::Value& value )
{
  TEXTURE_SET_IMPL.SetSceneGraphProperty( GetEventThreadServices(), this, index, entry, value );
  OnPropertySet(index, value);
}

Property::Value TextureSet::GetDefaultProperty( Property::Index index ) const
{
  Property::Value value;
  return value;
}

const SceneGraph::PropertyOwner* TextureSet::GetPropertyOwner() const
{
  return mSceneObject;
}

const SceneGraph::PropertyOwner* TextureSet::GetSceneObject() const
{
  return mSceneObject;
}

const SceneGraph::PropertyBase* TextureSet::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  PropertyMetadata* property = index >= PROPERTY_CUSTOM_START_INDEX ? static_cast<PropertyMetadata*>(FindCustomProperty( index )) : static_cast<PropertyMetadata*>(FindAnimatableProperty( index ));
  DALI_ASSERT_ALWAYS( property && "Property index is invalid" );
  return property->GetSceneGraphProperty();
}

const PropertyInputImpl* TextureSet::GetSceneObjectInputProperty( Property::Index index ) const
{
  return GetSceneObjectAnimatableProperty( index );
}

int TextureSet::GetPropertyComponentIndex( Property::Index index ) const
{
  return Property::INVALID_COMPONENT_INDEX;
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
      SceneGraph::SetImageMessage( GetEventThreadServices(), *mSceneObject, i, mTextures[i].image->GetResourceId() );
    }
    else
    {
      SceneGraph::SetImageMessage( GetEventThreadServices(), *mSceneObject, i, Integration::InvalidResourceId );
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
: mSceneObject( NULL ),
  mTextures(),
  mOnStage( false )
{
}

void TextureSet::Initialize()
{
  EventThreadServices& eventThreadServices = GetEventThreadServices();
  SceneGraph::UpdateManager& updateManager = eventThreadServices.GetUpdateManager();

  mSceneObject = SceneGraph::TextureSet::New();
  AddMessage( updateManager, updateManager.GetTexturesOwner(), *mSceneObject );

  eventThreadServices.RegisterObject( this );
}

TextureSet::~TextureSet()
{
  if( EventThreadServices::IsCoreRunning() )
  {
    EventThreadServices& eventThreadServices = GetEventThreadServices();
    SceneGraph::UpdateManager& updateManager = eventThreadServices.GetUpdateManager();
    RemoveMessage( updateManager, updateManager.GetTexturesOwner(), *mSceneObject );

    eventThreadServices.UnregisterObject( this );
  }
}

} // namespace Internal
} // namespace Dali
