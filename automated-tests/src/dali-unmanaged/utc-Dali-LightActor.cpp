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
#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

#include "mesh-builder.h"


int UtcDaliLightActorConstructorVoid(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::LightActor::LightActor() UtcDaliLightActorConstructorVoid");

  LightActor actor;

  DALI_TEST_CHECK(!actor);
  END_TEST;
}


int UtcDaliLightActorDestructor(void)
{
  // This test is achieve 100% line and function coverage
  TestApplication application;
  tet_infoline("Testing Dali::LightActor::~LightActor() UtcDaliLightActorDestructor");

  LightActor* actor = new LightActor;

  DALI_TEST_CHECK( ! *actor );

  delete actor;

  DALI_TEST_CHECK( true );
  END_TEST;
}

int UtcDaliLightActorNew(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::LightActor::New() UtcDaliLightActorNew");

  LightActor actor = LightActor::New();

  DALI_TEST_CHECK(actor);
  END_TEST;
}

int UtcDaliLightActorDownCast(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::LightActor::DownCast() UtcDaliLightActorDownCast");

  LightActor actor1 = LightActor::New();
  Actor anActor = Actor::New();
  anActor.Add(actor1);

  Actor child = anActor.GetChildAt(0);
  LightActor lightActor = LightActor::DownCast(child);

  DALI_TEST_CHECK(lightActor);

  Light light;
  light = Light::New( "TestLight" );
  BaseHandle handle = light;

  DALI_TEST_CHECK( Light::DownCast( handle ) );
  END_TEST;
}

int UtcDaliLightActorDownCast2(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::LightActor::DownCast2() UtcDaliLightActorDownCast2");

  Actor actor1 = Actor::New();
  Actor anActor = Actor::New();
  anActor.Add(actor1);

  Actor child = anActor.GetChildAt(0);
  LightActor lightActor = LightActor::DownCast(child);
  DALI_TEST_CHECK(!lightActor);

  Actor unInitialzedActor;
  lightActor = DownCast< LightActor >( unInitialzedActor );
  DALI_TEST_CHECK(!lightActor);
  END_TEST;
}

