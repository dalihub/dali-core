/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/actors/camera-actor-impl.h>

// EXTERNAL INCLUDES
#include <cmath>
#include <cstring> // for strcmp

// INTERNAL INCLUDES
#include <dali/public-api/common/stage.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/internal/event/actor-attachments/camera-attachment-impl.h>
#include <dali/internal/event/common/property-helper.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>
#include <dali/internal/event/render-tasks/render-task-list-impl.h>
#include <dali/internal/event/common/projection.h>
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Internal
{

namespace
{

// Properties

/**
 * We want to discourage the use of property strings (minimize string comparisons),
 * particularly for the default properties.
 *              Name                     Type   writable animatable constraint-input  enum for index-checking
 */
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY( "type",                   STRING,   true,    false,   true,   Dali::CameraActor::Property::TYPE                  )
DALI_PROPERTY( "projection-mode",        STRING,   true,    false,   true,   Dali::CameraActor::Property::PROJECTION_MODE       )
DALI_PROPERTY( "field-of-view",          FLOAT,    true,    false,   true,   Dali::CameraActor::Property::FIELD_OF_VIEW         )
DALI_PROPERTY( "aspect-ratio",           FLOAT,    true,    false,   true,   Dali::CameraActor::Property::ASPECT_RATIO          )
DALI_PROPERTY( "near-plane-distance",    FLOAT,    true,    false,   true,   Dali::CameraActor::Property::NEAR_PLANE_DISTANCE   )
DALI_PROPERTY( "far-plane-distance",     FLOAT,    true,    false,   true,   Dali::CameraActor::Property::FAR_PLANE_DISTANCE    )
DALI_PROPERTY( "left-plane-distance",    FLOAT,    true,    false,   true,   Dali::CameraActor::Property::LEFT_PLANE_DISTANCE   )
DALI_PROPERTY( "right-plane-distance",   FLOAT,    true,    false,   true,   Dali::CameraActor::Property::RIGHT_PLANE_DISTANCE  )
DALI_PROPERTY( "top-plane-distance",     FLOAT,    true,    false,   true,   Dali::CameraActor::Property::TOP_PLANE_DISTANCE    )
DALI_PROPERTY( "bottom-plane-distance",  FLOAT,    true,    false,   true,   Dali::CameraActor::Property::BOTTOM_PLANE_DISTANCE )
DALI_PROPERTY( "target-position",        VECTOR3,  true,    false,   true,   Dali::CameraActor::Property::TARGET_POSITION       )
DALI_PROPERTY( "projection-matrix",      MATRIX,   false,   false,   true,   Dali::CameraActor::Property::PROJECTION_MATRIX     )
DALI_PROPERTY( "view-matrix",            MATRIX,   false,   false,   true,   Dali::CameraActor::Property::VIEW_MATRIX           )
DALI_PROPERTY( "invert-y-axis",          BOOLEAN,  true,    false,   true,   Dali::CameraActor::Property::INVERT_Y_AXIS         )
DALI_PROPERTY_TABLE_END( DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX )

// calculate the far plane distance for a 16bit depth buffer with 4 bits per unit precision
void CalculateClippingAndZ( float width, float height, float& nearClippingPlane, float& farClippingPlane, float& cameraZ )
{
  nearClippingPlane = std::max( width, height );
  farClippingPlane = nearClippingPlane + static_cast<float>( 0xFFFF >> 4 );
  cameraZ = 2.0f * nearClippingPlane;
}

BaseHandle Create()
{
  return Dali::CameraActor::New();
}

TypeRegistration mType( typeid( Dali::CameraActor ), typeid( Dali::Actor ), Create );

/**
 * Builds the picking ray in the world reference system from an orthographic camera
 * The ray origin is the screen coordinate in the near plane translated to a parallel
 * plane at the camera origin. The ray direction is the direction the camera is facing
 * (i.e. Z=-1 in view space).
 */
void BuildOrthoPickingRay( const Matrix& viewMatrix,
                           const Matrix& projectionMatrix,
                           const Viewport& viewport,
                           float screenX,
                           float screenY,
                           Vector4& rayOrigin,
                           Vector4& rayDir,
                           float nearPlaneDistance )
{
  //          inv( modelMatrix )          inv( viewMatrix )    inv( projectionMatrix )           normalize
  //          <-----------------         <-----------------         <--------------           <-------------
  //  Local                      World                      Camera                 Normalized                 Screen
  // reference                  reference                  reference                  clip                  coordinates
  //  system                     system                     system                 coordinates
  //          ----------------->         ----------------->         -------------->           ------------->
  //             modelMatrix                 viewMatrix             projectionMatrix             viewport

  // Transforms the touch point from the screen reference system to the world reference system.
  Matrix invViewProjection( false ); // Don't initialize.
  Matrix::Multiply( invViewProjection, viewMatrix, projectionMatrix );
  if( !invViewProjection.Invert() )
  {
    DALI_ASSERT_DEBUG( false );
  }

  Vector4 near( screenX - viewport.x, viewport.height - (screenY - viewport.y), 0.f, 1.f );
  if( !Unproject( near, invViewProjection, viewport.width, viewport.height, rayOrigin ) )
  {
    DALI_ASSERT_DEBUG( false );
  }

  Matrix invView = viewMatrix;
  if( !invView.Invert() )
  {
    DALI_ASSERT_DEBUG( false );
  }

  Vector4 cameraOrigin = invView * Vector4( 0.f, 0.f, 0.f, 1.f );
  Vector4 nearPlaneOrigin = invView * Vector4( 0.0f, 0.0f, -nearPlaneDistance, 1.0f);

  // Vector pointing from the camera to the near plane
  rayDir = cameraOrigin - nearPlaneOrigin;
  rayOrigin -= rayDir;
  rayDir.Normalize();
  rayDir.w = 1.0f;
}

} // namespace

CameraActorPtr CameraActor::New( const Size& size )
{
  CameraActorPtr actor(new CameraActor());

  // Second-phase construction

  actor->Initialize();

  actor->SetName("DefaultCamera");

  // Create the attachment
  actor->mCameraAttachment = CameraAttachment::New( actor->GetEventThreadServices(), *actor->mNode );

  actor->Attach(*actor->mCameraAttachment);

  actor->SetPerspectiveProjection( size );

  // By default Actors face in the positive Z direction in world space
  // CameraActors should face in the negative Z direction, towards the other actors
  actor->SetOrientation( Quaternion( Dali::ANGLE_180, Vector3::YAXIS ) );

  return actor;
}

void CameraActor::OnInitialize()
{
}

CameraActor::CameraActor()
: Actor( Actor::BASIC )
{
}

CameraActor::~CameraActor()
{
}

void CameraActor::SetType( Dali::Camera::Type type )
{
  mCameraAttachment->SetType(type);
}

Dali::Camera::Type CameraActor::GetType() const
{
  return mCameraAttachment->GetType();
}

void CameraActor::SetProjectionMode( Dali::Camera::ProjectionMode mode )
{
  mCameraAttachment->SetProjectionMode(mode);
}

Dali::Camera::ProjectionMode CameraActor::GetProjectionMode() const
{
  return mCameraAttachment->GetProjectionMode();
}

void CameraActor::SetFieldOfView( float fieldOfView )
{
  mCameraAttachment->SetFieldOfView(fieldOfView);
}

float CameraActor::GetFieldOfView( ) const
{
  return mCameraAttachment->GetFieldOfView();
}

void CameraActor::SetAspectRatio( float aspectRatio )
{
  mCameraAttachment->SetAspectRatio(aspectRatio);
}

float CameraActor::GetAspectRatio( ) const
{
  return mCameraAttachment->GetAspectRatio();
}

void CameraActor::SetNearClippingPlane( float nearClippingPlane )
{
  mCameraAttachment->SetNearClippingPlane(nearClippingPlane);
}

float CameraActor::GetNearClippingPlane( ) const
{
  return mCameraAttachment->GetNearClippingPlane();
}

void CameraActor::SetFarClippingPlane( float farClippingPlane )
{
  mCameraAttachment->SetFarClippingPlane(farClippingPlane);
}

float CameraActor::GetFarClippingPlane( ) const
{
  return mCameraAttachment->GetFarClippingPlane();
}

void CameraActor::SetTargetPosition(const Vector3& target)
{
  mCameraAttachment->SetTargetPosition(target);
}

Vector3 CameraActor::GetTargetPosition() const
{
  return mCameraAttachment->GetTargetPosition();
}

void CameraActor::SetInvertYAxis(bool invertYAxis)
{
  mCameraAttachment->SetInvertYAxis(invertYAxis);
}

bool CameraActor::GetInvertYAxis() const
{
  return mCameraAttachment->GetInvertYAxis();
}

void CameraActor::SetPerspectiveProjection( const Size& size, const Vector2& stereoBias /* = Vector2::ZERO */ )
{
  float width = size.width;
  float height = size.height;

  if( Size::ZERO == size )
  {
    StagePtr stage = Stage::GetCurrent();
    if( stage )
    {
      const Size& stageSize = stage->GetSize();

      width = stageSize.width;
      height = stageSize.height;
    }
  }

  if( ( width < Math::MACHINE_EPSILON_1000 ) || ( height < Math::MACHINE_EPSILON_1000 ) )
  {
    // On the stage initialization this method is called but the size has not been set.
    // There is no point to set any value if width or height is zero.
    return;
  }

  float nearClippingPlane;
  float farClippingPlane;
  float cameraZ;
  CalculateClippingAndZ( width, height, nearClippingPlane, farClippingPlane, cameraZ );

  // calculate the position of the camera to have the desired aspect ratio
  const float fieldOfView = 2.0f * std::atan( height * 0.5f / cameraZ );

  // unless it is too small, we want at least as much space to the back as we have torwards the front
  const float minClippingFarPlane = 2.f * nearClippingPlane;
  if ( farClippingPlane < minClippingFarPlane )
  {
    farClippingPlane = minClippingFarPlane;
  }

  const float aspectRatio = width / height;

  SetProjectionMode(Dali::Camera::PERSPECTIVE_PROJECTION);
  SetFieldOfView( fieldOfView );
  SetNearClippingPlane( nearClippingPlane );
  SetFarClippingPlane( farClippingPlane );
  SetAspectRatio( aspectRatio );
  mCameraAttachment->SetStereoBias( stereoBias );
  SetZ( cameraZ );
}


void CameraActor::SetOrthographicProjection( const Vector2& size )
{
  // Choose near, far and Z parameters to match the SetPerspectiveProjection above.
  float nearClippingPlane;
  float farClippingPlane;
  float cameraZ;
  CalculateClippingAndZ( size.width, size.height, nearClippingPlane, farClippingPlane, cameraZ );
  SetOrthographicProjection( -size.x*0.5f, size.x*0.5f, size.y*0.5f, -size.y*0.5f,
                             nearClippingPlane, farClippingPlane );
  SetZ( cameraZ );
}

void CameraActor::SetOrthographicProjection( float left, float right, float top, float bottom, float near, float far )
{
  mCameraAttachment->SetLeftClippingPlane(left);
  mCameraAttachment->SetRightClippingPlane(right);
  mCameraAttachment->SetTopClippingPlane(top);
  mCameraAttachment->SetBottomClippingPlane(bottom);
  SetNearClippingPlane( near );
  SetFarClippingPlane( far );
  SetProjectionMode(Dali::Camera::ORTHOGRAPHIC_PROJECTION);
}

bool CameraActor::BuildPickingRay( const Vector2& screenCoordinates,
                                   const Viewport& viewport,
                                   Vector4& rayOrigin,
                                   Vector4& rayDirection )
{
  bool success = true;
  if( GetProjectionMode() == Dali::Camera::PERSPECTIVE_PROJECTION )
  {
    // Build a picking ray in the world reference system.
    // ray starts from the camera world position
    rayOrigin = mNode->GetWorldPosition( GetEventThreadServices().GetEventBufferIndex() );
    rayOrigin.w = 1.0f;

    // Transform the touch point from the screen coordinate system to the world coordinates system.
    Vector4 near( screenCoordinates.x - viewport.x, viewport.height - (screenCoordinates.y - viewport.y), 0.f, 1.f );
    if( !Unproject( near, mCameraAttachment->GetInverseViewProjectionMatrix(), viewport.width, viewport.height, near ) )
    {
      // unproject failed so no picking ray possible
      success = false;
    }

    // Compute the ray's director vector.
    rayDirection.x = near.x - rayOrigin.x;
    rayDirection.y = near.y - rayOrigin.y;
    rayDirection.z = near.z - rayOrigin.z;
    rayDirection.Normalize();
    rayDirection.w = 1.f;
  }
  else
  {
    float nearPlaneDistance = GetNearClippingPlane();
    BuildOrthoPickingRay( GetViewMatrix(),
                          GetProjectionMatrix(),
                          viewport, screenCoordinates.x,
                          screenCoordinates.y,
                          rayOrigin,
                          rayDirection,
                          nearPlaneDistance );
  }

  return success;
}

const Matrix& CameraActor::GetViewMatrix() const
{
  if ( OnStage() )
  {
    return mCameraAttachment->GetViewMatrix();
  }
  else
  {
    return Matrix::IDENTITY;
  }
}

const Matrix& CameraActor::GetProjectionMatrix() const
{
  if ( OnStage() )
  {
    return mCameraAttachment->GetProjectionMatrix();
  }
  else
  {
    return Matrix::IDENTITY;
  }
}

unsigned int CameraActor::GetDefaultPropertyCount() const
{
  return Actor::GetDefaultPropertyCount() + DEFAULT_PROPERTY_COUNT;
}

void CameraActor::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  Actor::GetDefaultPropertyIndices( indices ); // Actor class properties

  indices.Reserve( indices.Size() + DEFAULT_PROPERTY_COUNT );

  int index = DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX;
  for ( int i = 0; i < DEFAULT_PROPERTY_COUNT; ++i, ++index )
  {
    indices.PushBack( index );
  }
}

