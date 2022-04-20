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
    // Store the size of rendered area
    size = inputData.size;

    return false;
  }

  Size size{};
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
  auto size(drawable.size);
  DALI_TEST_EQUALS(drawable.size, Size(100, 100), TEST_LOCATION);

  END_TEST;
}