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
#include <dali/internal/event/geometry/geometry-impl.h> // Dali::Internal::Geometry

// INTERNAL INCLUDES
#include <dali/internal/event/common/object-impl-helper.h> // Dali::Internal::ObjectHelper
#include <dali/internal/event/common/property-helper.h> // DALI_PROPERTY_TABLE_BEGIN, DALI_PROPERTY, DALI_PROPERTY_TABLE_END


namespace Dali
{
namespace Internal
{

namespace
{

/**
 *            |name                    |type     |writable|animatable|constraint-input|enum for index-checking|
 */
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY( "geometry-type",         STRING,   true, false,  true, Dali::Geometry::Property::GEOMETRY_TYPE )
DALI_PROPERTY( "geometry-center",       VECTOR3,  true, true,   true, Dali::Geometry::Property::GEOMETRY_CENTER )
DALI_PROPERTY( "geometry-half-extents", VECTOR3,  true, true,   true, Dali::Geometry::Property::GEOMETRY_HALF_EXTENTS )
DALI_PROPERTY( "requires-depth-test",   BOOLEAN,  true, false,  true, Dali::Geometry::Property::REQUIRES_DEPTH_TEST )
DALI_PROPERTY_TABLE_END( DEFAULT_ACTOR_PROPERTY_START_INDEX )

const ObjectImplHelper<DEFAULT_PROPERTY_COUNT> GEOMETRY_IMPL = { DEFAULT_PROPERTY_DETAILS };

} // unnamed namespace

GeometryPtr Geometry::New()
{
  return GeometryPtr( new Geometry() );
}

std::size_t Geometry::AddVertexBuffer( PropertyBuffer& vertexBuffer )
{
  PropertyBufferConnector connector;
  connector.Set( vertexBuffer, OnStage() );
  mVertexBufferConnectors.push_back( connector );
  return mVertexBufferConnectors.size() - 1u;
}

std::size_t Geometry::GetNumberOfVertexBuffers() const
{
  return mVertexBufferConnectors.size();
}

void Geometry::RemoveVertexBuffer( std::size_t index )
{
  mVertexBufferConnectors.erase( mVertexBufferConnectors.begin() + index );
}

void Geometry::SetIndexBuffer( PropertyBuffer& indexBuffer )
{
  mIndexBufferConnector.Set( indexBuffer, OnStage() );
}

void Geometry::SetGeometryType( Dali::Geometry::GeometryType geometryType )
{
  //TODO: MESH_REWORK
  DALI_ASSERT_ALWAYS( false && "MESH REWORK" );
}

Dali::Geometry::GeometryType Geometry::GetGeometryType() const
{
  //TODO: MESH_REWORK
  return Dali::Geometry::TRIANGLES;
}

void Geometry::SetRequiresDepthTesting( bool requiresDepthTest )
{
  //TODO: MESH_REWORK
  DALI_ASSERT_ALWAYS( false && "MESH REWORK" );
}

bool Geometry::GetRequiresDepthTesting() const
{
  //TODO: MESH_REWORK
  DALI_ASSERT_ALWAYS( false && "MESH REWORK" );
  return false;
}

unsigned int Geometry::GetDefaultPropertyCount() const
{
  return GEOMETRY_IMPL.GetDefaultPropertyCount();
}

void Geometry::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  GEOMETRY_IMPL.GetDefaultPropertyIndices( indices );
}

const char* Geometry::GetDefaultPropertyName(Property::Index index) const
{
  return GEOMETRY_IMPL.GetDefaultPropertyName( index );
}

Property::Index Geometry::GetDefaultPropertyIndex( const std::string& name ) const
{
  return GEOMETRY_IMPL.GetDefaultPropertyIndex( name );
}

bool Geometry::IsDefaultPropertyWritable( Property::Index index ) const
{
  return GEOMETRY_IMPL.IsDefaultPropertyWritable( index );
}

bool Geometry::IsDefaultPropertyAnimatable( Property::Index index ) const
{
  return GEOMETRY_IMPL.IsDefaultPropertyAnimatable( index );
}

bool Geometry::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  return GEOMETRY_IMPL.IsDefaultPropertyAConstraintInput( index );
}

Property::Type Geometry::GetDefaultPropertyType( Property::Index index ) const
{
  return GEOMETRY_IMPL.GetDefaultPropertyType( index );
}

void Geometry::SetDefaultProperty( Property::Index index,
                                   const Property::Value& propertyValue )
{
  GEOMETRY_IMPL.SetDefaultProperty( index, propertyValue );
}

void Geometry::SetSceneGraphProperty( Property::Index index,
                                      const CustomProperty& entry,
                                      const Property::Value& value )
{
  GEOMETRY_IMPL.SetSceneGraphProperty( index, entry, value );
}

Property::Value Geometry::GetDefaultProperty( Property::Index index ) const
{
  return GEOMETRY_IMPL.GetDefaultProperty( index );
}

const SceneGraph::PropertyOwner* Geometry::GetPropertyOwner() const
{
  return GEOMETRY_IMPL.GetPropertyOwner();
}

const SceneGraph::PropertyOwner* Geometry::GetSceneObject() const
{
  return GEOMETRY_IMPL.GetSceneObject();
}

const SceneGraph::PropertyBase* Geometry::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  return GEOMETRY_IMPL.GetSceneObjectAnimatableProperty( index );
}

const PropertyInputImpl* Geometry::GetSceneObjectInputProperty( Property::Index index ) const
{
  return GEOMETRY_IMPL.GetSceneObjectInputProperty( index );
}

int Geometry::GetPropertyComponentIndex( Property::Index index ) const
{
  return GEOMETRY_IMPL.GetPropertyComponentIndex( index );
}

bool Geometry::OnStage() const
{
  // TODO: MESH_REWORK
  DALI_ASSERT_ALWAYS( false && "TODO: MESH_REWORK" );
  return false;
}

void Geometry::Connect()
{
  PropertyBufferConnectorContainer::const_iterator end = mVertexBufferConnectors.end();
  for( PropertyBufferConnectorContainer::iterator it = mVertexBufferConnectors.begin();
       it < end;
       ++it )
  {
    it->OnStageConnect();
  }
  mIndexBufferConnector.OnStageConnect();
}

void Geometry::Disconnect()
{
  PropertyBufferConnectorContainer::const_iterator end = mVertexBufferConnectors.end();
  for( PropertyBufferConnectorContainer::iterator it = mVertexBufferConnectors.begin();
       it < end;
       ++it )
  {
    it->OnStageDisconnect();
  }
  mIndexBufferConnector.OnStageDisconnect();
}

Geometry::Geometry()
{
}

} // namespace Internal
} // namespace Dali

