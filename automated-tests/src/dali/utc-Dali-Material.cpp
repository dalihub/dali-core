//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <iostream>

#include <stdlib.h>
#include <dali/dali.h>
#include <dali-test-suite-utils.h>

using namespace Dali;


void utc_dali_material_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_material_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

static AnimatableMesh CreateMeshData(Material material)
{
  AnimatableMesh::Faces faces;
  for(int i=0; i<10-3; i++)
  {
    faces.push_back(i);
    faces.push_back(i+1);
    faces.push_back(i+2);
  }
  return AnimatableMesh::New(10, faces, material);
}

}// anonymous namespace


// Positive test case for a method
int UtcDaliMaterialNew01(void)
{
  TestApplication application;
  tet_infoline("Testing constructors, New and destructors");

  Material material;
  DALI_TEST_CHECK( ! material );

  material = Material::New("material");
  DALI_TEST_CHECK( material );

  Material* material2 = new Material();
  DALI_TEST_CHECK( ! *material2 );
  delete material2;

  Material material3 = material;
  Material material4;
  material4 = material;
  Material material5 = material;
  END_TEST;
}


int UtcDaliMaterialDownCast(void)
{
  TestApplication application;

  Material material = Material::New("material");
  BaseHandle handle(material);

  Material mat2 = Material::DownCast( handle );
  DALI_TEST_CHECK( mat2 );
  END_TEST;
}

int UtcDaliMaterialSettersAndGetters(void)
{
  TestApplication application;
  Material material = Material::New("material");
  DALI_TEST_EQUALS( material.GetName(), "material", TEST_LOCATION );
  material.SetName( "AnotherMaterial" );
  DALI_TEST_EQUALS( material.GetName(), "AnotherMaterial", TEST_LOCATION );

  DALI_TEST_EQUALS( material.GetOpacity(), Material::DEFAULT_OPACITY, 0.001, TEST_LOCATION);
  material.SetOpacity(0.38f);
  DALI_TEST_EQUALS( material.GetOpacity(), 0.38f, 0.001, TEST_LOCATION);

  DALI_TEST_EQUALS( material.GetShininess(), Material::DEFAULT_SHININESS, 0.001, TEST_LOCATION);
  material.SetShininess(0.47f);
  DALI_TEST_EQUALS( material.GetShininess(), 0.47f, 0.001, TEST_LOCATION);

  DALI_TEST_EQUALS( material.GetAmbientColor(), Material::DEFAULT_AMBIENT_COLOR, 0.001, TEST_LOCATION);
  material.SetAmbientColor(Color::BLACK);
  DALI_TEST_EQUALS( material.GetAmbientColor(), Color::BLACK, 0.001, TEST_LOCATION);

  DALI_TEST_EQUALS( material.GetDiffuseColor(), Material::DEFAULT_DIFFUSE_COLOR, 0.001, TEST_LOCATION);
  material.SetDiffuseColor(Color::BLUE);
  DALI_TEST_EQUALS( material.GetDiffuseColor(), Color::BLUE, 0.001, TEST_LOCATION);

  DALI_TEST_EQUALS( material.GetSpecularColor(), Material::DEFAULT_SPECULAR_COLOR, 0.001, TEST_LOCATION);
  material.SetSpecularColor(Color::GREEN);
  DALI_TEST_EQUALS( material.GetSpecularColor(), Color::GREEN, 0.001, TEST_LOCATION);

  DALI_TEST_EQUALS( material.GetEmissiveColor(), Material::DEFAULT_EMISSIVE_COLOR, 0.001, TEST_LOCATION);
  material.SetEmissiveColor(Color::MAGENTA);
  DALI_TEST_EQUALS( material.GetEmissiveColor(), Color::MAGENTA, 0.001, TEST_LOCATION);

  material.SetDiffuseTextureFileName("diffuse-texture.png");
  DALI_TEST_EQUALS( material.GetDiffuseFileName(), "diffuse-texture.png", TEST_LOCATION);

  material.SetOpacityTextureFileName("opacity-texture.png");
  DALI_TEST_EQUALS( material.GetOpacityTextureFileName(), "opacity-texture.png", TEST_LOCATION);

  material.SetNormalMapFileName("normal-map.png");
  DALI_TEST_EQUALS( material.GetNormalMapFileName(), "normal-map.png", TEST_LOCATION);

  Image diffuseTexture = Image::New("diffuse-texture.png");
  material.SetDiffuseTexture(diffuseTexture);
  DALI_TEST_EQUALS( material.GetDiffuseTexture(), diffuseTexture, TEST_LOCATION );

  Image opacityTexture = Image::New("opacity-texture.png");
  material.SetOpacityTexture(opacityTexture);
  DALI_TEST_EQUALS( material.GetOpacityTexture(), opacityTexture, TEST_LOCATION);

  Image normalMap = Image::New("normal-map.png");
  material.SetNormalMap(normalMap);
  DALI_TEST_EQUALS( material.GetNormalMap(), normalMap, TEST_LOCATION);

  DALI_TEST_EQUALS( material.GetMapU(), (unsigned int)Material::DEFAULT_MAPPING_MODE, TEST_LOCATION );
  DALI_TEST_EQUALS( material.GetMapV(), (unsigned int)Material::DEFAULT_MAPPING_MODE, TEST_LOCATION );
  material.SetMapU( Material::MAPPING_MODE_WRAP );
  material.SetMapV( Material::MAPPING_MODE_MIRROR );
  DALI_TEST_EQUALS( material.GetMapU(), (unsigned int)Material::MAPPING_MODE_WRAP, TEST_LOCATION );
  DALI_TEST_EQUALS( material.GetMapV(), (unsigned int)Material::MAPPING_MODE_MIRROR, TEST_LOCATION );

  DALI_TEST_EQUALS( material.GetDiffuseUVIndex(), Material::DEFAULT_DIFFUSE_UV_INDEX, TEST_LOCATION );
  material.SetDiffuseUVIndex( 1u );
  DALI_TEST_EQUALS( material.GetDiffuseUVIndex(), 1u, TEST_LOCATION );

  DALI_TEST_EQUALS( material.GetOpacityUVIndex(), Material::DEFAULT_OPACITY_UV_INDEX, TEST_LOCATION );
  material.SetOpacityUVIndex( 1u );
  DALI_TEST_EQUALS( material.GetOpacityUVIndex(), 1u, TEST_LOCATION );

  DALI_TEST_EQUALS( material.GetNormalUVIndex(), Material::DEFAULT_NORMAL_UV_INDEX, TEST_LOCATION );
  material.SetNormalUVIndex( 1u );
  DALI_TEST_EQUALS( material.GetNormalUVIndex(), 1u, TEST_LOCATION );

  DALI_TEST_EQUALS( material.GetHasHeightMap(), Material::DEFAULT_HAS_HEIGHT_MAP, TEST_LOCATION );
  material.SetHasHeightMap(true);
  DALI_TEST_EQUALS( material.GetHasHeightMap(), true, TEST_LOCATION );
  END_TEST;
}