int UtcDaliLightActorSetGetLight(void)
{
  TestApplication application;
  tet_infoline( "Testing UtcDaliLightActorSetGetLight" );

  try
  {
    LightActor lightActor = LightActor::New();
    Light light1 = Light::New( "" );
    light1.SetName( "TestLight" );
    light1.SetDirection( Vector3::ZAXIS );
    light1.SetSpotAngle( Vector2::YAXIS );
    lightActor.SetLight( light1 );

    Light light2 = lightActor.GetLight();

    DALI_TEST_EQUALS( light1.GetName(), light2.GetName(), TEST_LOCATION );
    DALI_TEST_EQUALS( light1.GetType(), light2.GetType(), TEST_LOCATION );
    DALI_TEST_EQUALS( light1.GetFallOff(), light2.GetFallOff(), TEST_LOCATION );
    DALI_TEST_EQUALS( light1.GetSpotAngle(), light2.GetSpotAngle(), TEST_LOCATION );
    DALI_TEST_EQUALS( light1.GetAmbientColor(), light2.GetAmbientColor(), TEST_LOCATION );
    DALI_TEST_EQUALS( light1.GetDiffuseColor(), light2.GetDiffuseColor(), TEST_LOCATION );
    DALI_TEST_EQUALS( light1.GetSpecularColor(), light2.GetSpecularColor(), TEST_LOCATION );
    DALI_TEST_EQUALS( light1.GetDirection(), light2.GetDirection(), TEST_LOCATION );
  }
  catch( Dali::DaliException& e )
  {
    tet_printf( "Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str() );
    tet_result(TET_FAIL);
  }
  catch( ... )
  {
    tet_infoline( "Unknown exception." );
    tet_result(TET_FAIL);
  }
  END_TEST;
}

int UtcDaliLightActorSetGetActive(void)
{
  TestApplication application;
  tet_infoline( "Testing UtcDaliLightActorSetGetActive" );

  try
  {
    LightActor lightActor = LightActor::New();
    lightActor.SetActive( true );

    DALI_TEST_CHECK( lightActor.GetActive() );

    lightActor.SetActive( false );

    DALI_TEST_CHECK( !lightActor.GetActive() );

    lightActor.SetActive( true );

    DALI_TEST_CHECK( lightActor.GetActive() );

    lightActor.SetActive( false );

    DALI_TEST_CHECK( !lightActor.GetActive() );
  }
  catch( Dali::DaliException& e )
  {
    tet_printf( "Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str() );
    tet_result(TET_FAIL);
  }
  catch( ... )
  {
    tet_infoline( "Unknown exception." );
    tet_result(TET_FAIL);
  }
  END_TEST;
}


int UtcDaliLightActorMeshTest(void)
{
  TestApplication application;
  tet_infoline( "Testing UtcDaliLightActorMeshTest" );

  try
  {
    Mesh mesh = ConstructMesh( 50 );
    Actor actor = MeshActor::New( mesh );
    Stage::GetCurrent().Add( actor );

    actor.SetParentOrigin( ParentOrigin::CENTER );
    actor.SetAnchorPoint( AnchorPoint::CENTER );
    actor.SetPosition( 0.0f, 0.0f, 0.0f );

    Light light = Light::New("Light");
    light.SetType( POINT );
    light.SetAmbientColor( Vector3( 0.22f, 0.33f, 0.44f ) );
    light.SetDiffuseColor( Vector3( 0.55f, 0.66f, 0.77f) );
    light.SetSpecularColor( Vector3( 0.88f, 0.99f, 0.11f) );
    LightActor lightActor = LightActor::New();
    lightActor.SetParentOrigin( ParentOrigin::CENTER );
    lightActor.SetPosition( 0.f, 0.f, 100.0f );
    lightActor.SetLight( light );
    lightActor.SetName( light.GetName() );

    Stage::GetCurrent().Add( lightActor );
    lightActor.SetActive( true );

    application.SendNotification();
    application.Render();

    // Test Ligh ambient.
    DALI_TEST_CHECK( application.GetGlAbstraction().CheckUniformValue( "uLight0.mAmbient", Vector3( 0.22f, 0.33f, 0.44f ) ) );

    // Test Ligh diffuse.
    DALI_TEST_CHECK( application.GetGlAbstraction().CheckUniformValue( "uLight0.mDiffuse", Vector3( 0.55f, 0.66f, 0.77f ) ) );

    // Test Ligh specular.
    DALI_TEST_CHECK( application.GetGlAbstraction().CheckUniformValue( "uLight0.mSpecular", Vector3( 0.88f, 0.99f, 0.11f ) ) );

    // Test Opacity.
    DALI_TEST_CHECK( application.GetGlAbstraction().CheckUniformValue( "uMaterial.mOpacity", 0.76f ) );

    // Test material Ambient color.
    DALI_TEST_CHECK( application.GetGlAbstraction().CheckUniformValue( "uMaterial.mAmbient", Vector4(0.2f, 1.0f, 0.6f, 1.0f) ) );

    // Test material Diffuse color.
    DALI_TEST_CHECK( application.GetGlAbstraction().CheckUniformValue( "uMaterial.mDiffuse", Vector4(0.8f, 0.0f, 0.4f, 1.0f) ) );

    // Test Specular color.
    DALI_TEST_CHECK( application.GetGlAbstraction().CheckUniformValue( "uMaterial.mSpecular", Vector4(0.5f, 0.6f, 0.7f, 1.0f) ) );
  }
  catch( Dali::DaliException& e )
  {
    tet_printf( "Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str() );
    tet_result(TET_FAIL);
  }
  catch( ... )
  {
    tet_infoline( "Unknown exception." );
    tet_result(TET_FAIL);
  }

  DALI_TEST_CHECK( true );
  END_TEST;
}

int UtcDaliLightActorDefaultProperties(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::LightActor DefaultProperties");

  LightActor actor = LightActor::New();

  std::vector<Property::Index> indices ;
  indices.push_back(LightActor::LIGHT_TYPE       );
  indices.push_back(LightActor::ENABLE           );
  indices.push_back(LightActor::FALL_OFF         );
  indices.push_back(LightActor::SPOT_ANGLE       );
  indices.push_back(LightActor::AMBIENT_COLOR    );
  indices.push_back(LightActor::DIFFUSE_COLOR    );
  indices.push_back(LightActor::SPECULAR_COLOR   );
  indices.push_back(LightActor::DIRECTION        );

  DALI_TEST_CHECK(actor.GetPropertyCount() == ( Actor::New().GetPropertyCount() + indices.size() ) );

  for(std::vector<Property::Index>::iterator iter = indices.begin(); iter != indices.end(); ++iter)
  {
    DALI_TEST_CHECK( *iter == actor.GetPropertyIndex(actor.GetPropertyName(*iter)) );
    DALI_TEST_CHECK( actor.IsPropertyWritable(*iter) );
    DALI_TEST_CHECK( !actor.IsPropertyAnimatable(*iter) );
    DALI_TEST_CHECK( actor.GetPropertyType(*iter) == actor.GetPropertyType(*iter) );  // just checking call succeeds
  }

  // set/get one of them
  actor.GetLight().SetAmbientColor( Vector3( 0.f, 0.f, 0.f ) );
  Vector3 col( 0.22f, 0.33f, 0.44f ) ;
  DALI_TEST_CHECK(actor.GetLight().GetAmbientColor() != col);
  actor.SetProperty(LightActor::AMBIENT_COLOR, col);
  Property::Value v = actor.GetProperty(LightActor::AMBIENT_COLOR);
  DALI_TEST_CHECK(v.GetType() == Property::VECTOR3);

  DALI_TEST_CHECK(v.Get<Vector3>() == col);

  END_TEST;
}

int UtcDaliLightActorPropertyIndices(void)
{
  TestApplication application;
  Actor basicActor = Actor::New();
  LightActor light = LightActor::New();

  Property::IndexContainer indices;
  light.GetPropertyIndices( indices );
  DALI_TEST_CHECK( indices.size() > basicActor.GetPropertyCount() );
  DALI_TEST_EQUALS( indices.size(), light.GetPropertyCount(), TEST_LOCATION );
  END_TEST;
}

namespace
{

struct PropertyDetails
{
  Property::Index index;
  std::string name;
  Property::Type type;
};

const PropertyDetails DEFAULT_LIGHT_ACTOR_PROPERTY_DETAILS[] =
{
  // Index Name             Type
  { LightActor::LIGHT_TYPE,     "light-type",     Property::STRING  },
  { LightActor::ENABLE,         "enable",         Property::BOOLEAN },
  { LightActor::FALL_OFF,       "fall-off",       Property::VECTOR2 },
  { LightActor::SPOT_ANGLE,     "spot-angle",     Property::VECTOR2 },
  { LightActor::AMBIENT_COLOR,  "ambient-color",  Property::VECTOR3 },
  { LightActor::DIFFUSE_COLOR,  "diffuse-color",  Property::VECTOR3 },
  { LightActor::SPECULAR_COLOR, "specular-color", Property::VECTOR3 },
  { LightActor::DIRECTION,      "direction",      Property::VECTOR3 },
};
const unsigned int DEFAULT_LIGHT_ACTOR_PROPERTY_COUNT = sizeof( DEFAULT_LIGHT_ACTOR_PROPERTY_DETAILS ) / sizeof( PropertyDetails );
} // unnamed namespace

int UtcDaliLightActorProperties(void)
{
  TestApplication application;
  Actor basicActor = Actor::New();
  LightActor light = LightActor::New();

  Property::IndexContainer indices;
  light.GetPropertyIndices( indices );
  DALI_TEST_EQUALS( DEFAULT_LIGHT_ACTOR_PROPERTY_COUNT, indices.size() - basicActor.GetPropertyCount(), TEST_LOCATION );

  for ( unsigned int i = 0; i < DEFAULT_LIGHT_ACTOR_PROPERTY_COUNT; ++i )
  {
    tet_printf( "Checking: %s\n", DEFAULT_LIGHT_ACTOR_PROPERTY_DETAILS[i].name.c_str() );
    DALI_TEST_EQUALS( light.GetPropertyIndex( DEFAULT_LIGHT_ACTOR_PROPERTY_DETAILS[i].name ), DEFAULT_LIGHT_ACTOR_PROPERTY_DETAILS[i].index, TEST_LOCATION );
    DALI_TEST_EQUALS( light.GetPropertyName( DEFAULT_LIGHT_ACTOR_PROPERTY_DETAILS[i].index ), DEFAULT_LIGHT_ACTOR_PROPERTY_DETAILS[i].name, TEST_LOCATION );
    DALI_TEST_EQUALS( light.GetPropertyType( DEFAULT_LIGHT_ACTOR_PROPERTY_DETAILS[i].index ), DEFAULT_LIGHT_ACTOR_PROPERTY_DETAILS[i].type, TEST_LOCATION );
    DALI_TEST_EQUALS( light.IsPropertyWritable( DEFAULT_LIGHT_ACTOR_PROPERTY_DETAILS[i].index ), true, TEST_LOCATION );
    DALI_TEST_EQUALS( light.IsPropertyAnimatable( DEFAULT_LIGHT_ACTOR_PROPERTY_DETAILS[i].index ), false, TEST_LOCATION );
    DALI_TEST_EQUALS( light.IsPropertyAConstraintInput( DEFAULT_LIGHT_ACTOR_PROPERTY_DETAILS[i].index ), true, TEST_LOCATION );
  }
  END_TEST;
}
