/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/actors/drawable-actor.h>
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/devel-api/signals/render-callback.h>

namespace
{
struct DrawableObject
{
  bool Render(const RenderCallbackInput& inputData)
  {
    // Store the size and clipping box of rendered area
    size        = inputData.size;
    clippingBox = inputData.clippingBox;
    worldColor  = inputData.worldColor;
    terminate   = inputData.isTerminated;

    return false;
  }

  bool RenderWithTextures(const RenderCallbackInput& inputData)
  {
    // Store the size and clipping box of rendered area
    size        = inputData.size;
    clippingBox = inputData.clippingBox;
    worldColor  = inputData.worldColor;

    auto count = inputData.textureBindings.Count();

    // test whether number of textures matches 1
    DALI_TEST_EQUALS(count, 1, TEST_LOCATION);

    return false;
  }

  /**
   * @brief Copies the texture bindings out so the test body can inspect them.
   */
  bool RenderAndCaptureTextures(const RenderCallbackInput& inputData)
  {
    size            = inputData.size;
    textureBindings = inputData.textureBindings;
    ++renderCount;

    return false;
  }

  Size                   size{};
  BoundsInteger          clippingBox{};
  Vector4                worldColor{};
  bool                   terminate{};
  Dali::Vector<uint32_t> textureBindings{};
  uint32_t               renderCount{0u};
};
} // namespace