int UtcDaliMaterialStage01(void)
{
  TestApplication application;
  TraceCallStack& textureTrace = application.GetGlAbstraction().GetTextureTrace();
  textureTrace.Enable(true);
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();

  {
    Material material = Material::New("material");
    Image image = Image::New( "image.png", Image::Immediate, Image::Never );
    DALI_TEST_CHECK(image);
    application.SendNotification();
    application.Render(16);

    std::vector<GLuint> ids;
    ids.push_back( 23 );
    application.GetGlAbstraction().SetNextTextureIds( ids );
    Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, true );
    Integration::ResourcePointer resource(bitmap);
    bitmap->GetPackedPixelsProfile()->ReserveBuffer(Pixel::RGBA8888, 80, 80, 80, 80);
    DALI_TEST_CHECK( application.GetPlatform().WasCalled(TestPlatformAbstraction::LoadResourceFunc) );
    Integration::ResourceRequest* request = application.GetPlatform().GetRequest();
    DALI_TEST_CHECK( request != NULL );
    if(request)
    {
      application.GetPlatform().SetResourceLoaded(request->GetId(), request->GetType()->id, resource);
    }

    material.SetDiffuseTexture(image);
    application.SendNotification();
    application.Render();

    AnimatableMesh mesh = CreateMeshData(material);

    application.SendNotification();
    application.Render();
    {
      MeshActor meshActor = MeshActor::New(mesh);
      meshActor.SetSize(100, 100);
      meshActor.SetAffectedByLighting(false);
      Stage::GetCurrent().Add(meshActor);
      application.SendNotification();
      application.Render();

      material.SetOpacity(0.5f);
      application.SendNotification();
      application.Render();
      DALI_TEST_EQUALS( material.GetOpacity(), 0.5f, 0.001f, TEST_LOCATION );

      Stage::GetCurrent().Remove(meshActor);
      application.SendNotification();
      application.Render();

      DALI_TEST_CHECK( glAbstraction.CheckNoTexturesDeleted() );
      DALI_TEST_CHECK( ! textureTrace.FindMethod( "DeleteTextures" ) );
    }
    application.SendNotification();
    application.Render();

    // Mesh should be destroyed, reducing connection count on material to zero.
    // This should also reduce connection count on image to zero
    DALI_TEST_EQUALS( material.GetOpacity(), 0.5f, 0.001f, TEST_LOCATION );
  }
  // SceneGraph::Material & SceneGraph::RenderMaterial should be destroyed
  // Image should be destroyed
  application.SendNotification();
  application.Render();

  application.Render();
  DALI_TEST_CHECK( textureTrace.FindMethod( "DeleteTextures" ) );
  DALI_TEST_CHECK( glAbstraction.CheckTextureDeleted( 23 ) );
  END_TEST;
}


