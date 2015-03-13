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
#include <dali/internal/event/effects/material-impl.h> // Dali::Internal::Material

// INTERNAL INCLUDES
#include <dali/public-api/shader-effects/material.h> // Dali::Internal::Material
#include <dali/internal/event/common/object-impl-helper.h> // Dali::Internal::ObjectHelper
#include <dali/internal/event/common/property-helper.h> // DALI_PROPERTY_TABLE_BEGIN, DALI_PROPERTY, DALI_PROPERTY_TABLE_END
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/update/effects/scene-graph-sampler.h>
#include <dali/internal/update/manager/update-manager.h>

namespace Dali
{
namespace Internal
{

namespace
{

/**
 *            |name                              |type     |writable|animatable|constraint-input|enum for index-checking|
 */
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY( "color",                           VECTOR4,  true, true,   true, Dali::Material::Property::COLOR )
DALI_PROPERTY( "face-culling-mode",               STRING,   true, false,  true, Dali::Material::Property::FACE_CULLING_MODE )
DALI_PROPERTY( "blending-mode",                   STRING,   true, false,  true, Dali::Material::Property::BLENDING_MODE )
DALI_PROPERTY( "blend-equation",                  STRING,   true, false,  true, Dali::Material::Property::BLEND_EQUATION )
DALI_PROPERTY( "source-blend-factor-rgb",         STRING,   true, false,  true, Dali::Material::Property::BLENDING_SRC_FACTOR_RGB )
DALI_PROPERTY( "destination-blend-factor-rgb",    STRING,   true, false,  true, Dali::Material::Property::BLENDING_DEST_FACTOR_RGB )
DALI_PROPERTY( "source-blend-factor-alpha",       STRING,   true, false,  true, Dali::Material::Property::BLENDING_SRC_FACTOR_ALPHA )
DALI_PROPERTY( "destination-blend-factor-alpha",  STRING,   true, false,  true, Dali::Material::Property::BLENDING_DEST_FACTOR_ALPHA )
DALI_PROPERTY( "blend-color",                     VECTOR4,  true, true,   true, Dali::Material::Property::BLEND_COLOR )
DALI_PROPERTY_TABLE_END( DEFAULT_ACTOR_PROPERTY_START_INDEX )

const ObjectImplHelper<DEFAULT_PROPERTY_COUNT> MATERIAL_IMPL = { DEFAULT_PROPERTY_DETAILS };

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
  mShaderConnector.Set( shader, OnStage() );

  StagePtr stage = Stage::GetCurrent();

