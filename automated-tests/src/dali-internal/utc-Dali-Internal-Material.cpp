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

#include <mesh-builder.h>

#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/update/resources/resource-manager.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/event/resources/resource-client.h>
#include <dali/internal/event/resources/resource-ticket.h>
#include <dali/internal/event/resources/image-ticket.h>
#include <dali/internal/event/resources/resource-ticket-observer.h>
#include <dali/internal/event/images/image-impl.h>
#include <dali/internal/event/modeling/model-data-impl.h>
#include <dali/integration-api/resource-cache.h>
#include <dali/internal/event/modeling/material-impl.h>
#include <dali/internal/update/modeling/scene-graph-material.h>
#include <dali/internal/render/renderers/render-material.h>
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/render/shaders/program.h>
#include <dali/internal/common/image-sampler.h>

namespace
{
Dali::Internal::MaterialProperties TEST_PROPS;
}

// Called only once before first test is run.
void utc_dali_material_startup(void)
{
  TEST_PROPS.mOpacity       = 0.4f;
  TEST_PROPS.mShininess     = 0.27f;
  TEST_PROPS.mDiffuseColor  = Color::MAGENTA;
  TEST_PROPS.mAmbientColor  = Color::GREEN;
  TEST_PROPS.mSpecularColor = Color::BLUE;
  TEST_PROPS.mEmissiveColor = Color::RED;
  test_return_value = TET_UNDEF;
}

// Called only once after last test is run
void utc_dali_material_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

class TestProgram
{
public:
  TestProgram( TestApplication& application )
  : app(application)
  {
    Internal::Context* testContext = new Internal::Context( application.GetGlAbstraction() );
    Integration::ShaderDataPtr shaderData = new Integration::ShaderData("123", "132");
    shaderData->AllocateBuffer(10);

    Integration::ResourceId resourceId = 100;
    program = Internal::Program::New(resourceId, shaderData.Get(), *testContext, true);
    programId = app.GetGlAbstraction().GetLastProgramCreated();
    program->Use();
  }

  virtual ~TestProgram()
  {
  }

  Internal::Program& GetProgram()
  {
    return *program;
  }

  float GetUniformF(std::string uniform)
  {
    GLint uniformLoc = program->GetUniformLocation( program->RegisterUniform( uniform.c_str() ) );
    float value=0.0f;
    if(app.GetGlAbstraction().GetUniformValue( programId, (GLuint) uniformLoc, value))
    {
      return value;
    }
    return 0.0f;
  }

  Vector4 GetUniformV(std::string uniform)
  {
    GLint uniformLoc = program->GetUniformLocation( program->RegisterUniform( uniform.c_str() ) );
    Vector4 value;
    if(app.GetGlAbstraction().GetUniformValue( programId, (GLuint) uniformLoc, value))
    {
      return value;
    }
    return Vector4();
  }

  float GetOpacity()         { return GetUniformF("uMaterial.mOpacity"); }
  float GetShininess()       { return GetUniformF("uMaterial.mShininess"); }
  Vector4 GetAmbientColor()  { return GetUniformV("uMaterial.mAmbient"); }
  Vector4 GetDiffuseColor()  { return GetUniformV("uMaterial.mDiffuse"); }
  Vector4 GetSpecularColor() { return GetUniformV("uMaterial.mSpecular"); }
  Vector4 GetEmissiveColor() { return GetUniformV("uMaterial.mEmissive"); }

  TestApplication& app;
  GLuint programId;
  Internal::Program* program;
  Integration::ShaderDataPtr shaderData;
};


class TestBoundTextures
{
public:
  TestBoundTextures(TestApplication& application)
  : app(application)
  {
    std::vector<GLuint> ids;
    ids.push_back( 8 ); // 8 = actor1
    ids.push_back( 9 ); // 9 = actor2
    ids.push_back( 10 ); // 10 = actor3
    application.GetGlAbstraction().SetNextTextureIds( ids );
  }

  std::size_t GetNumBoundTextures()
  {
    const std::vector<GLuint>& boundTextures = app.GetGlAbstraction().GetBoundTextures();
    return boundTextures.size();
  }

  bool CheckFirstTextureBound( GLuint activeTextureUnit )
  {
    bool bound=false;
    const std::vector<GLuint>& boundTextures = app.GetGlAbstraction().GetBoundTextures( activeTextureUnit );

    if ( boundTextures.size() == 1 )
    {
      if( boundTextures[0] == 8u )
      {
        bound = true;
      }
    }
    return bound;
  }

