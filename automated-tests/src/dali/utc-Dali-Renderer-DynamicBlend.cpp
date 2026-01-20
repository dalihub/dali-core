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

#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/common/capabilities.h>
#include <dali/devel-api/common/stage.h>
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>
#include <mesh-builder.h>
#include "test-graphics-command-buffer.h"
#include "test-graphics-controller.h"

using namespace Dali;

void renderer_dynamic_blend_test_startup(void)
{
  test_return_value = TET_UNDEF;
}

void renderer_dynamic_blend_test_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliRendererDynamicBlend01(void)
{
  TestApplication application;
  tet_infoline("Test Dynamic Blend Enable");

  // TestGraphicsController now always supports dynamic blend states
  auto& graphicsController = application.GetGraphicsController();
  graphicsController.SetDeviceLimitation(Graphics::DeviceCapability::SUPPORTED_DYNAMIC_STATES,
                                         Graphics::PipelineDynamicStateBits::COLOR_BLEND_ENABLE_BIT |
                                         Graphics::PipelineDynamicStateBits::COLOR_BLEND_EQUATION_BIT);

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::OPACITY, 0.5f); // Enable blending
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  // Set some blend properties to ensure we enter the inner if
  renderer.SetProperty(Renderer::Property::BLEND_FACTOR_SRC_RGB, BlendFactor::SRC_ALPHA);
  renderer.SetProperty(Renderer::Property::BLEND_FACTOR_DEST_RGB, BlendFactor::ONE_MINUS_SRC_ALPHA);

  application.SendNotification();
  application.Render();

  // Verify that SetColorBlendEnable was called on the command buffer
  // We need to access the command buffer from the submit stack
  const auto& commands = graphicsController.mSubmitStack;
  DALI_TEST_CHECK(!commands.empty());

  bool colorBlendEnableFound = false;
  for(uint32_t i = 0; i < commands.size(); ++i)
  {
    const auto& submitInfo = commands[i];
    for(uint32_t j = 0; j < submitInfo.cmdBuffer.size(); ++j)
    {
      const auto& cmdBuffer = *static_cast<const TestGraphicsCommandBuffer*>(submitInfo.cmdBuffer[j]);
      const auto& blendEnableCommands = cmdBuffer.GetChildCommandsByType(static_cast<CommandTypeMask>(CommandType::SET_COLOR_BLEND_ENABLE));
      if(!blendEnableCommands.empty())
      {
        colorBlendEnableFound = true;
        // Verify the command data if needed
        for(const auto* cmd : blendEnableCommands)
        {
          DALI_TEST_EQUALS(cmd->data.colorBlendEnable.enabled, true, TEST_LOCATION);
        }
      }
    }
  }
  DALI_TEST_CHECK(colorBlendEnableFound);

  END_TEST;
}