  const SceneGraph::Shader& sceneGraphShader = dynamic_cast<const SceneGraph::Shader&>( *shader.GetSceneObject() );
  SceneGraph::SetShaderMessage( stage->GetUpdateInterface(), *mSceneObject, sceneGraphShader );
}

void Material::AddSampler( Sampler& sampler )
{
  SamplerConnector connector;
  connector.Set( sampler, OnStage() );
  mSamplerConnectors.push_back( connector );

  StagePtr stage = Stage::GetCurrent();

  const SceneGraph::Sampler& sceneGraphSampler = dynamic_cast<const SceneGraph::Sampler&>( *sampler.GetSceneObject() );
  SceneGraph::AddSamplerMessage( stage->GetUpdateInterface(), *mSceneObject, sceneGraphSampler );
}

std::size_t Material::GetNumberOfSamplers() const
{
  return mSamplerConnectors.size();
}

void Material::RemoveSampler( std::size_t index )
{
  mSamplerConnectors.erase( mSamplerConnectors.begin() + index );
}

void Material::SetFaceCullingMode( Dali::Material::FaceCullingMode cullingMode )
{
  // TODO: MESH_REWORK
  DALI_ASSERT_ALWAYS( false && "TODO: MESH_REWORK" );
}

void Material::SetBlendMode( BlendingMode::Type mode )
{
  // TODO: MESH_REWORK
  DALI_ASSERT_ALWAYS( false && "TODO: MESH_REWORK" );
}

BlendingMode::Type Material::GetBlendMode() const
{
  // TODO: MESH_REWORK
  DALI_ASSERT_ALWAYS( false && "TODO: MESH_REWORK" );
  return BlendingMode::AUTO;
}

void Material::SetBlendFunc( BlendingFactor::Type srcFactorRgba, BlendingFactor::Type destFactorRgba )
{
  // TODO: MESH_REWORK
  DALI_ASSERT_ALWAYS( false && "TODO: MESH_REWORK" );
}

void Material::SetBlendFunc( BlendingFactor::Type srcFactorRgb,
                             BlendingFactor::Type destFactorRgb,
                             BlendingFactor::Type srcFactorAlpha,
                             BlendingFactor::Type destFactorAlpha )
{
  // TODO: MESH_REWORK
  DALI_ASSERT_ALWAYS( false && "TODO: MESH_REWORK" );
}

void Material::GetBlendFunc( BlendingFactor::Type& srcFactorRgb,
                             BlendingFactor::Type& destFactorRgb,
                             BlendingFactor::Type& srcFactorAlpha,
                             BlendingFactor::Type& destFactorAlpha ) const
{
  // TODO: MESH_REWORK
  DALI_ASSERT_ALWAYS( false && "TODO: MESH_REWORK" );
}

void Material::SetBlendEquation( BlendingEquation::Type equationRgba )
{
  // TODO: MESH_REWORK
  DALI_ASSERT_ALWAYS( false && "TODO: MESH_REWORK" );
}

void Material::SetBlendEquation( BlendingEquation::Type equationRgb,
                                 BlendingEquation::Type equationAlpha )
{
  // TODO: MESH_REWORK
  DALI_ASSERT_ALWAYS( false && "TODO: MESH_REWORK" );
}

void Material::GetBlendEquation( BlendingEquation::Type& equationRgb,
                                 BlendingEquation::Type& equationAlpha ) const
{
  // TODO: MESH_REWORK
  DALI_ASSERT_ALWAYS( false && "TODO: MESH_REWORK" );
}

void Material::SetBlendColor( const Vector4& color )
{
  // TODO: MESH_REWORK
  DALI_ASSERT_ALWAYS( false && "TODO: MESH_REWORK" );
}

const Vector4& Material::GetBlendColor() const
{
  // TODO: MESH_REWORK
  DALI_ASSERT_ALWAYS( false && "TODO: MESH_REWORK" );
  return Color::WHITE;
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
  MATERIAL_IMPL.SetDefaultProperty( index, propertyValue );
}

void Material::SetSceneGraphProperty( Property::Index index,
                                      const CustomProperty& entry,
                                      const Property::Value& value )
{
  MATERIAL_IMPL.SetSceneGraphProperty( index, entry, value );
}

Property::Value Material::GetDefaultProperty( Property::Index index ) const
{
  return MATERIAL_IMPL.GetDefaultProperty( index );
}

const SceneGraph::PropertyOwner* Material::GetPropertyOwner() const
{
  return MATERIAL_IMPL.GetPropertyOwner();
}

const SceneGraph::PropertyOwner* Material::GetSceneObject() const
{
  return mSceneObject;
}

const SceneGraph::PropertyBase* Material::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  return MATERIAL_IMPL.GetSceneObjectAnimatableProperty( index );
}

const PropertyInputImpl* Material::GetSceneObjectInputProperty( Property::Index index ) const
{
  return MATERIAL_IMPL.GetSceneObjectInputProperty( index );
}

int Material::GetPropertyComponentIndex( Property::Index index ) const
{
  return MATERIAL_IMPL.GetPropertyComponentIndex( index );
}

bool Material::OnStage() const
{
  // TODO: MESH_REWORK
  //DALI_ASSERT_ALWAYS( false && "TODO: MESH_REWORK" );
  return mSceneObject != NULL;
}

void Material::Connect()
{
  SamplerConnectorContainer::const_iterator end = mSamplerConnectors.end();
  for( SamplerConnectorContainer::iterator it = mSamplerConnectors.begin();
       it < end;
       ++it )
  {
    it->OnStageConnect();
  }
}

void Material::Disconnect()
{
  SamplerConnectorContainer::const_iterator end = mSamplerConnectors.end();
  for( SamplerConnectorContainer::iterator it = mSamplerConnectors.begin();
       it < end;
       ++it )
  {
    it->OnStageDisconnect();
  }
}

Material::Material()
: mSceneObject( NULL )
{
}

void Material::Initialize()
{
  StagePtr stage = Stage::GetCurrent();
  DALI_ASSERT_ALWAYS( stage && "Stage doesn't exist" );

  mSceneObject = new SceneGraph::Material();
  AddMessage( stage->GetUpdateManager(), stage->GetUpdateManager().GetMaterialOwner(), *mSceneObject );
}

} // namespace Internal
} // namespace Dali
