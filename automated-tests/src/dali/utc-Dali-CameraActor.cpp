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

#include <iostream>

#include <stdlib.h>
#include <cmath>
#include <dali/public-api/dali-core.h>

#include "dali-test-suite-utils/dali-test-suite-utils.h"

using namespace Dali;

void camera_actor_test_startup(void)
{
  test_return_value = TET_UNDEF;
}

void camera_actor_test_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

const float FLOAT_EPSILON = 0.001f;
const float TEST_ASPECT_RATIO = 0.123f;
const float TEST_FIELD_OF_VIEW = Radian(Degree(40.0f));
const float TEST_NEAR_PLANE_DISTANCE = 0.23f;
const float TEST_FAR_PLANE_DISTANCE = 0.973f;

const std::string SHADER_LIGHT_CAMERA_PROJECTION_MATRIX_PROPERTY_NAME( "uLightCameraProjectionMatrix" );
const std::string SHADER_LIGHT_CAMERA_VIEW_MATRIX_PROPERTY_NAME( "uLightCameraViewMatrix" );
const char* const RENDER_SHADOW_VERTEX_SOURCE =
  " uniform mediump mat4 uLightCameraProjectionMatrix;\n"
  " uniform mediump mat4 uLightCameraViewMatrix;\n"
  "\n"
  "void main()\n"
  "{\n"
  "  gl_Position = uProjection * uModelView * vec4(aPosition,1.0);\n"
  "  vec4 textureCoords = uLightCameraProjectionMatrix * uLightCameraViewMatrix * uModelMatrix  * vec4(aPosition,1.0);\n"
  "  vTexCoord = 0.5 + 0.5 * (textureCoords.xy/textureCoords.w);\n"
  "}\n";

const char* const RENDER_SHADOW_FRAGMENT_SOURCE =
  "uniform lowp vec4 uShadowColor;\n"
  "void main()\n"
  "{\n"
  "  lowp float alpha;\n"
  "  alpha = texture2D(sTexture, vec2(vTexCoord.x, vTexCoord.y)).a;\n"
  "  gl_FragColor = vec4(uShadowColor.rgb, uShadowColor.a * alpha);\n"
  "}\n";

} // Anonymous namespace


int UtcDaliCameraActorConstructorP(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor::CameraActor()" );

  CameraActor actor;

  DALI_TEST_CHECK( !actor );
  END_TEST;
}

// Note: No negative test for UtcDaliCameraActorConstructor.

int UtcDaliCameraActorDestructorP(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::~CameraActor (P)" );
  CameraActor* actor = new CameraActor();
  delete actor;
  actor = NULL;

  DALI_TEST_CHECK( true );
  END_TEST;
}

// Note: No negative test for UtcDaliCameraActorDestructor.

int UtcDaliCameraActorCopyConstructorP(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor Copy Constructor (P)" );
  CameraActor actor = CameraActor::New();

  CameraActor copyActor( actor );

  DALI_TEST_CHECK( copyActor );
  DALI_TEST_CHECK( copyActor == actor );

  END_TEST;
}

int UtcDaliCameraActorCopyConstructorN(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor Copy Constructor (N)" );
  CameraActor actor;

  CameraActor copyActor( actor );

  DALI_TEST_CHECK( !copyActor );

  END_TEST;
}

int UtcDaliCameraActorAssignmentOperatorP(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor Assignment Operator (P)" );
  const CameraActor actor = CameraActor::New();

  CameraActor copyActor = actor;

  DALI_TEST_CHECK( copyActor );
  DALI_TEST_CHECK( copyActor == actor );

  END_TEST;
}

int UtcDaliCameraActorAssignmentOperatorN(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor = (N)" );
  CameraActor actor;

  CameraActor copyActor = actor;

  DALI_TEST_CHECK( !copyActor );

  END_TEST;
}

int UtcDaliCameraActorNewP(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor::New (P)" );

  CameraActor actor = CameraActor::New();

  DALI_TEST_CHECK( actor );

  actor.Reset();

  DALI_TEST_CHECK( !actor );
  END_TEST;
}

// Note: No negative test for UtcDaliCameraActorNew.

int UtcDaliCameraActorDownCastP(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor::DownCast (P)" );

  CameraActor camera = CameraActor::New();
  Actor anActor = Actor::New();
  anActor.Add( camera );

  Actor child = anActor.GetChildAt( 0 );
  CameraActor cameraActor = CameraActor::DownCast( child );
  DALI_TEST_CHECK( cameraActor );

  cameraActor.Reset();
  DALI_TEST_CHECK( !cameraActor );

  cameraActor = DownCast< CameraActor >( child );
  DALI_TEST_CHECK( cameraActor );
  END_TEST;
}

int UtcDaliCameraActorDownCastN(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor::DownCast (N)" );

  Actor actor1 = Actor::New();
  Actor anActor = Actor::New();
  anActor.Add( actor1 );

  Actor child = anActor.GetChildAt( 0 );
  CameraActor cameraActor = CameraActor::DownCast( child );
  DALI_TEST_CHECK( !cameraActor );

  Actor unInitialzedActor;
  cameraActor = CameraActor::DownCast( unInitialzedActor );
  DALI_TEST_CHECK( !cameraActor );

  cameraActor = DownCast< CameraActor >( unInitialzedActor );
  DALI_TEST_CHECK( !cameraActor );
  END_TEST;
}

// Note: SetType and GetType are tested within the same test cases.

