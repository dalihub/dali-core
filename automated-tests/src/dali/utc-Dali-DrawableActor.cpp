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
    size            = inputData.size;
    clippingBox     = inputData.clippingBox;
    worldColorMultiplier      = inputData.worldColorMultiplier;
    terminate       = inputData.isTerminated;
    nativeApiUsable = inputData.isNativeApiUsable;
    ++invokeCount;

    return false;
  }

  bool RenderWithTextures(const RenderCallbackInput& inputData)
  {
    // Store the size and clipping box of rendered area
    size        = inputData.size;
    clippingBox = inputData.clippingBox;
    worldColorMultiplier  = inputData.worldColorMultiplier;

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
  Vector4                worldColorMultiplier{};
  bool                   terminate{};
  bool                   nativeApiUsable{true};
  uint32_t               invokeCount{0u};
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
  actor.SetProperty(Actor::Property::COLOR_MULTIPLIER, Color::MAROON * Vector4(1.0f, 1.0f, 1.0f, opacity));

  auto renderer = DevelRenderer::New(*callback);
  actor.AddRenderer(renderer);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check the size ad color (whether callback has been called)
  DALI_TEST_EQUALS(drawable.terminate, false, TEST_LOCATION);
  DALI_TEST_EQUALS(drawable.size, Size(100, 100), TEST_LOCATION);
  DALI_TEST_EQUALS(drawable.worldColorMultiplier, Color::MAROON * Vector4(1.0f, 1.0f, 1.0f, opacity), TEST_LOCATION);

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
  actor.SetProperty(Actor::Property::COLOR_MULTIPLIER, Color::MAROON * Vector4(1.0f, 1.0f, 1.0f, opacity));

  auto renderer = DevelRenderer::New(*callback);
  actor.AddRenderer(renderer);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check the size ad color (whether callback has been called)
  DALI_TEST_EQUALS(drawable.terminate, false, TEST_LOCATION);
  DALI_TEST_EQUALS(drawable.size, Size(100, 100), TEST_LOCATION);
  DALI_TEST_EQUALS(drawable.worldColorMultiplier, Color::MAROON * Vector4(1.0f, 1.0f, 1.0f, opacity), TEST_LOCATION);

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
  actor.SetProperty(Actor::Property::COLOR_MULTIPLIER, Color::MAROON * Vector4(1.0f, 1.0f, 1.0f, opacity));

  auto renderer = DevelRenderer::New(*callback);
  actor.AddRenderer(renderer);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check the size ad color (whether callback has been called)
  DALI_TEST_EQUALS(drawable.terminate, false, TEST_LOCATION);
  DALI_TEST_EQUALS(drawable.size, Size(100, 100), TEST_LOCATION);
  DALI_TEST_EQUALS(drawable.worldColorMultiplier, Color::MAROON * Vector4(1.0f, 1.0f, 1.0f, opacity), TEST_LOCATION);

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

int UtcDaliRendererTerminateRenderCallbackNeverDrawnP(void)
{
  tet_infoline("Testing Renderer::TerminateRenderCallback() for a callback that was never drawn");
  TestApplication application;

  DrawableObject drawable{};

  auto callback = RenderCallback::New<DrawableObject>(&drawable, &DrawableObject::Render);

  // Never added to an actor, so the callback is not associated with any render target and
  // there is nothing to schedule the terminate invocation against.
  auto renderer = DevelRenderer::New(*callback);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(drawable.invokeCount, 0u, TEST_LOCATION);

  DevelRenderer::TerminateRenderCallback(renderer, true);

  application.SendNotification();
  application.Render();

  // The terminate is delivered all the same - exactly once - reporting that the graphics
  // API cannot be used from it.
  DALI_TEST_EQUALS(drawable.invokeCount, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(drawable.terminate, true, TEST_LOCATION);
  DALI_TEST_EQUALS(drawable.nativeApiUsable, false, TEST_LOCATION);

  // ...and not again on subsequent frames, nor when the terminate is requested again.
  DevelRenderer::TerminateRenderCallback(renderer, true);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(drawable.invokeCount, 1u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRendererTerminateRenderCallbackRepeatedP(void)
{
  tet_infoline("Testing Renderer::TerminateRenderCallback() delivers once however often it is asked");
  TestApplication application;

  DrawableObject drawable{};

  auto callback = RenderCallback::New<DrawableObject>(&drawable, &DrawableObject::Render);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100, 100));
  application.GetScene().Add(actor);

  auto renderer = DevelRenderer::New(*callback);
  actor.AddRenderer(renderer);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(drawable.terminate, false, TEST_LOCATION);

  const uint32_t drawnCount = drawable.invokeCount;
  DALI_TEST_CHECK(drawnCount > 0u);

  DevelRenderer::TerminateRenderCallback(renderer, true);
  actor.RemoveRenderer(renderer);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(drawable.invokeCount, drawnCount + 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(drawable.terminate, true, TEST_LOCATION);
  DALI_TEST_EQUALS(drawable.nativeApiUsable, true, TEST_LOCATION);

  // Asking again changes nothing.
  DevelRenderer::TerminateRenderCallback(renderer, true);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(drawable.invokeCount, drawnCount + 1u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRendererTerminateRenderCallbackReplacedP(void)
{
  tet_infoline("Testing Renderer::TerminateRenderCallback() after the callback has been replaced");
  TestApplication application;

  DrawableObject firstDrawable{};
  DrawableObject secondDrawable{};

  auto firstCallback  = RenderCallback::New<DrawableObject>(&firstDrawable, &DrawableObject::Render);
  auto secondCallback = RenderCallback::New<DrawableObject>(&secondDrawable, &DrawableObject::Render);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100, 100));
  application.GetScene().Add(actor);

  auto renderer = DevelRenderer::New(*firstCallback);
  actor.AddRenderer(renderer);

  application.SendNotification();
  application.Render();

  DevelRenderer::TerminateRenderCallback(renderer, true);
  actor.RemoveRenderer(renderer);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(firstDrawable.terminate, true, TEST_LOCATION);

  // The replacement gets a terminate of its own - the delivery already made belongs to the
  // callback that has been swapped out.
  DevelRenderer::SetRenderCallback(renderer, secondCallback.Get());
  actor.AddRenderer(renderer);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(secondDrawable.terminate, false, TEST_LOCATION);
  DALI_TEST_CHECK(secondDrawable.invokeCount > 0u);

  DevelRenderer::TerminateRenderCallback(renderer, true);
  actor.RemoveRenderer(renderer);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(secondDrawable.terminate, true, TEST_LOCATION);
  DALI_TEST_EQUALS(secondDrawable.nativeApiUsable, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRendererTerminateRenderCallbackRenderTargetDestroyedP(void)
{
  tet_infoline("Testing Renderer::TerminateRenderCallback() when its render target is destroyed first");
  TestApplication application;

  DrawableObject drawable{};

  auto callback = RenderCallback::New<DrawableObject>(&drawable, &DrawableObject::Render);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100, 100));
  application.GetScene().Add(actor);

  auto renderer = DevelRenderer::New(*callback);
  actor.AddRenderer(renderer);

  // Exclusive, so the offscreen target is the only one the callback is drawn into and
  // therefore the only one the terminate can be queued against.
  Texture     texture     = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 100u, 100u);
  FrameBuffer frameBuffer = FrameBuffer::New(100u, 100u);
  frameBuffer.AttachColorTexture(texture);

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();
  RenderTask     task     = taskList.CreateTask();
  task.SetSourceActor(actor);
  task.SetExclusive(true);
  task.SetFrameBuffer(frameBuffer);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(drawable.terminate, false, TEST_LOCATION);

  const uint32_t drawnCount = drawable.invokeCount;
  DALI_TEST_CHECK(drawnCount > 0u);

  // Queue the terminate, then drop the render target it was queued against before the
  // terminate draw can be submitted.
  DevelRenderer::TerminateRenderCallback(renderer, true);
  actor.RemoveRenderer(renderer);

  task.SetFrameBuffer(FrameBuffer());
  taskList.RemoveTask(task);
  task.Reset();
  frameBuffer.Reset();
  texture.Reset();

  application.SendNotification();
  application.Render();

  // Delivered all the same, reporting that the native API cannot be used from it.
  DALI_TEST_EQUALS(drawable.invokeCount, drawnCount + 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(drawable.terminate, true, TEST_LOCATION);
  DALI_TEST_EQUALS(drawable.nativeApiUsable, false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRendererDestroyedWithoutTerminateRenderCallbackP(void)
{
  tet_infoline("Testing a render callback renderer destroyed without TerminateRenderCallback()");
  TestApplication application;

  DrawableObject drawable{};

  auto callback = RenderCallback::New<DrawableObject>(&drawable, &DrawableObject::Render);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100, 100));
  application.GetScene().Add(actor);

  Texture     texture     = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 100u, 100u);
  FrameBuffer frameBuffer = FrameBuffer::New(100u, 100u);
  frameBuffer.AttachColorTexture(texture);

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();
  RenderTask     task     = taskList.CreateTask();
  task.SetSourceActor(actor);
  task.SetExclusive(true);
  task.SetFrameBuffer(frameBuffer);

  {
    auto renderer = DevelRenderer::New(*callback);
    actor.AddRenderer(renderer);

    application.SendNotification();
    application.Render();

    DALI_TEST_CHECK(drawable.invokeCount > 0u);

    // Let the renderer go without ever asking for a terminate.
    actor.RemoveRenderer(renderer);
  }

  application.SendNotification();
  application.Render();

  const uint32_t invokeCount = drawable.invokeCount;

  // The render target outlives the renderer, so it must no longer be holding on to it.
  task.SetFrameBuffer(FrameBuffer());
  taskList.RemoveTask(task);
  task.Reset();
  frameBuffer.Reset();
  texture.Reset();

  application.SendNotification();
  application.Render();

  // No terminate was ever asked for, so the callback hears nothing about any of this.
  DALI_TEST_EQUALS(drawable.invokeCount, invokeCount, TEST_LOCATION);
  DALI_TEST_EQUALS(drawable.terminate, false, TEST_LOCATION);

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
  actor.SetProperty(Actor::Property::COLOR_MULTIPLIER, Color::MAROON * Vector4(1.0f, 1.0f, 1.0f, opacity));

  auto renderer = DevelRenderer::New(*callback);
  actor.AddRenderer(renderer);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check the size ad color (whether callback has been called)
  DALI_TEST_EQUALS(drawable.terminate, false, TEST_LOCATION);
  DALI_TEST_EQUALS(drawable.size, Size(100, 100), TEST_LOCATION);
  DALI_TEST_EQUALS(drawable.worldColorMultiplier, Color::MAROON * Vector4(1.0f, 1.0f, 1.0f, opacity), TEST_LOCATION);

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
  actor.SetProperty(Actor::Property::COLOR_MULTIPLIER, Color::MAROON * Vector4(1.0f, 1.0f, 1.0f, opacity));

  auto renderer = DevelRenderer::New(*callback);
  actor.AddRenderer(renderer);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check the size ad color (whether callback has been called)
  DALI_TEST_EQUALS(drawable.terminate, false, TEST_LOCATION);
  DALI_TEST_EQUALS(drawable.size, Size(100, 100), TEST_LOCATION);
  DALI_TEST_EQUALS(drawable.worldColorMultiplier, Color::MAROON * Vector4(1.0f, 1.0f, 1.0f, opacity), TEST_LOCATION);

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
  drawableActor.SetProperty(Actor::Property::COLOR_MULTIPLIER, Color::MAROON * Vector4(1.0f, 1.0f, 1.0f, opacity));

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check the size and color (whether callback has been called)
  DALI_TEST_EQUALS(drawable.size, Size(100, 100), TEST_LOCATION);
  DALI_TEST_EQUALS(drawable.worldColorMultiplier, Color::MAROON * Vector4(1.0f, 1.0f, 1.0f, opacity), TEST_LOCATION);

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
  DALI_TEST_EQUALS(callback->AccessTextureResources().Get().Count(), texturesToBind.Count(), TEST_LOCATION);

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

  // Re-binding replaces the list wholesale. The render thread works from a snapshot, so
  // the storage the previous list used can be released without it walking freed memory.
  Dali::Vector<Texture> rebound;
  rebound.PushBack(uploaded);
  callback->BindTextureResources(rebound);

  gfxTrace.Reset();
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(drawable.renderCount, 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(drawable.textureBindings.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(gfxTrace.CountMethod("GetTextureProperties"), 1, TEST_LOCATION);

  // Binding an empty list unbinds everything. The input structure outlives a single
  // frame, so the previous entries have to be cleared rather than left behind.
  callback->BindTextureResources(Dali::Vector<Texture>());

  gfxTrace.Reset();
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(drawable.renderCount, 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(drawable.textureBindings.Count(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(gfxTrace.CountMethod("GetTextureProperties"), 0, TEST_LOCATION);

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
  parentActor.SetProperty(Actor::Property::COLOR_MULTIPLIER, Color::DARK_OLIVE_GREEN * Vector4(1.0f, 1.0f, 1.0f, opacity));

  actor.SetProperty(Actor::Property::SIZE, Vector2(100, 200));
  actor.SetProperty(Actor::Property::POSITION, Vector2(50, 70));
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  actor.SetProperty(Actor::Property::COLOR_MULTIPLIER, Color::MAROON * Vector4(1.0f, 1.0f, 1.0f, opacity));

  auto renderer = DevelRenderer::New(*callback);
  actor.AddRenderer(renderer);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  // Check the size and color (whether callback has been called)
  DALI_TEST_EQUALS(drawable.size, Size(100, 200), TEST_LOCATION);
  DALI_TEST_EQUALS(drawable.worldColorMultiplier, Color::MAROON * Vector4(1.0f, 1.0f, 1.0f, opacity * opacity), TEST_LOCATION);

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
