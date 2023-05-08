/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

#include <dali-test-suite-utils.h>
#include <mesh-builder.h>
#include <stdlib.h>

// access private members
#define private public
#include <dali/internal/render/renderers/pipeline-cache.h>

#include <dlfcn.h>

using namespace Dali;

template<class Object, class... Args>
void InvokeNext(Object* obj, Args... args)
{
  auto    addr = __builtin_return_address(0);
  Dl_info info;
  dladdr(addr, &info);
  auto func = dlsym(RTLD_NEXT, info.dli_sname);
  typedef void (*FuncPtr)(void*, Args...);
  auto memb = FuncPtr(func);
  memb(obj, args...);
}

template<class Ret, class Object, class... Args>
Ret InvokeReturnNext(Object* obj, Args... args)
{
  auto    addr = __builtin_return_address(0);
  Dl_info info;
  dladdr(addr, &info);
  auto func = dlsym(RTLD_NEXT, info.dli_sname);
  typedef Ret (*FuncPtr)(void*, Args...);
  auto memb = FuncPtr(func);
  return memb(obj, args...);
}

static Dali::Internal::Render::PipelineCache* gPipelineCache{nullptr};
namespace Dali
{
namespace Internal
{
namespace Render
{
// Store internal PipelineCache as singleton

PipelineCache::PipelineCache(Dali::Graphics::Controller& controller)
{
  gPipelineCache = this;
  InvokeNext(this, &controller);
}

} // namespace Render
} // namespace Internal
} // namespace Dali

int UtcDaliCorePipelineCacheTest(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Internal::Render::Pipeline");

  using Dali::Internal::Render::PipelineCache;

  // PipelineCache* cache = PipelineCache::GetPipelineCacheWithController( &application.GetGraphicsController() );
  // Pipeline cache must be initialized
  DALI_TEST_EQUALS(gPipelineCache != 0, true, TEST_LOCATION);

  // Test size of level0 nodes (should be 0, nothing added yet)
  DALI_TEST_EQUALS((gPipelineCache->level0nodes.size() == 0), true, TEST_LOCATION);

  // Create something to render
  Geometry   geometry   = CreateQuadGeometry();
  Shader     shader     = Shader::New("vertexSrc", "fragmentSrc");
  TextureSet textureSet = TextureSet::New();
  Texture    image      = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 50, 50);

  textureSet.SetTexture(0u, image);
  Renderer renderer = Renderer::New(geometry, shader);
  renderer.SetProperty(Dali::Renderer::Property::BLEND_MODE, Dali::BlendMode::ON);
  renderer.SetTextures(textureSet);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  // 1 pipeline should be added
  DALI_TEST_EQUALS((gPipelineCache->level0nodes.size() == 1), true, TEST_LOCATION);

  // Add another actor, new pipeline will be created
  Shader   shader1   = Shader::New("newVertexSrc", "newFragmentSrc");
  Actor    actor1    = Actor::New();
  Renderer renderer1 = Renderer::New(geometry, shader1);
  renderer1.SetProperty(Dali::Renderer::Property::BLEND_MODE, Dali::BlendMode::ON);
  actor1.AddRenderer(renderer1);
  actor1.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor1);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS((gPipelineCache->level0nodes.size() == 2), true, TEST_LOCATION);

  // Now add 3rd actor reusing first pipeline
  {
    Actor    actor2    = Actor::New();
    Renderer renderer2 = Renderer::New(geometry, shader);
    renderer2.SetProperty(Dali::Renderer::Property::BLEND_MODE, Dali::BlendMode::ON);
    actor2.AddRenderer(renderer);
    actor2.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
    application.GetScene().Add(actor2);
  }
  application.SendNotification();
  application.Render();

  // Number of pipelines shouldn't change
  DALI_TEST_EQUALS((gPipelineCache->level0nodes.size() == 2), true, TEST_LOCATION);

  // Test final 'noBlend' path on first pipeline
  {
    Actor    actor3    = Actor::New();
    Renderer renderer3 = Renderer::New(geometry, shader);
    renderer3.SetProperty(Dali::Renderer::Property::BLEND_MODE, Dali::BlendMode::OFF);
    actor3.AddRenderer(renderer3);
    actor3.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
    application.GetScene().Add(actor3);
  }
  application.SendNotification();
  application.Render();

  // Test whether noBlend pipeline is set in cache
  DALI_TEST_EQUALS(gPipelineCache->level0nodes[0].level1nodes[0].noBlend.pipeline != nullptr, true, TEST_LOCATION);

  // Remove renderer to test whether old pipeline is removed
  application.GetScene().Remove(actor1);
  actor1.RemoveRenderer(renderer1);
  renderer1.Reset();

  // Make the frame count of the pipeline cache large to clean cache
  gPipelineCache->mFrameCount = 1000;

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(gPipelineCache->level0nodes.size(), 1, TEST_LOCATION);

  END_TEST;
}