bool CameraActor::IsDefaultPropertyWritable( Property::Index index ) const
{
  if( index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT )
  {
    return Actor::IsDefaultPropertyWritable( index );
  }

  return DEFAULT_PROPERTY_DETAILS[index - DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX].writable;
}

bool CameraActor::IsDefaultPropertyAnimatable( Property::Index index ) const
{
  if( index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT )
  {
    return Actor::IsDefaultPropertyAnimatable( index );
  }

  return DEFAULT_PROPERTY_DETAILS[index - DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX].animatable;
}

bool CameraActor::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  if( index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT )
  {
    return Actor::IsDefaultPropertyAConstraintInput( index );
  }

  return DEFAULT_PROPERTY_DETAILS[index - DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX].constraintInput;
}

Property::Type CameraActor::GetDefaultPropertyType( Property::Index index ) const
{
  if( index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT )
  {
    return Actor::GetDefaultPropertyType( index );
  }
  else
  {
    index -= DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX;

    if ( ( index >= 0 ) && ( index < DEFAULT_PROPERTY_COUNT ) )
    {
      return DEFAULT_PROPERTY_DETAILS[index].type;
    }
    else
    {
      // index out-of-bounds
      return Property::NONE;
    }
  }
}

const char* CameraActor::GetDefaultPropertyName( Property::Index index ) const
{
  if(index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT)
  {
    return Actor::GetDefaultPropertyName(index);
  }
  else
  {
    index -= DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX;

    if ( ( index >= 0 ) && ( index < DEFAULT_PROPERTY_COUNT ) )
    {
      return DEFAULT_PROPERTY_DETAILS[index].name;
    }
    return NULL;
  }
}

