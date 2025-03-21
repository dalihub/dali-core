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

#include <dali-test-suite-utils.h>
#include <dali/public-api/actors/drawable-actor.h>

struct DrawableObject
{
  bool Render(const RenderCallbackInput& inputData)
  {
    // Store the size and clipping box of rendered area
    size        = inputData.size;
    clippingBox = inputData.clippingBox;

    return false;
  }

  bool RenderWithTextures(const RenderCallbackInput& inputData)
  {
    // Store the size and clipping box of rendered area
    size        = inputData.size;
    clippingBox = inputData.clippingBox;

    auto count = inputData.textureBindings.size();

    // test whether number of textures matches 1
    DALI_TEST_EQUALS(count, 1, TEST_LOCATION);

    return false;
  }

  Size        size{};
  ClippingBox clippingBox{};
};

int UtcDaliRendererSetRenderCallbackP(void)
{
  tet_infoline("Testing Renderer:LSetRenderCallback()");
  TestApplication application;

  DrawableObject drawable{};

  auto callback = RenderCallback::New<DrawableObject>(&drawable, &DrawableObject::Render);

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  actor.SetProperty(Actor::Property::SIZE, Vector2(100, 100));

  auto renderer = Renderer::New(*callback);
  actor.AddRenderer(renderer);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check the size (whether callback has been called)
  auto size(drawable.size);
  DALI_TEST_EQUALS(drawable.size, Size(100, 100), TEST_LOCATION);

  END_TEST;
}

int UtcDaliDrawableActor1P(void)
{
  tet_infoline("Testing DrawableActor");
  TestApplication application;

  DrawableObject drawable{};

  auto callback = RenderCallback::New<DrawableObject>(&drawable, &DrawableObject::Render);

  DrawableActor drawableActor = DrawableActor::New(*callback);
  application.GetScene().Add(drawableActor);

  drawableActor.SetProperty(Actor::Property::SIZE, Vector2(100, 100));

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check the size (whether callback has been called)
  DALI_TEST_EQUALS(drawable.size, Size(100, 100), TEST_LOCATION);

  END_TEST;
}

int UtcRenderCallbackTextureBindingP(void)
{
  tet_infoline("Testing RenderCallback texture bindings");
  TestApplication application;

  DrawableObject drawable{};

  auto callback = RenderCallback::New<DrawableObject>(&drawable, &DrawableObject::RenderWithTextures);

  // Prepare texture
  Texture   texture   = Texture::New(Dali::TextureType::TEXTURE_2D, Pixel::Format::RGBA8888, 512, 512);
  auto*     data      = reinterpret_cast<uint8_t*>(malloc(512 * 512 * 4));
  PixelData pixelData = PixelData::New(data, 512 * 512 * 4, 512, 512, Pixel::Format::RGBA8888, PixelData::ReleaseFunction::FREE);
  texture.Upload(pixelData);

  std::vector<Texture> texturesToBind;
  texturesToBind.push_back(texture);
  callback->BindTextureResources(texturesToBind);

  DrawableActor drawableActor = DrawableActor::New(*callback);
  application.GetScene().Add(drawableActor);

  drawableActor.SetProperty(Actor::Property::SIZE, Vector2(100, 100));

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check the size (whether callback has been called)
  DALI_TEST_EQUALS(drawable.size, Size(100, 100), TEST_LOCATION);

  END_TEST;
}

int UtcDaliDrawableActor2P(void)
{
  tet_infoline("Testing Renderer:LSetRenderCallback() and check clipping box");
  TestApplication application;

  DrawableObject drawable{};

  auto callback = RenderCallback::New<DrawableObject>(&drawable, &DrawableObject::Render);

  Actor actor       = Actor::New();
  Actor parentActor = Actor::New();
  application.GetScene().Add(parentActor);
  parentActor.Add(actor);

  parentActor.SetProperty(Actor::Property::POSITION, Vector2(20, 50));
  parentActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  parentActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  actor.SetProperty(Actor::Property::SIZE, Vector2(100, 200));
  actor.SetProperty(Actor::Property::POSITION, Vector2(50, 70));
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  auto renderer = Renderer::New(*callback);
  actor.AddRenderer(renderer);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check the size (whether callback has been called)
  DALI_TEST_EQUALS(drawable.size, Size(100, 200), TEST_LOCATION);

  // Check clippingBox. Note that clippingBox coordinate is in screen coordinates
  DALI_TEST_EQUALS(drawable.clippingBox, Rect<int32_t>(20 + 50, 800 - (50 + 70 + 200), 100, 200), TEST_LOCATION);

  END_TEST;
}

int UtcDaliDrawableActorSceneRotated(void)
{
  tet_infoline("Testing Renderer:LSetRenderCallback()");
  TestApplication application;

  DrawableObject drawable{};

  auto callback = RenderCallback::New<DrawableObject>(&drawable, &DrawableObject::Render);

  Actor actor       = Actor::New();
  Actor parentActor = Actor::New();
  application.GetScene().Add(parentActor);
  parentActor.Add(actor);

  parentActor.SetProperty(Actor::Property::POSITION, Vector2(20, 50));
  parentActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  parentActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  actor.SetProperty(Actor::Property::SIZE, Vector2(100, 200));
  actor.SetProperty(Actor::Property::POSITION, Vector2(50, 70));
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  auto renderer = Renderer::New(*callback);
  actor.AddRenderer(renderer);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check the size (whether callback has been called)
  DALI_TEST_EQUALS(drawable.size, Size(100, 200), TEST_LOCATION);

  // Check clippingBox. Note that clippingBox coordinate is in screen coordinates
  DALI_TEST_EQUALS(drawable.clippingBox, Rect<int32_t>(20 + 50, TestApplication::DEFAULT_SURFACE_HEIGHT - (50 + 70 + 200), 100, 200), TEST_LOCATION);

  // Reset size (to check callback comes)
  drawable.size = Size();

  application.GetScene().SurfaceRotated(TestApplication::DEFAULT_SURFACE_WIDTH,
                                        TestApplication::DEFAULT_SURFACE_HEIGHT,
                                        90,
                                        0);

  application.SendNotification();
  application.Render();

  // Check the size (whether callback has been called)
  DALI_TEST_EQUALS(drawable.size, Size(100, 200), TEST_LOCATION);

  // Check clippingBox. Note that clippingBox coordinate is in screen coordinates
  DALI_TEST_EQUALS(drawable.clippingBox, Rect<int32_t>(50 + 70, 20 + 50, 200, 100), TEST_LOCATION);

  END_TEST;
}