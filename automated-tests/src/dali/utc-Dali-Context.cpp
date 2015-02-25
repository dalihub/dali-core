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


namespace
{
// Size of the VertexAttributeArray enables
// GLES specification states that there's minimum of
const unsigned int TEST_MAX_ATTRIBUTE_CACHE_SIZE = 8;

enum TestAttribType
{
  ATTRIB_UNKNOWN = -1,
  ATTRIB_POSITION,
  ATTRIB_NORMAL,
  ATTRIB_TEXCOORD,
  ATTRIB_COLOR,
  ATTRIB_BONE_WEIGHTS,
  ATTRIB_BONE_INDICES,
  ATTRIB_TYPE_LAST
};

// Create bitmap image
static BufferImage CreateBufferImage()
{
  BufferImage image = BufferImage::New(4,4,Pixel::RGBA8888);

  return image;
}

static MeshActor CreateMeshActor()
{
  MeshData meshData;
  MeshData::VertexContainer    vertices;
  MeshData::FaceIndices        faces;
  BoneContainer                bones;
  ConstructVertices(vertices, 60);
  ConstructFaces(vertices, faces);
  Material customMaterial = ConstructMaterial();
  meshData.SetData(vertices, faces, bones, customMaterial);
  meshData.SetHasNormals(true);
  meshData.SetHasTextureCoords(true);

  Mesh mesh = Mesh::New(meshData);
  MeshActor actor = MeshActor::New(mesh);
  actor.SetName("Test MeshActor");

  return actor;
}

static LightActor CreateLight()
{
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

  return lightActor;
}

static ImageActor CreateImageActor()
{
  BufferImage image = CreateBufferImage();
  ImageActor actor = ImageActor::New( image );
  actor.SetSize( 100.0f, 100.0f );
  actor.SetName("Test ImageActor");
  return actor;
}

} // anonymous namespace


// Positive test case for a method
int UtcDaliContextVertexAttribStartup(void)
{
  tet_infoline("Testing vertex attrib initial state in context");

  TestApplication application;

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  // context class should initially set the vertex attrib locations to disable
  // Make sure it has been modified
  DALI_TEST_CHECK(application.GetGlAbstraction().GetVertexAttribArrayChanged());

  // check the locations
  for (unsigned int i = 0; i < TEST_MAX_ATTRIBUTE_CACHE_SIZE; i++)
  {
    DALI_TEST_CHECK( application.GetGlAbstraction().GetVertexAttribArrayState(i) == false);
  }

  tet_result(TET_PASS);
  END_TEST;
}

// Tests to make the attribs only get set once when continually rendering an image actor
int UtcDaliContextVertexAttribImageRendering(void)
{
  tet_infoline("Testing vertex attrib rendering state in context with images");

  TestApplication application;

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  // the vertex attribs get modified on startup to set them to disabled
  // clear the flag to say they've changed
  application.GetGlAbstraction().ClearVertexAttribArrayChanged();


  // create a test image actor
  ImageActor imageActor(CreateImageActor());
  Stage::GetCurrent().Add(imageActor);


  application.SendNotification();
  application.Render();
  application.Render();

  // check to make sure the state has changed (the image renderer will enable some
  // locations).
  DALI_TEST_CHECK(application.GetGlAbstraction().GetVertexAttribArrayChanged());

  // Now check to make sure the state is cached, and isn't being set each frame.
  application.GetGlAbstraction().ClearVertexAttribArrayChanged();

  application.Render();
  application.Render();
  application.Render();

  // if it has changed then the caching has failed
  DALI_TEST_CHECK(application.GetGlAbstraction().GetVertexAttribArrayChanged() == false);


  tet_result(TET_PASS);
  END_TEST;
}

// test to make sure the attribs change when rendering both image and mode actors
int UtcDaliContextVertexAttribImageAndModelRendering(void)
{
  tet_infoline("Testing vertex attrib rendering state in context with images and models");

  TestApplication application;

  // start up
  application.SendNotification();
  application.Render();
  application.Render();

  // the vertex attribs get modified on startup to set them to disabled
  // clear the flag to say they've changed
  application.GetGlAbstraction().ClearVertexAttribArrayChanged();

  // create a test light, image and mesh actor. (meshes won't render without light)

  LightActor lightActor(CreateLight());
  Stage::GetCurrent().Add(lightActor);
  lightActor.SetActive(true);

  MeshActor meshActor(CreateMeshActor());
  Stage::GetCurrent().Add(meshActor);

  ImageActor imageActor(CreateImageActor());
  Stage::GetCurrent().Add(imageActor);


  application.SendNotification();
  application.Render();
  application.Render();

  // check to make sure the state changes during the rendering of a frame
  DALI_TEST_CHECK(application.GetGlAbstraction().GetVertexAttribArrayChanged());

  // Now check to make sure the state is changing each frame.
  application.GetGlAbstraction().ClearVertexAttribArrayChanged();

  application.Render();
  application.Render();
  application.Render();

  // make sure the state has changed
  DALI_TEST_CHECK(application.GetGlAbstraction().GetVertexAttribArrayChanged());

  // depending on the order of drawing, one of the attrib locations should be disabled
  // Image uses locations 0 & 2  (position, texture)
  // Model uses locations 0 & 1  (position, normals) -no textures
  // so either location 1 or location 2 should be disabled after drawing.

  // see if mesh was last to draw
  if (application.GetGlAbstraction().GetVertexAttribArrayState(ATTRIB_NORMAL))
  {
    // texture should be disabled
    DALI_TEST_CHECK( application.GetGlAbstraction().GetVertexAttribArrayState(ATTRIB_TEXCOORD) == false)
  }
  else
  {
    // image was to draw so, normals should be disabled
    DALI_TEST_CHECK( application.GetGlAbstraction().GetVertexAttribArrayState(ATTRIB_NORMAL) == false)
  }

  tet_result(TET_PASS);

  END_TEST;
}