Property::Index CameraActor::GetDefaultPropertyIndex(const std::string& name) const
{
  Property::Index index = Property::INVALID_INDEX;

  // Look for name in current class' default properties
  for( int i = 0; i < DEFAULT_PROPERTY_COUNT; ++i )
  {
    if( 0 == strcmp( name.c_str(), DEFAULT_PROPERTY_DETAILS[i].name ) ) // dont want to convert rhs to string
    {
      index = i + DEFAULT_DERIVED_ACTOR_PROPERTY_START_INDEX;
      break;
    }
  }

  // If not found, check in base class
  if( Property::INVALID_INDEX == index )
  {
    index = Actor::GetDefaultPropertyIndex( name );
  }

  return index;
}

void CameraActor::SetDefaultProperty( Property::Index index, const Property::Value& propertyValue )
{
  if(index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT)
  {
    Actor::SetDefaultProperty(index, propertyValue);
  }
  else
  {
    DALI_ASSERT_DEBUG(mCameraAttachment && "where is the camera?");
    switch(index)
    {
      case Dali::CameraActor::Property::TYPE:
      {
        std::string s( propertyValue.Get<std::string>() );
        if(s == "LOOK_AT_TARGET")
        {
          mCameraAttachment->SetType(Dali::Camera::LOOK_AT_TARGET);
        }
        else if(s == "FREE_LOOK")
        {
          mCameraAttachment->SetType(Dali::Camera::FREE_LOOK);
        }
        else
        {
          DALI_LOG_WARNING("Unknown camera type\n");
        }
        break;
      }
      case Dali::CameraActor::Property::PROJECTION_MODE:
      {
        std::string s(propertyValue.Get<std::string>());
        if(s == "PERSPECTIVE_PROJECTION")
        {
          mCameraAttachment->SetProjectionMode(Dali::Camera::PERSPECTIVE_PROJECTION);
        }
        else if(s == "ORTHOGRAPHIC_PROJECTION")
        {
          mCameraAttachment->SetProjectionMode(Dali::Camera::ORTHOGRAPHIC_PROJECTION);
        }
        else
        {
          DALI_LOG_WARNING("Unknown projection mode\n");
        }
        break;
      }
      case Dali::CameraActor::Property::FIELD_OF_VIEW:
      {
        mCameraAttachment->SetFieldOfView(propertyValue.Get<float>());
        break;
      }
      case Dali::CameraActor::Property::ASPECT_RATIO:
      {
        mCameraAttachment->SetAspectRatio(propertyValue.Get<float>());
        break;
      }
      case Dali::CameraActor::Property::LEFT_PLANE_DISTANCE:
      {
        mCameraAttachment->SetLeftClippingPlane(propertyValue.Get<float>());
        break;
      }
      case Dali::CameraActor::Property::RIGHT_PLANE_DISTANCE:
      {
        mCameraAttachment->SetRightClippingPlane(propertyValue.Get<float>());
        break;
      }
      case Dali::CameraActor::Property::TOP_PLANE_DISTANCE:
      {
        mCameraAttachment->SetTopClippingPlane(propertyValue.Get<float>());
        break;
      }
      case Dali::CameraActor::Property::BOTTOM_PLANE_DISTANCE:
      {
        mCameraAttachment->SetBottomClippingPlane(propertyValue.Get<float>());
        break;
      }
      case Dali::CameraActor::Property::NEAR_PLANE_DISTANCE:
      {
        mCameraAttachment->SetNearClippingPlane(propertyValue.Get<float>());
        break;
      }
      case Dali::CameraActor::Property::FAR_PLANE_DISTANCE:
      {
        mCameraAttachment->SetFarClippingPlane(propertyValue.Get<float>());
        break;
      }
      case Dali::CameraActor::Property::TARGET_POSITION:
      {
        mCameraAttachment->SetTargetPosition(propertyValue.Get<Vector3>());
        break;
      }
      case Dali::CameraActor::Property::PROJECTION_MATRIX:
      {
        DALI_LOG_WARNING("projection-matrix property is not animatable \n");
        break;
      }
      case Dali::CameraActor::Property::VIEW_MATRIX:
      {
        DALI_LOG_WARNING("view-matrix property is not animatable \n");
        break;
      }
      case Dali::CameraActor::Property::INVERT_Y_AXIS:
      {
        mCameraAttachment->SetInvertYAxis(propertyValue.Get<bool>());
        break;
      }
      default:
      {
        DALI_LOG_WARNING("Unknown property (%d)\n", index);
        break;
      }
    } // switch(index)

  } // else
}

