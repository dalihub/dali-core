/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

using namespace Dali;

#include <mesh-builder.h>

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

  PropertyBuffer propertyBuffer = PropertyBuffer::New( texturedQuadVertexFormat );

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

  PropertyBuffer propertyBuffer = PropertyBuffer::New( texturedQuadVertexFormat );

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
  DALI_TEST_EQUALS( propertyBufferCopy.GetSize(), 0u, TEST_LOCATION );

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
  DALI_TEST_EQUALS( propertyBuffer2.GetSize(), 0u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliPropertyBufferSetData01(void)
{
  TestApplication application;

  Property::Map texturedQuadVertexFormat;
  texturedQuadVertexFormat["aPosition"] = Property::VECTOR2;
  texturedQuadVertexFormat["aVertexCoord"] = Property::VECTOR2;

  {
    PropertyBuffer propertyBuffer = PropertyBuffer::New( texturedQuadVertexFormat );
    DALI_TEST_EQUALS( (bool)propertyBuffer, true, TEST_LOCATION );

    const float halfQuadSize = .5f;
    struct TexturedQuadVertex { Vector2 position; Vector2 textureCoordinates; };
    TexturedQuadVertex texturedQuadVertexData[4] = {
      { Vector2(-halfQuadSize, -halfQuadSize), Vector2(0.f, 0.f) },
      { Vector2( halfQuadSize, -halfQuadSize), Vector2(1.f, 0.f) },
      { Vector2(-halfQuadSize,  halfQuadSize), Vector2(0.f, 1.f) },
      { Vector2( halfQuadSize,  halfQuadSize), Vector2(1.f, 1.f) } };

    propertyBuffer.SetData( texturedQuadVertexData, 4 );

    Geometry geometry = Geometry::New();
    geometry.AddVertexBuffer( propertyBuffer );

    Shader shader = CreateShader();
    Renderer renderer = Renderer::New(geometry, shader);
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

  }
  // end of scope to let the buffer and geometry die; do another notification and render to get the deletion processed
  application.SendNotification();
  application.Render(0);

  END_TEST;
}

int UtcDaliPropertyBufferSetData02(void)
{
  TestApplication application;

  Property::Map texturedQuadVertexFormat;
  texturedQuadVertexFormat["aPosition"] = Property::VECTOR2;
  texturedQuadVertexFormat["aVertexCoord"] = Property::VECTOR2;

  PropertyBuffer propertyBuffer = PropertyBuffer::New( texturedQuadVertexFormat );
  DALI_TEST_EQUALS( (bool)propertyBuffer, true, TEST_LOCATION );

  const float halfQuadSize = .5f;
  struct TexturedQuadVertex { Vector2 position; Vector2 textureCoordinates; };
  TexturedQuadVertex texturedQuadVertexData[4] = {
    { Vector2(-halfQuadSize, -halfQuadSize), Vector2(0.f, 0.f) },
    { Vector2( halfQuadSize, -halfQuadSize), Vector2(1.f, 0.f) },
    { Vector2(-halfQuadSize,  halfQuadSize), Vector2(0.f, 1.f) },
    { Vector2( halfQuadSize,  halfQuadSize), Vector2(1.f, 1.f) } };

  propertyBuffer.SetData( texturedQuadVertexData, 4 );

  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer( propertyBuffer );

  Shader shader = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);
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
  propertyBuffer.SetData( texturedQuadVertexData, 4 );

  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();

  {
    const TestGlAbstraction::BufferSubDataCalls& bufferSubDataCalls =
      application.GetGlAbstraction().GetBufferSubDataCalls();

    const TestGlAbstraction::BufferDataCalls& bufferDataCalls =
          application.GetGlAbstraction().GetBufferDataCalls();

    DALI_TEST_EQUALS( bufferSubDataCalls.size(), 1u, TEST_LOCATION );
    DALI_TEST_EQUALS( bufferDataCalls.size(), 1u, TEST_LOCATION );

    if ( bufferSubDataCalls.size() )
    {
      DALI_TEST_EQUALS( bufferSubDataCalls[0], sizeof(texturedQuadVertexData), TEST_LOCATION );
    }
  }

  END_TEST;
}

int UtcDaliPropertyBufferInvalidTypeN(void)
{
  TestApplication application;

  Property::Map texturedQuadVertexFormat;
  texturedQuadVertexFormat["aPosition"] = Property::MAP;
  texturedQuadVertexFormat["aVertexCoord"] = Property::STRING;

  try
  {
    PropertyBuffer propertyBuffer = PropertyBuffer::New( texturedQuadVertexFormat );
    tet_result(TET_FAIL);
  }
  catch ( Dali::DaliException& e )
  {
    DALI_TEST_ASSERT( e, "Property::Type not supported in PropertyBuffer", TEST_LOCATION );
  }
  END_TEST;
}

