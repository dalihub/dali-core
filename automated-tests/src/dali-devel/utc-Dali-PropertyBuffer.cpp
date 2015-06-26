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

#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>
#include <dali/devel-api/rendering/renderer.h>

using namespace Dali;

#include <mesh-builder.h>

namespace
{
void TestConstraintNoBlue( Vector4& current, const PropertyInputContainer& inputs )
{
  current.b = 0.0f;
}
}

void propertyBuffer_test_startup(void)
{
  test_return_value = TET_UNDEF;
}

void propertyBuffer_test_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliPropertyBufferNew01(void)
{
  TestApplication application;

  Property::Map texturedQuadVertexFormat;
  texturedQuadVertexFormat["aPosition"] = Property::VECTOR2;
  texturedQuadVertexFormat["aVertexCoord"] = Property::VECTOR2;

  PropertyBuffer propertyBuffer = PropertyBuffer::New( texturedQuadVertexFormat, 4 );

  DALI_TEST_EQUALS( (bool)propertyBuffer, true, TEST_LOCATION );
  END_TEST;
}

int UtcDaliPropertyBufferNew02(void)
{
  TestApplication application;
  PropertyBuffer propertyBuffer;
  DALI_TEST_EQUALS( (bool)propertyBuffer, false, TEST_LOCATION );
  END_TEST;
}

int UtcDaliPropertyBufferDownCast01(void)
{
  TestApplication application;

  Property::Map texturedQuadVertexFormat;
  texturedQuadVertexFormat["aPosition"] = Property::VECTOR2;
  texturedQuadVertexFormat["aVertexCoord"] = Property::VECTOR2;

  PropertyBuffer propertyBuffer = PropertyBuffer::New( texturedQuadVertexFormat, 4 );

  BaseHandle handle(propertyBuffer);
  PropertyBuffer propertyBuffer2 = PropertyBuffer::DownCast(handle);
  DALI_TEST_EQUALS( (bool)propertyBuffer2, true, TEST_LOCATION );
  END_TEST;
}

int UtcDaliPropertyBufferDownCast02(void)
{
  TestApplication application;

  Handle handle = Handle::New(); // Create a custom object
  PropertyBuffer propertyBuffer = PropertyBuffer::DownCast(handle);
  DALI_TEST_EQUALS( (bool)propertyBuffer, false, TEST_LOCATION );
  END_TEST;
}