  bool CheckFirstTextureDeleted()
  {
    return ( app.GetGlAbstraction().CheckTextureDeleted( 8u ));
  }

  TestApplication& app;
};


Internal::ResourceTicketPtr CheckLoadBitmap(TestApplication& application, const char* name, int w, int h)
{
  Internal::ResourceClient& resourceClient = Internal::ThreadLocalStorage::Get().GetResourceClient();
  ImageAttributes attr;
  Integration::BitmapResourceType bitmapRequest(attr);
  Internal::ResourceTicketPtr ticket = resourceClient.RequestResource( bitmapRequest, name );
  application.SendNotification(); // Flush update messages
  application.Render();           // Process resource request
  Integration::ResourceRequest*   req = application.GetPlatform().GetRequest();
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, false );
  bitmap->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGBA8888, w, h, w, h );
  Integration::ResourcePointer resourcePtr(bitmap); // reference it
  application.GetPlatform().SetResourceLoaded(req->GetId(), req->GetType()->id, resourcePtr);
  application.Render();           // Process LoadComplete
  application.SendNotification(); // Process event messages
  DALI_TEST_CHECK( ticket->GetLoadingState() == ResourceLoadingSucceeded );
  application.GetPlatform().DiscardRequest(); // Ensure load request is discarded
  req=NULL;
  application.GetPlatform().ResetTrace();

  return ticket;
}

Internal::ImagePtr LoadImage(TestApplication& application, const char* name)
{
  Internal::ImagePtr image = Internal::Image::New(name);
  application.SendNotification(); // Flush update messages
  application.Render();           // Process resource request
  Integration::ResourceRequest* req = application.GetPlatform().GetRequest();
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, false );
  bitmap->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGBA8888, 80,80,80,80 );
  Integration::ResourcePointer resourcePtr(bitmap); // reference it

  application.GetPlatform().SetResourceLoaded(req->GetId(), req->GetType()->id, resourcePtr);
  application.Render();           // Process LoadComplete
  application.SendNotification(); // Process event messages
  application.GetPlatform().DiscardRequest(); // Ensure load request is discarded
  req=NULL;
  application.GetPlatform().ResetTrace();
  return image;
}

} // Anonymous Namespace

/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

// Test new with no parameters sets up default object
int UtcDaliMaterialMethodNew01(void)
{
  TestApplication application;

  Internal::SceneGraph::Material* sceneObject = Internal::SceneGraph::Material::New();
  Internal::MaterialProperties props = sceneObject->GetProperties(); // copy.
  DALI_TEST_CHECK(props.mOpacity == Dali::Material::DEFAULT_OPACITY);
  DALI_TEST_CHECK(props.mShininess == Dali::Material::DEFAULT_SHININESS);
  DALI_TEST_CHECK(props.mAmbientColor == Dali::Material::DEFAULT_AMBIENT_COLOR);
  DALI_TEST_CHECK(props.mDiffuseColor == Dali::Material::DEFAULT_DIFFUSE_COLOR);
  DALI_TEST_CHECK(props.mSpecularColor == Dali::Material::DEFAULT_SPECULAR_COLOR);
  DALI_TEST_CHECK(props.mEmissiveColor == Dali::Material::DEFAULT_EMISSIVE_COLOR);

  Internal::ResourceId textureId = sceneObject->GetDiffuseTextureId();
  DALI_TEST_CHECK( !textureId );
  textureId = sceneObject->GetOpacityTextureId();
  DALI_TEST_CHECK( !textureId );
  textureId = sceneObject->GetNormalMapId();
  DALI_TEST_CHECK( !textureId );

  DALI_TEST_CHECK(! sceneObject->HasDiffuseTexture());
  DALI_TEST_CHECK(! sceneObject->HasOpacityTexture());
  DALI_TEST_CHECK(! sceneObject->HasNormalMap());
  END_TEST;
}

