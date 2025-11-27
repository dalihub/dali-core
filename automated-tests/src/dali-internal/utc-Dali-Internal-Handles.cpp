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

#include <dali-test-suite-utils.h>
#include <dali/public-api/dali-core.h>
#include <mesh-builder.h>

#include <stdlib.h>
#include <iostream>

// Internal headers are allowed here
#include <dali/internal/event/common/object-impl.h> // Dali::Internal::Object
#include <dali/internal/event/rendering/renderer-impl.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/update/rendering/scene-graph-renderer.h>

using namespace Dali;

void utc_dali_internal_handles_startup()
{
  test_return_value = TET_UNDEF;
}

void utc_dali_internal_handles_cleanup()
{
  test_return_value = TET_PASS;
}

int UtcDaliCameraActorConstructorRefObject(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::CameraActor::CameraActor(Internal::CameraActor*)");

  CameraActor actor(NULL);

  DALI_TEST_CHECK(!actor);
  END_TEST;
}

namespace Impl
{
struct DerivedRenderer : public Dali::Internal::Renderer
{
  static IntrusivePtr<DerivedRenderer> New()
  {
    auto                          sceneObjectKey = Dali::Internal::SceneGraph::Renderer::NewKey();
    IntrusivePtr<DerivedRenderer> impl           = new DerivedRenderer(sceneObjectKey.Get());

    // transfer scene object ownership to update manager
    Internal::EventThreadServices&       eventThreadServices = impl->GetEventThreadServices();
    Internal::SceneGraph::UpdateManager& updateManager       = eventThreadServices.GetUpdateManager();
    AddRendererMessage(updateManager, sceneObjectKey);
    eventThreadServices.RegisterObject(impl.Get());
    return impl;
  }

  DerivedRenderer(const Dali::Internal::SceneGraph::Renderer* sceneObject)
  : Internal::Renderer(sceneObject)
  {
  }
  ~DerivedRenderer()
  {
  }
};
} // namespace Impl

struct DerivedRenderer : public Renderer
{
  static DerivedRenderer New(Geometry geometry, Shader shader)
  {
    IntrusivePtr<Impl::DerivedRenderer> impl = Impl::DerivedRenderer::New();
    impl->SetGeometry(GetImplementation(geometry));
    impl->SetShader(GetImplementation(shader));
    DerivedRenderer custom(impl.Get());
    return custom;
  }
  DerivedRenderer()  = default;
  ~DerivedRenderer() = default;
  DerivedRenderer(Impl::DerivedRenderer* impl)
  : Renderer(impl)
  {
  }
};

int UtcDaliInternalHandleRendererPropertyComponents(void)
{
  TestApplication application;

  Dali::TypeRegistration typeRegistration(typeid(DerivedRenderer), typeid(Dali::Renderer), nullptr);

  Shader   shader   = Shader::New("VertexSource", "FragmentSource");
  Geometry geometry = CreateQuadGeometry();
  auto     derived  = DerivedRenderer::New(geometry, shader);
  Actor    actor    = Actor::New();
  actor.AddRenderer(derived);
  application.GetScene().Add(actor);

  const Property::Index foobarIndex = ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX;
  const Property::Index fooIndex    = foobarIndex + 1;
  const Property::Index barIndex    = foobarIndex + 2;

  const Property::Index rgbIndex   = foobarIndex + 3;
  const Property::Index redIndex   = rgbIndex + 1;
  const Property::Index greenIndex = rgbIndex + 2;
  const Property::Index blueIndex  = rgbIndex + 3;

  const Property::Index rgbaIndex   = rgbIndex + 4;
  const Property::Index red2Index   = rgbaIndex + 1;
  const Property::Index green2Index = rgbaIndex + 2;
  const Property::Index blue2Index  = rgbaIndex + 3;
  const Property::Index alphaIndex  = rgbaIndex + 4;

  // If we don't properly register a scene graph property for the "parent" type,
  // then Object::GetSceneGraphProperty registers a float type for the child...

  AnimatablePropertyRegistration(typeRegistration, "Foobar", foobarIndex, Vector2(10.0f, 20.0f));
  AnimatablePropertyComponentRegistration(typeRegistration, "Foobar.x", fooIndex, foobarIndex, 0);
  AnimatablePropertyComponentRegistration(typeRegistration, "Foobar.y", barIndex, foobarIndex, 1);

  AnimatablePropertyRegistration(typeRegistration, "RGB", rgbIndex, Vector3(0.5f, 0.5, 1.0f));
  AnimatablePropertyComponentRegistration(typeRegistration, "RGB.red", redIndex, rgbIndex, 0);
  AnimatablePropertyComponentRegistration(typeRegistration, "RGB.green", greenIndex, rgbIndex, 1);
  AnimatablePropertyComponentRegistration(typeRegistration, "RGB.blue", blueIndex, rgbIndex, 2);

  AnimatablePropertyRegistration(typeRegistration, "RGBA", rgbaIndex, Vector4(0.5f, 0.5, 1.0f, 1.0f));
  AnimatablePropertyComponentRegistration(typeRegistration, "RGBA.red", red2Index, rgbaIndex, 0);
  AnimatablePropertyComponentRegistration(typeRegistration, "RGBA.green", green2Index, rgbaIndex, 1);
  AnimatablePropertyComponentRegistration(typeRegistration, "RGBA.blue", blue2Index, rgbaIndex, 2);
  AnimatablePropertyComponentRegistration(typeRegistration, "RGBA.alpha", alphaIndex, rgbaIndex, 3);

  derived.SetProperty(foobarIndex, Vector2(9.0f, 10.0f));
  derived.SetProperty(fooIndex, 100.0f);
  derived.SetProperty(barIndex, 200.0f);

  derived.SetProperty(rgbIndex, Vector3(0.9f, 0.9f, 0.1f));
  derived.SetProperty(redIndex, 1.0f);
  derived.SetProperty(greenIndex, 1.0f);
  derived.SetProperty(blueIndex, 1.0f);

  derived.SetProperty(rgbaIndex, Color::WHITE * 0.5f);
  derived.SetProperty(red2Index, Color::SEA_GREEN.r);
  derived.SetProperty(green2Index, Color::SEA_GREEN.g);
  derived.SetProperty(blue2Index, Color::SEA_GREEN.b);
  derived.SetProperty(alphaIndex, Color::SEA_GREEN.a);

  application.SendNotification();
  application.Render(16);

  Vector2 foobar = derived.GetCurrentProperty<Vector2>(foobarIndex);
  DALI_TEST_EQUALS(foobar, Vector2(100.0f, 200.0f), 0.0001f, TEST_LOCATION);

  Vector3 colour = derived.GetCurrentProperty<Vector3>(rgbIndex);
  DALI_TEST_EQUALS(colour, Vector3(1.0f, 1.0f, 1.0f), 0.0001f, TEST_LOCATION);

  Vector4 col2 = derived.GetCurrentProperty<Vector4>(rgbaIndex);
  DALI_TEST_EQUALS(col2, Color::SEA_GREEN, 0.0001f, TEST_LOCATION);

  END_TEST;
}