int UtcDaliPropertyBufferCopyConstructor(void)
{
  TestApplication application;

  PropertyBuffer propertyBuffer = CreatePropertyBuffer();

  PropertyBuffer propertyBufferCopy(propertyBuffer);

  DALI_TEST_EQUALS( (bool)propertyBufferCopy, true, TEST_LOCATION );
  DALI_TEST_EQUALS( propertyBufferCopy.GetSize(), 4u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliPropertyBufferAssignmentOperator(void)
{
  TestApplication application;

  PropertyBuffer propertyBuffer = CreatePropertyBuffer();

  PropertyBuffer propertyBuffer2;
  DALI_TEST_EQUALS( (bool)propertyBuffer2, false, TEST_LOCATION );

  propertyBuffer2 = propertyBuffer;
  DALI_TEST_EQUALS( (bool)propertyBuffer2, true, TEST_LOCATION );
  DALI_TEST_EQUALS( propertyBuffer2.GetSize(), 4u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliPropertyBufferConstraint01(void)
{
  TestApplication application;

  tet_infoline("Test that a non-uniform propertyBuffer property can be constrained");

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  Material material = Material::New( shader );
  material.SetProperty(Material::Property::COLOR, Color::WHITE);

  PropertyBuffer propertyBuffer = CreatePropertyBuffer();
  Geometry geometry = CreateQuadGeometryFromBuffer(propertyBuffer);
  Renderer renderer = Renderer::New( geometry, material );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  Vector4 initialColor = Color::WHITE;
  Property::Index colorIndex = propertyBuffer.RegisterProperty( "uFadeColor", initialColor );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( propertyBuffer.GetProperty<Vector4>(colorIndex), initialColor, TEST_LOCATION );

  // Apply constraint
  Constraint constraint = Constraint::New<Vector4>( propertyBuffer, colorIndex, TestConstraintNoBlue );
  constraint.Apply();
  application.SendNotification();
  application.Render(0);

  // Expect no blue component in either buffer - yellow
  DALI_TEST_EQUALS( propertyBuffer.GetProperty<Vector4>(colorIndex), Color::YELLOW, TEST_LOCATION );
  application.Render(0);
  DALI_TEST_EQUALS( propertyBuffer.GetProperty<Vector4>(colorIndex), Color::YELLOW, TEST_LOCATION );

  propertyBuffer.RemoveConstraints();
  propertyBuffer.SetProperty(colorIndex, Color::WHITE );
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( propertyBuffer.GetProperty<Vector4>(colorIndex), Color::WHITE, TEST_LOCATION );

  END_TEST;
}

int UtcDaliPropertyBufferConstraint02(void)
{
  TestApplication application;

  tet_infoline("Test that a uniform map propertyBuffer property can be constrained");

  Shader shader = Shader::New( "VertexSource", "FragmentSource" );
  Material material = Material::New( shader );
  material.SetProperty(Material::Property::COLOR, Color::WHITE);

  PropertyBuffer propertyBuffer = CreatePropertyBuffer();
  Geometry geometry = CreateQuadGeometryFromBuffer(propertyBuffer);
  Renderer renderer = Renderer::New( geometry, material );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  Vector4 initialColor = Color::WHITE;
  Property::Index colorIndex = propertyBuffer.RegisterProperty( "uFadeColor", initialColor );

  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, initialColor, TEST_LOCATION );

  // Apply constraint
  Constraint constraint = Constraint::New<Vector4>( propertyBuffer, colorIndex, TestConstraintNoBlue );
  constraint.Apply();
  application.SendNotification();
  application.Render(0);

   // Expect no blue component in either buffer - yellow
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::YELLOW, TEST_LOCATION );

  application.Render(0);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::YELLOW, TEST_LOCATION );

  propertyBuffer.RemoveConstraints();
  propertyBuffer.SetProperty(colorIndex, Color::WHITE );
  application.SendNotification();
  application.Render(0);

  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::WHITE, TEST_LOCATION );

  END_TEST;
}



int UtcDaliPropertyBufferAnimatedProperty01(void)
{
  TestApplication application;

  tet_infoline("Test that a non-uniform propertyBuffer property can be animated");

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  Material material = Material::New( shader );
  material.SetProperty(Material::Property::COLOR, Color::WHITE);

  PropertyBuffer propertyBuffer = CreatePropertyBuffer();
  Geometry geometry = CreateQuadGeometryFromBuffer(propertyBuffer);
  Renderer renderer = Renderer::New( geometry, material );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);

  Vector4 initialColor = Color::WHITE;
  Property::Index colorIndex = propertyBuffer.RegisterProperty( "uFadeColor", initialColor );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( propertyBuffer.GetProperty<Vector4>(colorIndex), initialColor, TEST_LOCATION );

  Animation  animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, initialColor);
  keyFrames.Add(1.0f, Color::TRANSPARENT);
  animation.AnimateBetween( Property( propertyBuffer, colorIndex ), keyFrames );
  animation.Play();

  application.SendNotification();
  application.Render(500);

  DALI_TEST_EQUALS( propertyBuffer.GetProperty<Vector4>(colorIndex), Color::WHITE * 0.5f, TEST_LOCATION );

  application.Render(500);

  DALI_TEST_EQUALS( propertyBuffer.GetProperty<Vector4>(colorIndex), Color::TRANSPARENT, TEST_LOCATION );

  END_TEST;
}