// Test new with event object sets up parameters appropriately
int UtcDaliMaterialMethodNew02(void)
{
  TestApplication application;

  Internal::Material* material = Internal::Material::New("cloth");
  DALI_TEST_CHECK(material->GetShininess() == Dali::Material::DEFAULT_SHININESS);
  DALI_TEST_CHECK(material->GetAmbientColor() == Dali::Material::DEFAULT_AMBIENT_COLOR);
  material->SetOpacity(0.4f);
  material->SetDiffuseColor(Color::MAGENTA);

  // Create directly
  Internal::SceneGraph::Material* sceneObject = Internal::SceneGraph::Material::New(material);
  Internal::MaterialProperties props = sceneObject->GetProperties(); // copy.
  DALI_TEST_CHECK(props.mOpacity == 0.4f);
  DALI_TEST_CHECK(props.mDiffuseColor == Color::MAGENTA);
  DALI_TEST_CHECK(props.mShininess == Dali::Material::DEFAULT_SHININESS);
  DALI_TEST_CHECK(props.mAmbientColor == Dali::Material::DEFAULT_AMBIENT_COLOR);
  DALI_TEST_CHECK(props.mSpecularColor == Dali::Material::DEFAULT_SPECULAR_COLOR);
  DALI_TEST_CHECK(props.mEmissiveColor == Dali::Material::DEFAULT_EMISSIVE_COLOR);

  Internal::ResourceId textureId = sceneObject->GetDiffuseTextureId();
  DALI_TEST_CHECK( !textureId );
  textureId = sceneObject->GetOpacityTextureId();
  DALI_TEST_CHECK( !textureId );
  textureId = sceneObject->GetNormalMapId();
  DALI_TEST_CHECK( !textureId );

  DALI_TEST_CHECK(! sceneObject->HasDiffuseTexture());
  DALI_TEST_CHECK(! sceneObject->HasOpacityTexture());
  DALI_TEST_CHECK(! sceneObject->HasNormalMap());

  // Create indirectly
  const Internal::SceneGraph::Material* sceneObject2 = material->GetSceneObject();
  DALI_TEST_CHECK( sceneObject2 != NULL );
  Internal::MaterialProperties props2 = sceneObject2->GetProperties(); // copy.
  DALI_TEST_CHECK(props2.mOpacity == 0.4f);
  DALI_TEST_CHECK(props2.mDiffuseColor == Color::MAGENTA);
  DALI_TEST_CHECK(props2.mShininess == Dali::Material::DEFAULT_SHININESS);
  DALI_TEST_CHECK(props2.mAmbientColor == Dali::Material::DEFAULT_AMBIENT_COLOR);
  DALI_TEST_CHECK(props2.mSpecularColor == Dali::Material::DEFAULT_SPECULAR_COLOR);
  DALI_TEST_CHECK(props2.mEmissiveColor == Dali::Material::DEFAULT_EMISSIVE_COLOR);

  DALI_TEST_CHECK(! sceneObject2->GetDiffuseTextureId());
  DALI_TEST_CHECK(! sceneObject2->GetOpacityTextureId());
  DALI_TEST_CHECK(! sceneObject2->GetNormalMapId());

  DALI_TEST_CHECK(! sceneObject2->HasDiffuseTexture());
  DALI_TEST_CHECK(! sceneObject2->HasOpacityTexture());
  DALI_TEST_CHECK(! sceneObject2->HasNormalMap());
  END_TEST;
}

// Test setting ready texture off stage
int UtcDaliMaterialReadyTextureOffstage(void)
{
  TestApplication application;

  Internal::ResourceTicketPtr ticket = CheckLoadBitmap(application, "diffuse.png", 80, 80);
  Internal::SceneGraph::Material* sceneObject = Internal::SceneGraph::Material::New();
  sceneObject->SetDiffuseTextureId(ticket->GetId());

  DALI_TEST_EQUALS( sceneObject->GetDiffuseTextureId(), ticket->GetId(), TEST_LOCATION );
  END_TEST;
}

