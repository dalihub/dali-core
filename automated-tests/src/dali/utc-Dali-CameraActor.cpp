/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

#include <dali/public-api/dali-core.h>

#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/actors/camera-actor-devel.h>
#include <stdlib.h>

#include <cmath>
#include <iostream>

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
const float FLOAT_EPSILON            = 0.001f;
const float TEST_ASPECT_RATIO        = 0.123f;
const float TEST_FIELD_OF_VIEW       = Radian(Degree(40.0f));
const float TEST_NEAR_PLANE_DISTANCE = 0.23f;
const float TEST_FAR_PLANE_DISTANCE  = 0.973f;

const std::string SHADER_LIGHT_CAMERA_PROJECTION_MATRIX_PROPERTY_NAME("uLightCameraProjectionMatrix");
const std::string SHADER_LIGHT_CAMERA_VIEW_MATRIX_PROPERTY_NAME("uLightCameraViewMatrix");
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

struct PropertyDetails
{
  std::string     name;            ///< The name of the property.
  Property::Type  type;            ///< The property type.
  bool            writable;        ///< Whether the property is writable
  bool            animatable;      ///< Whether the property is animatable.
  bool            constraintInput; ///< Whether the property can be used as an input to a constraint.
  Property::Index enumIndex;       ///< Used to check the index is correct within a debug build.
};

} // Anonymous namespace

int UtcDaliCameraActorConstructorP(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor::CameraActor()");

  CameraActor actor;

  DALI_TEST_CHECK(!actor);
  END_TEST;
}

// Note: No negative test for UtcDaliCameraActorConstructor.

int UtcDaliCameraActorDestructorP(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::~CameraActor (P)");
  CameraActor* actor = new CameraActor();
  delete actor;
  actor = NULL;

  DALI_TEST_CHECK(true);
  END_TEST;
}

// Note: No negative test for UtcDaliCameraActorDestructor.

int UtcDaliCameraActorCopyConstructorP(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor Copy Constructor (P)");
  CameraActor actor = CameraActor::New();

  CameraActor copyActor(actor);

  DALI_TEST_CHECK(copyActor);
  DALI_TEST_CHECK(copyActor == actor);

  END_TEST;
}

int UtcDaliCameraActorCopyConstructorN(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor Copy Constructor (N)");
  CameraActor actor;

  CameraActor copyActor(actor);

  DALI_TEST_CHECK(!copyActor);

  END_TEST;
}

int UtcDaliCameraActorAssignmentOperatorP(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor Assignment Operator (P)");
  const CameraActor actor = CameraActor::New();

  CameraActor copyActor = actor;

  DALI_TEST_CHECK(copyActor);
  DALI_TEST_CHECK(copyActor == actor);

  END_TEST;
}

int UtcDaliCameraActorAssignmentOperatorN(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor = (N)");
  CameraActor actor;

  CameraActor copyActor = actor;

  DALI_TEST_CHECK(!copyActor);

  END_TEST;
}

int UtcDaliCameraActorNewP(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor::New (P)");

  CameraActor actor = CameraActor::New();

  DALI_TEST_CHECK(actor);

  actor.Reset();

  DALI_TEST_CHECK(!actor);
  END_TEST;
}

