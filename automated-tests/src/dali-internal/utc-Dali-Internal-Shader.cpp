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
#include <dali/integration-api/shader-integ.h>
#include <dali/internal/event/common/property-helper.h> // DALI_PROPERTY_TABLE_BEGIN, DALI_PROPERTY, DALI_PROPERTY_TABLE_END
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/effects/shader-factory.h>
#include <dali/internal/event/rendering/shader-impl.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/public-api/dali-core.h>
#include <dali/public-api/object/type-registry.h>

using namespace Dali;

int UtcDaliShaderTestVersion(void)
{
  TestApplication application;

  std::string vertexShader =
    "//@version 100\n"
    "some code\n";
  std::string fragmentShader =
    "//@version 101\n"
    "some code\n";

  Dali::Shader shader = Dali::Shader::New(vertexShader, fragmentShader);
  {
    auto vertexPrefix   = Dali::Shader::GetVertexShaderPrefix();
    auto fragmentPrefix = Dali::Shader::GetFragmentShaderPrefix();

    DALI_TEST_EQUALS(vertexPrefix.substr(0, 20), "//@legacy-prefix-end", TEST_LOCATION);
    DALI_TEST_EQUALS(fragmentPrefix.substr(0, 20), "//@legacy-prefix-end", TEST_LOCATION);
  }

  // Test version number in the shader data
  Dali::Internal::ThreadLocalStorage& tls           = Dali::Internal::ThreadLocalStorage::Get();
  Dali::Internal::ShaderFactory&      shaderFactory = tls.GetShaderFactory();
  size_t                              shaderHash;
  Internal::ShaderDataPtr             shaderData = shaderFactory.Load(vertexShader, fragmentShader, {}, {}, "", shaderHash);

  bool dataValid = (shaderData != nullptr);
  DALI_TEST_EQUALS(dataValid, true, TEST_LOCATION);

  DALI_TEST_EQUALS(shaderData->GetVertexShaderVersion(), 100, TEST_LOCATION);
  DALI_TEST_EQUALS(shaderData->GetFragmentShaderVersion(), 101, TEST_LOCATION);

  END_TEST;
}

int UtcDaliShaderWithPrefixTestVersion(void)
{
  TestApplication application;

  std::string vertexShader =
    "//@version 100\n"
    "some code\n";
  std::string fragmentShader =
    "//@version 101\n"
    "some code\n";

  auto vertexPrefix   = Dali::Shader::GetVertexShaderPrefix();
  auto fragmentPrefix = Dali::Shader::GetFragmentShaderPrefix();

  Dali::Shader shader = Dali::Shader::New(
    vertexPrefix + vertexShader,
    fragmentPrefix + fragmentShader);

  DALI_TEST_EQUALS(vertexPrefix.substr(0, 20), "//@legacy-prefix-end", TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentPrefix.substr(0, 20), "//@legacy-prefix-end", TEST_LOCATION);

  // Test version number in the shader data
  Dali::Internal::ThreadLocalStorage& tls           = Dali::Internal::ThreadLocalStorage::Get();
  Dali::Internal::ShaderFactory&      shaderFactory = tls.GetShaderFactory();
  size_t                              shaderHash;
  Internal::ShaderDataPtr             shaderData = shaderFactory.Load(vertexShader, fragmentShader, {}, {}, "", shaderHash);

  bool dataValid = (shaderData != nullptr);
  DALI_TEST_EQUALS(dataValid, true, TEST_LOCATION);

  DALI_TEST_EQUALS(shaderData->GetVertexShaderVersion(), 100, TEST_LOCATION);
  DALI_TEST_EQUALS(shaderData->GetFragmentShaderVersion(), 101, TEST_LOCATION);

  END_TEST;
}