int UtcDaliPropertyBufferAnimatedProperty02(void)
{
  TestApplication application;

  tet_infoline("Test that a uniform map propertyBuffer property can be animated");

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  Material material = Material::New( shader );
  material.SetProperty(Material::Property::COLOR, Color::WHITE);

  PropertyBuffer propertyBuffer = CreatePropertyBuffer();
  Geometry geometry = CreateQuadGeometryFromBuffer(propertyBuffer);
  Renderer renderer = Renderer::New( geometry, material );

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetSize(400, 400);
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render(0);

  Vector4 initialColor = Color::WHITE;
  Property::Index colorIndex = propertyBuffer.RegisterProperty( "uFadeColor", initialColor );

  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, initialColor, TEST_LOCATION );

  Animation  animation = Animation::New(1.0f);
  KeyFrames keyFrames = KeyFrames::New();
  keyFrames.Add(0.0f, initialColor);
  keyFrames.Add(1.0f, Color::TRANSPARENT);
  animation.AnimateBetween( Property( propertyBuffer, colorIndex ), keyFrames );
  animation.Play();

  application.SendNotification();
  application.Render(500);

  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::WHITE * 0.5f, TEST_LOCATION );

  application.Render(500);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uFadeColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::TRANSPARENT, TEST_LOCATION );

  END_TEST;
}

int UtcDaliPropertyBufferSetData01(void)
{
  TestApplication application;

  Property::Map texturedQuadVertexFormat;
  texturedQuadVertexFormat["aPosition"] = Property::VECTOR2;
  texturedQuadVertexFormat["aVertexCoord"] = Property::VECTOR2;

  PropertyBuffer propertyBuffer = PropertyBuffer::New( texturedQuadVertexFormat, 4 );
  DALI_TEST_EQUALS( (bool)propertyBuffer, true, TEST_LOCATION );

  const float halfQuadSize = .5f;
  struct TexturedQuadVertex { Vector2 position; Vector2 textureCoordinates; };
  TexturedQuadVertex texturedQuadVertexData[4] = {
    { Vector2(-halfQuadSize, -halfQuadSize), Vector2(0.f, 0.f) },
    { Vector2( halfQuadSize, -halfQuadSize), Vector2(1.f, 0.f) },
    { Vector2(-halfQuadSize,  halfQuadSize), Vector2(0.f, 1.f) },
    { Vector2( halfQuadSize,  halfQuadSize), Vector2(1.f, 1.f) } };

  propertyBuffer.SetData( texturedQuadVertexData );

  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer( propertyBuffer );

  Material material = CreateMaterial(1.f);
  Renderer renderer = Renderer::New(geometry, material);
  Actor actor = Actor::New();
  actor.SetSize(Vector3::ONE * 100.f);
  actor.AddRenderer(renderer);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();

  const TestGlAbstraction::BufferDataCalls& bufferDataCalls =
      application.GetGlAbstraction().GetBufferDataCalls();

  DALI_TEST_EQUALS( bufferDataCalls.size(), 1u, TEST_LOCATION );

  DALI_TEST_EQUALS( bufferDataCalls[0], sizeof(texturedQuadVertexData), TEST_LOCATION );

  END_TEST;
}