// Test setting unready texture offstage, becoming ready
int UtcDaliMaterialUnreadyTextureOffstage(void)
{
  TestApplication application;

  Internal::ResourceClient& resourceClient = Internal::ThreadLocalStorage::Get().GetResourceClient();
  ImageAttributes attr;
  Integration::BitmapResourceType bitmapRequest(attr);
  Internal::ResourceTicketPtr ticket = resourceClient.RequestResource( bitmapRequest, "image.png" );
  application.SendNotification(); // Flush update messages
  application.Render();           // Process resource request

  // Ticket is valid, but no resource yet
  Internal::SceneGraph::Material* sceneObject = Internal::SceneGraph::Material::New();
  Internal::ResourceId textureId = ticket->GetId();
  sceneObject->SetDiffuseTextureId(textureId);
  Internal::ResourceId textureId2 = sceneObject->GetDiffuseTextureId();
  DALI_TEST_CHECK( textureId == textureId2 );

  Integration::ResourceRequest*   req = application.GetPlatform().GetRequest();
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, false );
  bitmap->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGBA8888, 80, 80, 80, 80 );
  Integration::ResourcePointer resourcePtr(bitmap); // reference it
  application.GetPlatform().SetResourceLoaded(req->GetId(), req->GetType()->id, resourcePtr);
  application.Render();           // Process LoadComplete
  application.SendNotification(); // Process event messages
  DALI_TEST_CHECK( ticket->GetLoadingState() == ResourceLoadingSucceeded );

  Internal::ResourceId textureId3 = sceneObject->GetDiffuseTextureId();
  DALI_TEST_CHECK( textureId3 );
  DALI_TEST_CHECK( textureId3 == textureId );
  END_TEST;
}