int UtcDaliRendererDynamicBlend02(void)
{
  TestApplication application;
  auto& graphicsController = application.GetGraphicsController();
  graphicsController.SetDeviceLimitation(Graphics::DeviceCapability::SUPPORTED_DYNAMIC_STATES,
                                        Graphics::PipelineDynamicStateBits::COLOR_BLEND_ENABLE_BIT |
                                        Graphics::PipelineDynamicStateBits::COLOR_BLEND_EQUATION_BIT);

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  Geometry geometry = Geometry::New();
  Renderer renderer = Renderer::New(geometry, shader);

  // Set blending options
  renderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);
  renderer.SetProperty(Renderer::Property::BLEND_FACTOR_SRC_RGB, BlendFactor::SRC_ALPHA);
  renderer.SetProperty(Renderer::Property::BLEND_FACTOR_DEST_RGB, BlendFactor::ONE_MINUS_SRC_ALPHA);
  renderer.SetProperty(Renderer::Property::BLEND_FACTOR_SRC_ALPHA, BlendFactor::ONE);
  renderer.SetProperty(Renderer::Property::BLEND_FACTOR_DEST_ALPHA, BlendFactor::ONE_MINUS_SRC_ALPHA);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  // Verify that SetColorBlendEquation was called
  const auto& commands = graphicsController.mSubmitStack;
  DALI_TEST_CHECK(!commands.empty());

  bool colorBlendEquationFound = false;
  for(uint32_t i = 0; i < commands.size(); ++i)
  {
    const auto& submitInfo = commands[i];
    for(uint32_t j = 0; j < submitInfo.cmdBuffer.size(); ++j)
    {
      const auto& cmdBuffer = *static_cast<const TestGraphicsCommandBuffer*>(submitInfo.cmdBuffer[j]);
      const auto& blendEquationCommands = cmdBuffer.GetChildCommandsByType(static_cast<CommandTypeMask>(CommandType::SET_COLOR_BLEND_EQUATION));
      if(!blendEquationCommands.empty())
      {
        colorBlendEquationFound = true;
        for(const auto* cmd : blendEquationCommands)
        {
          DALI_TEST_EQUALS(cmd->data.colorBlendEquation.srcColorBlendFactor, Graphics::BlendFactor::SRC_ALPHA, TEST_LOCATION);
          DALI_TEST_EQUALS(cmd->data.colorBlendEquation.dstColorBlendFactor, Graphics::BlendFactor::ONE_MINUS_SRC_ALPHA, TEST_LOCATION);
          DALI_TEST_EQUALS(cmd->data.colorBlendEquation.srcAlphaBlendFactor, Graphics::BlendFactor::ONE, TEST_LOCATION);
          DALI_TEST_EQUALS(cmd->data.colorBlendEquation.dstAlphaBlendFactor, Graphics::BlendFactor::ONE_MINUS_SRC_ALPHA, TEST_LOCATION);
        }
      }
    }
  }
  DALI_TEST_CHECK(colorBlendEquationFound);

  END_TEST;
}

int UtcDaliRendererDynamicBlend03(void)
{
  TestApplication application;
  auto& graphicsController = application.GetGraphicsController();
  graphicsController.SetDeviceLimitation(Graphics::DeviceCapability::SUPPORTED_DYNAMIC_STATES,
                                        Graphics::PipelineDynamicStateBits::COLOR_BLEND_ENABLE_BIT |
                                        Graphics::PipelineDynamicStateBits::COLOR_BLEND_EQUATION_BIT);

  Shader shader = Shader::New("VertexSource", "FragmentSource");
  Geometry geometry = Geometry::New();
  Renderer renderer = Renderer::New(geometry, shader);

  // Set blending options for advanced blend
  renderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);
  // Use MULTIPLY which triggers advanced blend
  renderer.SetProperty(DevelRenderer::Property::BLEND_EQUATION, DevelBlendEquation::MULTIPLY);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  // Verify that SetColorBlendAdvanced was called
  const auto& commands = graphicsController.mSubmitStack;
  DALI_TEST_CHECK(!commands.empty());

  bool colorBlendAdvancedFound = false;
  for(uint32_t i = 0; i < commands.size(); ++i)
  {
    const auto& submitInfo = commands[i];
    for(uint32_t j = 0; j < submitInfo.cmdBuffer.size(); ++j)
    {
      const auto& cmdBuffer = *static_cast<const TestGraphicsCommandBuffer*>(submitInfo.cmdBuffer[j]);
      const auto& blendAdvancedCommands = cmdBuffer.GetChildCommandsByType(static_cast<CommandTypeMask>(CommandType::SET_COLOR_BLEND_ADVANCED));
      if(!blendAdvancedCommands.empty())
      {
        colorBlendAdvancedFound = true;
        for(const auto* cmd : blendAdvancedCommands)
        {
          // Verify blend op is MULTIPLY (which maps to Graphics::BlendOp::MULTIPLY)
          // We assume mapping is consistent, or we check if it's >= MULTIPLY
          DALI_TEST_CHECK(cmd->data.colorBlendAdvanced.blendOp >= Graphics::ADVANCED_BLEND_OPTIONS_START);
        }
      }
    }
  }
  DALI_TEST_CHECK(colorBlendAdvancedFound);

  END_TEST;
}
