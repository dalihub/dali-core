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
#include <dali/internal/event/rendering/geometry-impl.h> // Dali::Internal::Geometry

// INTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>

#include <dali/internal/event/common/object-impl-helper.h> // Dali::Internal::ObjectHelper
#include <dali/internal/event/common/property-helper.h> // DALI_PROPERTY_TABLE_BEGIN, DALI_PROPERTY, DALI_PROPERTY_TABLE_END
#include <dali/internal/update/common/double-buffered-property.h>
#include <dali/internal/update/manager/update-manager.h>

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
DALI_PROPERTY( "requires-depth-test",   BOOLEAN,  true, false,  true, Dali::Geometry::Property::REQUIRES_DEPTH_TEST )
DALI_PROPERTY_TABLE_END( DEFAULT_ACTOR_PROPERTY_START_INDEX )

const ObjectImplHelper<DEFAULT_PROPERTY_COUNT> GEOMETRY_IMPL = { DEFAULT_PROPERTY_DETAILS };

BaseHandle Create()
{
  return Dali::Geometry::New();
}

TypeRegistration mType( typeid( Dali::Geometry ), typeid( Dali::Handle ), Create );

} // unnamed namespace

GeometryPtr Geometry::New()
{
  GeometryPtr geometry( new Geometry() );
  geometry->Initialize();
  return geometry;
}

std::size_t Geometry::AddVertexBuffer( PropertyBuffer& vertexBuffer )
{
  mVertexBuffers.push_back( &vertexBuffer );
  SceneGraph::AddVertexBufferMessage( GetEventThreadServices(), *mSceneObject, *vertexBuffer.GetRenderObject() );
  return mVertexBuffers.size() - 1u;
}

std::size_t Geometry::GetNumberOfVertexBuffers() const
{
  return mVertexBuffers.size();
}

void Geometry::RemoveVertexBuffer( std::size_t index )
{
  const Render::PropertyBuffer& renderPropertyBuffer = static_cast<const Render::PropertyBuffer&>( *(mVertexBuffers[index]->GetRenderObject()) );
  SceneGraph::RemoveVertexBufferMessage( GetEventThreadServices(), *mSceneObject, renderPropertyBuffer );

  mVertexBuffers.erase( mVertexBuffers.begin() + index );
}

void Geometry::SetIndexBuffer( PropertyBuffer& indexBuffer )
{
  mIndexBuffer = &indexBuffer;
  SceneGraph::SetIndexBufferMessage( GetEventThreadServices(), *mSceneObject, *indexBuffer.GetRenderObject() );
}

void Geometry::SetGeometryType( Dali::Geometry::GeometryType geometryType )
{
  if( NULL != mSceneObject )
  {
    SceneGraph::DoubleBufferedPropertyMessage<int>::Send(
      GetEventThreadServices(),
      mSceneObject,
      &mSceneObject->mGeometryType,
      &SceneGraph::DoubleBufferedProperty<int>::Set,
      static_cast<int>(geometryType) );
  }
}

Dali::Geometry::GeometryType Geometry::GetGeometryType() const
{
  return mSceneObject->GetGeometryType(GetEventThreadServices().GetEventBufferIndex());
}

void Geometry::SetRequiresDepthTesting( bool requiresDepthTest )
{
  if( NULL != mSceneObject )
  {
    SceneGraph::DoubleBufferedPropertyMessage<bool>::Send( GetEventThreadServices(), mSceneObject, &mSceneObject->mRequiresDepthTest, &SceneGraph::DoubleBufferedProperty<bool>::Set, static_cast<int>(requiresDepthTest) );
  }
}

bool Geometry::GetRequiresDepthTesting() const
{
  if( mSceneObject )
  {
    // mSceneObject is being used in a separate thread; copy the value from the previous update
    return mSceneObject->GetRequiresDepthTesting(GetEventThreadServices().GetEventBufferIndex());
  }
  return false;
}

const SceneGraph::Geometry* Geometry::GetGeometrySceneObject() const
{
  return mSceneObject;
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
  switch( index )
  {
    case Dali::Geometry::Property::GEOMETRY_TYPE :
    {
      SceneGraph::DoubleBufferedPropertyMessage<int>::Send( GetEventThreadServices(), mSceneObject, &mSceneObject->mGeometryType, &SceneGraph::DoubleBufferedProperty<int>::Set, propertyValue.Get<int>() );
      break;
    }
    case Dali::Geometry::Property::GEOMETRY_CENTER :
    {
      SceneGraph::AnimatablePropertyMessage<Vector3>::Send( GetEventThreadServices(), mSceneObject, &mSceneObject->mCenter, &SceneGraph::AnimatableProperty<Vector3>::Bake, propertyValue.Get<Vector3>() );
      break;
    }
    case Dali::Geometry::Property::REQUIRES_DEPTH_TEST :
    {
      SceneGraph::DoubleBufferedPropertyMessage<bool>::Send( GetEventThreadServices(), mSceneObject, &mSceneObject->mRequiresDepthTest, &SceneGraph::DoubleBufferedProperty<bool>::Set, propertyValue.Get<bool>() );
      break;
    }
  }
}