// Test staging creates render material
int UtcDaliMaterialStaging01(void)
{
  TestApplication application;
  TestBoundTextures boundTextures(application);
  TestProgram testProgram(application);

  // Create object and set some properties
  Internal::SceneGraph::Material* sceneObject = Internal::SceneGraph::Material::New();
  DALI_TEST_CHECK( sceneObject != NULL );
  sceneObject->SetProperties(TEST_PROPS);

  // Stage the object
  Internal::SceneGraph::UpdateManager& updateManager = Internal::ThreadLocalStorage::Get().GetUpdateManager();
  AddMaterialMessage( updateManager, sceneObject );
  application.SendNotification(); // Flush update Q

  application.GetPlatform().IncrementGetTimeResult( 1 );
  Integration::UpdateStatus status;
  application.GetCore().Update( status );

  // Check that a render object has been created
  Internal::SceneGraph::RenderMaterial* renderMaterial = sceneObject->GetRenderMaterial();
  DALI_TEST_CHECK(renderMaterial != NULL);
  // Not on render manager, and should have default props

  Internal::SceneGraph::RenderMaterialUniforms materialUniforms;
  renderMaterial->SetUniforms( materialUniforms, testProgram.GetProgram(), Internal::SHADER_DEFAULT );

  DALI_TEST_EQUALS( testProgram.GetOpacity(),       1.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( testProgram.GetShininess(),     0.5f, TEST_LOCATION );
  DALI_TEST_EQUALS( testProgram.GetAmbientColor(),  Vector4(0.2f, 0.2f, 0.2f, 1.0f), TEST_LOCATION);
  DALI_TEST_EQUALS( testProgram.GetDiffuseColor(),  Vector4(0.8f, 0.8f, 0.8f, 1.0f), TEST_LOCATION);
  DALI_TEST_EQUALS( testProgram.GetSpecularColor(), Vector4(0.0f, 0.0f, 0.0f, 1.0f), TEST_LOCATION);
  DALI_TEST_EQUALS( testProgram.GetEmissiveColor(), Vector4(0.0f, 0.0f, 0.0f, 1.0f), TEST_LOCATION);

  application.Render(); //Process render Q stores & processes mat

  renderMaterial->SetUniforms( materialUniforms, testProgram.GetProgram(), Internal::SHADER_DEFAULT );
  renderMaterial->BindTextures( testProgram.GetProgram(), Internal::ImageSampler::PackBitfield( FilterMode::DEFAULT, FilterMode::DEFAULT ) );
  DALI_TEST_CHECK( boundTextures.GetNumBoundTextures() == 0 );

  DALI_TEST_EQUALS( testProgram.GetOpacity(),       TEST_PROPS.mOpacity,       TEST_LOCATION);
  DALI_TEST_EQUALS( testProgram.GetShininess(),     TEST_PROPS.mShininess,     TEST_LOCATION );
  DALI_TEST_EQUALS( testProgram.GetAmbientColor(),  TEST_PROPS.mAmbientColor,  TEST_LOCATION );
  DALI_TEST_EQUALS( testProgram.GetDiffuseColor(),  TEST_PROPS.mDiffuseColor,  TEST_LOCATION );
  DALI_TEST_EQUALS( testProgram.GetSpecularColor(), TEST_PROPS.mSpecularColor, TEST_LOCATION );
  DALI_TEST_EQUALS( testProgram.GetEmissiveColor(), TEST_PROPS.mEmissiveColor, TEST_LOCATION );
  END_TEST;
}

// Test staging creates render material
int UtcDaliMaterialStaging02(void)
{
  TestApplication application;
  TestBoundTextures boundTextures(application);
  TestProgram testProgram(application);

  Internal::Material* material = Internal::Material::New("cloth");
  material->SetOpacity(0.4f);
  material->SetDiffuseColor(Color::MAGENTA);

  // Create object and set some properties
  Internal::SceneGraph::Material* sceneObject = Internal::SceneGraph::Material::New(material);
  DALI_TEST_CHECK( sceneObject != NULL );

  // Stage the object
  Internal::SceneGraph::UpdateManager& updateManager = Internal::ThreadLocalStorage::Get().GetUpdateManager();
  AddMaterialMessage( updateManager, sceneObject );
  application.SendNotification(); // Flush update Q

  application.GetPlatform().IncrementGetTimeResult( 1 );
  Integration::UpdateStatus status;
  application.GetCore().Update( status );

  // Check that a render object has been created
  Internal::SceneGraph::RenderMaterial* renderMaterial = sceneObject->GetRenderMaterial();
  DALI_TEST_CHECK(renderMaterial != NULL);
  // Not on render manager, and should have default props

  Internal::SceneGraph::RenderMaterialUniforms materialUniforms;
  renderMaterial->SetUniforms( materialUniforms, testProgram.GetProgram(), Internal::SHADER_DEFAULT );

  DALI_TEST_EQUALS( testProgram.GetOpacity(),       1.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( testProgram.GetShininess(),     0.5f, TEST_LOCATION );
  DALI_TEST_EQUALS( testProgram.GetAmbientColor(),  Vector4(0.2f, 0.2f, 0.2f, 1.0f), TEST_LOCATION);
  DALI_TEST_EQUALS( testProgram.GetDiffuseColor(),  Vector4(0.8f, 0.8f, 0.8f, 1.0f), TEST_LOCATION);
  DALI_TEST_EQUALS( testProgram.GetSpecularColor(), Vector4(0.0f, 0.0f, 0.0f, 1.0f), TEST_LOCATION);
  DALI_TEST_EQUALS( testProgram.GetEmissiveColor(), Vector4(0.0f, 0.0f, 0.0f, 1.0f), TEST_LOCATION);

  application.Render(); //Process render Q stores & processes mat

  renderMaterial->SetUniforms( materialUniforms, testProgram.GetProgram(), Internal::SHADER_DEFAULT );
  renderMaterial->BindTextures( testProgram.GetProgram(), Internal::ImageSampler::PackBitfield( FilterMode::DEFAULT, FilterMode::DEFAULT ) );

  DALI_TEST_CHECK( boundTextures.GetNumBoundTextures() == 0 );
  DALI_TEST_EQUALS( testProgram.GetOpacity(),       0.4f, TEST_LOCATION);
  DALI_TEST_EQUALS( testProgram.GetShininess(),     Dali::Material::DEFAULT_SHININESS, TEST_LOCATION );
  DALI_TEST_EQUALS( testProgram.GetAmbientColor(),  Dali::Material::DEFAULT_AMBIENT_COLOR,  TEST_LOCATION );
  DALI_TEST_EQUALS( testProgram.GetDiffuseColor(),  Color::MAGENTA, TEST_LOCATION );
  DALI_TEST_EQUALS( testProgram.GetSpecularColor(), Dali::Material::DEFAULT_SPECULAR_COLOR, TEST_LOCATION );
  DALI_TEST_EQUALS( testProgram.GetEmissiveColor(), Dali::Material::DEFAULT_EMISSIVE_COLOR, TEST_LOCATION );
  END_TEST;
}



// Test setting properties on stage
int UtcDaliMaterialSetPropsWhilstStaged(void)
{
  TestApplication application;
  TestBoundTextures boundTextures(application);
  TestProgram testProgram(application);

  // Create object with default properties
  Internal::SceneGraph::Material* sceneObject = Internal::SceneGraph::Material::New();
  DALI_TEST_CHECK( sceneObject != NULL );

  // Stage the object
  Internal::SceneGraph::UpdateManager& updateManager = Internal::ThreadLocalStorage::Get().GetUpdateManager();
  AddMaterialMessage( updateManager, sceneObject );
  application.SendNotification(); // Flush update Q
  application.Render(); // Process update message Q then create & post to render Q

  // Check that a render object has been created
  Internal::SceneGraph::RenderMaterial* renderMaterial = sceneObject->GetRenderMaterial();
  DALI_TEST_CHECK(renderMaterial != NULL);
  application.Render(); // Update & Prepare renderMat, Process render Q stores & processes mat

  sceneObject->SetProperties(TEST_PROPS);
  application.SendNotification(); // Flush update Q
  application.Render(); // Update & Prepare material
  application.Render(); // Process render Q

  Internal::SceneGraph::RenderMaterialUniforms materialUniforms;
  renderMaterial->SetUniforms( materialUniforms, testProgram.GetProgram(), Internal::SHADER_DEFAULT );
  renderMaterial->BindTextures( testProgram.GetProgram(), Internal::ImageSampler::PackBitfield( FilterMode::DEFAULT, FilterMode::DEFAULT ) );

  DALI_TEST_EQUALS( boundTextures.GetNumBoundTextures(), 0u, TEST_LOCATION );

  DALI_TEST_EQUALS( testProgram.GetOpacity(),       TEST_PROPS.mOpacity,       TEST_LOCATION);
  DALI_TEST_EQUALS( testProgram.GetShininess(),     TEST_PROPS.mShininess,     TEST_LOCATION );
  DALI_TEST_EQUALS( testProgram.GetAmbientColor(),  TEST_PROPS.mAmbientColor,  TEST_LOCATION );
  DALI_TEST_EQUALS( testProgram.GetDiffuseColor(),  TEST_PROPS.mDiffuseColor,  TEST_LOCATION );
  DALI_TEST_EQUALS( testProgram.GetSpecularColor(), TEST_PROPS.mSpecularColor, TEST_LOCATION );
  DALI_TEST_EQUALS( testProgram.GetEmissiveColor(), TEST_PROPS.mEmissiveColor, TEST_LOCATION );
  END_TEST;
}

// Test setting ready texture on stage
int UtcDaliMaterialSetTextureWhilstStaged(void)
{
  TestApplication application;
  TestBoundTextures boundTextures(application);
  TestProgram testProgram(application);

  Internal::ResourceTicketPtr ticket = CheckLoadBitmap(application, "diffuse.png", 80, 80);

  // Create object with default properties
  Internal::SceneGraph::Material* sceneObject = Internal::SceneGraph::Material::New();
  DALI_TEST_CHECK( sceneObject != NULL );

  // Stage the object
  Internal::SceneGraph::UpdateManager& updateManager = Internal::ThreadLocalStorage::Get().GetUpdateManager();
  AddMaterialMessage( updateManager, sceneObject );
  application.SendNotification(); // Flush update Q
  application.Render(); // Process update message Q then create & post to render Q

  // Check that a render object has been created
  Internal::SceneGraph::RenderMaterial* renderMaterial = sceneObject->GetRenderMaterial();
  DALI_TEST_CHECK(renderMaterial != NULL);
  application.Render(); // Update & Prepare renderMat, Process render Q stores & processes mat

  sceneObject->SetDiffuseTextureId(ticket->GetId());
  application.SendNotification(); // Flush update Q
  application.Render(); // Update & Prepare material
  application.Render(); // Process render Q

  renderMaterial->BindTextures( testProgram.GetProgram(), Internal::ImageSampler::PackBitfield( FilterMode::DEFAULT, FilterMode::DEFAULT ) );
  DALI_TEST_CHECK( boundTextures.CheckFirstTextureBound( GL_TEXTURE0 ) );
  END_TEST;
}

// Test setting unready texture on stage, becoming ready
int UtcDaliMaterialSetUnreadyTextureWhilstStaged(void)
{
  TestApplication application;
  TestBoundTextures boundTextures(application);
  TestProgram testProgram(application);

  Internal::ResourceClient& resourceClient = Internal::ThreadLocalStorage::Get().GetResourceClient();
  ImageAttributes attr;
  Integration::BitmapResourceType bitmapRequest(attr);
  Internal::ResourceTicketPtr ticket = resourceClient.RequestResource( bitmapRequest, "image.png" );
  application.SendNotification(); // Flush update messages
  application.Render();           // Process resource request


  // Create object with default properties
  Internal::SceneGraph::Material* sceneObject = Internal::SceneGraph::Material::New();
  DALI_TEST_CHECK( sceneObject != NULL );

  // Stage the object
  Internal::SceneGraph::UpdateManager& updateManager = Internal::ThreadLocalStorage::Get().GetUpdateManager();
  AddMaterialMessage( updateManager, sceneObject );
  application.SendNotification(); // Flush update Q
  application.Render(); // Process update message Q then create & post to render Q

  // Check that a render object has been created
  Internal::SceneGraph::RenderMaterial* renderMaterial = sceneObject->GetRenderMaterial();
  DALI_TEST_CHECK(renderMaterial != NULL);
  application.Render(); // Update & Prepare renderMat, Process render Q stores & processes mat

  sceneObject->SetDiffuseTextureId(ticket->GetId());
  application.SendNotification(); // Flush update Q
  application.Render(); // Update & Prepare material
  application.Render(); // Process render Q

  renderMaterial->BindTextures( testProgram.GetProgram(), Internal::ImageSampler::PackBitfield( FilterMode::DEFAULT, FilterMode::DEFAULT ) );

  DALI_TEST_EQUALS( boundTextures.GetNumBoundTextures(), 0u, TEST_LOCATION );

  Integration::ResourceRequest*   req = application.GetPlatform().GetRequest();
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, false );
  bitmap->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGBA8888, 80, 80, 80, 80 );
  Integration::ResourcePointer resourcePtr(bitmap); // reference it
  application.GetPlatform().SetResourceLoaded(req->GetId(), req->GetType()->id, resourcePtr);
  application.Render();           // Process LoadComplete
  application.SendNotification(); // Process event messages

  renderMaterial->BindTextures( testProgram.GetProgram(), Internal::ImageSampler::PackBitfield( FilterMode::DEFAULT, FilterMode::DEFAULT ) );
  DALI_TEST_CHECK( boundTextures.CheckFirstTextureBound( GL_TEXTURE0 ) );
  END_TEST;
}

