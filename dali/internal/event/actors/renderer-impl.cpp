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
#include <dali/internal/event/actors/renderer-impl.h> // Dali::Internal::Renderer

// INTERNAL INCLUDES
#include <dali/public-api/actors/renderer.h> // Dali::Renderer
#include <dali/internal/event/common/object-impl-helper.h> // Dali::Internal::ObjectHelper
#include <dali/internal/event/common/property-helper.h> // DALI_PROPERTY_TABLE_BEGIN, DALI_PROPERTY, DALI_PROPERTY_TABLE_END
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/update/node-attachments/scene-graph-renderer-attachment.h>

namespace Dali
{
namespace Internal
{

namespace
{

/**
 *            |name          |type     |writable|animatable|constraint-input|enum for index-checking|
 */
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY( "depth-index", INTEGER,  true, true, true, Dali::Renderer::Property::DEPTH_INDEX )
DALI_PROPERTY_TABLE_END( DEFAULT_DERIVED_HANDLE_PROPERTY_START_INDEX )

const ObjectImplHelper<DEFAULT_PROPERTY_COUNT> RENDERER_IMPL = { DEFAULT_PROPERTY_DETAILS };

} // unnamed namespace

RendererPtr Renderer::New()
{
  RendererPtr rendererPtr( new Renderer() );
  rendererPtr->Initialize();
  return rendererPtr;
}

void Renderer::SetGeometry( Geometry& geometry )
{
  mGeometryConnector.Set( geometry, OnStage() );
  const SceneGraph::Geometry* geometrySceneObject = geometry.GetGeometrySceneObject();
  SetGeometryMessage( Stage::GetCurrent()->GetUpdateInterface(), *mSceneObject, *geometrySceneObject );
}

void Renderer::SetMaterial( Material& material )
{
  mMaterialConnector.Set( material, OnStage() );
  const SceneGraph::Material* materialSceneObject = material.GetMaterialSceneObject();
  SetMaterialMessage( Stage::GetCurrent()->GetUpdateInterface(), *mSceneObject, *materialSceneObject );
}

void Renderer::SetDepthIndex( int depthIndex )
{
  //SceneGraph::NodePropertyMessage<Vector3>::Send( mStage->GetUpdateManager(), mNode, &mNode->mPosition, &AnimatableProperty<Vector3>::Bake, position );
  // TODO: MESH_REWORK
  DALI_ASSERT_ALWAYS( false && "TODO: MESH_REWORK" );
}

SceneGraph::RendererAttachment* Renderer::GetRendererSceneObject()
{
  return mSceneObject;
}

unsigned int Renderer::GetDefaultPropertyCount() const
{
  return RENDERER_IMPL.GetDefaultPropertyCount();
}

void Renderer::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  RENDERER_IMPL.GetDefaultPropertyIndices( indices );
}

const char* Renderer::GetDefaultPropertyName(Property::Index index) const
{
  return RENDERER_IMPL.GetDefaultPropertyName( index );
}

Property::Index Renderer::GetDefaultPropertyIndex( const std::string& name ) const
{
  return RENDERER_IMPL.GetDefaultPropertyIndex( name );
}

bool Renderer::IsDefaultPropertyWritable( Property::Index index ) const
{
  return RENDERER_IMPL.IsDefaultPropertyWritable( index );
}

bool Renderer::IsDefaultPropertyAnimatable( Property::Index index ) const
{
  return RENDERER_IMPL.IsDefaultPropertyAnimatable( index );
}

bool Renderer::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  return RENDERER_IMPL.IsDefaultPropertyAConstraintInput( index );
}

Property::Type Renderer::GetDefaultPropertyType( Property::Index index ) const
{
  return RENDERER_IMPL.GetDefaultPropertyType( index );
}

void Renderer::SetDefaultProperty( Property::Index index,
                                   const Property::Value& propertyValue )
{
  RENDERER_IMPL.SetDefaultProperty( index, propertyValue );
}

void Renderer::SetSceneGraphProperty( Property::Index index,
                                      const CustomProperty& entry,
                                      const Property::Value& value )
{
  RENDERER_IMPL.SetSceneGraphProperty( index, entry, value );
}

Property::Value Renderer::GetDefaultProperty( Property::Index index ) const
{
  return RENDERER_IMPL.GetDefaultProperty( index );
}

const SceneGraph::PropertyOwner* Renderer::GetPropertyOwner() const
{
  return RENDERER_IMPL.GetPropertyOwner();
}

const SceneGraph::PropertyOwner* Renderer::GetSceneObject() const
{
  return mSceneObject;
}

const SceneGraph::PropertyBase* Renderer::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  return RENDERER_IMPL.GetSceneObjectAnimatableProperty( index );
}

const PropertyInputImpl* Renderer::GetSceneObjectInputProperty( Property::Index index ) const
{
  return RENDERER_IMPL.GetSceneObjectInputProperty( index );
}

int Renderer::GetPropertyComponentIndex( Property::Index index ) const
{
  return RENDERER_IMPL.GetPropertyComponentIndex( index );
}

bool Renderer::OnStage() const
{
  // TODO: MESH_REWORK
  //DALI_ASSERT_ALWAYS( false && "TODO: MESH_REWORK" );
  return mOnStage;
}

void Renderer::Connect()
{
  // TODO: MESH_REWORK : check this
  mGeometryConnector.OnStageConnect();
  mMaterialConnector.OnStageConnect();
  mOnStage = true;
}

void Renderer::Disconnect()
{
  // TODO: MESH_REWORK : check this
  mGeometryConnector.OnStageDisconnect();
  mMaterialConnector.OnStageDisconnect();
  mOnStage = false;
}

Renderer::Renderer()
: mSceneObject(NULL),
  mOnStage(false)
{
}

void Renderer::Initialize()
{
  // Transfer object ownership of scene-object to message
  mSceneObject = SceneGraph::RendererAttachment::New();

  // Send message to update to connect to scene graph:
  AttachToSceneGraphMessage( Stage::GetCurrent()->GetUpdateManager(), mSceneObject );
}


} // namespace Internal
} // namespace Dali
