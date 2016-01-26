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
#include <dali/internal/event/rendering/material-impl.h> // Dali::Internal::Material

//EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
#include <dali/devel-api/rendering/material.h> // Dali::Internal::Material
#include <dali/internal/event/common/object-impl-helper.h> // Dali::Internal::ObjectHelper
#include <dali/internal/event/common/property-helper.h> // DALI_PROPERTY_TABLE_BEGIN, DALI_PROPERTY, DALI_PROPERTY_TABLE_END
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/update/rendering/scene-graph-material.h>

namespace Dali
{
namespace Internal
{

namespace
{

const ObjectImplHelper<0> MATERIAL_IMPL = { NULL };

BaseHandle Create()
{
  return Dali::BaseHandle();
}

TypeRegistration mType( typeid( Dali::Material ), typeid( Dali::Handle ), Create );

} // unnamed namespace

MaterialPtr Material::New()
{
  MaterialPtr material( new Material() );
  material->Initialize();
  return material;
}

void Material::SetShader( Shader& shader )
{
  DALI_ASSERT_DEBUG( mSceneObject )
  mShader = &shader;

  SceneGraph::Shader& sceneGraphShader = *shader.GetShaderSceneObject();
  SceneGraph::SetShaderMessage( GetEventThreadServices(), *mSceneObject, sceneGraphShader );
}

Shader* Material::GetShader() const
{
  return mShader.Get();
}

size_t Material::AddTexture( ImagePtr image, const std::string& uniformName, SamplerPtr sampler )
{
  size_t index = mTextures.size();
  mTextures.push_back( Texture( uniformName, image, sampler ) );

  Render::Sampler* renderSampler(0);
  if( sampler )
  {
    renderSampler = sampler->GetSamplerRenderObject();
  }

  if( mOnStage )
  {
    image->Connect();
  }

  SceneGraph::AddTextureMessage( GetEventThreadServices(), *mSceneObject, uniformName, image->GetResourceId(), renderSampler );
  return index;
}

void Material::RemoveTexture( size_t index )
{
  if( index < GetNumberOfTextures() )
  {
    mTextures.erase( mTextures.begin() + index );
    SceneGraph::RemoveTextureMessage( GetEventThreadServices(), *mSceneObject, index );
  }
}

void Material::SetTextureImage( size_t index, Image* image )
{
  if( index < GetNumberOfTextures() )
  {
    if( mTextures[index].mImage && mOnStage )
    {
      mTextures[index].mImage->Disconnect();

      if( image )
      {
        image->Connect();
      }
    }

    mTextures[index].mImage.Reset(image);
    SceneGraph::SetTextureImageMessage( GetEventThreadServices(), *mSceneObject, index, mTextures[index].mImage.Get()->GetResourceId() );
  }
}

void Material::SetTextureSampler( size_t index, Sampler* sampler )
{
  if( index < GetNumberOfTextures() )
  {
    mTextures[index].mSampler.Reset(sampler);

    Render::Sampler* renderSampler(0);
    if( sampler )
    {
      renderSampler = sampler->GetSamplerRenderObject();
    }
    SceneGraph::SetTextureSamplerMessage( GetEventThreadServices(), *mSceneObject, index,  renderSampler );
  }
}

Sampler* Material::GetTextureSampler( size_t index ) const
{
  if( index < GetNumberOfTextures() )
  {
    return mTextures[index].mSampler.Get();
  }

  return NULL;
}

void Material::SetTextureUniformName( size_t index, const std::string& uniformName )
{
  if( index < GetNumberOfTextures() )
  {
    mTextures[index].mUniformName = uniformName;
    SceneGraph::SetTextureUniformNameMessage( GetEventThreadServices(), *mSceneObject, index,  uniformName );
  }
}

int Material::GetTextureIndex( const std::string& uniformName ) const
{
  size_t textureCount(GetNumberOfTextures());
  for( size_t i(0); i<textureCount; ++i )
  {
    if( uniformName.compare( mTextures[i].mUniformName ) == 0 )
    {
      return i;
    }
  }

  return -1;
}

Image* Material::GetTexture( const std::string& uniformName ) const
{
  int textureId = GetTextureIndex( uniformName );
  if( textureId != -1 )
  {
    return GetTexture( textureId );
  }

  return NULL;
}

Image* Material::GetTexture( size_t index ) const
{
  if( index < GetNumberOfTextures() )
  {
    return mTextures[ index ].mImage.Get();
  }

  return NULL;
}

size_t Material::GetNumberOfTextures() const
{
  return mTextures.size();
}

const SceneGraph::Material* Material::GetMaterialSceneObject() const
{
  return mSceneObject;
}

unsigned int Material::GetDefaultPropertyCount() const
{
  return MATERIAL_IMPL.GetDefaultPropertyCount();
}

void Material::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  MATERIAL_IMPL.GetDefaultPropertyIndices( indices );
}

const char* Material::GetDefaultPropertyName(Property::Index index) const
{
  return MATERIAL_IMPL.GetDefaultPropertyName( index );
}

Property::Index Material::GetDefaultPropertyIndex( const std::string& name ) const
{
  return MATERIAL_IMPL.GetDefaultPropertyIndex( name );
}

bool Material::IsDefaultPropertyWritable( Property::Index index ) const
{
  return MATERIAL_IMPL.IsDefaultPropertyWritable( index );
}

bool Material::IsDefaultPropertyAnimatable( Property::Index index ) const
{
  return MATERIAL_IMPL.IsDefaultPropertyAnimatable( index );
}

bool Material::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  return MATERIAL_IMPL.IsDefaultPropertyAConstraintInput( index );
}