// Test IsOpaque without texture, with unready texture, with ready texture

int UtcDaliMaterialIsOpaqueWithoutTexture(void)
{
  TestApplication application;

  // Create object with default properties
  Internal::SceneGraph::Material* sceneObject = Internal::SceneGraph::Material::New();
  DALI_TEST_CHECK( sceneObject != NULL );

  // Stage the object
  Internal::SceneGraph::UpdateManager& updateManager = Internal::ThreadLocalStorage::Get().GetUpdateManager();
  AddMaterialMessage( updateManager, sceneObject );
  application.SendNotification(); // Flush update Q
  application.Render(); // Process update message Q then create & post to render Q

  // Check that a render object has been created
  Internal::SceneGraph::RenderMaterial* renderMaterial = sceneObject->GetRenderMaterial();
  DALI_TEST_CHECK(renderMaterial != NULL);
  application.Render(); // Update & Prepare renderMat, Process render Q stores & processes mat

  DALI_TEST_CHECK( sceneObject->IsOpaque() );
  END_TEST;
}

int UtcDaliMaterialIsOpaqueWithTexture(void)
{
  TestApplication application;

  Internal::ResourceTicketPtr ticket = CheckLoadBitmap(application, "diffuse.png", 80, 80);

  // Create object with default properties
  Internal::SceneGraph::Material* sceneObject = Internal::SceneGraph::Material::New();
  DALI_TEST_CHECK( sceneObject != NULL );

  // Stage the object
  Internal::SceneGraph::UpdateManager& updateManager = Internal::ThreadLocalStorage::Get().GetUpdateManager();
  AddMaterialMessage( updateManager, sceneObject );
  application.SendNotification(); // Flush update Q
  application.Render(); // Process update message Q then create & post to render Q

  // Check that a render object has been created
  Internal::SceneGraph::RenderMaterial* renderMaterial = sceneObject->GetRenderMaterial();
  DALI_TEST_CHECK(renderMaterial != NULL);
  application.Render(); // Update & Prepare renderMat, Process render Q stores & processes mat

  DALI_TEST_CHECK( sceneObject->IsOpaque() );

  sceneObject->SetDiffuseTextureId(ticket->GetId());
  application.SendNotification(); // Flush update Q
  application.Render(); // Update & Prepare material
  application.Render(); // Process render Q

  DALI_TEST_CHECK( ! sceneObject->IsOpaque() );
  END_TEST;
}