int UtcDaliShaderWithPrefixTestVersion2(void)
{
  TestApplication application;

  tet_infoline("Test same case of UtcDaliShaderWithPrefixTestVersion with integration api");

  std::string vertexShader =
    "//@version 100\n"
    "some code\n";
  std::string fragmentShader =
    "//@version 101\n"
    "some code\n";

  // Get prefix from graphics config.
  auto vertexPrefix   = application.GetGlAbstraction().GetVertexShaderPrefix();
  auto fragmentPrefix = application.GetGlAbstraction().GetFragmentShaderPrefix();

  // And use GenerateTaggedShaderPrefix from integration-api.
  vertexPrefix   = Dali::Integration::GenerateTaggedShaderPrefix(vertexPrefix);
  fragmentPrefix = Dali::Integration::GenerateTaggedShaderPrefix(fragmentPrefix);

  Dali::Shader shader = Dali::Shader::New(
    vertexPrefix + vertexShader,
    fragmentPrefix + fragmentShader);

  DALI_TEST_EQUALS(vertexPrefix.substr(0, 20), "//@legacy-prefix-end", TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentPrefix.substr(0, 20), "//@legacy-prefix-end", TEST_LOCATION);

  // Test version number in the shader data
  Dali::Internal::ThreadLocalStorage& tls           = Dali::Internal::ThreadLocalStorage::Get();
  Dali::Internal::ShaderFactory&      shaderFactory = tls.GetShaderFactory();
  size_t                              shaderHash;
  Internal::ShaderDataPtr             shaderData = shaderFactory.Load(vertexShader, fragmentShader, {}, {}, "", shaderHash);

  bool dataValid = (shaderData != nullptr);
  DALI_TEST_EQUALS(dataValid, true, TEST_LOCATION);

  DALI_TEST_EQUALS(shaderData->GetVertexShaderVersion(), 100, TEST_LOCATION);
  DALI_TEST_EQUALS(shaderData->GetFragmentShaderVersion(), 101, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInternalShaderNewWithUniformBlock01(void)
{
  TestApplication application;

  std::string vertexShader1   = "some vertex code\n";
  std::string fragmentShader1 = "some fragment code\n";

  std::string vertexShader2   = "some another vertex code\n";
  std::string fragmentShader2 = "some another fragment code\n";

  UniformBlock sharedUniformBlock1 = UniformBlock::New("ubo1");
  UniformBlock sharedUniformBlock2 = UniformBlock::New("ubo2");
  UniformBlock sharedUniformBlock3 = UniformBlock::New("ubo3");

  Shader shader1 = Dali::Integration::ShaderNewWithUniformBlock(vertexShader1, fragmentShader1, Shader::Hint::NONE, "", {sharedUniformBlock1}, false);
  Shader shader2 = Dali::Integration::ShaderNewWithUniformBlock(vertexShader1, fragmentShader1, Shader::Hint::NONE, "", {sharedUniformBlock1, sharedUniformBlock2, sharedUniformBlock3}, true);
  Shader shader3 = Dali::Integration::ShaderNewWithUniformBlock(vertexShader1, fragmentShader1, Shader::Hint::NONE, "", {}, false);

  Shader shader4 = Dali::Integration::ShaderNewWithUniformBlock(vertexShader2, fragmentShader2, Shader::Hint::NONE, "", {sharedUniformBlock1}, false);

  DALI_TEST_CHECK(shader1);
  DALI_TEST_CHECK(shader2);
  DALI_TEST_CHECK(shader3);
  DALI_TEST_CHECK(shader4);

  DALI_TEST_CHECK(shader1 != shader2);
  DALI_TEST_CHECK(shader1 != shader3);
  DALI_TEST_CHECK(shader1 != shader4);
  DALI_TEST_CHECK(shader2 != shader3);
  DALI_TEST_CHECK(shader2 != shader4);
  DALI_TEST_CHECK(shader3 != shader4);

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliInternalShaderNewWithUniformBlock02(void)
{
  TestApplication application;

  std::string vertexShader1   = "some vertex code\n";
  std::string fragmentShader1 = "some fragment code\n";

  std::string vertexShader2   = "some another vertex code\n";
  std::string fragmentShader2 = "some another fragment code\n";

  UniformBlock sharedUniformBlock1 = UniformBlock::New("ubo1");
  UniformBlock sharedUniformBlock2 = UniformBlock::New("ubo2");
  UniformBlock sharedUniformBlock3 = UniformBlock::New("ubo3");

  std::string   hintSet = "MODIFIES_GEOMETRY";
  Property::Map map[2];
  map[0]["vertex"]        = vertexShader1;
  map[0]["fragment"]      = fragmentShader1;
  map[0]["renderPassTag"] = 0;
  map[0]["hints"]         = hintSet;
  map[0]["name"]          = "Test0";

  map[1]["vertex"]        = vertexShader2;
  map[1]["fragment"]      = fragmentShader2;
  map[1]["renderPassTag"] = 1;
  map[1]["hints"]         = hintSet;
  map[1]["name"]          = "Test1";

  Property::Array array;
  array.PushBack(map[0]);
  array.PushBack(map[1]);

  Shader shader1 = Dali::Integration::ShaderNewWithUniformBlock(array, {sharedUniformBlock1}, false);
  Shader shader2 = Dali::Integration::ShaderNewWithUniformBlock(array, {sharedUniformBlock1, sharedUniformBlock2, sharedUniformBlock3}, true);
  Shader shader3 = Dali::Integration::ShaderNewWithUniformBlock(array, {}, false);

  Shader shader4 = Dali::Integration::ShaderNewWithUniformBlock(map[0], {sharedUniformBlock1}, false);
  Shader shader5 = Dali::Integration::ShaderNewWithUniformBlock(map[1], {sharedUniformBlock1}, false);

  DALI_TEST_CHECK(shader1);
  DALI_TEST_CHECK(shader2);
  DALI_TEST_CHECK(shader3);
  DALI_TEST_CHECK(shader4);
  DALI_TEST_CHECK(shader5);

  DALI_TEST_CHECK(shader1 != shader2);
  DALI_TEST_CHECK(shader1 != shader3);
  DALI_TEST_CHECK(shader1 != shader4);
  DALI_TEST_CHECK(shader1 != shader5);
  DALI_TEST_CHECK(shader2 != shader3);
  DALI_TEST_CHECK(shader2 != shader4);
  DALI_TEST_CHECK(shader2 != shader5);
  DALI_TEST_CHECK(shader3 != shader4);
  DALI_TEST_CHECK(shader3 != shader5);
  DALI_TEST_CHECK(shader4 != shader5);

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliInternalShaderNewWithStrongConnectedUniformBlock01(void)
{
  TestApplication application;

  std::string vertexShader1   = "some vertex code\n";
  std::string fragmentShader1 = "some fragment code\n";

  UniformBlock sharedUniformBlock1 = UniformBlock::New("ubo1");
  UniformBlock sharedUniformBlock2 = UniformBlock::New("ubo2");

  Shader shader1 = Dali::Integration::ShaderNewWithUniformBlock(vertexShader1, fragmentShader1, Shader::Hint::NONE, "", {sharedUniformBlock1, sharedUniformBlock2}, false);
  Shader shader2 = Dali::Integration::ShaderNewWithUniformBlock(vertexShader1, fragmentShader1, Shader::Hint::NONE, "", {sharedUniformBlock1}, true);

  DALI_TEST_CHECK(shader1);
  DALI_TEST_CHECK(shader2);

  struct TestObjectDestroyedCallback
  {
    TestObjectDestroyedCallback(bool& signalReceived, const Dali::RefObject*& objectPointer)
    : mSignalVerified(signalReceived),
      mObjectPointer(objectPointer)
    {
    }

    void operator()(const Dali::RefObject* objectPointer)
    {
      tet_infoline("Verifying TestObjectDestroyedCallback()");

      if(objectPointer == mObjectPointer)
      {
        mSignalVerified = true;
      }
    }

    bool&                   mSignalVerified;
    const Dali::RefObject*& mObjectPointer;
  };

  // Test whether ubo2 is destroyed and ubo1 is alive
  const Dali::RefObject* ubo1Impl = sharedUniformBlock1.GetObjectPtr();
  const Dali::RefObject* ubo2Impl = sharedUniformBlock2.GetObjectPtr();

  bool ubo1Destroyed = false;
  bool ubo2Destroyed = false;

  ObjectRegistry registry = application.GetCore().GetObjectRegistry();
  DALI_TEST_CHECK(registry);
  registry.ObjectDestroyedSignal().Connect(&application, TestObjectDestroyedCallback(ubo1Destroyed, ubo1Impl));
  registry.ObjectDestroyedSignal().Connect(&application, TestObjectDestroyedCallback(ubo2Destroyed, ubo2Impl));

  DALI_TEST_EQUALS(ubo1Destroyed, false, TEST_LOCATION);
  DALI_TEST_EQUALS(ubo2Destroyed, false, TEST_LOCATION);

  sharedUniformBlock1.Reset();
  sharedUniformBlock2.Reset();

  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  // Now ubo2 is destroyed, but ubo1 still alive.
  DALI_TEST_EQUALS(ubo1Destroyed, false, TEST_LOCATION);
  DALI_TEST_EQUALS(ubo2Destroyed, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInternalShaderNewWithStrongConnectedUniformBlock02(void)
{
  TestApplication application;

  std::string vertexShader1   = "some vertex code\n";
  std::string fragmentShader1 = "some fragment code\n";

  UniformBlock sharedUniformBlock1 = UniformBlock::New("ubo1");
  UniformBlock sharedUniformBlock2 = UniformBlock::New("ubo2");

  Shader shader1 = Dali::Integration::ShaderNewWithUniformBlock(vertexShader1, fragmentShader1, Shader::Hint::NONE, "", {sharedUniformBlock1, sharedUniformBlock2}, false);
  Shader shader2 = Dali::Integration::ShaderNewWithUniformBlock(vertexShader1, fragmentShader1, Shader::Hint::NONE, "", {sharedUniformBlock1}, true);

  DALI_TEST_CHECK(shader1);
  DALI_TEST_CHECK(shader2);

  struct TestObjectDestroyedCallback
  {
    TestObjectDestroyedCallback(bool& signalReceived, const Dali::RefObject*& objectPointer)
    : mSignalVerified(signalReceived),
      mObjectPointer(objectPointer)
    {
    }

    void operator()(const Dali::RefObject* objectPointer)
    {
      tet_infoline("Verifying TestObjectDestroyedCallback()");

      if(objectPointer == mObjectPointer)
      {
        mSignalVerified = true;
      }
    }

    bool&                   mSignalVerified;
    const Dali::RefObject*& mObjectPointer;
  };

  // Disconnect with strong-connected shader.
  sharedUniformBlock1.DisconnectFromShader(shader2);

  const Dali::RefObject* ubo1Impl = sharedUniformBlock1.GetObjectPtr();
  const Dali::RefObject* ubo2Impl = sharedUniformBlock2.GetObjectPtr();

  bool ubo1Destroyed = false;
  bool ubo2Destroyed = false;

  ObjectRegistry registry = application.GetCore().GetObjectRegistry();
  DALI_TEST_CHECK(registry);
  registry.ObjectDestroyedSignal().Connect(&application, TestObjectDestroyedCallback(ubo1Destroyed, ubo1Impl));
  registry.ObjectDestroyedSignal().Connect(&application, TestObjectDestroyedCallback(ubo2Destroyed, ubo2Impl));

  DALI_TEST_EQUALS(ubo1Destroyed, false, TEST_LOCATION);
  DALI_TEST_EQUALS(ubo2Destroyed, false, TEST_LOCATION);

  sharedUniformBlock1.Reset();
  sharedUniformBlock2.Reset();

  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  // Now both ubo1 and ubo2 were destroyed.
  DALI_TEST_EQUALS(ubo1Destroyed, true, TEST_LOCATION);
  DALI_TEST_EQUALS(ubo2Destroyed, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInternalShaderFactoryCacheHit(void)
{
  TestApplication application;

  std::string vertexShader   = "some vertex code\n";
  std::string fragmentShader = "some fragment code\n";

  Dali::Internal::ThreadLocalStorage& tls           = Dali::Internal::ThreadLocalStorage::Get();
  Dali::Internal::ShaderFactory&      shaderFactory = tls.GetShaderFactory();
  size_t                              shaderHash;

  // Load shader data first time
  Internal::ShaderDataPtr shaderData1 = shaderFactory.Load(vertexShader, fragmentShader, Dali::Shader::Hint::NONE, 0, "Test", shaderHash);

  DALI_TEST_CHECK(shaderData1);
  DALI_TEST_EQUALS(shaderHash != 0u, true, TEST_LOCATION);

  // Load same shader data again - should hit cache
  Internal::ShaderDataPtr shaderData2 = shaderFactory.Load(vertexShader, fragmentShader, Dali::Shader::Hint::NONE, 0, "Test", shaderHash);

  DALI_TEST_CHECK(shaderData2);
  DALI_TEST_EQUALS(shaderData1.Get(), shaderData2.Get(), TEST_LOCATION);

  // Load shader data with different hints - should not hit cache
  Internal::ShaderDataPtr shaderData3 = shaderFactory.Load(vertexShader, fragmentShader, Dali::Shader::Hint::MODIFIES_GEOMETRY, 0, "Test", shaderHash);

  DALI_TEST_CHECK(shaderData3);
  DALI_TEST_EQUALS(shaderData2.Get() != shaderData3.Get(), true, TEST_LOCATION);

  // Load shader data with different renderPassTag - should not hit cache
  Internal::ShaderDataPtr shaderData4 = shaderFactory.Load(vertexShader, fragmentShader, Dali::Shader::Hint::NONE, 1, "Test", shaderHash);

  DALI_TEST_CHECK(shaderData4);
  DALI_TEST_EQUALS(shaderData2.Get() != shaderData4.Get(), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInternalShaderFactoryCacheRemove(void)
{
  TestApplication application;

  std::string vertexShader   = "some vertex code\n";
  std::string fragmentShader = "some fragment code\n";

  // Create shader which will cache the data
  Dali::Shader shader = Dali::Shader::New(vertexShader, fragmentShader, Dali::Shader::Hint::NONE, "Test");

  DALI_TEST_CHECK(shader);

  Dali::Internal::ThreadLocalStorage& tls           = Dali::Internal::ThreadLocalStorage::Get();
  Dali::Internal::ShaderFactory&      shaderFactory = tls.GetShaderFactory();
  size_t                              shaderHash;

  // Verify shader data is cached
  Internal::ShaderDataPtr shaderData1 = shaderFactory.Load(vertexShader, fragmentShader, Dali::Shader::Hint::NONE, 0, "Test", shaderHash);

  DALI_TEST_CHECK(shaderData1);

  // Reset shader
  shader.Reset();

  // Render to ensure destructor is called
  application.SendNotification();
  application.Render();

  // Load shader data again - should create new instance (cache was removed)
  Internal::ShaderDataPtr shaderData2 = shaderFactory.Load(vertexShader, fragmentShader, Dali::Shader::Hint::NONE, 0, "Test", shaderHash);

  DALI_TEST_CHECK(shaderData2);
  DALI_TEST_EQUALS(shaderData1.Get() != shaderData2.Get(), true, TEST_LOCATION);

  END_TEST;
}