Property::Type Material::GetDefaultPropertyType( Property::Index index ) const
{
  return MATERIAL_IMPL.GetDefaultPropertyType( index );
}

void Material::SetDefaultProperty( Property::Index index,
                                   const Property::Value& propertyValue )
{
}

void Material::SetSceneGraphProperty( Property::Index index,
                                      const PropertyMetadata& entry,
                                      const Property::Value& value )
{
  MATERIAL_IMPL.SetSceneGraphProperty( GetEventThreadServices(), this, index, entry, value );
  OnPropertySet(index, value);
}

Property::Value Material::GetDefaultProperty( Property::Index index ) const
{
  Property::Value value;
  return value;
}

const SceneGraph::PropertyOwner* Material::GetPropertyOwner() const
{
  return mSceneObject;
}

const SceneGraph::PropertyOwner* Material::GetSceneObject() const
{
  return mSceneObject;
}

const SceneGraph::PropertyBase* Material::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  PropertyMetadata* property = index >= PROPERTY_CUSTOM_START_INDEX ? static_cast<PropertyMetadata*>(FindCustomProperty( index )) : static_cast<PropertyMetadata*>(FindAnimatableProperty( index ));
  DALI_ASSERT_ALWAYS( property && "Property index is invalid" );
  return property->GetSceneGraphProperty();
}

const PropertyInputImpl* Material::GetSceneObjectInputProperty( Property::Index index ) const
{
  return GetSceneObjectAnimatableProperty( index );
}

int Material::GetPropertyComponentIndex( Property::Index index ) const
{
  return Property::INVALID_COMPONENT_INDEX;
}

bool Material::OnStage() const
{
  return mOnStage;
}

void Material::Connect()
{
  mOnStage = true;

  for( size_t i(0); i<mTextures.size(); ++i )
  {
    if( mTextures[i].mImage )
    {
      mTextures[i].mImage->Connect();
      if( mTextures[i].mImage->GetResourceId() != 0 )
      {
        SceneGraph::SetTextureImageMessage( GetEventThreadServices(), *mSceneObject, i, mTextures[i].mImage->GetResourceId() );
      }
    }
  }
}

void Material::Disconnect()
{
  for( size_t i(0); i<mTextures.size(); ++i )
  {
    if( mTextures[i].mImage )
    {
      mTextures[i].mImage->Disconnect();
    }
  }

  mOnStage = false;
}

Material::Material()
: mSceneObject( NULL ),
  mShader( NULL ),
  mTextures(),
  mOnStage( false )
{
}

void Material::Initialize()
{
  EventThreadServices& eventThreadServices = GetEventThreadServices();
  SceneGraph::UpdateManager& updateManager = eventThreadServices.GetUpdateManager();

  mSceneObject = SceneGraph::Material::New();
  AddMessage( updateManager, updateManager.GetMaterialOwner(), *mSceneObject );

  eventThreadServices.RegisterObject( this );
}

Material::~Material()
{
  if( EventThreadServices::IsCoreRunning() )
  {
    EventThreadServices& eventThreadServices = GetEventThreadServices();
    SceneGraph::UpdateManager& updateManager = eventThreadServices.GetUpdateManager();
    RemoveMessage( updateManager, updateManager.GetMaterialOwner(), *mSceneObject );

    eventThreadServices.UnregisterObject( this );
  }
}

} // namespace Internal
} // namespace Dali