int UtcDaliPropertyBufferSetData02(void)
{
  TestApplication application;

  Property::Map texturedQuadVertexFormat;
  texturedQuadVertexFormat["aPosition"] = Property::VECTOR2;
  texturedQuadVertexFormat["aVertexCoord"] = Property::VECTOR2;

  PropertyBuffer propertyBuffer = PropertyBuffer::New( texturedQuadVertexFormat, 4 );
  DALI_TEST_EQUALS( (bool)propertyBuffer, true, TEST_LOCATION );

  const float halfQuadSize = .5f;
  struct TexturedQuadVertex { Vector2 position; Vector2 textureCoordinates; };
  TexturedQuadVertex texturedQuadVertexData[4] = {
    { Vector2(-halfQuadSize, -halfQuadSize), Vector2(0.f, 0.f) },
    { Vector2( halfQuadSize, -halfQuadSize), Vector2(1.f, 0.f) },
    { Vector2(-halfQuadSize,  halfQuadSize), Vector2(0.f, 1.f) },
    { Vector2( halfQuadSize,  halfQuadSize), Vector2(1.f, 1.f) } };

  propertyBuffer.SetData( texturedQuadVertexData );

  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer( propertyBuffer );

  Material material = CreateMaterial(1.f);
  Renderer renderer = Renderer::New(geometry, material);
  Actor actor = Actor::New();
  actor.SetSize(Vector3::ONE * 100.f);
  actor.AddRenderer(renderer);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();

  {
    const TestGlAbstraction::BufferDataCalls& bufferDataCalls =
      application.GetGlAbstraction().GetBufferDataCalls();

    DALI_TEST_EQUALS( bufferDataCalls.size(), 1u, TEST_LOCATION );

    DALI_TEST_EQUALS( bufferDataCalls[0], sizeof(texturedQuadVertexData), TEST_LOCATION );
  }

  // Re-upload the data on the propertyBuffer
  propertyBuffer.SetData( texturedQuadVertexData );

  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();

  {
    const TestGlAbstraction::BufferSubDataCalls& bufferSubDataCalls =
      application.GetGlAbstraction().GetBufferSubDataCalls();

    DALI_TEST_EQUALS( bufferSubDataCalls.size(), 1u, TEST_LOCATION );

    if ( bufferSubDataCalls.size() )
    {
      DALI_TEST_EQUALS( bufferSubDataCalls[0], sizeof(texturedQuadVertexData), TEST_LOCATION );
    }
  }

  END_TEST;
}

int UtcDaliPropertyBufferSetGetSize01(void)
{
  TestApplication application;

  Property::Map texturedQuadVertexFormat;
  texturedQuadVertexFormat["aPosition"] = Property::VECTOR2;
  texturedQuadVertexFormat["aVertexCoord"] = Property::VECTOR2;

  PropertyBuffer propertyBuffer = PropertyBuffer::New( texturedQuadVertexFormat, 4u );
  DALI_TEST_EQUALS( (bool)propertyBuffer, true, TEST_LOCATION );

  size_t size = propertyBuffer.GetSize();
  DALI_TEST_EQUALS( size, 4u, TEST_LOCATION );

  propertyBuffer.SetSize( 10u );
  size = propertyBuffer.GetSize();
  DALI_TEST_EQUALS( size, 10u, TEST_LOCATION );

  END_TEST;
}

//Todo: also test that the SetSize function is equivalent to setting the property SIZE
int UtcDaliPropertyBufferSetGetSize02(void)
{
  TestApplication application;

  Property::Map texturedQuadVertexFormat;
  texturedQuadVertexFormat["aPosition"] = Property::VECTOR2;
  texturedQuadVertexFormat["aVertexCoord"] = Property::VECTOR2;

  unsigned int size = 5u;
  PropertyBuffer propertyBuffer = PropertyBuffer::New( texturedQuadVertexFormat, size );
  DALI_TEST_EQUALS( propertyBuffer.GetProperty<unsigned int>(PropertyBuffer::Property::SIZE), size, TEST_LOCATION );
  DALI_TEST_EQUALS( propertyBuffer.GetSize(), size, TEST_LOCATION );

  size += 3u;
  propertyBuffer.SetSize( size );
  DALI_TEST_EQUALS( propertyBuffer.GetProperty<unsigned int>(PropertyBuffer::Property::SIZE), size, TEST_LOCATION );
  DALI_TEST_EQUALS( propertyBuffer.GetSize(), size, TEST_LOCATION );

  size += 2u;
  propertyBuffer.SetProperty(PropertyBuffer::Property::SIZE, size );
  DALI_TEST_EQUALS( propertyBuffer.GetSize(), size, TEST_LOCATION );

  END_TEST;
}