int UtcDaliMaterialIsOpaqueWithProps(void)
{
  TestApplication application;

  // Create object with default properties
  Internal::SceneGraph::Material* sceneObject = Internal::SceneGraph::Material::New();
  DALI_TEST_CHECK( sceneObject != NULL );

  // Stage the object
  Internal::SceneGraph::UpdateManager& updateManager = Internal::ThreadLocalStorage::Get().GetUpdateManager();
  AddMaterialMessage( updateManager, sceneObject );
  application.SendNotification(); // Flush update Q
  application.Render(); // Process update message Q then create & post to render Q

  // Check that a render object has been created
  Internal::SceneGraph::RenderMaterial* renderMaterial = sceneObject->GetRenderMaterial();
  DALI_TEST_CHECK(renderMaterial != NULL);
  application.Render(); // Update & Prepare renderMat, Process render Q stores & processes mat

  DALI_TEST_CHECK( sceneObject->IsOpaque() );

  sceneObject->SetProperties(TEST_PROPS);
  application.SendNotification(); // Flush update Q
  application.Render(); // Update & Prepare material
  application.Render(); // Process render Q

  DALI_TEST_CHECK( ! sceneObject->IsOpaque() );
  END_TEST;
}

int UtcDaliMaterialRender(void)
{
  TestApplication application;
  TestBoundTextures boundTextures(application);

  {

    MeshData meshData;
    MeshData::VertexContainer    vertices;
    MeshData::FaceIndices        faces;
    BoneContainer                bones;
    ConstructVertices(vertices, 60);
    ConstructFaces(vertices, faces);
    Dali::Material               material  = ConstructMaterial();

    Internal::ImagePtr image = LoadImage(application, "texture.png");
    Image imageHandle(image.Get());
    material.SetDiffuseTexture(imageHandle);
    meshData.SetData(vertices, faces, bones, material);
    Mesh mesh = Mesh::New(meshData);

    MeshActor actor = MeshActor::New(mesh);
    std::string name = "AMeshActor";
    actor.SetName(name);
    actor.SetAffectedByLighting(false);
    Stage::GetCurrent().Add(actor);

    material.SetOpacity(TEST_PROPS.mOpacity);
    material.SetShininess(TEST_PROPS.mShininess);
    material.SetAmbientColor(TEST_PROPS.mAmbientColor);
    material.SetDiffuseColor(TEST_PROPS.mDiffuseColor);
    material.SetSpecularColor(TEST_PROPS.mSpecularColor);
    material.SetEmissiveColor(TEST_PROPS.mEmissiveColor);

    application.SendNotification();
    application.Render();
    application.SendNotification();
    application.Render();
    application.SendNotification();
    application.Render();
    application.SendNotification();

    DALI_TEST_CHECK(application.GetGlAbstraction().CheckUniformValue("uMaterial.mOpacity",   TEST_PROPS.mOpacity ) );
    DALI_TEST_CHECK(application.GetGlAbstraction().CheckUniformValue("uMaterial.mShininess", TEST_PROPS.mShininess ) );
    DALI_TEST_CHECK(application.GetGlAbstraction().CheckUniformValue("uMaterial.mAmbient",   TEST_PROPS.mAmbientColor ) );
    DALI_TEST_CHECK(application.GetGlAbstraction().CheckUniformValue("uMaterial.mDiffuse",   TEST_PROPS.mDiffuseColor ) );
    DALI_TEST_CHECK(application.GetGlAbstraction().CheckUniformValue("uMaterial.mSpecular",  TEST_PROPS.mSpecularColor ) );
    DALI_TEST_CHECK(application.GetGlAbstraction().CheckUniformValue("uMaterial.mEmissive",  TEST_PROPS.mEmissiveColor ) );

    DALI_TEST_CHECK(boundTextures.CheckFirstTextureBound( GL_TEXTURE0 ));

    Stage::GetCurrent().Remove(actor);
    application.SendNotification();
    application.Render();
    application.SendNotification();
    application.Render();
  }
  application.SendNotification();
  application.Render();

  // texture should have been removed:
  DALI_TEST_CHECK( boundTextures.CheckFirstTextureDeleted() );
  END_TEST;
}