void Geometry::SetSceneGraphProperty( Property::Index index,
                                      const PropertyMetadata& entry,
                                      const Property::Value& value )
{
  GEOMETRY_IMPL.SetSceneGraphProperty( GetEventThreadServices(), this, index, entry, value );
}

Property::Value Geometry::GetDefaultProperty( Property::Index index ) const
{
  BufferIndex bufferIndex = GetEventThreadServices().GetEventBufferIndex();
  Property::Value value;

  switch( index )
  {
    case Dali::Geometry::Property::GEOMETRY_TYPE :
    {
      if( mSceneObject )
      {
        value = mSceneObject->mGeometryType[bufferIndex];
      }
      break;
    }
    case Dali::Geometry::Property::GEOMETRY_CENTER :
    {
      if( mSceneObject )
      {
        value = mSceneObject->mCenter[bufferIndex];
      }
      break;
    }

    case Dali::Geometry::Property::REQUIRES_DEPTH_TEST :
    {
      if( mSceneObject )
      {
        value = mSceneObject->mRequiresDepthTest[bufferIndex];
      }
      break;
    }
  }

  return value;
}

const SceneGraph::PropertyOwner* Geometry::GetPropertyOwner() const
{
  return mSceneObject;
}

const SceneGraph::PropertyOwner* Geometry::GetSceneObject() const
{
  return mSceneObject;
}

const SceneGraph::PropertyBase* Geometry::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  const SceneGraph::PropertyBase* property = NULL;

  if( OnStage() )
  {
    property = GEOMETRY_IMPL.GetRegisteredSceneGraphProperty ( this,
                                                               &Geometry::FindAnimatableProperty,
                                                               &Geometry::FindCustomProperty,
                                                               index );

    if( property == NULL && index < DEFAULT_PROPERTY_MAX_COUNT )
    {
      switch(index)
      {
        case Dali::Geometry::Property::GEOMETRY_CENTER :
        {
          property = &mSceneObject->mCenter;
          break;
        }
        default:
        {
          DALI_ASSERT_ALWAYS( 0 && "Property is not animatable" );
          break;
        }
      }
    }
  }
  return property;
}

const PropertyInputImpl* Geometry::GetSceneObjectInputProperty( Property::Index index ) const
{
  const PropertyInputImpl* property = NULL;

  if( OnStage() )
  {
    const SceneGraph::PropertyBase* baseProperty =
      GEOMETRY_IMPL.GetRegisteredSceneGraphProperty ( this,
                                                      &Geometry::FindAnimatableProperty,
                                                      &Geometry::FindCustomProperty,
                                                      index );

    property = static_cast<const PropertyInputImpl*>( baseProperty );

    if( property == NULL && index < DEFAULT_PROPERTY_MAX_COUNT )
    {
      switch(index)
      {
        case Dali::Geometry::Property::GEOMETRY_TYPE :
        {
          property = &mSceneObject->mGeometryType;
          break;
        }
        case Dali::Geometry::Property::GEOMETRY_CENTER :
        {
          property = &mSceneObject->mCenter;
          break;
        }
        case Dali::Geometry::Property::REQUIRES_DEPTH_TEST :
        {
          property = &mSceneObject->mRequiresDepthTest;
          break;
        }
        default:
        {
          DALI_ASSERT_ALWAYS( 0 && "Property cannot be a constraint input");
          break;
        }
      }
    }
  }

  return property;
}

int Geometry::GetPropertyComponentIndex( Property::Index index ) const
{
  // @todo MESH_REWORK - Change this if component properties are added for center/half-extent
  return Property::INVALID_COMPONENT_INDEX;
}

bool Geometry::OnStage() const
{
  return mOnStage;
}

void Geometry::Connect()
{
  mOnStage = true;
}

void Geometry::Disconnect()
{
  mOnStage = false;
}

Geometry::Geometry()
: mIndexBuffer( NULL ),
  mSceneObject( NULL ),
  mOnStage( false )
{
}

void Geometry::Initialize()
{
  EventThreadServices& eventThreadServices = GetEventThreadServices();
  SceneGraph::UpdateManager& updateManager = eventThreadServices.GetUpdateManager();

  mSceneObject = new SceneGraph::Geometry();
  AddMessage( updateManager, updateManager.GetGeometryOwner(), *mSceneObject );

  eventThreadServices.RegisterObject( this );
}

Geometry::~Geometry()
{
  if( EventThreadServices::IsCoreRunning() )
  {
    EventThreadServices& eventThreadServices = GetEventThreadServices();
    SceneGraph::UpdateManager& updateManager = eventThreadServices.GetUpdateManager();
    RemoveMessage( updateManager, updateManager.GetGeometryOwner(), *mSceneObject );

    eventThreadServices.UnregisterObject( this );
  }
}


} // namespace Internal
} // namespace Dali