int UtcDaliMaterialStage01MemCheck(void)
{
  TestApplication application;
  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliMaterialStage02(void)
{
  TestApplication application;
  TraceCallStack& textureTrace = application.GetGlAbstraction().GetTextureTrace();
  textureTrace.Enable(true);
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();

  {
    Material material = Material::New("material");

    Image image = Image::New( "image.png", Image::OnDemand, Image::Unused );
    DALI_TEST_CHECK(image);
    application.SendNotification();
    application.Render(16);
    DALI_TEST_CHECK( !application.GetPlatform().WasCalled(TestPlatformAbstraction::LoadResourceFunc) );
    DALI_TEST_CHECK( application.GetPlatform().GetRequest() == NULL );

    std::vector<GLuint> ids;
    ids.push_back( 23 );
    application.GetGlAbstraction().SetNextTextureIds( ids );

    material.SetDiffuseTexture(image);
    application.SendNotification();
    application.Render();
    DALI_TEST_CHECK( !application.GetPlatform().WasCalled(TestPlatformAbstraction::LoadResourceFunc) );
    DALI_TEST_CHECK( application.GetPlatform().GetRequest() == NULL );

    AnimatableMesh mesh = CreateMeshData(material);

    application.SendNotification();
    application.Render();
    DALI_TEST_CHECK( !application.GetPlatform().WasCalled(TestPlatformAbstraction::LoadResourceFunc) );
    DALI_TEST_CHECK( application.GetPlatform().GetRequest() == NULL );

    {
      MeshActor meshActor = MeshActor::New(mesh);
      meshActor.SetSize(100, 100);
      meshActor.SetAffectedByLighting(false);
      Stage::GetCurrent().Add(meshActor);
      application.SendNotification();
      application.Render();

      // Image connection count should go to one - image should get loaded
      DALI_TEST_CHECK( application.GetPlatform().WasCalled(TestPlatformAbstraction::LoadResourceFunc) );
      Integration::ResourceRequest* request = application.GetPlatform().GetRequest();
      DALI_TEST_CHECK( request != NULL );

      Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, true );
      Integration::ResourcePointer resource(bitmap);
      bitmap->GetPackedPixelsProfile()->ReserveBuffer(Pixel::RGBA8888, 80, 80, 80, 80);
      if(request)
      {
        application.GetPlatform().SetResourceLoaded(request->GetId(), request->GetType()->id, resource);
      }
      application.Render();

      material.SetOpacity(0.5f);
      application.SendNotification();
      application.Render();
      DALI_TEST_EQUALS( material.GetOpacity(), 0.5f, 0.001f, TEST_LOCATION );

      Stage::GetCurrent().Remove(meshActor);
      application.SendNotification();
      application.Render();

      // This should reduce connection count on material to zero, freeing the texture:
      DALI_TEST_CHECK( textureTrace.FindMethod( "DeleteTextures" ) );
      DALI_TEST_CHECK( glAbstraction.CheckTextureDeleted( 23 ) );
    }
    application.SendNotification();
    application.Render();

    // Mesh should be destroyed, reducing connection count on material to zero.
    // This should also reduce connection count on image to zero, freeing it
    DALI_TEST_EQUALS( material.GetOpacity(), 0.5f, 0.001f, TEST_LOCATION );
  }
  application.SendNotification();
  application.Render();

  // SceneGraph::Material & SceneGraph::RenderMaterial should be destroyed
  END_TEST;
}
