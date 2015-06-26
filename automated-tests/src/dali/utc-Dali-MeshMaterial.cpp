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

void mesh_material_test_startup(void)
{
  test_return_value = TET_UNDEF;
}

void mesh_material_test_cleanup(void)
{
  test_return_value = TET_PASS;
}


namespace
{

Material ConstructMaterial( const std::string& vertexShader,
                            const std::string& fragmentShader,
                            float opacity )
{
  Shader shader = Shader::New( vertexShader, fragmentShader );
  Material customMaterial = Material::New(shader);
  Vector4 color = Color::WHITE;
  color.a = opacity;
  customMaterial.SetProperty(Material::Property::COLOR, color);
  return customMaterial;
}

void TestBlending( TestApplication& application, Material material, float actorOpacity, BlendingMode::Type blendingMode, bool expectedBlend )
{
  // Generate geometry & renderers
  //Mesh mesh = Mesh::New(meshData);

  application.SendNotification();
  application.Render(0);
  application.Render();
  application.SendNotification();

  Actor actor = Actor::New();
  Stage::GetCurrent().Add(actor);

  //material.SetBlendMode(blendingMode);
  actor.SetOpacity(actorOpacity);

  TraceCallStack& cullFaceTrace = application.GetGlAbstraction().GetCullFaceTrace();
  cullFaceTrace.Enable(true);
  application.SendNotification();
  application.Render();
  //DALI_TEST_EQUALS( BlendEnabled( cullFaceTrace ), expectedBlend, TEST_LOCATION );
}

} // anonymous namespace


/**
 * Test cases
 *
 * Check construction of mesh objects
 * Check downcast of mesh objects (+ve & -ve)
 * Check staging of mesh objects
 *
 * Check staging of some mesh objects but not all
 * Create geometry with no verts - ensure it asserts
 * Create an element based geometry with no indices - ensure it asserts
 * Create a renderer without a material - ensure nothing renders
 * Create a render with a material, ensure the material can be changed
 * Create a render with a material&Sampler, ensure the sampler can be changed
 * Create a render with a material, sampler, ensure the sampler's texture can be changed
 * Create a render with a material, sampler, ensure that removing the sampler works
 *
 * Blend tests:
 * 1 Set Material with translucent color, actor color opaque, Set Use image alpha to true
 *   Expect blending
 * 2 Set material to translucent, set use image alpha to false, set actor opacity to 1.0f
 *   Expect no blending
 * 3 Set material to opaque, set use image alpha to true, set actor opacity to 1.0f
 *   Expect no blending
 * 4 Set material to have image with alpha, set use image alpha to true, set actor opacity to 1.0f
 *   Expect blending
 * 5 Set material to have image with alpha, set use image alpha to false, set actor opacity to 1.0f
 *   Expect no blending
 * 6 Set material to have image without alpha, set use image alpha to true, set actor opacity to 1.0f
 *   Expect no blending
 * 7 Set material to have framebuffer with alpha, set use image alpha to true, set actor opacity to 1.0f
 *   Expect blending
 * 8 Set material to have image with alpha, set use image alpha to false, set actor opacity to 0.5f
 *   Expect blending
 * 9 Set material to have image with no alpha, set material opacity to 0.5, set use image alpha to true, set actor opacity to 1.0f
 *   Expect blending
 *
 * Check defaults of renderer
 *
 * Bounding box of geometry?
 *
 * Check rendered vertex buffer is the right size for the initial property buffer
 *
 * Check PropertyBuffer set via SetData can be read thru property system
 * Check PropertyBuffer property setters / getters
 * Check vertex PropertyBuffer set via properties renders as expected
 * Check Index propertyBuffer set via properties renders as expected
 *
 * Check geometry type renders correctly as the matching GL draw call and type
 *
 * Check attributes change when rendering different actors with different
 * vertex formats (utc-Dali-Context.cpp)
 *
 * utc-Dali-Material.cpp
 * Check material color affects output - see shader uniform
 *
 * Check material sampler's image load/release policies affect rendering correctly.
 */