int UtcDaliCameraActorSetGetTypeP(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor GetType (P)" );

  CameraActor actor = CameraActor::New();
  DALI_TEST_EQUALS( actor.GetType(), Dali::Camera::FREE_LOOK, TEST_LOCATION );

  actor.SetType( Dali::Camera::LOOK_AT_TARGET );
  DALI_TEST_EQUALS( actor.GetType(), Dali::Camera::LOOK_AT_TARGET, TEST_LOCATION );

  std::string sValue;
  actor.GetProperty( CameraActor::Property::TYPE ).Get( sValue );
  std::string result( "LOOK_AT_TARGET");
  DALI_TEST_EQUALS( result, sValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliCameraActorSetGetTypeN(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor GetType (N)" );

  CameraActor actor;

  Dali::Camera::Type cameraType = Dali::Camera::FREE_LOOK;
  try
  {
    cameraType = actor.GetType();
  }
  catch ( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT( e, "camera", TEST_LOCATION );
  }

  const CameraActor aConstActor;

  try
  {
    cameraType = aConstActor.GetType();
  }
  catch ( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT( e, "camera", TEST_LOCATION );
  }

  DALI_TEST_EQUALS( cameraType, Dali::Camera::FREE_LOOK, TEST_LOCATION );
  END_TEST;
}

int UtcDaliCameraActorSetFieldOfViewP(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor Set Field of view (P)" );

  CameraActor defaultCamera = CameraActor::New( Size( TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT ) );
  const float defaultFieldOfView = defaultCamera.GetFieldOfView();

  CameraActor actor = CameraActor::New();
  DALI_TEST_EQUALS( actor.GetFieldOfView(), defaultFieldOfView, TEST_LOCATION );

  float fieldOfView = Math::PI / 3.0f;
  actor.SetFieldOfView( fieldOfView );
  DALI_TEST_EQUALS( actor.GetFieldOfView(), fieldOfView, TEST_LOCATION );

  float value;
  actor.GetProperty( CameraActor::Property::FIELD_OF_VIEW ).Get( value );
  DALI_TEST_EQUALS( fieldOfView, value, FLOAT_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliCameraActorSetFieldOfViewN(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor Set Field of view (N)" );

  CameraActor defaultCamera = CameraActor::New( Size( TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT ) );
  const float defaultFieldOfView = defaultCamera.GetFieldOfView();

  CameraActor actor = CameraActor::New();
  DALI_TEST_EQUALS( actor.GetFieldOfView(), defaultFieldOfView, TEST_LOCATION );

  float fieldOfView = Math::PI / 3.0f;
  actor.SetFieldOfView( fieldOfView );
  DALI_TEST_EQUALS( actor.GetFieldOfView(), fieldOfView, TEST_LOCATION );

  float value;
  actor.GetProperty( CameraActor::Property::FIELD_OF_VIEW ).Get( value );
  DALI_TEST_EQUALS( fieldOfView, value, FLOAT_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliCameraActorGetFieldOfViewP(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor Get Field of view (P)" );
  const Vector2 size( TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT );

  CameraActor defaultCamera = CameraActor::New( size );

  const float cameraZ = 2.0f * std::max( size.width, size.height );
  const float expectedFieldOfView = 2.0f * std::atan( size.height * 0.5f / cameraZ );

  CameraActor actor = CameraActor::New();
  DALI_TEST_EQUALS( actor.GetFieldOfView(), expectedFieldOfView, TEST_LOCATION );

  float value;
  actor.GetProperty( CameraActor::Property::FIELD_OF_VIEW ).Get( value );
  DALI_TEST_EQUALS( expectedFieldOfView, value, FLOAT_EPSILON, TEST_LOCATION);
  END_TEST;
}

int UtcDaliCameraActorGetFieldOfViewN(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor Get Field of view (N)" );

  CameraActor defaultCamera = CameraActor::New();

  bool asserted = true;
  try
  {
    defaultCamera.GetFieldOfView();
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT( e, "camera && \"Camera handle is empty\"", TEST_LOCATION );
    asserted = true;
  }
  DALI_TEST_CHECK( asserted );

  END_TEST;
}

int UtcDaliCameraActorSetAspectRatioP(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor Set Aspect Ratio (P)" );

  CameraActor actor = CameraActor::New();
  DALI_TEST_EQUALS( actor.GetAspectRatio(), static_cast<float>( TestApplication::DEFAULT_SURFACE_WIDTH ) / static_cast<float>( TestApplication::DEFAULT_SURFACE_HEIGHT ), TEST_LOCATION );

  // Set an initial value to confirm a further set changes it.
  float aspect = 4.0f / 3.0f;
  actor.SetAspectRatio( aspect );
  DALI_TEST_EQUALS( actor.GetAspectRatio(), aspect, TEST_LOCATION );

  aspect = 16.0f / 9.0f;
  actor.SetAspectRatio( aspect );
  DALI_TEST_EQUALS( actor.GetAspectRatio(), aspect, TEST_LOCATION );

  END_TEST;
}

int UtcDaliCameraActorSetAspectRatioN(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor Set Aspect Ratio (N)" );

  CameraActor actor;

  bool asserted = true;
  try
  {
    actor.SetAspectRatio( 16.0f / 9.0f );
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT( e, "camera && \"Camera handle is empty\"", TEST_LOCATION );
    asserted = true;
  }
  DALI_TEST_CHECK( asserted );

  END_TEST;
}

int UtcDaliCameraActorGetAspectRatioP(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor Get Aspect Ratio");

  CameraActor actor = CameraActor::New();
  float defaultAspect = static_cast<float>( TestApplication::DEFAULT_SURFACE_WIDTH ) / static_cast<float>( TestApplication::DEFAULT_SURFACE_HEIGHT );

  DALI_TEST_EQUALS( actor.GetAspectRatio(), defaultAspect, TEST_LOCATION );

  float value = 0.0f;
  actor.GetProperty( CameraActor::Property::ASPECT_RATIO ).Get( value );
  DALI_TEST_EQUALS( defaultAspect, value, FLOAT_EPSILON, TEST_LOCATION );

  END_TEST;
}

int UtcDaliCameraActorGetAspectRatioN(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor Get Aspect Ratio (N)" );

  CameraActor actor;

  bool asserted = true;
  try
  {
    actor.GetAspectRatio();
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT( e, "camera && \"Camera handle is empty\"", TEST_LOCATION );
    asserted = true;
  }

  DALI_TEST_CHECK( asserted );

  END_TEST;
}

int UtcDaliCameraActorSetNearClippingPlaneP(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor Set Near clipping plane (P)" );

  CameraActor actor = CameraActor::New();

  // Set a value so we are not relying on a particular default for this test case.
  actor.SetNearClippingPlane( 200.0f );
  DALI_TEST_EQUALS( actor.GetNearClippingPlane(), 200.0f, TEST_LOCATION );

  actor.SetNearClippingPlane( 400.0f );
  DALI_TEST_EQUALS( actor.GetNearClippingPlane(), 400.0f, TEST_LOCATION );

  // Check setting the property.
  actor.SetProperty( CameraActor::Property::NEAR_PLANE_DISTANCE, Property::Value( 300.0f ) );
  DALI_TEST_EQUALS( actor.GetNearClippingPlane(), 300.0f, FLOAT_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliCameraActorSetNearClippingPlaneN(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor Set Near clipping plane (N)" );

  CameraActor actor;

  bool asserted = true;
  try
  {
    actor.SetNearClippingPlane( 200.0f );
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT( e, "camera && \"Camera handle is empty\"", TEST_LOCATION );
    asserted = true;
  }

  DALI_TEST_CHECK( asserted );

  END_TEST;
}

int UtcDaliCameraActorGetNearClippingPlaneP(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor Get Near clipping plane (P)" );

  // Check the default value.
  CameraActor actor = CameraActor::New();
  float defaultValue = 800.0f;
  DALI_TEST_EQUALS( actor.GetNearClippingPlane(), defaultValue, TEST_LOCATION );

  // Check getting the property.
  float value;
  actor.GetProperty( CameraActor::Property::NEAR_PLANE_DISTANCE ).Get( value );
  DALI_TEST_EQUALS( defaultValue, value, FLOAT_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliCameraActorGetNearClippingPlaneN(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor Get Near clipping plane (N)" );

  CameraActor actor;
  bool asserted = true;
  try
  {
    actor.GetNearClippingPlane();
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT( e, "camera && \"Camera handle is empty\"", TEST_LOCATION );
    asserted = true;
  }

  DALI_TEST_CHECK( asserted );

  END_TEST;
}

int UtcDaliCameraActorSetFarClippingPlaneP(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor Set Far clipping plane (P)" );

  CameraActor actor = CameraActor::New();

  // Set a value so we are not relying on a particular default for this test case.
  actor.SetFarClippingPlane( 2000.0f );
  DALI_TEST_EQUALS( actor.GetFarClippingPlane(), 2000.0f, TEST_LOCATION );

  actor.SetFarClippingPlane( 4000.0f );
  DALI_TEST_EQUALS( actor.GetFarClippingPlane(), 4000.0f, TEST_LOCATION );

  // Check setting the property.
  actor.SetProperty( CameraActor::Property::FAR_PLANE_DISTANCE, 2000.0f );
  DALI_TEST_EQUALS( actor.GetFarClippingPlane(), 2000.0f, FLOAT_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliCameraActorSetFarClippingPlaneN(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor Set Far clipping plane (N)" );

  CameraActor actor;

  bool asserted = true;
  try
  {
    actor.SetFarClippingPlane( 2000.0f );
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT( e, "camera && \"Camera handle is empty\"", TEST_LOCATION );
    asserted = true;
  }

  DALI_TEST_CHECK( asserted );

  END_TEST;
}

int UtcDaliCameraActorGetFarClippingPlaneP(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor Get Far clipping plane (P)" );

  CameraActor actor = CameraActor::New();
  float defaultValue = 800.0f + ( 0xFFFF >> 4 );
  DALI_TEST_EQUALS( actor.GetFarClippingPlane(), defaultValue, TEST_LOCATION );

  // Check getting the property.
  float value;
  actor.GetProperty( CameraActor::Property::FAR_PLANE_DISTANCE ).Get( value );
  DALI_TEST_EQUALS( defaultValue, value, FLOAT_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliCameraActorGetFarClippingPlaneN(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor Get Far clipping plane (N)" );

  CameraActor actor;

  bool asserted = true;
  try
  {
    actor.GetFarClippingPlane();
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT( e, "camera && \"Camera handle is empty\"", TEST_LOCATION );
    asserted = true;
  }

  DALI_TEST_CHECK( asserted );

  END_TEST;
}

int UtcDaliCameraActorSetTargetPositionP(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor Set Target Position (P)" );

  CameraActor actor = CameraActor::New();

  Vector3 target1( 10.0f, 20.0f, 30.0f );
  Vector3 target2( 15.0f, 25.0f, 35.0f );

  // Set a value so we are not relying on a particular default for this test case.
  actor.SetTargetPosition( target1 );
  DALI_TEST_EQUALS( actor.GetTargetPosition(), target1, TEST_LOCATION );

  actor.SetTargetPosition( target2 );
  DALI_TEST_EQUALS( actor.GetTargetPosition(), target2, TEST_LOCATION );

  // Check setting the property.
  actor.SetProperty( CameraActor::Property::TARGET_POSITION, target1 );
  DALI_TEST_EQUALS( actor.GetTargetPosition(), target1, FLOAT_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliCameraActorSetTargetPositionN(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor Set Target Position (N)" );

  CameraActor actor;

  bool asserted = true;
  try
  {
    actor.SetTargetPosition( Vector3( 10.0f, 20.0f, 30.0f ) );
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT( e, "camera && \"Camera handle is empty\"", TEST_LOCATION );
    asserted = true;
  }
  DALI_TEST_CHECK( asserted );

  END_TEST;
}

int UtcDaliCameraActorGetTargetPositionP(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor Get Target Position (P)" );

  CameraActor actor = CameraActor::New();
  Vector3 defaultValue( Vector3::ZERO );
  DALI_TEST_EQUALS( actor.GetTargetPosition(), defaultValue, TEST_LOCATION );

  // Check getting the property.
  Vector3 value;
  actor.GetProperty( CameraActor::Property::TARGET_POSITION ).Get( value );
  DALI_TEST_EQUALS( defaultValue, value, FLOAT_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliCameraActorGetTargetPositionN(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor Get Target Position (N)" );

  CameraActor actor;

  bool asserted = true;
  try
  {
    actor.GetTargetPosition();
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT( e, "camera && \"Camera handle is empty\"", TEST_LOCATION );
    asserted = true;
  }
  DALI_TEST_CHECK( asserted );

  END_TEST;
}

int UtcDaliCameraActorSetInvertYAxisP(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor Set InvertYAxis (P)" );

  CameraActor actor = CameraActor::New();

  // Set a value so we are not relying on a particular default for this test case.
  actor.SetInvertYAxis( false );
  DALI_TEST_EQUALS( actor.GetInvertYAxis(), false, TEST_LOCATION );

  actor.SetInvertYAxis( true );
  DALI_TEST_EQUALS( actor.GetInvertYAxis(), true, TEST_LOCATION );

  actor.SetProperty( CameraActor::Property::INVERT_Y_AXIS, false );
  DALI_TEST_EQUALS( actor.GetInvertYAxis(), false, TEST_LOCATION );
  END_TEST;
}

int UtcDaliCameraActorSetInvertYAxisN(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor Set InvertYAxis (N)" );

  CameraActor actor;

  bool asserted = true;
  try
  {
    actor.SetInvertYAxis( false );
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT( e, "camera && \"Camera handle is empty\"", TEST_LOCATION );
    asserted = true;
  }
  DALI_TEST_CHECK( asserted );
  END_TEST;
}

int UtcDaliCameraActorGetInvertYAxisP(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor Get InvertYAxis (P)" );

  // Check the default value.
  CameraActor actor = CameraActor::New();
  DALI_TEST_EQUALS( actor.GetInvertYAxis(), false, TEST_LOCATION );

  // Check getting the property.
  bool bValue;
  actor.GetProperty( CameraActor::Property::INVERT_Y_AXIS ).Get( bValue );
  DALI_TEST_EQUALS( false, bValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliCameraActorGetInvertYAxisN(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor Get InvertYAxis (N)" );

  CameraActor actor;

  bool asserted = true;
  try
  {
    actor.GetInvertYAxis();
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT( e, "camera && \"Camera handle is empty\"", TEST_LOCATION );
    asserted = true;
  }
  DALI_TEST_CHECK( asserted );
  END_TEST;
}

int UtcDaliCameraActorSetPerspectiveProjectionP(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor::SetPerspectiveProjection (P)" );

  CameraActor actor = CameraActor::New();
  actor.SetPerspectiveProjection( Size( 100.f, 150.f ) );

  DALI_TEST_CHECK( actor );

  float value;
  actor.GetProperty( CameraActor::Property::ASPECT_RATIO ).Get( value );
  DALI_TEST_EQUALS( 0.666666f, value, FLOAT_EPSILON, TEST_LOCATION );
  actor.GetProperty( CameraActor::Property::FIELD_OF_VIEW ).Get( value );
  DALI_TEST_EQUALS( 0.489957f, value, FLOAT_EPSILON, TEST_LOCATION );
  actor.GetProperty( CameraActor::Property::NEAR_PLANE_DISTANCE ).Get( value );
  DALI_TEST_EQUALS( 150.f, value, FLOAT_EPSILON, TEST_LOCATION );
  actor.GetProperty( CameraActor::Property::FAR_PLANE_DISTANCE ).Get( value );
  DALI_TEST_EQUALS( 4245.f, value, FLOAT_EPSILON, TEST_LOCATION );

  DALI_TEST_EQUALS( actor.GetProjectionMode(), Dali::Camera::PERSPECTIVE_PROJECTION, TEST_LOCATION );

  END_TEST;
}

int UtcDaliCameraActorSetPerspectiveProjectionN(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor::SetPerspectiveProjection (N)" );

  Stage stage = Stage::GetCurrent();
  Vector2 stageSize = stage.GetSize();

  CameraActor actor = CameraActor::New();

  // Check that setting perspective projection without a size (using zero size) uses the stages size.
  actor.SetPerspectiveProjection( Size::ZERO );

  float nearClippingPlane = std::max( stageSize.width, stageSize.height );
  float farClippingPlane = nearClippingPlane + static_cast<float>( 0xFFFF >> 4 );

  DALI_TEST_EQUALS( nearClippingPlane, actor.GetNearClippingPlane(), FLOAT_EPSILON, TEST_LOCATION );
  DALI_TEST_EQUALS( farClippingPlane, actor.GetFarClippingPlane(), FLOAT_EPSILON, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetProjectionMode(), Dali::Camera::PERSPECTIVE_PROJECTION, TEST_LOCATION );

  END_TEST;
}

int UtcDaliCameraActorSetOrthographicProjectionP1(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor::SetOrthographicProjection (P,1)" );

  CameraActor actor = CameraActor::New( Size( 1080.0f, 1920.0f ) );
  DALI_TEST_CHECK( actor );

  Stage::GetCurrent().Add( actor );

  actor.SetOrthographicProjection( Size( 1080.0f, 1920.0f ) );
  application.SendNotification();
  application.Render( 0 );
  application.Render();
  application.SendNotification();

  float defaultAspectRatio;
  float defaultFieldOfView;
  float defaultNearPlaneDistance;
  float defaultFarPlaneDistance;
  actor.GetProperty( CameraActor::Property::ASPECT_RATIO ).Get( defaultAspectRatio );
  actor.GetProperty( CameraActor::Property::FIELD_OF_VIEW ).Get( defaultFieldOfView );
  actor.GetProperty( CameraActor::Property::NEAR_PLANE_DISTANCE ).Get( defaultNearPlaneDistance );
  actor.GetProperty( CameraActor::Property::FAR_PLANE_DISTANCE ).Get( defaultFarPlaneDistance );
  Vector3 defaultPos = actor.GetCurrentPosition();

  actor.SetOrthographicProjection( Size( 1080.0f, 1920.0f ) );

  application.SendNotification();
  application.Render( 0 );
  application.Render();
  application.SendNotification();

  float value;
  actor.GetProperty( CameraActor::Property::ASPECT_RATIO ).Get( value );
  DALI_TEST_EQUALS( defaultAspectRatio, value, FLOAT_EPSILON, TEST_LOCATION );
  actor.GetProperty( CameraActor::Property::FIELD_OF_VIEW ).Get( value );
  DALI_TEST_EQUALS( defaultFieldOfView, value, FLOAT_EPSILON, TEST_LOCATION );
  actor.GetProperty( CameraActor::Property::NEAR_PLANE_DISTANCE ).Get( value );
  DALI_TEST_EQUALS( defaultNearPlaneDistance, value, FLOAT_EPSILON, TEST_LOCATION );
  actor.GetProperty( CameraActor::Property::FAR_PLANE_DISTANCE ).Get( value );
  DALI_TEST_EQUALS( defaultFarPlaneDistance, value, FLOAT_EPSILON, TEST_LOCATION );

  actor.GetProperty( CameraActor::Property::LEFT_PLANE_DISTANCE ).Get( value );
  DALI_TEST_EQUALS( -540.0f, value, FLOAT_EPSILON, TEST_LOCATION );
  actor.GetProperty( CameraActor::Property::RIGHT_PLANE_DISTANCE ).Get( value );
  DALI_TEST_EQUALS( 540.0f, value, FLOAT_EPSILON, TEST_LOCATION );
  actor.GetProperty( CameraActor::Property::TOP_PLANE_DISTANCE ).Get( value );
  DALI_TEST_EQUALS( 960.0f, value, FLOAT_EPSILON, TEST_LOCATION );
  actor.GetProperty( CameraActor::Property::BOTTOM_PLANE_DISTANCE ).Get( value );
  DALI_TEST_EQUALS( -960.0f, value, FLOAT_EPSILON, TEST_LOCATION );

  Vector3 pos = actor.GetCurrentPosition();
  DALI_TEST_EQUALS( defaultPos.z, pos.z, 0.001f, TEST_LOCATION );

  DALI_TEST_EQUALS( actor.GetProjectionMode(), Dali::Camera::ORTHOGRAPHIC_PROJECTION, TEST_LOCATION );
  END_TEST;
}

int UtcDaliCameraActorSetOrthographicProjectionN(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor::SetOrthographicProjection (N)" );

  CameraActor actor;
  bool asserted = true;
  try
  {
    actor.GetProjectionMode();
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT( e, "camera && \"Camera handle is empty\"", TEST_LOCATION );
    asserted = true;
  }
  DALI_TEST_CHECK( asserted );
  END_TEST;
}

int UtcDaliCameraActorSetOrthographicProjectionP2(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor::SetOrthographicProjection (P,2)" );

  CameraActor actor = CameraActor::New();
  DALI_TEST_CHECK( actor );

  float defaultAspectRatio;
  float defaultFieldOfView;
  float defaultNearPlaneDistance;
  float defaultFarPlaneDistance;
  actor.GetProperty( CameraActor::Property::ASPECT_RATIO ).Get( defaultAspectRatio );
  actor.GetProperty( CameraActor::Property::FIELD_OF_VIEW ).Get( defaultFieldOfView );
  actor.GetProperty( CameraActor::Property::NEAR_PLANE_DISTANCE ).Get( defaultNearPlaneDistance );
  actor.GetProperty( CameraActor::Property::FAR_PLANE_DISTANCE ).Get( defaultFarPlaneDistance );

  // Check setting with specific near and far plane distances.
  actor.SetOrthographicProjection( -100.0f, 200.0f, -300.0f, 500.0f, 400.0f, 4000.0f );

  float value;
  actor.GetProperty( CameraActor::Property::ASPECT_RATIO ).Get( value );
  DALI_TEST_EQUALS( defaultAspectRatio, value, FLOAT_EPSILON, TEST_LOCATION );
  actor.GetProperty( CameraActor::Property::FIELD_OF_VIEW ).Get( value );
  DALI_TEST_EQUALS( defaultFieldOfView, value, FLOAT_EPSILON, TEST_LOCATION );
  actor.GetProperty( CameraActor::Property::NEAR_PLANE_DISTANCE ).Get( value );
  DALI_TEST_EQUALS( 400.0f, value, FLOAT_EPSILON, TEST_LOCATION );
  actor.GetProperty( CameraActor::Property::FAR_PLANE_DISTANCE ).Get( value );
  DALI_TEST_EQUALS( 4000.0f, value, FLOAT_EPSILON, TEST_LOCATION );

  actor.GetProperty( CameraActor::Property::LEFT_PLANE_DISTANCE ).Get( value );
  DALI_TEST_EQUALS( -100.0f, value, FLOAT_EPSILON, TEST_LOCATION );
  actor.GetProperty( CameraActor::Property::RIGHT_PLANE_DISTANCE ).Get( value );
  DALI_TEST_EQUALS( 200.0f, value, FLOAT_EPSILON, TEST_LOCATION );
  actor.GetProperty( CameraActor::Property::TOP_PLANE_DISTANCE ).Get( value );
  DALI_TEST_EQUALS( -300.0f, value, FLOAT_EPSILON, TEST_LOCATION );
  actor.GetProperty( CameraActor::Property::BOTTOM_PLANE_DISTANCE ).Get( value );
  DALI_TEST_EQUALS( 500.0f, value, FLOAT_EPSILON, TEST_LOCATION );

  DALI_TEST_EQUALS( actor.GetProjectionMode(), Dali::Camera::ORTHOGRAPHIC_PROJECTION, TEST_LOCATION );

  END_TEST;
}

int UtcDaliCameraActorSetOrthographicProjectionP3(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor::SetOrthographicProjection (P,3)" );

  CameraActor actor = CameraActor::New();
  DALI_TEST_CHECK( actor );

  float defaultAspectRatio;
  float defaultFieldOfView;
  float defaultNearPlaneDistance;
  float defaultFarPlaneDistance;
  actor.GetProperty( CameraActor::Property::ASPECT_RATIO ).Get( defaultAspectRatio );
  actor.GetProperty( CameraActor::Property::FIELD_OF_VIEW ).Get( defaultFieldOfView );
  actor.GetProperty( CameraActor::Property::NEAR_PLANE_DISTANCE ).Get( defaultNearPlaneDistance );
  actor.GetProperty( CameraActor::Property::FAR_PLANE_DISTANCE ).Get( defaultFarPlaneDistance );

  actor.SetProjectionMode( Dali::Camera::ORTHOGRAPHIC_PROJECTION );

  actor.SetProperty( CameraActor::Property::LEFT_PLANE_DISTANCE, -100.0f );
  actor.SetProperty( CameraActor::Property::RIGHT_PLANE_DISTANCE, 200.0f );
  actor.SetProperty( CameraActor::Property::TOP_PLANE_DISTANCE, -300.0f );
  actor.SetProperty( CameraActor::Property::BOTTOM_PLANE_DISTANCE, 500.0f );
  actor.SetNearClippingPlane( 400.0f );
  actor.SetFarClippingPlane( 4000.0f );

  float value;
  actor.GetProperty( CameraActor::Property::ASPECT_RATIO ).Get( value );
  DALI_TEST_EQUALS( defaultAspectRatio, value, FLOAT_EPSILON, TEST_LOCATION );
  actor.GetProperty( CameraActor::Property::FIELD_OF_VIEW ).Get( value );
  DALI_TEST_EQUALS( defaultFieldOfView, value, FLOAT_EPSILON, TEST_LOCATION );
  actor.GetProperty( CameraActor::Property::NEAR_PLANE_DISTANCE ).Get( value );
  DALI_TEST_EQUALS( 400.0f, value, FLOAT_EPSILON, TEST_LOCATION );
  actor.GetProperty( CameraActor::Property::FAR_PLANE_DISTANCE ).Get( value );
  DALI_TEST_EQUALS( 4000.0f, value, FLOAT_EPSILON, TEST_LOCATION );

  actor.GetProperty( CameraActor::Property::LEFT_PLANE_DISTANCE ).Get( value );
  DALI_TEST_EQUALS( -100.0f, value, FLOAT_EPSILON, TEST_LOCATION );
  actor.GetProperty( CameraActor::Property::RIGHT_PLANE_DISTANCE ).Get( value );
  DALI_TEST_EQUALS( 200.0f, value, FLOAT_EPSILON, TEST_LOCATION );
  actor.GetProperty( CameraActor::Property::TOP_PLANE_DISTANCE ).Get( value );
  DALI_TEST_EQUALS( -300.0f, value, FLOAT_EPSILON, TEST_LOCATION );
  actor.GetProperty( CameraActor::Property::BOTTOM_PLANE_DISTANCE ).Get( value );
  DALI_TEST_EQUALS( 500.0f, value, FLOAT_EPSILON, TEST_LOCATION );

  DALI_TEST_EQUALS( actor.GetProjectionMode(), Dali::Camera::ORTHOGRAPHIC_PROJECTION, TEST_LOCATION );
  std::string stringValue;
  actor.GetProperty( CameraActor::Property::PROJECTION_MODE ).Get( stringValue );
  DALI_TEST_EQUALS( stringValue, "ORTHOGRAPHIC_PROJECTION", TEST_LOCATION );
  END_TEST;
}

int UtcDaliCameraActorSetProjectionModeP(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor::SetProjectionModeP (P)" );

  CameraActor actor = CameraActor::New();

  // Check that changing the projection mode alone does not alter other presets.
  actor.SetNearClippingPlane( 200.0f );
  actor.SetFarClippingPlane( 400.0f );

  actor.SetProjectionMode( Dali::Camera::PERSPECTIVE_PROJECTION );

  DALI_TEST_EQUALS( actor.GetProjectionMode(), Dali::Camera::PERSPECTIVE_PROJECTION, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetNearClippingPlane(), 200.0f, FLOAT_EPSILON, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetFarClippingPlane(), 400.0f, FLOAT_EPSILON, TEST_LOCATION );

  actor.SetProjectionMode( Dali::Camera::ORTHOGRAPHIC_PROJECTION );

  DALI_TEST_EQUALS( actor.GetProjectionMode(), Dali::Camera::ORTHOGRAPHIC_PROJECTION, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetNearClippingPlane(), 200.0f, FLOAT_EPSILON, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetFarClippingPlane(), 400.0f, FLOAT_EPSILON, TEST_LOCATION );

  // Check setting the property.
  Property::Value setValue = "PERSPECTIVE_PROJECTION";
  actor.SetProperty( CameraActor::Property::PROJECTION_MODE, setValue );
  DALI_TEST_EQUALS( actor.GetProjectionMode(), Dali::Camera::PERSPECTIVE_PROJECTION, TEST_LOCATION );
  END_TEST;
}

int UtcDaliCameraActorSetProjectionModeN(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor::SetProjectionModeP (N)" );

  CameraActor actor;

  bool asserted = true;
  try
  {
    actor.SetProjectionMode( Dali::Camera::PERSPECTIVE_PROJECTION );
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT( e, "camera && \"Camera handle is empty\"", TEST_LOCATION );
    asserted = true;
  }
  DALI_TEST_CHECK( asserted );
  END_TEST;
}

int UtcDaliCameraActorGetProjectionModeP(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor::GetPerspectiveProjection (P)" );

  CameraActor actor = CameraActor::New();

  actor.SetOrthographicProjection( Size::ONE );
  DALI_TEST_EQUALS( actor.GetProjectionMode(), Dali::Camera::ORTHOGRAPHIC_PROJECTION, TEST_LOCATION );

  actor.SetPerspectiveProjection( Size( 100.f, 150.f ) );
  DALI_TEST_EQUALS( actor.GetProjectionMode(), Dali::Camera::PERSPECTIVE_PROJECTION, TEST_LOCATION );

  // Check getting the property.
  std::string stringValue;
  actor.GetProperty( CameraActor::Property::PROJECTION_MODE ).Get( stringValue );
  DALI_TEST_EQUALS( stringValue, "PERSPECTIVE_PROJECTION", TEST_LOCATION );
  END_TEST;
}

int UtcDaliCameraActorGetProjectionModeN(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor::GetProjectionMode (N)" );

  CameraActor actor;

  bool asserted = true;
  try
  {
    actor.GetProjectionMode();
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT( e, "camera && \"Camera handle is empty\"", TEST_LOCATION );
    asserted = true;
  }
  DALI_TEST_CHECK( asserted );
  END_TEST;
}

int UtcDaliCameraActorSetCameraOffStage(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor::SetCamera()" );

  CameraActor actor = CameraActor::New();

  actor.SetType( Camera::FREE_LOOK );
  actor.SetFieldOfView( TEST_FIELD_OF_VIEW );
  actor.SetAspectRatio( TEST_ASPECT_RATIO );
  actor.SetNearClippingPlane( TEST_NEAR_PLANE_DISTANCE );
  actor.SetFarClippingPlane( TEST_FAR_PLANE_DISTANCE );
  actor.SetProjectionMode( Camera::PERSPECTIVE_PROJECTION );

  actor.SetInvertYAxis( false );

  DALI_TEST_EQUALS( TEST_ASPECT_RATIO, actor.GetAspectRatio(), FLOAT_EPSILON, TEST_LOCATION );//change to machine epsilon
  DALI_TEST_EQUALS( TEST_FIELD_OF_VIEW, actor.GetFieldOfView(), FLOAT_EPSILON, TEST_LOCATION );
  DALI_TEST_EQUALS( TEST_NEAR_PLANE_DISTANCE, actor.GetNearClippingPlane(), FLOAT_EPSILON, TEST_LOCATION );
  DALI_TEST_EQUALS( TEST_FAR_PLANE_DISTANCE, actor.GetFarClippingPlane(), FLOAT_EPSILON, TEST_LOCATION );
  DALI_TEST_EQUALS( Camera::PERSPECTIVE_PROJECTION, actor.GetProjectionMode(), TEST_LOCATION );
  DALI_TEST_EQUALS( false, actor.GetInvertYAxis(), TEST_LOCATION );

  float value;
  std::string sValue;
  actor.GetProperty( CameraActor::Property::ASPECT_RATIO).Get( value );
  DALI_TEST_EQUALS( TEST_ASPECT_RATIO, value, FLOAT_EPSILON, TEST_LOCATION );
  actor.GetProperty( CameraActor::Property::FIELD_OF_VIEW).Get( value );
  DALI_TEST_EQUALS( TEST_FIELD_OF_VIEW, value, FLOAT_EPSILON, TEST_LOCATION );
  actor.GetProperty( CameraActor::Property::NEAR_PLANE_DISTANCE).Get( value );
  DALI_TEST_EQUALS( TEST_NEAR_PLANE_DISTANCE, value, FLOAT_EPSILON, TEST_LOCATION );
  actor.GetProperty( CameraActor::Property::FAR_PLANE_DISTANCE).Get( value );
  DALI_TEST_EQUALS( TEST_FAR_PLANE_DISTANCE, value, FLOAT_EPSILON, TEST_LOCATION );
  actor.GetProperty( CameraActor::Property::PROJECTION_MODE).Get( sValue );
  DALI_TEST_EQUALS( "PERSPECTIVE_PROJECTION", sValue, TEST_LOCATION );
  bool bValue;
  actor.GetProperty( CameraActor::Property::INVERT_Y_AXIS).Get( bValue );
  DALI_TEST_EQUALS( false, bValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliCameraActorSetCameraOnStage(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor::SetCamera()" );

  CameraActor actor = CameraActor::New();
  Stage::GetCurrent().Add( actor );
  application.Render( 0 );
  application.SendNotification();

  actor.SetType( Camera::LOOK_AT_TARGET );
  actor.SetFieldOfView( TEST_FIELD_OF_VIEW );
  actor.SetAspectRatio( TEST_ASPECT_RATIO );
  actor.SetNearClippingPlane( TEST_NEAR_PLANE_DISTANCE );
  actor.SetFarClippingPlane( TEST_FAR_PLANE_DISTANCE );
  actor.SetInvertYAxis( false );

  DALI_TEST_EQUALS( false, actor.GetInvertYAxis(), TEST_LOCATION );

  // Will need 2 frames to ensure both buffers are set to same values:
  application.Render();
  application.SendNotification();
  application.Render();
  application.SendNotification();

  DALI_TEST_EQUALS( TEST_ASPECT_RATIO, actor.GetAspectRatio(), FLOAT_EPSILON, TEST_LOCATION );
  DALI_TEST_EQUALS( TEST_FIELD_OF_VIEW, actor.GetFieldOfView(), FLOAT_EPSILON, TEST_LOCATION );
  DALI_TEST_EQUALS( TEST_NEAR_PLANE_DISTANCE, actor.GetNearClippingPlane(), FLOAT_EPSILON, TEST_LOCATION );
  DALI_TEST_EQUALS( TEST_FAR_PLANE_DISTANCE, actor.GetFarClippingPlane(), FLOAT_EPSILON, TEST_LOCATION );
  DALI_TEST_EQUALS( false, actor.GetInvertYAxis(), TEST_LOCATION );

  std::string sValue;
  actor.GetProperty( CameraActor::Property::TYPE ).Get( sValue );
  DALI_TEST_EQUALS( sValue, "LOOK_AT_TARGET", TEST_LOCATION );

  float value;
  actor.GetProperty( CameraActor::Property::ASPECT_RATIO ).Get( value );
  DALI_TEST_EQUALS( TEST_ASPECT_RATIO, value, FLOAT_EPSILON, TEST_LOCATION );
  actor.GetProperty( CameraActor::Property::FIELD_OF_VIEW ).Get( value );
  DALI_TEST_EQUALS( TEST_FIELD_OF_VIEW, value, FLOAT_EPSILON, TEST_LOCATION );
  actor.GetProperty( CameraActor::Property::NEAR_PLANE_DISTANCE ).Get( value );
  DALI_TEST_EQUALS( TEST_NEAR_PLANE_DISTANCE, value, FLOAT_EPSILON, TEST_LOCATION );
  actor.GetProperty( CameraActor::Property::FAR_PLANE_DISTANCE ).Get( value);
  DALI_TEST_EQUALS( TEST_FAR_PLANE_DISTANCE, value, FLOAT_EPSILON, TEST_LOCATION );

  bool bValue;
  actor.GetProperty( CameraActor::Property::INVERT_Y_AXIS ).Get( bValue );
  DALI_TEST_EQUALS( false, bValue, TEST_LOCATION );
  END_TEST;
}

int UtcDaliCameraActorGetCamera(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor::GetCamera()" );

  CameraActor actor = CameraActor::New();

  actor.SetAspectRatio( TEST_ASPECT_RATIO );

  DALI_TEST_EQUALS( actor.GetAspectRatio(), TEST_ASPECT_RATIO, FLOAT_EPSILON, TEST_LOCATION );

  actor.SetProperty( CameraActor::Property::TYPE, "FREE_LOOK" );
  actor.SetProperty( CameraActor::Property::ASPECT_RATIO, TEST_ASPECT_RATIO );
  actor.SetProperty( CameraActor::Property::FIELD_OF_VIEW, TEST_FIELD_OF_VIEW );
  actor.SetProperty( CameraActor::Property::NEAR_PLANE_DISTANCE, TEST_NEAR_PLANE_DISTANCE );
  actor.SetProperty( CameraActor::Property::FAR_PLANE_DISTANCE, TEST_FAR_PLANE_DISTANCE );

  DALI_TEST_EQUALS( Camera::FREE_LOOK, actor.GetType(), TEST_LOCATION );
  DALI_TEST_EQUALS( TEST_ASPECT_RATIO, actor.GetAspectRatio(), FLOAT_EPSILON, TEST_LOCATION );
  DALI_TEST_EQUALS( TEST_FIELD_OF_VIEW, actor.GetFieldOfView(), FLOAT_EPSILON, TEST_LOCATION );
  DALI_TEST_EQUALS( TEST_NEAR_PLANE_DISTANCE, actor.GetNearClippingPlane(), FLOAT_EPSILON, TEST_LOCATION );
  DALI_TEST_EQUALS( TEST_FAR_PLANE_DISTANCE, actor.GetFarClippingPlane(), FLOAT_EPSILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliCameraActorDefaultProperties(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor DefaultProperties" );

  CameraActor actor = CameraActor::New();
  Stage stage = Stage::GetCurrent();
  stage.Add(actor);
  stage.GetRenderTaskList().GetTask(0).SetCameraActor( actor );

  actor.SetAspectRatio( TEST_ASPECT_RATIO );
  Stage::GetCurrent().Add( actor );
  application.Render( 0 );
  application.SendNotification();
  bool bValue;
  actor.GetProperty( CameraActor::Property::INVERT_Y_AXIS ).Get( bValue );
  DALI_TEST_EQUALS( false, bValue, TEST_LOCATION );

  std::vector<Property::Index> indices ;
  indices.push_back( CameraActor::Property::TYPE );
  indices.push_back( CameraActor::Property::PROJECTION_MODE );
  indices.push_back( CameraActor::Property::FIELD_OF_VIEW );
  indices.push_back( CameraActor::Property::ASPECT_RATIO );
  indices.push_back( CameraActor::Property::NEAR_PLANE_DISTANCE );
  indices.push_back( CameraActor::Property::FAR_PLANE_DISTANCE );
  indices.push_back( CameraActor::Property::LEFT_PLANE_DISTANCE );
  indices.push_back( CameraActor::Property::RIGHT_PLANE_DISTANCE );
  indices.push_back( CameraActor::Property::TOP_PLANE_DISTANCE );
  indices.push_back( CameraActor::Property::BOTTOM_PLANE_DISTANCE );
  indices.push_back( CameraActor::Property::TARGET_POSITION );
  indices.push_back( CameraActor::Property::PROJECTION_MATRIX );
  indices.push_back( CameraActor::Property::VIEW_MATRIX );
  indices.push_back( CameraActor::Property::INVERT_Y_AXIS );

  DALI_TEST_CHECK( actor.GetPropertyCount() == ( Actor::New().GetPropertyCount() + indices.size() ) );

  for( std::vector<Property::Index>::iterator iter = indices.begin(); iter != indices.end(); ++iter )
  {
    DALI_TEST_EQUALS( *iter, actor.GetPropertyIndex( actor.GetPropertyName( *iter ) ), TEST_LOCATION);
    DALI_TEST_CHECK( !actor.IsPropertyAnimatable( *iter ) );

    if ( ( *iter == CameraActor::Property::PROJECTION_MATRIX ) ||
         ( *iter == CameraActor::Property::VIEW_MATRIX ) )
    {
      DALI_TEST_CHECK( !actor.IsPropertyWritable( *iter ) );
    }
    else
    {
      DALI_TEST_CHECK( actor.IsPropertyWritable( *iter ) );
    }

    DALI_TEST_CHECK( actor.GetPropertyType( *iter ) == actor.GetPropertyType( *iter ) ); // just checking call succeeds
  }

  // Set/Get one of them.
  const float newAspect = TEST_ASPECT_RATIO * 2.0f;

  actor.SetProperty( CameraActor::Property::ASPECT_RATIO, Property::Value( newAspect ) );
  application.Render();
  application.SendNotification();
  application.Render();
  application.SendNotification();

  DALI_TEST_EQUALS( actor.GetAspectRatio(), newAspect, TEST_LOCATION );
  END_TEST;
}

int UtcDaliCameraActorModelView(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor Test view application" );

  BufferImage image = CreateBufferImage();

  Actor actor = CreateRenderableActor(image);
  actor.SetSize( 100.0f, 100.0f );
  actor.SetPosition( 20.0f, 30.0f, 40.0f );
  actor.SetParentOrigin( ParentOrigin::CENTER );
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render( 0 );
  application.Render();
  application.SendNotification();

  Matrix resultMatrix( true );
  resultMatrix.SetTransformComponents( Vector3::ONE, Quaternion::IDENTITY, actor.GetCurrentPosition() );

  RenderTask task = Stage::GetCurrent().GetRenderTaskList().GetTask( 0 );
  CameraActor cameraActor = task.GetCameraActor();

  Matrix viewMatrix( false );
  cameraActor.GetProperty( CameraActor::Property::VIEW_MATRIX ).Get( viewMatrix );
  Matrix::Multiply( resultMatrix, resultMatrix, viewMatrix );

  DALI_TEST_CHECK( application.GetGlAbstraction().CheckUniformValue( "uModelView", resultMatrix ) );
  END_TEST;
}

int UtcDaliCameraActorReadProjectionMatrix(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::CameraActor::ReadProjectionMatrix()" );

  CameraActor camera = Stage::GetCurrent().GetRenderTaskList().GetTask( 0u ).GetCameraActor();
  application.SendNotification();
  application.Render( 0 );
  application.Render();
  application.SendNotification();
  Image image = CreateBufferImage();
  Actor actor = CreateRenderableActor( image, RENDER_SHADOW_VERTEX_SOURCE, RENDER_SHADOW_FRAGMENT_SOURCE );
  actor.SetSize( 100.0f, 100.0f );
  Stage::GetCurrent().Add( actor );

  Matrix projectionMatrix;
  Matrix viewMatrix;

  camera.GetProperty( CameraActor::CameraActor::Property::PROJECTION_MATRIX ).Get( projectionMatrix );
  camera.GetProperty( CameraActor::CameraActor::Property::VIEW_MATRIX ).Get( viewMatrix );

  actor.RegisterProperty( SHADER_LIGHT_CAMERA_PROJECTION_MATRIX_PROPERTY_NAME, Matrix::IDENTITY );
  actor.RegisterProperty( SHADER_LIGHT_CAMERA_VIEW_MATRIX_PROPERTY_NAME, Matrix::IDENTITY );

  Property::Index projectionMatrixPropertyIndex = actor.GetPropertyIndex( SHADER_LIGHT_CAMERA_PROJECTION_MATRIX_PROPERTY_NAME );
  Property::Index viewMatrixPropertyIndex = actor.GetPropertyIndex( SHADER_LIGHT_CAMERA_VIEW_MATRIX_PROPERTY_NAME );

  Constraint projectionMatrixConstraint = Constraint::New<Dali::Matrix>( actor, projectionMatrixPropertyIndex, EqualToConstraint() );
  projectionMatrixConstraint.AddSource( Source( camera, CameraActor::Property::PROJECTION_MATRIX ) );
  Constraint viewMatrixConstraint = Constraint::New<Dali::Matrix>( actor, viewMatrixPropertyIndex, EqualToConstraint() );
  viewMatrixConstraint.AddSource( Source( camera, CameraActor::Property::VIEW_MATRIX ) );

  projectionMatrixConstraint.Apply();
  viewMatrixConstraint.Apply();

  application.SendNotification();
  application.Render();

  // Test effects of Constraint.
  DALI_TEST_CHECK( application.GetGlAbstraction().CheckUniformValue( SHADER_LIGHT_CAMERA_PROJECTION_MATRIX_PROPERTY_NAME.c_str(), projectionMatrix) );

  DALI_TEST_CHECK( application.GetGlAbstraction().CheckUniformValue( SHADER_LIGHT_CAMERA_VIEW_MATRIX_PROPERTY_NAME.c_str(), viewMatrix ) );
  END_TEST;
}

int UtcDaliCameraActorAnimatedProperties(void)
{
  TestApplication application;
  tet_infoline( "Testing Dali::Internal::CameraActor::GetSceneObjectAnimatableProperty()" );

  CameraActor camera = Stage::GetCurrent().GetRenderTaskList().GetTask( 0u ).GetCameraActor();
  Actor actor = Actor::New();
  actor.SetSize( 100.0f, 100.0f );
  Stage::GetCurrent().Add( actor );

  Constraint constraint = Constraint::New<Dali::Vector3>( actor, Actor::Property::POSITION, EqualToConstraint() );
  constraint.AddSource( Source( camera, Actor::Property::POSITION ) );
  constraint.Apply();

  camera.SetPosition( 100.0f, 200.0f, 300.0f );
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3( 100.0f, 200.0f, 300.0f ), TEST_LOCATION);
  END_TEST;
}

int UtcDaliCameraActorPropertyIndices(void)
{
  TestApplication application;
  CameraActor camera = Stage::GetCurrent().GetRenderTaskList().GetTask( 0u ).GetCameraActor();

  Actor basicActor = Actor::New();
  Property::IndexContainer indices;
  camera.GetPropertyIndices( indices );
  DALI_TEST_CHECK( indices.Size() > basicActor.GetPropertyCount() );
  DALI_TEST_EQUALS( indices.Size(), camera.GetPropertyCount(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliCameraActorCheckLookAtAndFreeLookViews01(void)
{
  TestApplication application;
  Stage stage = Stage::GetCurrent();
  Vector2 stageSize = stage.GetSize();

  CameraActor freeLookCameraActor = CameraActor::New( stageSize );
  freeLookCameraActor.SetParentOrigin( ParentOrigin::CENTER );
  freeLookCameraActor.SetType( Camera::FREE_LOOK );

  Vector3 targetPosition( 30.0f, 240.0f, -256.0f );
  Actor target = Actor::New();
  target.SetParentOrigin( ParentOrigin::CENTER );
  target.SetPosition( targetPosition );

  Constraint cameraOrientationConstraint = Constraint::New<Quaternion> ( freeLookCameraActor, Actor::Property::ORIENTATION, &LookAt );
  cameraOrientationConstraint.AddSource( Source( target, Actor::Property::WORLD_POSITION ) );
  cameraOrientationConstraint.AddSource( Source( freeLookCameraActor,  Actor::Property::WORLD_POSITION ) );
  cameraOrientationConstraint.AddSource( Source( target, Actor::Property::WORLD_ORIENTATION ) );
  cameraOrientationConstraint.Apply();

  CameraActor lookAtCameraActor = CameraActor::New( stageSize );
  lookAtCameraActor.SetType( Camera::LOOK_AT_TARGET );
  lookAtCameraActor.SetTargetPosition( targetPosition );
  lookAtCameraActor.SetParentOrigin( ParentOrigin::CENTER );

  stage.Add( target );
  stage.Add( freeLookCameraActor );
  stage.Add( lookAtCameraActor );

  // Create an arbitrary vector
  for( float x=-1.0f; x<=1.0f; x+=0.1f )
  {
    for( float y=-1.0f; y<1.0f; y+=0.1f )
    {
      for( float z=-1.0f; z<1.0f; z+=0.1f )
      {
        Vector3 position( x, y, z );
        position.Normalize();
        position *= 200.0f;

        freeLookCameraActor.SetPosition( position );
        lookAtCameraActor.SetPosition( position );

        application.SendNotification();
        application.Render();
        application.SendNotification();
        application.Render();
        Matrix freeLookViewMatrix;
        Matrix lookAtViewMatrix;
        freeLookCameraActor.GetProperty( CameraActor::CameraActor::Property::VIEW_MATRIX ).Get( freeLookViewMatrix );
        lookAtCameraActor.GetProperty( CameraActor::CameraActor::Property::VIEW_MATRIX ).Get( lookAtViewMatrix );

        DALI_TEST_EQUALS( freeLookViewMatrix, lookAtViewMatrix, 0.01, TEST_LOCATION );
      }
    }
  }
  END_TEST;
}

int UtcDaliCameraActorCheckLookAtAndFreeLookViews02(void)
{
  TestApplication application;
  Stage stage = Stage::GetCurrent();
  Vector2 stageSize = stage.GetSize();

  CameraActor freeLookCameraActor = CameraActor::New( stageSize );
  freeLookCameraActor.SetParentOrigin( ParentOrigin::CENTER );
  freeLookCameraActor.SetType( Camera::FREE_LOOK );

  Vector3 targetPosition( 30.0f, 240.0f, -256.0f );
  Actor target = Actor::New();
  target.SetParentOrigin( ParentOrigin::CENTER );
  target.SetPosition( targetPosition );

  Constraint cameraOrientationConstraint = Constraint::New<Quaternion> ( freeLookCameraActor, Actor::Property::ORIENTATION, &LookAt );
  cameraOrientationConstraint.AddSource( Source( target, Actor::Property::WORLD_POSITION ) );
  cameraOrientationConstraint.AddSource( Source( freeLookCameraActor,  Actor::Property::WORLD_POSITION ) );
  cameraOrientationConstraint.AddSource( Source( target, Actor::Property::WORLD_ORIENTATION ) );
  cameraOrientationConstraint.Apply();

  CameraActor lookAtCameraActor = CameraActor::New( stageSize );
  lookAtCameraActor.SetType( Camera::LOOK_AT_TARGET );
  lookAtCameraActor.SetTargetPosition( targetPosition );
  lookAtCameraActor.SetParentOrigin( ParentOrigin::CENTER );

  stage.Add( target );
  stage.Add( freeLookCameraActor );
  stage.Add( lookAtCameraActor );
  stage.GetRenderTaskList().GetTask(0).SetCameraActor( freeLookCameraActor );

  // Create an arbitrary vector
  for( float x=-1.0f; x<=1.0f; x+=0.1f )
  {
    for( float y=-1.0f; y<1.0f; y+=0.1f )
    {
      for( float z=-1.0f; z<1.0f; z+=0.1f )
      {
        Vector3 position( x, y, z );
        position.Normalize();
        position *= 200.0f;

        freeLookCameraActor.SetPosition( position );
        lookAtCameraActor.SetPosition( position );

        application.SendNotification();
        application.Render();
        application.SendNotification();
        application.Render();
        Matrix freeLookViewMatrix;
        freeLookCameraActor.GetProperty( CameraActor::CameraActor::Property::VIEW_MATRIX ).Get( freeLookViewMatrix );

        Matrix freeLookWorld = freeLookCameraActor.GetCurrentWorldMatrix();

        Matrix freeLookTest( false );
        Matrix::Multiply( freeLookTest, freeLookViewMatrix, freeLookWorld );
        DALI_TEST_EQUALS( freeLookTest, Matrix::IDENTITY, 0.01f, TEST_LOCATION );
      }
    }
  }

  END_TEST;
}

int UtcDaliCameraActorCheckLookAtAndFreeLookViews03(void)
{
  TestApplication application;
  Stage stage = Stage::GetCurrent();
  Vector2 stageSize = stage.GetSize();

  Vector3 targetPosition( Vector3::ZERO );
  Vector3 cameraOffset( 0.0f, 0.0f, 100.0f );

  CameraActor freeLookCameraActor = CameraActor::New( stageSize );
  freeLookCameraActor.SetType( Camera::FREE_LOOK );
  freeLookCameraActor.SetParentOrigin( ParentOrigin::CENTER );

  Quaternion cameraOrientation( Radian( Degree( 180.0f ) ), Vector3::YAXIS );
  freeLookCameraActor.SetPosition( cameraOffset );
  freeLookCameraActor.SetOrientation( cameraOrientation );

  Actor cameraAnchor = Actor::New();
  cameraAnchor.Add( freeLookCameraActor );
  stage.Add( cameraAnchor );
  stage.GetRenderTaskList().GetTask(0).SetCameraActor( freeLookCameraActor );

  for( float angle = 1.0f; angle <= 180.0f; angle += 1.0f )
  {
    Quaternion rotation( Radian( Degree( angle ) ), Vector3::YAXIS );

    freeLookCameraActor.SetPosition( rotation.Rotate( cameraOffset ) );
    cameraAnchor.SetOrientation( rotation );

    application.SendNotification();
    application.Render();
    application.SendNotification();
    application.Render();

    Matrix freeLookViewMatrix;
    freeLookCameraActor.GetProperty( CameraActor::CameraActor::Property::VIEW_MATRIX ).Get( freeLookViewMatrix );

    Matrix freeLookWorld = freeLookCameraActor.GetCurrentWorldMatrix();

    Matrix freeLookTest( false );
    Matrix::Multiply( freeLookTest,  freeLookViewMatrix,  freeLookWorld );
    DALI_TEST_EQUALS( freeLookTest, Matrix::IDENTITY, 0.01f, TEST_LOCATION );
  }
  END_TEST;
}