int UtcDaliCameraActorNewDefaultPerspectiveProjection(void)
{
  TestApplication application;
  tet_infoline("Test the perspective projection of a camera actor is set appropriately when not passing in a size");

  CameraActor actor = CameraActor::New();
  DALI_TEST_CHECK(actor);

  // All the properties should still be the default values
  // Defaults taken from scene-graph-camera.cpp
  DALI_TEST_EQUALS(800.0f / 480.0f, actor.GetProperty(CameraActor::Property::ASPECT_RATIO).Get<float>(), FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(45.0f * (Math::PI / 180.0f), actor.GetProperty(CameraActor::Property::FIELD_OF_VIEW).Get<float>(), FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(800.0f, actor.GetProperty(CameraActor::Property::NEAR_PLANE_DISTANCE).Get<float>(), FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(3.0f * 800.0f, actor.GetProperty(CameraActor::Property::FAR_PLANE_DISTANCE).Get<float>(), FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(0.0f, actor.GetProperty(Actor::Property::POSITION_Z).Get<float>(), TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetProjectionMode(), Dali::Camera::PERSPECTIVE_PROJECTION, TEST_LOCATION);

  // Add it to the stage, then the values should be updated to reflect a 480.0f by 800.0f scene (default stage size)
  application.GetScene().Add(actor);

  DALI_TEST_EQUALS(0.6f, actor.GetProperty(CameraActor::Property::ASPECT_RATIO).Get<float>(), FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(0.489957f, actor.GetProperty(CameraActor::Property::FIELD_OF_VIEW).Get<float>(), FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(800.0f, actor.GetProperty(CameraActor::Property::NEAR_PLANE_DISTANCE).Get<float>(), FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(4895.0f, actor.GetProperty(CameraActor::Property::FAR_PLANE_DISTANCE).Get<float>(), FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(1600.0f, actor.GetProperty(Actor::Property::POSITION_Z).Get<float>(), TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetProjectionMode(), Dali::Camera::PERSPECTIVE_PROJECTION, TEST_LOCATION);

  // Ensure the values stay the same after update/render
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(0.6f, actor.GetProperty(CameraActor::Property::ASPECT_RATIO).Get<float>(), FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(0.489957f, actor.GetProperty(CameraActor::Property::FIELD_OF_VIEW).Get<float>(), FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(800.0f, actor.GetProperty(CameraActor::Property::NEAR_PLANE_DISTANCE).Get<float>(), FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(4895.0f, actor.GetProperty(CameraActor::Property::FAR_PLANE_DISTANCE).Get<float>(), FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(1600.0f, actor.GetProperty(Actor::Property::POSITION_Z).Get<float>(), TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetProjectionMode(), Dali::Camera::PERSPECTIVE_PROJECTION, TEST_LOCATION);

  END_TEST;
}

// Note: No negative test for UtcDaliCameraActorNew.

int UtcDaliCameraActorDownCastP(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor::DownCast (P)");

  CameraActor camera  = CameraActor::New();
  Actor       anActor = Actor::New();
  anActor.Add(camera);

  Actor       child       = anActor.GetChildAt(0);
  CameraActor cameraActor = CameraActor::DownCast(child);
  DALI_TEST_CHECK(cameraActor);

  cameraActor.Reset();
  DALI_TEST_CHECK(!cameraActor);

  cameraActor = DownCast<CameraActor>(child);
  DALI_TEST_CHECK(cameraActor);
  END_TEST;
}

int UtcDaliCameraActorDownCastN(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor::DownCast (N)");

  Actor actor1  = Actor::New();
  Actor anActor = Actor::New();
  anActor.Add(actor1);

  Actor       child       = anActor.GetChildAt(0);
  CameraActor cameraActor = CameraActor::DownCast(child);
  DALI_TEST_CHECK(!cameraActor);

  Actor unInitialzedActor;
  cameraActor = CameraActor::DownCast(unInitialzedActor);
  DALI_TEST_CHECK(!cameraActor);

  cameraActor = DownCast<CameraActor>(unInitialzedActor);
  DALI_TEST_CHECK(!cameraActor);
  END_TEST;
}

// Note: SetType and GetType are tested within the same test cases.

int UtcDaliCameraActorSetGetTypeP(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor GetType (P)");

  CameraActor actor = CameraActor::New();
  DALI_TEST_EQUALS(actor.GetType(), Dali::Camera::FREE_LOOK, TEST_LOCATION);

  actor.SetType(Dali::Camera::LOOK_AT_TARGET);
  DALI_TEST_EQUALS(actor.GetType(), Dali::Camera::LOOK_AT_TARGET, TEST_LOCATION);

  Dali::Camera::Type cameraType = actor.GetProperty<Dali::Camera::Type>(CameraActor::Property::TYPE);
  Dali::Camera::Type currentCameraType = actor.GetCurrentProperty<Dali::Camera::Type>(CameraActor::Property::TYPE);
  DALI_TEST_EQUALS(Camera::LOOK_AT_TARGET, cameraType, TEST_LOCATION);
  DALI_TEST_EQUALS(Camera::LOOK_AT_TARGET, currentCameraType, TEST_LOCATION);
  END_TEST;
}

int UtcDaliCameraActorSetGetTypeN(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor GetType (N)");

  CameraActor actor;

  Dali::Camera::Type cameraType = Dali::Camera::FREE_LOOK;
  try
  {
    cameraType = actor.GetType();
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "camera", TEST_LOCATION);
  }

  const CameraActor aConstActor;

  try
  {
    cameraType = aConstActor.GetType();
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "camera", TEST_LOCATION);
  }

  DALI_TEST_EQUALS(cameraType, Dali::Camera::FREE_LOOK, TEST_LOCATION);
  END_TEST;
}

int UtcDaliCameraActorSetFieldOfViewP(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor Set Field of view (P)");

  CameraActor defaultCamera      = CameraActor::New(Size(TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT));
  const float defaultFieldOfView = defaultCamera.GetFieldOfView();

  CameraActor actor = CameraActor::New(Size(TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT));
  DALI_TEST_EQUALS(actor.GetFieldOfView(), defaultFieldOfView, TEST_LOCATION);

  float fieldOfView = Math::PI / 3.0f;
  actor.SetFieldOfView(fieldOfView);
  DALI_TEST_EQUALS(actor.GetFieldOfView(), fieldOfView, TEST_LOCATION);

  float value;
  actor.GetProperty(CameraActor::Property::FIELD_OF_VIEW).Get(value);
  DALI_TEST_EQUALS(fieldOfView, value, FLOAT_EPSILON, TEST_LOCATION);
  END_TEST;
}

int UtcDaliCameraActorSetFieldOfViewN(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor Set Field of view (N)");

  CameraActor defaultCamera      = CameraActor::New(Size(TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT));
  const float defaultFieldOfView = defaultCamera.GetFieldOfView();

  CameraActor actor = CameraActor::New(Size(TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT));
  DALI_TEST_EQUALS(actor.GetFieldOfView(), defaultFieldOfView, TEST_LOCATION);

  float fieldOfView = Math::PI / 3.0f;
  actor.SetFieldOfView(fieldOfView);
  DALI_TEST_EQUALS(actor.GetFieldOfView(), fieldOfView, TEST_LOCATION);

  float value;
  actor.GetProperty(CameraActor::Property::FIELD_OF_VIEW).Get(value);
  DALI_TEST_EQUALS(fieldOfView, value, FLOAT_EPSILON, TEST_LOCATION);
  END_TEST;
}

int UtcDaliCameraActorGetFieldOfViewP(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor Get Field of view (P)");
  const Vector2 size(TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT);

  CameraActor defaultCamera = CameraActor::New(size);

  const float cameraZ             = 2.0f * std::max(size.width, size.height);
  const float expectedFieldOfView = 2.0f * std::atan(size.height * 0.5f / cameraZ);

  CameraActor actor = CameraActor::New(size);
  DALI_TEST_EQUALS(actor.GetFieldOfView(), expectedFieldOfView, TEST_LOCATION);

  float value;
  actor.GetProperty(CameraActor::Property::FIELD_OF_VIEW).Get(value);
  DALI_TEST_EQUALS(expectedFieldOfView, value, FLOAT_EPSILON, TEST_LOCATION);
  END_TEST;
}

int UtcDaliCameraActorGetFieldOfViewN(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor Get Field of view (N)");

  CameraActor defaultCamera = CameraActor::New();

  bool asserted = true;
  try
  {
    defaultCamera.GetFieldOfView();
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "camera && \"Camera handle is empty\"", TEST_LOCATION);
    asserted = true;
  }
  DALI_TEST_CHECK(asserted);

  END_TEST;
}

int UtcDaliCameraActorSetAspectRatioP(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor Set Aspect Ratio (P)");

  CameraActor actor = CameraActor::New(Size(TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT));
  DALI_TEST_EQUALS(actor.GetAspectRatio(), static_cast<float>(TestApplication::DEFAULT_SURFACE_WIDTH) / static_cast<float>(TestApplication::DEFAULT_SURFACE_HEIGHT), TEST_LOCATION);

  // Set an initial value to confirm a further set changes it.
  float aspect = 4.0f / 3.0f;
  actor.SetAspectRatio(aspect);
  DALI_TEST_EQUALS(actor.GetAspectRatio(), aspect, TEST_LOCATION);

  aspect = 16.0f / 9.0f;
  actor.SetAspectRatio(aspect);
  DALI_TEST_EQUALS(actor.GetAspectRatio(), aspect, TEST_LOCATION);

  END_TEST;
}

int UtcDaliCameraActorSetAspectRatioN(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor Set Aspect Ratio (N)");

  CameraActor actor;

  bool asserted = true;
  try
  {
    actor.SetAspectRatio(16.0f / 9.0f);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "camera && \"Camera handle is empty\"", TEST_LOCATION);
    asserted = true;
  }
  DALI_TEST_CHECK(asserted);

  END_TEST;
}

int UtcDaliCameraActorGetAspectRatioP(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor Get Aspect Ratio");

  CameraActor actor         = CameraActor::New(Size(TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT));
  float       defaultAspect = static_cast<float>(TestApplication::DEFAULT_SURFACE_WIDTH) / static_cast<float>(TestApplication::DEFAULT_SURFACE_HEIGHT);

  DALI_TEST_EQUALS(actor.GetAspectRatio(), defaultAspect, TEST_LOCATION);

  float value = 0.0f;
  actor.GetProperty(CameraActor::Property::ASPECT_RATIO).Get(value);
  DALI_TEST_EQUALS(defaultAspect, value, FLOAT_EPSILON, TEST_LOCATION);

  END_TEST;
}

int UtcDaliCameraActorGetAspectRatioN(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor Get Aspect Ratio (N)");

  CameraActor actor;

  bool asserted = true;
  try
  {
    actor.GetAspectRatio();
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "camera && \"Camera handle is empty\"", TEST_LOCATION);
    asserted = true;
  }

  DALI_TEST_CHECK(asserted);

  END_TEST;
}

int UtcDaliCameraActorSetNearClippingPlaneP(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor Set Near clipping plane (P)");

  CameraActor actor = CameraActor::New();

  // Set a value so we are not relying on a particular default for this test case.
  actor.SetNearClippingPlane(200.0f);
  DALI_TEST_EQUALS(actor.GetNearClippingPlane(), 200.0f, TEST_LOCATION);

  actor.SetNearClippingPlane(400.0f);
  DALI_TEST_EQUALS(actor.GetNearClippingPlane(), 400.0f, TEST_LOCATION);

  // Check setting the property.
  actor.SetProperty(CameraActor::Property::NEAR_PLANE_DISTANCE, Property::Value(300.0f));
  DALI_TEST_EQUALS(actor.GetNearClippingPlane(), 300.0f, FLOAT_EPSILON, TEST_LOCATION);
  END_TEST;
}

int UtcDaliCameraActorSetNearClippingPlaneN(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor Set Near clipping plane (N)");

  CameraActor actor;

  bool asserted = true;
  try
  {
    actor.SetNearClippingPlane(200.0f);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "camera && \"Camera handle is empty\"", TEST_LOCATION);
    asserted = true;
  }

  DALI_TEST_CHECK(asserted);

  END_TEST;
}

int UtcDaliCameraActorGetNearClippingPlaneP(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor Get Near clipping plane (P)");

  // Check the default value.
  CameraActor actor        = CameraActor::New();
  float       defaultValue = 800.0f;
  DALI_TEST_EQUALS(actor.GetNearClippingPlane(), defaultValue, TEST_LOCATION);

  // Check getting the property.
  float value;
  actor.GetProperty(CameraActor::Property::NEAR_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(defaultValue, value, FLOAT_EPSILON, TEST_LOCATION);
  END_TEST;
}

int UtcDaliCameraActorGetNearClippingPlaneN(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor Get Near clipping plane (N)");

  CameraActor actor;
  bool        asserted = true;
  try
  {
    actor.GetNearClippingPlane();
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "camera && \"Camera handle is empty\"", TEST_LOCATION);
    asserted = true;
  }

  DALI_TEST_CHECK(asserted);

  END_TEST;
}

int UtcDaliCameraActorSetFarClippingPlaneP(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor Set Far clipping plane (P)");

  CameraActor actor = CameraActor::New();

  // Set a value so we are not relying on a particular default for this test case.
  actor.SetFarClippingPlane(2000.0f);
  DALI_TEST_EQUALS(actor.GetFarClippingPlane(), 2000.0f, TEST_LOCATION);

  actor.SetFarClippingPlane(4000.0f);
  DALI_TEST_EQUALS(actor.GetFarClippingPlane(), 4000.0f, TEST_LOCATION);

  // Check setting the property.
  actor.SetProperty(CameraActor::Property::FAR_PLANE_DISTANCE, 2000.0f);
  DALI_TEST_EQUALS(actor.GetFarClippingPlane(), 2000.0f, FLOAT_EPSILON, TEST_LOCATION);
  END_TEST;
}

int UtcDaliCameraActorSetFarClippingPlaneN(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor Set Far clipping plane (N)");

  CameraActor actor;

  bool asserted = true;
  try
  {
    actor.SetFarClippingPlane(2000.0f);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "camera && \"Camera handle is empty\"", TEST_LOCATION);
    asserted = true;
  }

  DALI_TEST_CHECK(asserted);

  END_TEST;
}

int UtcDaliCameraActorGetFarClippingPlaneP(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor Get Far clipping plane (P)");

  CameraActor actor        = CameraActor::New(Size(TestApplication::DEFAULT_SURFACE_WIDTH, TestApplication::DEFAULT_SURFACE_HEIGHT));
  float       defaultValue = 800.0f + (0xFFFF >> 4);
  DALI_TEST_EQUALS(actor.GetFarClippingPlane(), defaultValue, TEST_LOCATION);

  // Check getting the property.
  float value;
  actor.GetProperty(CameraActor::Property::FAR_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(defaultValue, value, FLOAT_EPSILON, TEST_LOCATION);
  END_TEST;
}

int UtcDaliCameraActorGetFarClippingPlaneN(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor Get Far clipping plane (N)");

  CameraActor actor;

  bool asserted = true;
  try
  {
    actor.GetFarClippingPlane();
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "camera && \"Camera handle is empty\"", TEST_LOCATION);
    asserted = true;
  }

  DALI_TEST_CHECK(asserted);

  END_TEST;
}

int UtcDaliCameraActorSetTargetPositionP(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor Set Target Position (P)");

  CameraActor actor = CameraActor::New();

  Vector3 target1(10.0f, 20.0f, 30.0f);
  Vector3 target2(15.0f, 25.0f, 35.0f);

  // Set a value so we are not relying on a particular default for this test case.
  actor.SetTargetPosition(target1);
  DALI_TEST_EQUALS(actor.GetTargetPosition(), target1, TEST_LOCATION);

  actor.SetTargetPosition(target2);
  DALI_TEST_EQUALS(actor.GetTargetPosition(), target2, TEST_LOCATION);

  // Check setting the property.
  actor.SetProperty(CameraActor::Property::TARGET_POSITION, target1);
  DALI_TEST_EQUALS(actor.GetTargetPosition(), target1, FLOAT_EPSILON, TEST_LOCATION);
  END_TEST;
}

int UtcDaliCameraActorSetTargetPositionN(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor Set Target Position (N)");

  CameraActor actor;

  bool asserted = true;
  try
  {
    actor.SetTargetPosition(Vector3(10.0f, 20.0f, 30.0f));
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "camera && \"Camera handle is empty\"", TEST_LOCATION);
    asserted = true;
  }
  DALI_TEST_CHECK(asserted);

  END_TEST;
}

int UtcDaliCameraActorGetTargetPositionP(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor Get Target Position (P)");

  CameraActor actor = CameraActor::New();
  Vector3     defaultValue(Vector3::ZERO);
  DALI_TEST_EQUALS(actor.GetTargetPosition(), defaultValue, TEST_LOCATION);

  // Check getting the property.
  Vector3 value;
  actor.GetProperty(CameraActor::Property::TARGET_POSITION).Get(value);
  DALI_TEST_EQUALS(defaultValue, value, FLOAT_EPSILON, TEST_LOCATION);
  END_TEST;
}

int UtcDaliCameraActorGetTargetPositionN(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor Get Target Position (N)");

  CameraActor actor;

  bool asserted = true;
  try
  {
    actor.GetTargetPosition();
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "camera && \"Camera handle is empty\"", TEST_LOCATION);
    asserted = true;
  }
  DALI_TEST_CHECK(asserted);

  END_TEST;
}

int UtcDaliCameraActorSetInvertYAxisP(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor Set InvertYAxis (P)");

  CameraActor actor = CameraActor::New();

  // Set a value so we are not relying on a particular default for this test case.
  actor.SetInvertYAxis(false);
  DALI_TEST_EQUALS(actor.GetInvertYAxis(), false, TEST_LOCATION);

  actor.SetInvertYAxis(true);
  DALI_TEST_EQUALS(actor.GetInvertYAxis(), true, TEST_LOCATION);

  actor.SetProperty(CameraActor::Property::INVERT_Y_AXIS, false);
  DALI_TEST_EQUALS(actor.GetInvertYAxis(), false, TEST_LOCATION);
  END_TEST;
}

int UtcDaliCameraActorSetInvertYAxisN(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor Set InvertYAxis (N)");

  CameraActor actor;

  bool asserted = true;
  try
  {
    actor.SetInvertYAxis(false);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "camera && \"Camera handle is empty\"", TEST_LOCATION);
    asserted = true;
  }
  DALI_TEST_CHECK(asserted);
  END_TEST;
}

int UtcDaliCameraActorGetInvertYAxisP(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor Get InvertYAxis (P)");

  // Check the default value.
  CameraActor actor = CameraActor::New();
  DALI_TEST_EQUALS(actor.GetInvertYAxis(), false, TEST_LOCATION);

  // Check getting the property.
  bool bValue;
  actor.GetProperty(CameraActor::Property::INVERT_Y_AXIS).Get(bValue);
  DALI_TEST_EQUALS(false, bValue, TEST_LOCATION);
  END_TEST;
}

int UtcDaliCameraActorGetInvertYAxisN(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor Get InvertYAxis (N)");

  CameraActor actor;

  bool asserted = true;
  try
  {
    actor.GetInvertYAxis();
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "camera && \"Camera handle is empty\"", TEST_LOCATION);
    asserted = true;
  }
  DALI_TEST_CHECK(asserted);
  END_TEST;
}

int UtcDaliCameraActorSetPerspectiveProjectionP(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor::SetPerspectiveProjection (P)");

  CameraActor actor = CameraActor::New();
  actor.SetPerspectiveProjection(Size(100.f, 150.f));

  DALI_TEST_CHECK(actor);

  float value;
  actor.GetProperty(CameraActor::Property::ASPECT_RATIO).Get(value);
  DALI_TEST_EQUALS(0.666666f, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::Property::FIELD_OF_VIEW).Get(value);
  DALI_TEST_EQUALS(0.489957f, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::Property::NEAR_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(150.f, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::Property::FAR_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(4245.f, value, FLOAT_EPSILON, TEST_LOCATION);

  DALI_TEST_EQUALS(actor.GetProjectionMode(), Dali::Camera::PERSPECTIVE_PROJECTION, TEST_LOCATION);

  // Ensure these values persist after adding to the stage and an update/render pass
  application.GetScene().Add(actor);
  application.SendNotification();
  application.Render();

  actor.GetProperty(CameraActor::Property::ASPECT_RATIO).Get(value);
  DALI_TEST_EQUALS(0.666666f, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::Property::FIELD_OF_VIEW).Get(value);
  DALI_TEST_EQUALS(0.489957f, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::Property::NEAR_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(150.f, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::Property::FAR_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(4245.f, value, FLOAT_EPSILON, TEST_LOCATION);

  // Call method with a ZERO size, this should reset the perspective projection using the size of the scene we've been added to
  actor.SetPerspectiveProjection(Vector2::ZERO);

  DALI_TEST_EQUALS(0.6f, actor.GetProperty(CameraActor::Property::ASPECT_RATIO).Get<float>(), FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(0.489957f, actor.GetProperty(CameraActor::Property::FIELD_OF_VIEW).Get<float>(), FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(800.0f, actor.GetProperty(CameraActor::Property::NEAR_PLANE_DISTANCE).Get<float>(), FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(4895.0f, actor.GetProperty(CameraActor::Property::FAR_PLANE_DISTANCE).Get<float>(), FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(1600.0f, actor.GetProperty(Actor::Property::POSITION_Z).Get<float>(), TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetProjectionMode(), Dali::Camera::PERSPECTIVE_PROJECTION, TEST_LOCATION);

  END_TEST;
}

int UtcDaliCameraActorSetPerspectiveProjectionN(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor::SetPerspectiveProjection (N)");

  CameraActor actor = CameraActor::New();

  // Check that setting perspective projection without a size does not do anything.
  actor.SetPerspectiveProjection(Size::ZERO);

  // So the default values should be the same as defined in CameraActor
  float nearClippingPlane = 800.0f;
  float farClippingPlane  = nearClippingPlane + 2.0f * nearClippingPlane;

  DALI_TEST_EQUALS(nearClippingPlane, actor.GetNearClippingPlane(), FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(farClippingPlane, actor.GetFarClippingPlane(), FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetProjectionMode(), Dali::Camera::PERSPECTIVE_PROJECTION, TEST_LOCATION);

  END_TEST;
}

int UtcDaliCameraActorSetOrthographicProjectionP1(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor::SetOrthographicProjection (P,1)");

  CameraActor actor = CameraActor::New(Size(1080.0f, 1920.0f));
  DALI_TEST_CHECK(actor);

  application.GetScene().Add(actor);

  actor.SetOrthographicProjection(Size(1080.0f, 1920.0f));
  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();

  float defaultAspectRatio;
  float defaultFieldOfView;
  float defaultNearPlaneDistance;
  float defaultFarPlaneDistance;
  actor.GetProperty(CameraActor::Property::ASPECT_RATIO).Get(defaultAspectRatio);
  actor.GetProperty(CameraActor::Property::FIELD_OF_VIEW).Get(defaultFieldOfView);
  actor.GetProperty(CameraActor::Property::NEAR_PLANE_DISTANCE).Get(defaultNearPlaneDistance);
  actor.GetProperty(CameraActor::Property::FAR_PLANE_DISTANCE).Get(defaultFarPlaneDistance);
  Vector3 defaultPos = actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION);

  actor.SetOrthographicProjection(Size(1080.0f, 1920.0f));

  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();

  float value;
  actor.GetProperty(CameraActor::Property::ASPECT_RATIO).Get(value);
  DALI_TEST_EQUALS(defaultAspectRatio, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::Property::FIELD_OF_VIEW).Get(value);
  DALI_TEST_EQUALS(defaultFieldOfView, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::Property::NEAR_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(defaultNearPlaneDistance, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::Property::FAR_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(defaultFarPlaneDistance, value, FLOAT_EPSILON, TEST_LOCATION);

  actor.GetProperty(CameraActor::Property::LEFT_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(-540.0f, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::Property::RIGHT_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(540.0f, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::Property::TOP_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(960.0f, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::Property::BOTTOM_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(-960.0f, value, FLOAT_EPSILON, TEST_LOCATION);

  Vector3 pos = actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION);
  DALI_TEST_EQUALS(defaultPos.z, pos.z, 0.001f, TEST_LOCATION);

  DALI_TEST_EQUALS(actor.GetProjectionMode(), Dali::Camera::ORTHOGRAPHIC_PROJECTION, TEST_LOCATION);
  END_TEST;
}

int UtcDaliCameraActorSetOrthographicProjectionN(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor::SetOrthographicProjection (N)");

  CameraActor actor;
  bool        asserted = true;
  try
  {
    actor.GetProjectionMode();
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "camera && \"Camera handle is empty\"", TEST_LOCATION);
    asserted = true;
  }
  DALI_TEST_CHECK(asserted);
  END_TEST;
}

int UtcDaliCameraActorSetOrthographicProjectionP2(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor::SetOrthographicProjection (P,2)");

  CameraActor actor = CameraActor::New();
  DALI_TEST_CHECK(actor);

  float defaultAspectRatio;
  float defaultFieldOfView;
  float defaultNearPlaneDistance;
  float defaultFarPlaneDistance;
  actor.GetProperty(CameraActor::Property::ASPECT_RATIO).Get(defaultAspectRatio);
  actor.GetProperty(CameraActor::Property::FIELD_OF_VIEW).Get(defaultFieldOfView);
  actor.GetProperty(CameraActor::Property::NEAR_PLANE_DISTANCE).Get(defaultNearPlaneDistance);
  actor.GetProperty(CameraActor::Property::FAR_PLANE_DISTANCE).Get(defaultFarPlaneDistance);

  // Check setting with specific near and far plane distances.
  actor.SetOrthographicProjection(-100.0f, 200.0f, -300.0f, 500.0f, 400.0f, 4000.0f);

  float value;
  actor.GetProperty(CameraActor::Property::ASPECT_RATIO).Get(value);
  DALI_TEST_EQUALS(defaultAspectRatio, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::Property::FIELD_OF_VIEW).Get(value);
  DALI_TEST_EQUALS(defaultFieldOfView, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::Property::NEAR_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(400.0f, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::Property::FAR_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(4000.0f, value, FLOAT_EPSILON, TEST_LOCATION);

  actor.GetProperty(CameraActor::Property::LEFT_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(-100.0f, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::Property::RIGHT_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(200.0f, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::Property::TOP_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(-300.0f, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::Property::BOTTOM_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(500.0f, value, FLOAT_EPSILON, TEST_LOCATION);

  DALI_TEST_EQUALS(actor.GetProjectionMode(), Dali::Camera::ORTHOGRAPHIC_PROJECTION, TEST_LOCATION);

  END_TEST;
}

int UtcDaliCameraActorSetOrthographicProjectionP3(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor::SetOrthographicProjection (P,3)");

  CameraActor actor = CameraActor::New();
  DALI_TEST_CHECK(actor);

  float defaultAspectRatio;
  float defaultFieldOfView;
  float defaultNearPlaneDistance;
  float defaultFarPlaneDistance;
  actor.GetProperty(CameraActor::Property::ASPECT_RATIO).Get(defaultAspectRatio);
  actor.GetProperty(CameraActor::Property::FIELD_OF_VIEW).Get(defaultFieldOfView);
  actor.GetProperty(CameraActor::Property::NEAR_PLANE_DISTANCE).Get(defaultNearPlaneDistance);
  actor.GetProperty(CameraActor::Property::FAR_PLANE_DISTANCE).Get(defaultFarPlaneDistance);

  actor.SetProjectionMode(Dali::Camera::ORTHOGRAPHIC_PROJECTION);

  actor.SetProperty(CameraActor::Property::LEFT_PLANE_DISTANCE, -100.0f);
  actor.SetProperty(CameraActor::Property::RIGHT_PLANE_DISTANCE, 200.0f);
  actor.SetProperty(CameraActor::Property::TOP_PLANE_DISTANCE, -300.0f);
  actor.SetProperty(CameraActor::Property::BOTTOM_PLANE_DISTANCE, 500.0f);
  actor.SetNearClippingPlane(400.0f);
  actor.SetFarClippingPlane(4000.0f);

  float value;
  actor.GetProperty(CameraActor::Property::ASPECT_RATIO).Get(value);
  DALI_TEST_EQUALS(defaultAspectRatio, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::Property::FIELD_OF_VIEW).Get(value);
  DALI_TEST_EQUALS(defaultFieldOfView, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::Property::NEAR_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(400.0f, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::Property::FAR_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(4000.0f, value, FLOAT_EPSILON, TEST_LOCATION);

  actor.GetProperty(CameraActor::Property::LEFT_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(-100.0f, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::Property::RIGHT_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(200.0f, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::Property::TOP_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(-300.0f, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::Property::BOTTOM_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(500.0f, value, FLOAT_EPSILON, TEST_LOCATION);

  DALI_TEST_EQUALS(actor.GetProjectionMode(), Dali::Camera::ORTHOGRAPHIC_PROJECTION, TEST_LOCATION);

  Dali::Camera::ProjectionMode projectionMode = actor.GetProperty<Dali::Camera::ProjectionMode>(CameraActor::Property::PROJECTION_MODE);
  DALI_TEST_EQUALS(projectionMode, Dali::Camera::ORTHOGRAPHIC_PROJECTION, TEST_LOCATION);
  END_TEST;
}

int UtcDaliCameraActorSetProjectionModeP(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor::SetProjectionModeP (P)");

  CameraActor actor = CameraActor::New();

  // Check that changing the projection mode alone does not alter other presets.
  actor.SetNearClippingPlane(200.0f);
  actor.SetFarClippingPlane(400.0f);

  actor.SetProjectionMode(Dali::Camera::PERSPECTIVE_PROJECTION);

  DALI_TEST_EQUALS(actor.GetProjectionMode(), Dali::Camera::PERSPECTIVE_PROJECTION, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetNearClippingPlane(), 200.0f, FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetFarClippingPlane(), 400.0f, FLOAT_EPSILON, TEST_LOCATION);

  actor.SetProjectionMode(Dali::Camera::ORTHOGRAPHIC_PROJECTION);

  DALI_TEST_EQUALS(actor.GetProjectionMode(), Dali::Camera::ORTHOGRAPHIC_PROJECTION, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetNearClippingPlane(), 200.0f, FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetFarClippingPlane(), 400.0f, FLOAT_EPSILON, TEST_LOCATION);

  // Check setting the property.
  Property::Value setValue = Dali::Camera::PERSPECTIVE_PROJECTION;
  actor.SetProperty(CameraActor::Property::PROJECTION_MODE, setValue);
  DALI_TEST_EQUALS(actor.GetProjectionMode(), Dali::Camera::PERSPECTIVE_PROJECTION, TEST_LOCATION);
  END_TEST;
}

int UtcDaliCameraActorSetProjectionModeN(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor::SetProjectionModeP (N)");

  CameraActor actor;

  bool asserted = true;
  try
  {
    actor.SetProjectionMode(Dali::Camera::PERSPECTIVE_PROJECTION);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "camera && \"Camera handle is empty\"", TEST_LOCATION);
    asserted = true;
  }
  DALI_TEST_CHECK(asserted);
  END_TEST;
}

int UtcDaliCameraActorGetProjectionModeP(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor::GetPerspectiveProjection (P)");

  CameraActor actor = CameraActor::New();

  actor.SetOrthographicProjection(Size::ONE);
  DALI_TEST_EQUALS(actor.GetProjectionMode(), Dali::Camera::ORTHOGRAPHIC_PROJECTION, TEST_LOCATION);

  actor.SetPerspectiveProjection(Size(100.f, 150.f));
  DALI_TEST_EQUALS(actor.GetProjectionMode(), Dali::Camera::PERSPECTIVE_PROJECTION, TEST_LOCATION);

  // Check getting the property.
  Dali::Camera::ProjectionMode projectionMode = actor.GetProperty<Dali::Camera::ProjectionMode>(CameraActor::Property::PROJECTION_MODE);
  DALI_TEST_EQUALS(projectionMode, Dali::Camera::PERSPECTIVE_PROJECTION, TEST_LOCATION);
  END_TEST;
}

int UtcDaliCameraActorGetProjectionModeN(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor::GetProjectionMode (N)");

  CameraActor actor;

  bool asserted = true;
  try
  {
    actor.GetProjectionMode();
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "camera && \"Camera handle is empty\"", TEST_LOCATION);
    asserted = true;
  }
  DALI_TEST_CHECK(asserted);
  END_TEST;
}

int UtcDaliCameraActorSetCameraOffScene(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor::SetCamera()");

  CameraActor actor = CameraActor::New();

  actor.SetType(Camera::FREE_LOOK);
  actor.SetFieldOfView(TEST_FIELD_OF_VIEW);
  actor.SetAspectRatio(TEST_ASPECT_RATIO);
  actor.SetNearClippingPlane(TEST_NEAR_PLANE_DISTANCE);
  actor.SetFarClippingPlane(TEST_FAR_PLANE_DISTANCE);
  actor.SetProjectionMode(Camera::PERSPECTIVE_PROJECTION);

  actor.SetInvertYAxis(false);

  DALI_TEST_EQUALS(TEST_ASPECT_RATIO, actor.GetAspectRatio(), FLOAT_EPSILON, TEST_LOCATION); //change to machine epsilon
  DALI_TEST_EQUALS(TEST_FIELD_OF_VIEW, actor.GetFieldOfView(), FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(TEST_NEAR_PLANE_DISTANCE, actor.GetNearClippingPlane(), FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(TEST_FAR_PLANE_DISTANCE, actor.GetFarClippingPlane(), FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(Camera::PERSPECTIVE_PROJECTION, actor.GetProjectionMode(), TEST_LOCATION);
  DALI_TEST_EQUALS(false, actor.GetInvertYAxis(), TEST_LOCATION);

  float       value;
  std::string sValue;
  actor.GetProperty(CameraActor::Property::ASPECT_RATIO).Get(value);
  DALI_TEST_EQUALS(TEST_ASPECT_RATIO, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::Property::FIELD_OF_VIEW).Get(value);
  DALI_TEST_EQUALS(TEST_FIELD_OF_VIEW, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::Property::NEAR_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(TEST_NEAR_PLANE_DISTANCE, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::Property::FAR_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(TEST_FAR_PLANE_DISTANCE, value, FLOAT_EPSILON, TEST_LOCATION);

  Dali::Camera::ProjectionMode projectionMode = actor.GetProperty<Dali::Camera::ProjectionMode>(CameraActor::Property::PROJECTION_MODE);
  DALI_TEST_EQUALS(Dali::Camera::PERSPECTIVE_PROJECTION, projectionMode, TEST_LOCATION);
  bool bValue;
  actor.GetProperty(CameraActor::Property::INVERT_Y_AXIS).Get(bValue);
  DALI_TEST_EQUALS(false, bValue, TEST_LOCATION);
  END_TEST;
}

int UtcDaliCameraActorSetCameraOnScene(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor::SetCamera()");

  CameraActor actor = CameraActor::New();
  application.GetScene().Add(actor);
  application.Render(0);
  application.SendNotification();

  actor.SetType(Camera::LOOK_AT_TARGET);
  actor.SetFieldOfView(TEST_FIELD_OF_VIEW);
  actor.SetAspectRatio(TEST_ASPECT_RATIO);
  actor.SetNearClippingPlane(TEST_NEAR_PLANE_DISTANCE);
  actor.SetFarClippingPlane(TEST_FAR_PLANE_DISTANCE);
  actor.SetInvertYAxis(false);

  DALI_TEST_EQUALS(false, actor.GetInvertYAxis(), TEST_LOCATION);

  // Will need 2 frames to ensure both buffers are set to same values:
  application.Render();
  application.SendNotification();
  application.Render();
  application.SendNotification();

  DALI_TEST_EQUALS(TEST_ASPECT_RATIO, actor.GetAspectRatio(), FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(TEST_FIELD_OF_VIEW, actor.GetFieldOfView(), FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(TEST_NEAR_PLANE_DISTANCE, actor.GetNearClippingPlane(), FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(TEST_FAR_PLANE_DISTANCE, actor.GetFarClippingPlane(), FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(false, actor.GetInvertYAxis(), TEST_LOCATION);


  Dali::Camera::Type cameraType = actor.GetProperty<Dali::Camera::Type>(CameraActor::Property::TYPE);
  DALI_TEST_EQUALS(cameraType, Camera::LOOK_AT_TARGET, TEST_LOCATION);

  float value;
  actor.GetProperty(CameraActor::Property::ASPECT_RATIO).Get(value);
  DALI_TEST_EQUALS(TEST_ASPECT_RATIO, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::Property::FIELD_OF_VIEW).Get(value);
  DALI_TEST_EQUALS(TEST_FIELD_OF_VIEW, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::Property::NEAR_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(TEST_NEAR_PLANE_DISTANCE, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::Property::FAR_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(TEST_FAR_PLANE_DISTANCE, value, FLOAT_EPSILON, TEST_LOCATION);

  bool bValue;
  actor.GetProperty(CameraActor::Property::INVERT_Y_AXIS).Get(bValue);
  DALI_TEST_EQUALS(false, bValue, TEST_LOCATION);
  END_TEST;
}

int UtcDaliCameraActorGetCamera(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor::GetCamera()");

  CameraActor actor = CameraActor::New();

  actor.SetAspectRatio(TEST_ASPECT_RATIO);

  DALI_TEST_EQUALS(actor.GetAspectRatio(), TEST_ASPECT_RATIO, FLOAT_EPSILON, TEST_LOCATION);

  actor.SetProperty(CameraActor::Property::TYPE, Camera::FREE_LOOK);
  actor.SetProperty(CameraActor::Property::ASPECT_RATIO, TEST_ASPECT_RATIO);
  actor.SetProperty(CameraActor::Property::FIELD_OF_VIEW, TEST_FIELD_OF_VIEW);
  actor.SetProperty(CameraActor::Property::NEAR_PLANE_DISTANCE, TEST_NEAR_PLANE_DISTANCE);
  actor.SetProperty(CameraActor::Property::FAR_PLANE_DISTANCE, TEST_FAR_PLANE_DISTANCE);

  DALI_TEST_EQUALS(Camera::FREE_LOOK, actor.GetType(), TEST_LOCATION);
  DALI_TEST_EQUALS(TEST_ASPECT_RATIO, actor.GetAspectRatio(), FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(TEST_FIELD_OF_VIEW, actor.GetFieldOfView(), FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(TEST_NEAR_PLANE_DISTANCE, actor.GetNearClippingPlane(), FLOAT_EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(TEST_FAR_PLANE_DISTANCE, actor.GetFarClippingPlane(), FLOAT_EPSILON, TEST_LOCATION);
  END_TEST;
}

int UtcDaliCameraActorDefaultProperties(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor DefaultProperties");

  CameraActor        actor = CameraActor::New();
  Integration::Scene stage = application.GetScene();
  stage.Add(actor);
  stage.GetRenderTaskList().GetTask(0).SetCameraActor(actor);

  actor.SetAspectRatio(TEST_ASPECT_RATIO);
  application.GetScene().Add(actor);
  application.Render(0);
  application.SendNotification();
  bool bValue;
  actor.GetProperty(CameraActor::Property::INVERT_Y_AXIS).Get(bValue);
  DALI_TEST_EQUALS(false, bValue, TEST_LOCATION);

  std::vector<Property::Index> indices;
  indices.push_back(CameraActor::Property::TYPE);
  indices.push_back(CameraActor::Property::PROJECTION_MODE);
  indices.push_back(CameraActor::Property::FIELD_OF_VIEW);
  indices.push_back(CameraActor::Property::ASPECT_RATIO);
  indices.push_back(CameraActor::Property::NEAR_PLANE_DISTANCE);
  indices.push_back(CameraActor::Property::FAR_PLANE_DISTANCE);
  indices.push_back(CameraActor::Property::LEFT_PLANE_DISTANCE);
  indices.push_back(CameraActor::Property::RIGHT_PLANE_DISTANCE);
  indices.push_back(CameraActor::Property::TOP_PLANE_DISTANCE);
  indices.push_back(CameraActor::Property::BOTTOM_PLANE_DISTANCE);
  indices.push_back(CameraActor::Property::TARGET_POSITION);
  indices.push_back(CameraActor::Property::PROJECTION_MATRIX);
  indices.push_back(CameraActor::Property::VIEW_MATRIX);
  indices.push_back(CameraActor::Property::INVERT_Y_AXIS);

  DALI_TEST_CHECK(actor.GetPropertyCount() == (Actor::New().GetPropertyCount() + indices.size()));

  for(std::vector<Property::Index>::iterator iter = indices.begin(); iter != indices.end(); ++iter)
  {
    DALI_TEST_EQUALS(*iter, actor.GetPropertyIndex(actor.GetPropertyName(*iter)), TEST_LOCATION);
    DALI_TEST_CHECK(!actor.IsPropertyAnimatable(*iter));

    if((*iter == CameraActor::Property::PROJECTION_MATRIX) ||
       (*iter == CameraActor::Property::VIEW_MATRIX))
    {
      DALI_TEST_CHECK(!actor.IsPropertyWritable(*iter));
    }
    else
    {
      DALI_TEST_CHECK(actor.IsPropertyWritable(*iter));
    }

    DALI_TEST_CHECK(actor.GetPropertyType(*iter) == actor.GetPropertyType(*iter)); // just checking call succeeds
  }

  // Set/Get one of them.
  const float newAspect = TEST_ASPECT_RATIO * 2.0f;

  actor.SetProperty(CameraActor::Property::ASPECT_RATIO, Property::Value(newAspect));
  application.Render();
  application.SendNotification();
  application.Render();
  application.SendNotification();

  DALI_TEST_EQUALS(actor.GetAspectRatio(), newAspect, TEST_LOCATION);
  END_TEST;
}

template<typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
void TEST_CAMERA_PROPERTY(P1 camera, P2 stringName, P3 type, P4 isWriteable, P5 isAnimateable, P6 isConstraintInput, P7 enumName, P8 LOCATION)
{
  DALI_TEST_EQUALS(camera.GetPropertyName(enumName), stringName, LOCATION);
  DALI_TEST_EQUALS(camera.GetPropertyIndex(stringName), static_cast<Property::Index>(enumName), LOCATION);
  DALI_TEST_EQUALS(camera.GetPropertyType(enumName), type, LOCATION);
  DALI_TEST_EQUALS(camera.IsPropertyWritable(enumName), isWriteable, LOCATION);
  DALI_TEST_EQUALS(camera.IsPropertyAnimatable(enumName), isAnimateable, LOCATION);
  DALI_TEST_EQUALS(camera.IsPropertyAConstraintInput(enumName), isConstraintInput, LOCATION);
}
int UtcDaliCameraActorDefaultPropertiesInherited(void)
{
  TestApplication application;

  CameraActor        actor = CameraActor::New();
  Integration::Scene stage = application.GetScene();
  stage.Add(actor);
  stage.GetRenderTaskList().GetTask(0).SetCameraActor(actor);

  application.GetScene().Add(actor);
  application.Render(0);
  application.SendNotification();

  const PropertyDetails CAMERA_DEFAULT_PROPERTY[] =
    {
      // actor
      {"parentOrigin", Property::VECTOR3, true, false, true, Dali::Actor::Property::PARENT_ORIGIN},
      {"parentOriginX", Property::FLOAT, true, false, true, Dali::Actor::Property::PARENT_ORIGIN_X},
      {"parentOriginY", Property::FLOAT, true, false, true, Dali::Actor::Property::PARENT_ORIGIN_Y},
      {"parentOriginZ", Property::FLOAT, true, false, true, Dali::Actor::Property::PARENT_ORIGIN_Z},
      {"anchorPoint", Property::VECTOR3, true, false, true, Dali::Actor::Property::ANCHOR_POINT},
      {"anchorPointX", Property::FLOAT, true, false, true, Dali::Actor::Property::ANCHOR_POINT_X},
      {"anchorPointY", Property::FLOAT, true, false, true, Dali::Actor::Property::ANCHOR_POINT_Y},
      {"anchorPointZ", Property::FLOAT, true, false, true, Dali::Actor::Property::ANCHOR_POINT_Z},
      {"size", Property::VECTOR3, true, true, true, Dali::Actor::Property::SIZE},
      {"sizeWidth", Property::FLOAT, true, true, true, Dali::Actor::Property::SIZE_WIDTH},
      {"sizeHeight", Property::FLOAT, true, true, true, Dali::Actor::Property::SIZE_HEIGHT},
      {"sizeDepth", Property::FLOAT, true, true, true, Dali::Actor::Property::SIZE_DEPTH},
      {"position", Property::VECTOR3, true, true, true, Dali::Actor::Property::POSITION},
      {"positionX", Property::FLOAT, true, true, true, Dali::Actor::Property::POSITION_X},
      {"positionY", Property::FLOAT, true, true, true, Dali::Actor::Property::POSITION_Y},
      {"positionZ", Property::FLOAT, true, true, true, Dali::Actor::Property::POSITION_Z},
      {"worldPosition", Property::VECTOR3, false, false, true, Dali::Actor::Property::WORLD_POSITION},
      {"worldPositionX", Property::FLOAT, false, false, true, Dali::Actor::Property::WORLD_POSITION_X},
      {"worldPositionY", Property::FLOAT, false, false, true, Dali::Actor::Property::WORLD_POSITION_Y},
      {"worldPositionZ", Property::FLOAT, false, false, true, Dali::Actor::Property::WORLD_POSITION_Z},
      {"orientation", Property::ROTATION, true, true, true, Dali::Actor::Property::ORIENTATION},
      {"worldOrientation", Property::ROTATION, false, false, true, Dali::Actor::Property::WORLD_ORIENTATION},
      {"scale", Property::VECTOR3, true, true, true, Dali::Actor::Property::SCALE},
      {"scaleX", Property::FLOAT, true, true, true, Dali::Actor::Property::SCALE_X},
      {"scaleY", Property::FLOAT, true, true, true, Dali::Actor::Property::SCALE_Y},
      {"scaleZ", Property::FLOAT, true, true, true, Dali::Actor::Property::SCALE_Z},
      {"worldScale", Property::VECTOR3, false, false, true, Dali::Actor::Property::WORLD_SCALE},
      {"visible", Property::BOOLEAN, true, true, true, Dali::Actor::Property::VISIBLE},
      {"color", Property::VECTOR4, true, true, true, Dali::Actor::Property::COLOR},
      {"colorRed", Property::FLOAT, true, true, true, Dali::Actor::Property::COLOR_RED},
      {"colorGreen", Property::FLOAT, true, true, true, Dali::Actor::Property::COLOR_GREEN},
      {"colorBlue", Property::FLOAT, true, true, true, Dali::Actor::Property::COLOR_BLUE},
      {"colorAlpha", Property::FLOAT, true, true, true, Dali::Actor::Property::COLOR_ALPHA},
      {"worldColor", Property::VECTOR4, false, false, true, Dali::Actor::Property::WORLD_COLOR},
      {"worldMatrix", Property::MATRIX, false, false, true, Dali::Actor::Property::WORLD_MATRIX},
      {"name", Property::STRING, true, false, false, Dali::Actor::Property::NAME},
      {"sensitive", Property::BOOLEAN, true, false, false, Dali::Actor::Property::SENSITIVE},
      {"leaveRequired", Property::BOOLEAN, true, false, false, Dali::Actor::Property::LEAVE_REQUIRED},
      {"inheritOrientation", Property::BOOLEAN, true, false, false, Dali::Actor::Property::INHERIT_ORIENTATION},
      {"inheritScale", Property::BOOLEAN, true, false, false, Dali::Actor::Property::INHERIT_SCALE},
      {"colorMode", Property::INTEGER, true, false, false, Dali::Actor::Property::COLOR_MODE},
      {"drawMode", Property::INTEGER, true, false, false, Dali::Actor::Property::DRAW_MODE},
      {"sizeModeFactor", Property::VECTOR3, true, false, false, Dali::Actor::Property::SIZE_MODE_FACTOR},
      {"widthResizePolicy", Property::STRING, true, false, false, Dali::Actor::Property::WIDTH_RESIZE_POLICY},
      {"heightResizePolicy", Property::STRING, true, false, false, Dali::Actor::Property::HEIGHT_RESIZE_POLICY},
      {"sizeScalePolicy", Property::INTEGER, true, false, false, Dali::Actor::Property::SIZE_SCALE_POLICY},
      {"widthForHeight", Property::BOOLEAN, true, false, false, Dali::Actor::Property::WIDTH_FOR_HEIGHT},
      {"heightForWidth", Property::BOOLEAN, true, false, false, Dali::Actor::Property::HEIGHT_FOR_WIDTH},
      {"padding", Property::VECTOR4, true, false, false, Dali::Actor::Property::PADDING},
      {"minimumSize", Property::VECTOR2, true, false, false, Dali::Actor::Property::MINIMUM_SIZE},
      {"maximumSize", Property::VECTOR2, true, false, false, Dali::Actor::Property::MAXIMUM_SIZE},
      {"inheritPosition", Property::BOOLEAN, true, false, false, Dali::Actor::Property::INHERIT_POSITION},
      {"clippingMode", Property::STRING, true, false, false, Dali::Actor::Property::CLIPPING_MODE},
      {"layoutDirection", Property::STRING, true, false, false, Dali::Actor::Property::LAYOUT_DIRECTION},
      {"inheritLayoutDirection", Property::BOOLEAN, true, false, false, Dali::Actor::Property::INHERIT_LAYOUT_DIRECTION},
      {"opacity", Property::FLOAT, true, true, true, Dali::Actor::Property::OPACITY},
      {"screenPosition", Property::VECTOR2, false, false, false, Dali::Actor::Property::SCREEN_POSITION},
      {"positionUsesAnchorPoint", Property::BOOLEAN, true, false, false, Dali::Actor::Property::POSITION_USES_ANCHOR_POINT},
      {"culled", Property::BOOLEAN, false, false, true, Dali::Actor::Property::CULLED},
      {"id", Property::INTEGER, false, false, false, Dali::Actor::Property::ID},
      {"hierarchyDepth", Property::INTEGER, false, false, false, Dali::Actor::Property::HIERARCHY_DEPTH},
      {"isRoot", Property::BOOLEAN, false, false, false, Dali::Actor::Property::IS_ROOT},
      {"isLayer", Property::BOOLEAN, false, false, false, Dali::Actor::Property::IS_LAYER},
      {"connectedToScene", Property::BOOLEAN, false, false, false, Dali::Actor::Property::CONNECTED_TO_SCENE},
      {"keyboardFocusable", Property::BOOLEAN, true, false, false, Dali::Actor::Property::KEYBOARD_FOCUSABLE},
      {"siblingOrder", Property::INTEGER, true, false, false, Dali::DevelActor::Property::SIBLING_ORDER},
      // camera own
      {"type", Property::INTEGER, true, false, true, Dali::CameraActor::Property::TYPE},
      {"projectionMode", Property::INTEGER, true, false, true, Dali::CameraActor::Property::PROJECTION_MODE},
      {"fieldOfView", Property::FLOAT, true, false, true, Dali::CameraActor::Property::FIELD_OF_VIEW},
      {"aspectRatio", Property::FLOAT, true, false, true, Dali::CameraActor::Property::ASPECT_RATIO},
      {"nearPlaneDistance", Property::FLOAT, true, false, true, Dali::CameraActor::Property::NEAR_PLANE_DISTANCE},
      {"farPlaneDistance", Property::FLOAT, true, false, true, Dali::CameraActor::Property::FAR_PLANE_DISTANCE},
      {"leftPlaneDistance", Property::FLOAT, true, false, true, Dali::CameraActor::Property::LEFT_PLANE_DISTANCE},
      {"rightPlaneDistance", Property::FLOAT, true, false, true, Dali::CameraActor::Property::RIGHT_PLANE_DISTANCE},
      {"topPlaneDistance", Property::FLOAT, true, false, true, Dali::CameraActor::Property::TOP_PLANE_DISTANCE},
      {"bottomPlaneDistance", Property::FLOAT, true, false, true, Dali::CameraActor::Property::BOTTOM_PLANE_DISTANCE},
      {"targetPosition", Property::VECTOR3, true, false, true, Dali::CameraActor::Property::TARGET_POSITION},
      {"projectionMatrix", Property::MATRIX, false, false, true, Dali::CameraActor::Property::PROJECTION_MATRIX},
      {"viewMatrix", Property::MATRIX, false, false, true, Dali::CameraActor::Property::VIEW_MATRIX},
      {"invertYAxis", Property::BOOLEAN, true, false, true, Dali::CameraActor::Property::INVERT_Y_AXIS}};

  for(uint32_t index = 0; index < (sizeof(CAMERA_DEFAULT_PROPERTY) / sizeof(PropertyDetails)); ++index)
  {
    TEST_CAMERA_PROPERTY(actor,
                         CAMERA_DEFAULT_PROPERTY[index].name,
                         CAMERA_DEFAULT_PROPERTY[index].type,
                         CAMERA_DEFAULT_PROPERTY[index].writable,
                         CAMERA_DEFAULT_PROPERTY[index].animatable,
                         CAMERA_DEFAULT_PROPERTY[index].constraintInput,
                         CAMERA_DEFAULT_PROPERTY[index].enumIndex,
                         TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliCameraActorModelView(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor Test view application");

  Actor actor = CreateRenderableActor();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::POSITION, Vector3(20.0f, 30.0f, 40.0f));
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();

  Matrix resultMatrix(true);
  resultMatrix.SetTransformComponents(Vector3::ONE, Quaternion::IDENTITY, actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION));

  RenderTask  task        = application.GetScene().GetRenderTaskList().GetTask(0);
  CameraActor cameraActor = task.GetCameraActor();

  Matrix viewMatrix(false);
  cameraActor.GetProperty(CameraActor::Property::VIEW_MATRIX).Get(viewMatrix);
  Matrix::Multiply(resultMatrix, resultMatrix, viewMatrix);

  DALI_TEST_CHECK(application.GetGlAbstraction().CheckUniformValue("uModelView", resultMatrix));
  END_TEST;
}

int UtcDaliCameraActorReadProjectionMatrix(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor::ReadProjectionMatrix()");

  CameraActor camera = application.GetScene().GetRenderTaskList().GetTask(0u).GetCameraActor();
  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();
  Texture image = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 4u, 4u);
  Actor   actor = CreateRenderableActor(image, RENDER_SHADOW_VERTEX_SOURCE, RENDER_SHADOW_FRAGMENT_SOURCE);
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  application.GetScene().Add(actor);

  Matrix projectionMatrix;
  Matrix viewMatrix;

  camera.GetProperty(CameraActor::CameraActor::Property::PROJECTION_MATRIX).Get(projectionMatrix);
  camera.GetProperty(CameraActor::CameraActor::Property::VIEW_MATRIX).Get(viewMatrix);

  actor.RegisterProperty(SHADER_LIGHT_CAMERA_PROJECTION_MATRIX_PROPERTY_NAME, Matrix::IDENTITY);
  actor.RegisterProperty(SHADER_LIGHT_CAMERA_VIEW_MATRIX_PROPERTY_NAME, Matrix::IDENTITY);

  Property::Index projectionMatrixPropertyIndex = actor.GetPropertyIndex(SHADER_LIGHT_CAMERA_PROJECTION_MATRIX_PROPERTY_NAME);
  Property::Index viewMatrixPropertyIndex       = actor.GetPropertyIndex(SHADER_LIGHT_CAMERA_VIEW_MATRIX_PROPERTY_NAME);

  Constraint projectionMatrixConstraint = Constraint::New<Dali::Matrix>(actor, projectionMatrixPropertyIndex, EqualToConstraint());
  projectionMatrixConstraint.AddSource(Source(camera, CameraActor::Property::PROJECTION_MATRIX));
  Constraint viewMatrixConstraint = Constraint::New<Dali::Matrix>(actor, viewMatrixPropertyIndex, EqualToConstraint());
  viewMatrixConstraint.AddSource(Source(camera, CameraActor::Property::VIEW_MATRIX));

  projectionMatrixConstraint.Apply();
  viewMatrixConstraint.Apply();

  application.SendNotification();
  application.Render();

  // Test effects of Constraint.
  DALI_TEST_CHECK(application.GetGlAbstraction().CheckUniformValue(SHADER_LIGHT_CAMERA_PROJECTION_MATRIX_PROPERTY_NAME.c_str(), projectionMatrix));

  DALI_TEST_CHECK(application.GetGlAbstraction().CheckUniformValue(SHADER_LIGHT_CAMERA_VIEW_MATRIX_PROPERTY_NAME.c_str(), viewMatrix));
  END_TEST;
}

int UtcDaliCameraActorAnimatedProperties(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Internal::CameraActor::GetSceneObjectAnimatableProperty()");

  CameraActor camera = application.GetScene().GetRenderTaskList().GetTask(0u).GetCameraActor();
  Actor       actor  = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  application.GetScene().Add(actor);

  Constraint constraint = Constraint::New<Dali::Vector3>(actor, Actor::Property::POSITION, EqualToConstraint());
  constraint.AddSource(Source(camera, Actor::Property::POSITION));
  constraint.Apply();

  camera.SetProperty(Actor::Property::POSITION, Vector3(100.0f, 200.0f, 300.0f));
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(actor.GetCurrentProperty<Vector3>(Actor::Property::POSITION), Vector3(100.0f, 200.0f, 300.0f), TEST_LOCATION);
  END_TEST;
}

int UtcDaliCameraActorPropertyIndices(void)
{
  TestApplication application;
  CameraActor     camera = application.GetScene().GetRenderTaskList().GetTask(0u).GetCameraActor();

  Actor                    basicActor = Actor::New();
  Property::IndexContainer indices;
  camera.GetPropertyIndices(indices);
  DALI_TEST_CHECK(indices.Size() > basicActor.GetPropertyCount());
  DALI_TEST_EQUALS(indices.Size(), camera.GetPropertyCount(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliCameraActorCheckLookAtAndFreeLookViews01(void)
{
  TestApplication    application;
  Integration::Scene stage     = application.GetScene();
  Vector2            stageSize = stage.GetSize();

  CameraActor freeLookCameraActor = CameraActor::New(stageSize);
  freeLookCameraActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  freeLookCameraActor.SetType(Camera::FREE_LOOK);

  Vector3 targetPosition(30.0f, 240.0f, -256.0f);
  Actor   target = Actor::New();
  target.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  target.SetProperty(Actor::Property::POSITION, targetPosition);

  Constraint cameraOrientationConstraint = Constraint::New<Quaternion>(freeLookCameraActor, Actor::Property::ORIENTATION, &LookAt);
  cameraOrientationConstraint.AddSource(Source(target, Actor::Property::WORLD_POSITION));
  cameraOrientationConstraint.AddSource(Source(freeLookCameraActor, Actor::Property::WORLD_POSITION));
  cameraOrientationConstraint.AddSource(Source(target, Actor::Property::WORLD_ORIENTATION));
  cameraOrientationConstraint.Apply();

  CameraActor lookAtCameraActor = CameraActor::New(stageSize);
  lookAtCameraActor.SetType(Camera::LOOK_AT_TARGET);
  lookAtCameraActor.SetTargetPosition(targetPosition);
  lookAtCameraActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  stage.Add(target);
  stage.Add(freeLookCameraActor);
  stage.Add(lookAtCameraActor);

  // Create an arbitrary vector
  for(float x = -1.0f; x <= 1.0f; x += 0.1f)
  {
    for(float y = -1.0f; y < 1.0f; y += 0.1f)
    {
      for(float z = -1.0f; z < 1.0f; z += 0.1f)
      {
        Vector3 position(x, y, z);
        position.Normalize();
        position *= 200.0f;

        freeLookCameraActor.SetProperty(Actor::Property::POSITION, position);
        lookAtCameraActor.SetProperty(Actor::Property::POSITION, position);

        application.SendNotification();
        application.Render();
        application.SendNotification();
        application.Render();
        Matrix freeLookViewMatrix;
        Matrix lookAtViewMatrix;
        freeLookCameraActor.GetProperty(CameraActor::CameraActor::Property::VIEW_MATRIX).Get(freeLookViewMatrix);
        lookAtCameraActor.GetProperty(CameraActor::CameraActor::Property::VIEW_MATRIX).Get(lookAtViewMatrix);

        DALI_TEST_EQUALS(freeLookViewMatrix, lookAtViewMatrix, 0.01, TEST_LOCATION);
      }
    }
  }
  END_TEST;
}

int UtcDaliCameraActorCheckLookAtAndFreeLookViews02(void)
{
  TestApplication    application;
  Integration::Scene stage     = application.GetScene();
  Vector2            stageSize = stage.GetSize();

  CameraActor freeLookCameraActor = CameraActor::New(stageSize);
  freeLookCameraActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  freeLookCameraActor.SetType(Camera::FREE_LOOK);

  Vector3 targetPosition(30.0f, 240.0f, -256.0f);
  Actor   target = Actor::New();
  target.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  target.SetProperty(Actor::Property::POSITION, targetPosition);

  Constraint cameraOrientationConstraint = Constraint::New<Quaternion>(freeLookCameraActor, Actor::Property::ORIENTATION, &LookAt);
  cameraOrientationConstraint.AddSource(Source(target, Actor::Property::WORLD_POSITION));
  cameraOrientationConstraint.AddSource(Source(freeLookCameraActor, Actor::Property::WORLD_POSITION));
  cameraOrientationConstraint.AddSource(Source(target, Actor::Property::WORLD_ORIENTATION));
  cameraOrientationConstraint.Apply();

  CameraActor lookAtCameraActor = CameraActor::New(stageSize);
  lookAtCameraActor.SetType(Camera::LOOK_AT_TARGET);
  lookAtCameraActor.SetTargetPosition(targetPosition);
  lookAtCameraActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  stage.Add(target);
  stage.Add(freeLookCameraActor);
  stage.Add(lookAtCameraActor);
  stage.GetRenderTaskList().GetTask(0).SetCameraActor(freeLookCameraActor);

  // Create an arbitrary vector
  for(float x = -1.0f; x <= 1.0f; x += 0.1f)
  {
    for(float y = -1.0f; y < 1.0f; y += 0.1f)
    {
      for(float z = -1.0f; z < 1.0f; z += 0.1f)
      {
        Vector3 position(x, y, z);
        position.Normalize();
        position *= 200.0f;

        freeLookCameraActor.SetProperty(Actor::Property::POSITION, position);
        lookAtCameraActor.SetProperty(Actor::Property::POSITION, position);

        application.SendNotification();
        application.Render();
        application.SendNotification();
        application.Render();
        Matrix freeLookViewMatrix;
        freeLookCameraActor.GetProperty(CameraActor::CameraActor::Property::VIEW_MATRIX).Get(freeLookViewMatrix);

        Matrix freeLookWorld = freeLookCameraActor.GetCurrentProperty<Matrix>(Actor::Property::WORLD_MATRIX);

        Matrix freeLookTest(false);
        Matrix::Multiply(freeLookTest, freeLookViewMatrix, freeLookWorld);
        DALI_TEST_EQUALS(freeLookTest, Matrix::IDENTITY, 0.01f, TEST_LOCATION);
      }
    }
  }

  END_TEST;
}

int UtcDaliCameraActorCheckLookAtAndFreeLookViews03(void)
{
  TestApplication    application;
  Integration::Scene stage     = application.GetScene();
  Vector2            stageSize = stage.GetSize();

  Vector3 targetPosition(Vector3::ZERO);
  Vector3 cameraOffset(0.0f, 0.0f, 100.0f);

  CameraActor freeLookCameraActor = CameraActor::New(stageSize);
  freeLookCameraActor.SetType(Camera::FREE_LOOK);
  freeLookCameraActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  Quaternion cameraOrientation(Radian(Degree(180.0f)), Vector3::YAXIS);
  freeLookCameraActor.SetProperty(Actor::Property::POSITION, cameraOffset);
  freeLookCameraActor.SetProperty(Actor::Property::ORIENTATION, cameraOrientation);

  Actor cameraAnchor = Actor::New();
  cameraAnchor.Add(freeLookCameraActor);
  stage.Add(cameraAnchor);
  stage.GetRenderTaskList().GetTask(0).SetCameraActor(freeLookCameraActor);

  for(float angle = 1.0f; angle <= 180.0f; angle += 1.0f)
  {
    Quaternion rotation(Radian(Degree(angle)), Vector3::YAXIS);

    freeLookCameraActor.SetProperty(Actor::Property::POSITION, rotation.Rotate(cameraOffset));
    cameraAnchor.SetProperty(Actor::Property::ORIENTATION, rotation);

    application.SendNotification();
    application.Render();
    application.SendNotification();
    application.Render();

    Matrix freeLookViewMatrix;
    freeLookCameraActor.GetProperty(CameraActor::CameraActor::Property::VIEW_MATRIX).Get(freeLookViewMatrix);

    Matrix freeLookWorld = freeLookCameraActor.GetCurrentProperty<Matrix>(Actor::Property::WORLD_MATRIX);

    Matrix freeLookTest(false);
    Matrix::Multiply(freeLookTest, freeLookViewMatrix, freeLookWorld);
    DALI_TEST_EQUALS(freeLookTest, Matrix::IDENTITY, 0.01f, TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliCameraActorReflectionByPlane(void)
{
  TestApplication application;

  Integration::Scene stage = application.GetScene();

  Vector3 targetPosition(Vector3::ZERO);
  Vector3 cameraOffset(0.0f, 100.0f, 100.0f);

  CameraActor freeLookCameraActor = stage.GetRenderTaskList().GetTask(0).GetCameraActor();
  freeLookCameraActor.SetType(Camera::LOOK_AT_TARGET);
  freeLookCameraActor.SetTargetPosition(targetPosition);
  freeLookCameraActor.SetProperty(Actor::Property::POSITION, cameraOffset);

  stage.GetRootLayer().SetProperty(Actor::Property::POSITION, Vector2(1, 0));

  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  Matrix matrixBefore, matrixAfter;
  freeLookCameraActor.GetProperty(CameraActor::CameraActor::Property::VIEW_MATRIX).Get(matrixBefore);
  freeLookCameraActor.SetProperty(Dali::DevelCameraActor::Property::REFLECTION_PLANE, Vector4(0.0f, 1.0f, 0.0f, 0.0f));
  stage.GetRootLayer().SetProperty(Actor::Property::POSITION, Vector2(0, 0));
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  freeLookCameraActor.GetProperty(CameraActor::CameraActor::Property::VIEW_MATRIX).Get(matrixAfter);

  Vector3    position, scale;
  Quaternion rotation;
  matrixAfter.GetTransformComponents(position, rotation, scale);

  Quaternion reflected(0, 0, 1, 0);

  DALI_TEST_EQUALS(reflected, rotation, 0.01f, TEST_LOCATION);

  // Test Free Look camera
  freeLookCameraActor.SetType(Camera::FREE_LOOK);

  // Make sure the recalculation will take place
  freeLookCameraActor.SetProperty(Dali::DevelCameraActor::Property::REFLECTION_PLANE, Vector4(0.0f, 1.0f, 0.0f, 0.0f));

  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  // Nothing should change despite of different camera type
  matrixAfter.GetTransformComponents(position, rotation, scale);
  DALI_TEST_EQUALS(reflected, rotation, 0.01f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliCameraActorGetAspectRatioNegative(void)
{
  TestApplication   application;
  Dali::CameraActor instance;
  try
  {
    instance.GetAspectRatio();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliCameraActorGetFieldOfViewNegative(void)
{
  TestApplication   application;
  Dali::CameraActor instance;
  try
  {
    instance.GetFieldOfView();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliCameraActorGetInvertYAxisNegative(void)
{
  TestApplication   application;
  Dali::CameraActor instance;
  try
  {
    instance.GetInvertYAxis();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliCameraActorSetAspectRatioNegative(void)
{
  TestApplication   application;
  Dali::CameraActor instance;
  try
  {
    float arg1(0.0f);
    instance.SetAspectRatio(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliCameraActorSetFieldOfViewNegative(void)
{
  TestApplication   application;
  Dali::CameraActor instance;
  try
  {
    float arg1(0.0f);
    instance.SetFieldOfView(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliCameraActorSetInvertYAxisNegative(void)
{
  TestApplication   application;
  Dali::CameraActor instance;
  try
  {
    bool arg1(false);
    instance.SetInvertYAxis(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliCameraActorSetProjectionModeNegative(void)
{
  TestApplication   application;
  Dali::CameraActor instance;
  try
  {
    Dali::Camera::ProjectionMode arg1(Camera::PERSPECTIVE_PROJECTION);
    instance.SetProjectionMode(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliCameraActorSetTargetPositionNegative(void)
{
  TestApplication   application;
  Dali::CameraActor instance;
  try
  {
    Dali::Vector3 arg1;
    instance.SetTargetPosition(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliCameraActorGetFarClippingPlaneNegative(void)
{
  TestApplication   application;
  Dali::CameraActor instance;
  try
  {
    instance.GetFarClippingPlane();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliCameraActorSetFarClippingPlaneNegative(void)
{
  TestApplication   application;
  Dali::CameraActor instance;
  try
  {
    float arg1(0.0f);
    instance.SetFarClippingPlane(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliCameraActorGetNearClippingPlaneNegative(void)
{
  TestApplication   application;
  Dali::CameraActor instance;
  try
  {
    instance.GetNearClippingPlane();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliCameraActorSetNearClippingPlaneNegative(void)
{
  TestApplication   application;
  Dali::CameraActor instance;
  try
  {
    float arg1(0.0f);
    instance.SetNearClippingPlane(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliCameraActorSetPerspectiveProjectionNegative(void)
{
  TestApplication   application;
  Dali::CameraActor instance;
  try
  {
    Dali::Vector2 arg1;
    instance.SetPerspectiveProjection(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliCameraActorSetOrthographicProjectionNegative01(void)
{
  TestApplication   application;
  Dali::CameraActor instance;
  try
  {
    float arg1(0.0f);
    float arg2(0.0f);
    float arg3(0.0f);
    float arg4(0.0f);
    float arg5(0.0f);
    float arg6(0.0f);
    instance.SetOrthographicProjection(arg1, arg2, arg3, arg4, arg5, arg6);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliCameraActorSetOrthographicProjectionNegative02(void)
{
  TestApplication   application;
  Dali::CameraActor instance;
  try
  {
    Dali::Vector2 arg1;
    instance.SetOrthographicProjection(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliCameraActorSetTypeNegative(void)
{
  TestApplication   application;
  Dali::CameraActor instance;
  try
  {
    Dali::Camera::Type arg1(Camera::FREE_LOOK);
    instance.SetType(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliCameraActorGetProjectionModeNegative(void)
{
  TestApplication   application;
  Dali::CameraActor instance;
  try
  {
    instance.GetProjectionMode();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliCameraActorGetTargetPositionNegative(void)
{
  TestApplication   application;
  Dali::CameraActor instance;
  try
  {
    instance.GetTargetPosition();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliCameraActorGetTypeNegative(void)
{
  TestApplication   application;
  Dali::CameraActor instance;
  try
  {
    instance.GetType();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliCameraActorNewDefaultOrthogonalProjection01(void)
{
  TestApplication application;
  tet_infoline("Test that changing to orthogonal projection and then adding to scene calculates the right defaults");

  CameraActor actor = CameraActor::New();
  DALI_TEST_CHECK(actor);

  actor.SetProjectionMode(Camera::ORTHOGRAPHIC_PROJECTION);
  application.GetScene().Add(actor);

  // Test application screen size is 480x800
  // Check that the properties match to that screen size
  float value;
  actor.GetProperty(CameraActor::Property::ASPECT_RATIO).Get(value);
  DALI_TEST_EQUALS(800.0f / 480.0f, value, FLOAT_EPSILON, TEST_LOCATION);

  actor.GetProperty(CameraActor::Property::NEAR_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(800.0f, value, FLOAT_EPSILON, TEST_LOCATION);

  actor.GetProperty(CameraActor::Property::FAR_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(4895.0f, value, FLOAT_EPSILON, TEST_LOCATION);

  actor.GetProperty(CameraActor::Property::LEFT_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(-240.0f, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::Property::RIGHT_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(240.0f, value, FLOAT_EPSILON, TEST_LOCATION);

  actor.GetProperty(CameraActor::Property::TOP_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(400.0f, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::Property::BOTTOM_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(-400.0f, value, FLOAT_EPSILON, TEST_LOCATION);

  DALI_TEST_EQUALS(1600.0f, actor.GetProperty(Actor::Property::POSITION_Z).Get<float>(), TEST_LOCATION);

  DALI_TEST_EQUALS(actor.GetProjectionMode(), Dali::Camera::ORTHOGRAPHIC_PROJECTION, TEST_LOCATION);
  END_TEST;
}

int UtcDaliCameraActorNewDefaultOrthogonalProjection02(void)
{
  TestApplication application;
  tet_infoline("Test that changing to orthogonal projection and then adding to scene calculates the right defaults");

  CameraActor actor = CameraActor::New();
  DALI_TEST_CHECK(actor);

  actor.SetOrthographicProjection(Vector2::ZERO);
  DALI_TEST_EQUALS(actor.GetProjectionMode(), Dali::Camera::ORTHOGRAPHIC_PROJECTION, TEST_LOCATION);
  application.GetScene().Add(actor);

  // Test application screen size is 480x800
  // Check that the properties match to that screen size
  float value;
  actor.GetProperty(CameraActor::Property::ASPECT_RATIO).Get(value);
  DALI_TEST_EQUALS(800.0f / 480.0f, value, FLOAT_EPSILON, TEST_LOCATION);

  actor.GetProperty(CameraActor::Property::NEAR_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(800.0f, value, FLOAT_EPSILON, TEST_LOCATION);

  actor.GetProperty(CameraActor::Property::FAR_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(4895.0f, value, FLOAT_EPSILON, TEST_LOCATION);

  actor.GetProperty(CameraActor::Property::LEFT_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(-240.0f, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::Property::RIGHT_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(240.0f, value, FLOAT_EPSILON, TEST_LOCATION);

  actor.GetProperty(CameraActor::Property::TOP_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(400.0f, value, FLOAT_EPSILON, TEST_LOCATION);
  actor.GetProperty(CameraActor::Property::BOTTOM_PLANE_DISTANCE).Get(value);
  DALI_TEST_EQUALS(-400.0f, value, FLOAT_EPSILON, TEST_LOCATION);

  DALI_TEST_EQUALS(1600.0f, actor.GetProperty(Actor::Property::POSITION_Z).Get<float>(), TEST_LOCATION);

  DALI_TEST_EQUALS(actor.GetProjectionMode(), Dali::Camera::ORTHOGRAPHIC_PROJECTION, TEST_LOCATION);
  END_TEST;
}

// Add tests for culling:
//   add large actor just outside canvas, & rotate it 45% - should still be rendered
//   Rotate back to 0, should be culled.

int UtcDaliCameraActorCulling01(void)
{
  TestApplication application;
  auto&           gfx = application.GetGraphicsController();

  tet_infoline("Create a renderable actor and position it slightly to the left of the scene");
  tet_infoline("The actor should not be rendered");

  Actor a = CreateRenderableActor(CreateTexture(TextureType::TEXTURE_2D, Pixel::Format::RGBA8888, 200, 200));

  a[Actor::Property::PARENT_ORIGIN] = ParentOrigin::CENTER_LEFT;
  a[Actor::Property::ANCHOR_POINT]  = ParentOrigin::CENTER_RIGHT;
  a[Actor::Property::POSITION]      = Vector3(-10.0f, 0.0f, 0.0f);

  application.GetScene().Add(a);

  application.SendNotification();
  application.Render();

  auto& cmdStack = gfx.mCommandBufferCallStack;
  DALI_TEST_CHECK(!cmdStack.FindMethod("Draw") && !cmdStack.FindMethod("DrawIndexed"));

  tet_infoline("Rotate the actor 45 degrees, the actor should now be rendered");
  a[Actor::Property::ORIENTATION] = Quaternion(Dali::ANGLE_45, Vector3::ZAXIS);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(cmdStack.FindMethod("Draw") || cmdStack.FindMethod("DrawIndexed"));

  END_TEST;
}


int UtcDaliCameraActorSetProperty(void)
{
  TestApplication application;

  tet_infoline("Test the CameraActor reset properties when On Scene, if user set property explicitly.");

  CameraActor camera = CameraActor::New();
  camera.SetFieldOfView(1.0f);
  application.GetScene().Add(camera);
  DALI_TEST_EQUALS(1.0f, camera.GetFieldOfView(), TEST_LOCATION);
  camera.Unparent();
  camera.Reset();

  camera = CameraActor::New();
  camera.SetAspectRatio(1.0f);
  application.GetScene().Add(camera);
  DALI_TEST_EQUALS(1.0f, camera.GetAspectRatio(), TEST_LOCATION);
  camera.Unparent();
  camera.Reset();

  camera = CameraActor::New();
  camera.SetNearClippingPlane(1.0f);
  application.GetScene().Add(camera);
  DALI_TEST_EQUALS(1.0f, camera.GetNearClippingPlane(), TEST_LOCATION);
  camera.Unparent();
  camera.Reset();

  camera = CameraActor::New();
  camera.SetFarClippingPlane(1.0f);
  application.GetScene().Add(camera);
  DALI_TEST_EQUALS(1.0f, camera.GetFarClippingPlane(), TEST_LOCATION);
  camera.Unparent();
  camera.Reset();

  camera = CameraActor::New();
  camera.SetProperty(Dali::CameraActor::Property::LEFT_PLANE_DISTANCE, 1.0f);
  application.GetScene().Add(camera);
  DALI_TEST_EQUALS(1.0f, camera.GetProperty<float>(Dali::CameraActor::Property::LEFT_PLANE_DISTANCE), TEST_LOCATION);
  camera.Unparent();
  camera.Reset();

  camera = CameraActor::New();
  camera.SetProperty(Dali::CameraActor::Property::RIGHT_PLANE_DISTANCE, 1.0f);
  application.GetScene().Add(camera);
  DALI_TEST_EQUALS(1.0f, camera.GetProperty<float>(Dali::CameraActor::Property::RIGHT_PLANE_DISTANCE), TEST_LOCATION);
  camera.Unparent();
  camera.Reset();

  camera = CameraActor::New();
  camera.SetProperty(Dali::CameraActor::Property::TOP_PLANE_DISTANCE, 1.0f);
  application.GetScene().Add(camera);
  DALI_TEST_EQUALS(1.0f, camera.GetProperty<float>(Dali::CameraActor::Property::TOP_PLANE_DISTANCE), TEST_LOCATION);
  camera.Unparent();
  camera.Reset();

  camera = CameraActor::New();
  camera.SetProperty(Dali::CameraActor::Property::BOTTOM_PLANE_DISTANCE, 1.0f);
  application.GetScene().Add(camera);
  DALI_TEST_EQUALS(1.0f, camera.GetProperty<float>(Dali::CameraActor::Property::BOTTOM_PLANE_DISTANCE), TEST_LOCATION);
  camera.Unparent();
  camera.Reset();

  camera = CameraActor::New();
  camera.SetProperty(Dali::Actor::Property::POSITION, Vector3(100.0f, 100.0f, 100.0f));
  application.GetScene().Add(camera);
  DALI_TEST_EQUALS(Vector3(100.0f, 100.0f, 100.0f), camera.GetProperty<Vector3>(Dali::Actor::Property::POSITION), TEST_LOCATION);
  camera.Unparent();
  camera.Reset();

  camera = CameraActor::New();
  camera.SetProperty(Dali::Actor::Property::POSITION_X, 1.0f);
  application.GetScene().Add(camera);
  DALI_TEST_EQUALS(1.0f, camera.GetProperty<float>(Dali::Actor::Property::POSITION_X), TEST_LOCATION);
  camera.Unparent();
  camera.Reset();

  camera = CameraActor::New();
  camera.SetProperty(Dali::Actor::Property::POSITION_Y, 1.0f);
  application.GetScene().Add(camera);
  DALI_TEST_EQUALS(1.0f, camera.GetProperty<float>(Dali::Actor::Property::POSITION_Y), TEST_LOCATION);
  camera.Unparent();
  camera.Reset();

  camera = CameraActor::New();
  camera.SetProperty(Dali::Actor::Property::POSITION_Z, 1.0f);
  application.GetScene().Add(camera);
  DALI_TEST_EQUALS(1.0f, camera.GetProperty<float>(Dali::Actor::Property::POSITION_Z), TEST_LOCATION);
  camera.Unparent();
  camera.Reset();

  camera = CameraActor::New();
  camera.SetProperty(Dali::Actor::Property::ORIENTATION, Quaternion(Radian(Degree(90.0f)), Vector3::XAXIS));
  application.GetScene().Add(camera);
  DALI_TEST_EQUALS(Quaternion(Radian(Degree(90.0f)), Vector3::XAXIS), camera.GetProperty<Quaternion>(Dali::Actor::Property::ORIENTATION), TEST_LOCATION);
  camera.Unparent();
  camera.Reset();

  END_TEST;
}