Property::Value CameraActor::GetDefaultProperty( Property::Index index ) const
{
  Property::Value ret;
  if(index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT)
  {
    ret = Actor::GetDefaultProperty(index);
  }
  else
  {
    DALI_ASSERT_DEBUG(mCameraAttachment && "where is the camera?");
    switch(index)
    {
      case Dali::CameraActor::Property::TYPE:
      {
        if(mCameraAttachment->GetType() == Dali::Camera::LOOK_AT_TARGET)
        {
          ret = "LOOK_AT_TARGET";
        }
        else if(mCameraAttachment->GetType() == Dali::Camera::FREE_LOOK)
        {
          ret = "FREE_LOOK";
        }
        else
        {
          ret = "";
          DALI_ASSERT_DEBUG("Unknown camera type\n");
        }
        break;
      }
      case Dali::CameraActor::Property::PROJECTION_MODE:
      {
        if(mCameraAttachment->GetProjectionMode() == Dali::Camera::PERSPECTIVE_PROJECTION)
        {
          ret = "PERSPECTIVE_PROJECTION";
        }
        else if(mCameraAttachment->GetProjectionMode() == Dali::Camera::ORTHOGRAPHIC_PROJECTION)
        {
          ret = "ORTHOGRAPHIC_PROJECTION";
        }
        else
        {
          ret = "";
          DALI_ASSERT_DEBUG("Unknown projection mode\n");
        }
        break;
      }
      case Dali::CameraActor::Property::FIELD_OF_VIEW:
      {
        ret = mCameraAttachment->GetFieldOfView();
        break;
      }
      case Dali::CameraActor::Property::ASPECT_RATIO:
      {
        ret = mCameraAttachment->GetAspectRatio();
        break;
      }
      case Dali::CameraActor::Property::LEFT_PLANE_DISTANCE:
      {
        ret = mCameraAttachment->GetLeftClippingPlane();
        break;
      }
      case Dali::CameraActor::Property::RIGHT_PLANE_DISTANCE:
      {
        ret = mCameraAttachment->GetRightClippingPlane();
        break;
      }
      case Dali::CameraActor::Property::TOP_PLANE_DISTANCE:
      {
        ret = mCameraAttachment->GetTopClippingPlane();
        break;
      }
      case Dali::CameraActor::Property::BOTTOM_PLANE_DISTANCE:
      {
        ret = mCameraAttachment->GetBottomClippingPlane();
        break;
      }
      case Dali::CameraActor::Property::NEAR_PLANE_DISTANCE:
      {
        ret = mCameraAttachment->GetNearClippingPlane();
        break;
      }
      case Dali::CameraActor::Property::FAR_PLANE_DISTANCE:
      {
        ret = mCameraAttachment->GetFarClippingPlane();
        break;
      }
      case Dali::CameraActor::Property::TARGET_POSITION:
      {
        ret = mCameraAttachment->GetTargetPosition();
        break;
      }
      case Dali::CameraActor::Property::PROJECTION_MATRIX:
      {
        ret = mCameraAttachment->GetProjectionMatrix();
        break;
      }
      case Dali::CameraActor::Property::VIEW_MATRIX:
      {
        ret = mCameraAttachment->GetViewMatrix();
        break;
      }
      case Dali::CameraActor::Property::INVERT_Y_AXIS:
      {
        ret = mCameraAttachment->GetInvertYAxis();
        break;
      }
      default:
      {
        DALI_LOG_WARNING("Unknown property (%d)\n", index);
        break;
      }
    } // switch(index)
  }

  return ret;
}