int UtcDaliRendererSetRenderCallbackP(void)
{
  tet_infoline("Testing Renderer:LSetRenderCallback()");
  TestApplication application;

  DrawableObject drawable{};

  auto callback = RenderCallback::New<DrawableObject>(&drawable, &DrawableObject::Render);

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  const float opacity = 0.5f;
  actor.SetProperty(Actor::Property::SIZE, Vector2(100, 100));
  actor.SetProperty(Actor::Property::COLOR, Color::MAROON * Vector4(1.0f, 1.0f, 1.0f, opacity));

  auto renderer = DevelRenderer::New(*callback);
  actor.AddRenderer(renderer);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check the size ad color (whether callback has been called)
  DALI_TEST_EQUALS(drawable.terminate, false, TEST_LOCATION);
  DALI_TEST_EQUALS(drawable.size, Size(100, 100), TEST_LOCATION);
  DALI_TEST_EQUALS(drawable.worldColor, Color::MAROON * Vector4(1.0f, 1.0f, 1.0f, opacity), TEST_LOCATION);

  // render once again, for line coverage
  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliRendererSetRenderCallbackUnsafeP(void)
{
  tet_infoline("Testing Renderer:LSetRenderCallback() with Unsafe");
  TestApplication application;

  DrawableObject drawable{};

  auto callback = RenderCallback::New<DrawableObject>(&drawable, &DrawableObject::Render, RenderCallback::ExecutionMode::UNSAFE);

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  const float opacity = 0.5f;
  actor.SetProperty(Actor::Property::SIZE, Vector2(100, 100));
  actor.SetProperty(Actor::Property::COLOR, Color::MAROON * Vector4(1.0f, 1.0f, 1.0f, opacity));

  auto renderer = DevelRenderer::New(*callback);
  actor.AddRenderer(renderer);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check the size ad color (whether callback has been called)
  DALI_TEST_EQUALS(drawable.terminate, false, TEST_LOCATION);
  DALI_TEST_EQUALS(drawable.size, Size(100, 100), TEST_LOCATION);
  DALI_TEST_EQUALS(drawable.worldColor, Color::MAROON * Vector4(1.0f, 1.0f, 1.0f, opacity), TEST_LOCATION);

  // render once again, for line coverage
  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliRendererSetRenderCallbackViaDevelP(void)
{
  tet_infoline("Testing DevelRenderer::SetRenderCallback() on an existing renderer");
  TestApplication application;

  DrawableObject first{};
  DrawableObject second{};

  auto firstCallback  = RenderCallback::New<DrawableObject>(&first, &DrawableObject::Render);
  auto secondCallback = RenderCallback::New<DrawableObject>(&second, &DrawableObject::Render);

  Actor actor = Actor::New();
  application.GetScene().Add(actor);
  actor.SetProperty(Actor::Property::SIZE, Vector2(100, 100));

  // A renderer with neither geometry nor shader is only renderable once it has a callback,
  // so attach one before adding the renderer to the actor.
  Renderer renderer = Renderer::New();
  DevelRenderer::SetRenderCallback(renderer, firstCallback.Get());
  actor.AddRenderer(renderer);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(first.size, Size(100, 100), TEST_LOCATION);
  DALI_TEST_EQUALS(second.size, Size(Vector2::ZERO), TEST_LOCATION);

  // Replacing the callback drops the cached render target objects of the previous one.
  DevelRenderer::SetRenderCallback(renderer, secondCallback.Get());

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(second.size, Size(100, 100), TEST_LOCATION);

  // Clearing it makes the renderer non-renderable again, so the callback stops running.
  second.size = Size(Vector2::ZERO);
  DevelRenderer::SetRenderCallback(renderer, nullptr);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(second.size, Size(Vector2::ZERO), TEST_LOCATION);

  END_TEST;
}

int UtcDaliRendererTerminateRenderCallbackP(void)
{
  tet_infoline("Testing Renderer:LTerminateRenderCallback()");
  TestApplication application;

  DrawableObject drawable{};

  auto callback = RenderCallback::New<DrawableObject>(&drawable, &DrawableObject::Render);

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  const float opacity = 0.5f;
  actor.SetProperty(Actor::Property::SIZE, Vector2(100, 100));
  actor.SetProperty(Actor::Property::COLOR, Color::MAROON * Vector4(1.0f, 1.0f, 1.0f, opacity));

  auto renderer = DevelRenderer::New(*callback);
  actor.AddRenderer(renderer);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check the size ad color (whether callback has been called)
  DALI_TEST_EQUALS(drawable.terminate, false, TEST_LOCATION);
  DALI_TEST_EQUALS(drawable.size, Size(100, 100), TEST_LOCATION);
  DALI_TEST_EQUALS(drawable.worldColor, Color::MAROON * Vector4(1.0f, 1.0f, 1.0f, opacity), TEST_LOCATION);

  // render once again, for line coverage
  application.SendNotification();
  application.Render();

  DevelRenderer::TerminateRenderCallback(renderer, true);

  DALI_TEST_EQUALS(drawable.terminate, false, TEST_LOCATION);

  // Wait render callback comes with terminate flag.
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(drawable.terminate, true, TEST_LOCATION);

  // render once again, for line coverage
  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliRendererTerminateRenderCallbackUnsafeP(void)
{
  tet_infoline("Testing Renderer:LTerminateRenderCallback() with Unsafe");
  TestApplication application;

  DrawableObject drawable{};

  auto callback = RenderCallback::New<DrawableObject>(&drawable, &DrawableObject::Render, RenderCallback::ExecutionMode::UNSAFE);

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  const float opacity = 0.5f;
  actor.SetProperty(Actor::Property::SIZE, Vector2(100, 100));
  actor.SetProperty(Actor::Property::COLOR, Color::MAROON * Vector4(1.0f, 1.0f, 1.0f, opacity));

  auto renderer = DevelRenderer::New(*callback);
  actor.AddRenderer(renderer);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check the size ad color (whether callback has been called)
  DALI_TEST_EQUALS(drawable.terminate, false, TEST_LOCATION);
  DALI_TEST_EQUALS(drawable.size, Size(100, 100), TEST_LOCATION);
  DALI_TEST_EQUALS(drawable.worldColor, Color::MAROON * Vector4(1.0f, 1.0f, 1.0f, opacity), TEST_LOCATION);

  // render once again, for line coverage
  application.SendNotification();
  application.Render();

  DevelRenderer::TerminateRenderCallback(renderer, true);

  DALI_TEST_EQUALS(drawable.terminate, false, TEST_LOCATION);

  // Wait render callback comes with terminate flag.
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(drawable.terminate, true, TEST_LOCATION);

  // render once again, for line coverage
  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliRendererTerminateRenderCallbackUnsafeP2(void)
{
  tet_infoline("Testing Renderer:LTerminateRenderCallback() with Unsafe 2");
  TestApplication application;

  DrawableObject drawable{};

  auto callback = RenderCallback::New<DrawableObject>(&drawable, &DrawableObject::Render, RenderCallback::ExecutionMode::UNSAFE);

  Actor actor = Actor::New();
  application.GetScene().Add(actor);

  const float opacity = 0.5f;
  actor.SetProperty(Actor::Property::SIZE, Vector2(100, 100));
  actor.SetProperty(Actor::Property::COLOR, Color::MAROON * Vector4(1.0f, 1.0f, 1.0f, opacity));

  auto renderer = DevelRenderer::New(*callback);
  actor.AddRenderer(renderer);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check the size ad color (whether callback has been called)
  DALI_TEST_EQUALS(drawable.terminate, false, TEST_LOCATION);
  DALI_TEST_EQUALS(drawable.size, Size(100, 100), TEST_LOCATION);
  DALI_TEST_EQUALS(drawable.worldColor, Color::MAROON * Vector4(1.0f, 1.0f, 1.0f, opacity), TEST_LOCATION);

  // render once again, for line coverage
  application.SendNotification();
  application.Render();

  DevelRenderer::TerminateRenderCallback(renderer, false);

  DALI_TEST_EQUALS(drawable.terminate, false, TEST_LOCATION);

  // Wait render callback comes with terminate flag.
  application.SendNotification();
  application.Render();

  // Callback not comes!
  DALI_TEST_EQUALS(drawable.terminate, false, TEST_LOCATION);

  // render once again, for line coverage
  application.SendNotification();
  application.Render();

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

  const float opacity = 0.5f;
  drawableActor.SetProperty(Actor::Property::SIZE, Vector2(100, 100));
  drawableActor.SetProperty(Actor::Property::COLOR, Color::MAROON * Vector4(1.0f, 1.0f, 1.0f, opacity));

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check the size and color (whether callback has been called)
  DALI_TEST_EQUALS(drawable.size, Size(100, 100), TEST_LOCATION);
  DALI_TEST_EQUALS(drawable.worldColor, Color::MAROON * Vector4(1.0f, 1.0f, 1.0f, opacity), TEST_LOCATION);

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

  Dali::Vector<Texture> texturesToBind;
  texturesToBind.PushBack(texture);
  callback->BindTextureResources(texturesToBind);
  DALI_TEST_EQUALS(callback->GetTextureResources().Count(), texturesToBind.Count(), TEST_LOCATION);

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

int UtcRenderCallbackTextureBindingNotUploaded(void)
{
  tet_infoline("Testing RenderCallback texture bindings for a texture that has not been uploaded");
  TestApplication application;

  auto&           graphicsController = application.GetGraphicsController();
  TraceCallStack& gfxTrace           = graphicsController.mCallStack;
  gfxTrace.Enable(true);

  DrawableObject drawable{};

  auto callback = RenderCallback::New<DrawableObject>(&drawable, &DrawableObject::RenderAndCaptureTextures);

  // A texture with no upload has no graphics object, so it has no native handle to report.
  Texture notUploaded = Texture::New(Dali::TextureType::TEXTURE_2D, Pixel::Format::RGBA8888, 16, 16);

  // An uploaded texture does have one.
  Texture   uploaded  = Texture::New(Dali::TextureType::TEXTURE_2D, Pixel::Format::RGBA8888, 16, 16);
  auto*     data      = reinterpret_cast<uint8_t*>(malloc(16 * 16 * 4));
  PixelData pixelData = PixelData::New(data, 16 * 16 * 4, 16, 16, Pixel::Format::RGBA8888, PixelData::ReleaseFunction::FREE);
  uploaded.Upload(pixelData);

  // Bind the un-uploaded one first, so a skipped entry would shift the uploaded one.
  Dali::Vector<Texture> texturesToBind;
  texturesToBind.PushBack(notUploaded);
  texturesToBind.PushBack(uploaded);
  callback->BindTextureResources(texturesToBind);

  DrawableActor drawableActor = DrawableActor::New(*callback);
  application.GetScene().Add(drawableActor);
  drawableActor.SetProperty(Actor::Property::SIZE, Vector2(100, 100));

  gfxTrace.Reset();

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // The callback must still run - an unavailable texture is not a fatal condition.
  DALI_TEST_EQUALS(drawable.renderCount, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(drawable.size, Size(100, 100), TEST_LOCATION);

  // Every bound texture keeps its position, because the index is the only thing
  // associating an entry with what the client bound.
  DALI_TEST_EQUALS(drawable.textureBindings.Count(), 2u, TEST_LOCATION);

  // The un-uploaded texture reports an invalid native handle.
  DALI_TEST_EQUALS(drawable.textureBindings[0], 0u, TEST_LOCATION);

  // Properties are only queried for the texture that actually has a graphics object, so
  // exactly one of the two entries was resolved.
  DALI_TEST_EQUALS(gfxTrace.CountMethod("GetTextureProperties"), 1, TEST_LOCATION);

  // Uploading the remaining texture makes it resolvable on a later frame.
  notUploaded.Upload(PixelData::New(reinterpret_cast<uint8_t*>(malloc(16 * 16 * 4)), 16 * 16 * 4, 16, 16, Pixel::Format::RGBA8888, PixelData::ReleaseFunction::FREE));

  gfxTrace.Reset();
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(drawable.renderCount, 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(drawable.textureBindings.Count(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(gfxTrace.CountMethod("GetTextureProperties"), 2, TEST_LOCATION);

  END_TEST;
}

int UtcRenderCallbackTextureBindingAllNotUploaded(void)
{
  tet_infoline("Testing RenderCallback texture bindings when no bound texture has been uploaded");
  TestApplication application;

  auto&           graphicsController = application.GetGraphicsController();
  TraceCallStack& gfxTrace           = graphicsController.mCallStack;
  gfxTrace.Enable(true);

  DrawableObject drawable{};

  auto callback = RenderCallback::New<DrawableObject>(&drawable, &DrawableObject::RenderAndCaptureTextures);

  Dali::Vector<Texture> texturesToBind;
  texturesToBind.PushBack(Texture::New(Dali::TextureType::TEXTURE_2D, Pixel::Format::RGBA8888, 16, 16));
  texturesToBind.PushBack(Texture::New(Dali::TextureType::TEXTURE_2D));
  callback->BindTextureResources(texturesToBind);

  DrawableActor drawableActor = DrawableActor::New(*callback);
  application.GetScene().Add(drawableActor);
  drawableActor.SetProperty(Actor::Property::SIZE, Vector2(100, 100));

  gfxTrace.Reset();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(drawable.renderCount, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(drawable.textureBindings.Count(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(drawable.textureBindings[0], 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(drawable.textureBindings[1], 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(gfxTrace.CountMethod("GetTextureProperties"), 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliDrawableActor2P(void)
{
  tet_infoline("Testing Renderer:LSetRenderCallback() and check clipping box and color");
  TestApplication application;

  DrawableObject drawable{};

  auto callback = RenderCallback::New<DrawableObject>(&drawable, &DrawableObject::Render);

  Actor actor       = Actor::New();
  Actor parentActor = Actor::New();
  application.GetScene().Add(parentActor);
  parentActor.Add(actor);

  const float opacity = 0.5f;
  parentActor.SetProperty(Actor::Property::POSITION, Vector2(20, 50));
  parentActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  parentActor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  parentActor.SetProperty(Actor::Property::COLOR, Color::DARK_OLIVE_GREEN * Vector4(1.0f, 1.0f, 1.0f, opacity));

  actor.SetProperty(Actor::Property::SIZE, Vector2(100, 200));
  actor.SetProperty(Actor::Property::POSITION, Vector2(50, 70));
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  actor.SetProperty(Actor::Property::COLOR, Color::MAROON * Vector4(1.0f, 1.0f, 1.0f, opacity));

  auto renderer = DevelRenderer::New(*callback);
  actor.AddRenderer(renderer);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check the size and color (whether callback has been called)
  DALI_TEST_EQUALS(drawable.size, Size(100, 200), TEST_LOCATION);
  DALI_TEST_EQUALS(drawable.worldColor, Color::MAROON * Vector4(1.0f, 1.0f, 1.0f, opacity * opacity), TEST_LOCATION);

  // Check clippingBox. Note that clippingBox coordinate is in screen coordinates
  DALI_TEST_EQUALS(drawable.clippingBox, BoundsInteger(20 + 50, 800 - (50 + 70 + 200), 100, 200), TEST_LOCATION);

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
  parentActor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);

  actor.SetProperty(Actor::Property::SIZE, Vector2(100, 200));
  actor.SetProperty(Actor::Property::POSITION, Vector2(50, 70));
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);

  auto renderer = DevelRenderer::New(*callback);
  actor.AddRenderer(renderer);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check the size (whether callback has been called)
  DALI_TEST_EQUALS(drawable.size, Size(100, 200), TEST_LOCATION);

  // Check clippingBox. Note that clippingBox coordinate is in screen coordinates
  DALI_TEST_EQUALS(drawable.clippingBox, BoundsInteger(20 + 50, TestApplication::DEFAULT_SURFACE_HEIGHT - (50 + 70 + 200), 100, 200), TEST_LOCATION);

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
  DALI_TEST_EQUALS(drawable.clippingBox, BoundsInteger(50 + 70, 20 + 50, 200, 100), TEST_LOCATION);

  END_TEST;
}