const SceneGraph::PropertyBase* CameraActor::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  DALI_ASSERT_ALWAYS( IsPropertyAnimatable(index) && "Property is not animatable" );

  const SceneGraph::PropertyBase* property( NULL );

  // This method should only return a property of an object connected to the scene-graph
  if ( !OnStage() )
  {
    return property;
  }

  // let actor handle animatable properties, we have no animatable properties
  if( index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT )
  {
    property = Actor::GetSceneObjectAnimatableProperty(index);
  }

  return property;
}

const PropertyInputImpl* CameraActor::GetSceneObjectInputProperty( Property::Index index ) const
{
  const PropertyInputImpl* property( NULL );

  // This method should only return a property of an object connected to the scene-graph
  if ( !OnStage() )
  {
    return property;
  }

  // if its an actor default property or a custom property (actor already handles custom properties)
  if( ( index < DEFAULT_ACTOR_PROPERTY_MAX_COUNT ) || ( index >= DEFAULT_PROPERTY_MAX_COUNT ) )
  {
    property = Actor::GetSceneObjectInputProperty(index);
  }
  else
  {
    switch( index )
    {
      case Dali::CameraActor::Property::PROJECTION_MATRIX:
      {
        property = mCameraAttachment->GetProjectionMatrixProperty();
        break;
      }
      case Dali::CameraActor::Property::VIEW_MATRIX:
      {
        property = mCameraAttachment->GetViewMatrixProperty();
        break;
      }
      default:
        DALI_LOG_WARNING("Not an input property (%d)\n", index);
        break;
    }
  }

  return property;
}


} // namespace Internal

} // namespace Dali
