/*
* Copyright (c) 2024 Samsung Electronics Co., Ltd.
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

int UtcDaliInternalShaderSaveAndLoad01(void)
{
  TestApplication application;

  std::string vertexShader1   = "some vertex code\n";
  std::string fragmentShader1 = "some fragment code\n";

  std::string vertexShader2   = "some another vertex code\n";
  std::string fragmentShader2 = "some another fragment code\n";

  Dali::Vector<uint8_t> dummyBuffer(5);
  for(size_t i = 0; i < 5; ++i)
  {
    dummyBuffer[i] = static_cast<uint8_t>(i + 21);
  }

  // Make save and load failed successful
  auto& platformAbstraction = application.GetPlatform();
  platformAbstraction.SetLoadFileResult(false, dummyBuffer);

  // Test version number in the shader data
  Dali::Internal::ThreadLocalStorage& tls           = Dali::Internal::ThreadLocalStorage::Get();
  Dali::Internal::ShaderFactory&      shaderFactory = tls.GetShaderFactory();

  tet_printf("Load shader1. It should be fail to load from platform abstraction\n");
  size_t                  shaderHash1 = 0u;
  Internal::ShaderDataPtr shaderData1 = shaderFactory.Load(vertexShader1, fragmentShader1, Shader::Hint::NONE, 0u, "", shaderHash1);
  DALI_TEST_CHECK(shaderHash1 != 0u);
  DALI_TEST_EQUALS(shaderData1.Get()->HasBinary(), false, TEST_LOCATION);

  DALI_TEST_EQUALS(platformAbstraction.WasCalled(TestPlatformAbstraction::SaveShaderBinaryFileFunc), false, TEST_LOCATION);
  DALI_TEST_EQUALS(platformAbstraction.WasCalled(TestPlatformAbstraction::LoadShaderBinaryFileFunc), true, TEST_LOCATION);

  // Reset trace callstack
  platformAbstraction.GetTrace().Reset();

  // Copy the memory of dummy
  shaderData1.Get()->AllocateBuffer(7);
  for(size_t i = 0; i < 7; ++i)
  {
    shaderData1.Get()->GetBuffer()[i] = static_cast<uint8_t>(i + 1);
  }

  DALI_TEST_EQUALS(shaderData1.Get()->HasBinary(), true, TEST_LOCATION);

  tet_printf("Save shaderData1 as binary, but failed.\n");
  shaderFactory.SaveBinary(shaderData1);

  tet_printf("Check shader saved\n");
  DALI_TEST_EQUALS(platformAbstraction.WasCalled(TestPlatformAbstraction::SaveShaderBinaryFileFunc), true, TEST_LOCATION);
  DALI_TEST_EQUALS(platformAbstraction.WasCalled(TestPlatformAbstraction::LoadShaderBinaryFileFunc), false, TEST_LOCATION);

  // Reset trace callstack
  platformAbstraction.GetTrace().Reset();

  tet_printf("Save shaderData1 as binary, and success now.\n");
  platformAbstraction.SetSaveFileResult(true);
  shaderFactory.SaveBinary(shaderData1);

  tet_printf("Check shader saved\n");
  DALI_TEST_EQUALS(platformAbstraction.WasCalled(TestPlatformAbstraction::SaveShaderBinaryFileFunc), true, TEST_LOCATION);
  DALI_TEST_EQUALS(platformAbstraction.WasCalled(TestPlatformAbstraction::LoadShaderBinaryFileFunc), false, TEST_LOCATION);

  // Reset trace callstack
  platformAbstraction.GetTrace().Reset();

  tet_printf("Load shaderData2 with same code as shaderData1\n");
  size_t                  shaderHash2 = 0u;
  Internal::ShaderDataPtr shaderData2 = shaderFactory.Load(vertexShader1, fragmentShader1, Shader::Hint::NONE, 0u, "", shaderHash2);

  tet_printf("Check shaderData2 cached\n");
  DALI_TEST_EQUALS(shaderHash2, shaderHash1, TEST_LOCATION);
  DALI_TEST_EQUALS(shaderData2.Get()->HasBinary(), true, TEST_LOCATION);

  DALI_TEST_EQUALS(shaderData2->GetBufferSize(), shaderData1->GetBufferSize(), TEST_LOCATION);
  for(size_t i = 0; i < shaderData1->GetBufferSize(); ++i)
  {
    DALI_TEST_EQUALS(shaderData2->GetBuffer()[i], shaderData1->GetBuffer()[i], TEST_LOCATION);
  }

  DALI_TEST_EQUALS(platformAbstraction.WasCalled(TestPlatformAbstraction::SaveShaderBinaryFileFunc), false, TEST_LOCATION);
  DALI_TEST_EQUALS(platformAbstraction.WasCalled(TestPlatformAbstraction::LoadShaderBinaryFileFunc), false, TEST_LOCATION);

  // Reset trace callstack
  platformAbstraction.GetTrace().Reset();

  tet_printf("Make shaderData3 load dummyBuffer from filesystem\n");
  platformAbstraction.SetLoadFileResult(true, dummyBuffer);

  tet_printf("Load shader3. It will get binary same as dummyBuffer\n");
  size_t                  shaderHash3 = 0u;
  Internal::ShaderDataPtr shaderData3 = shaderFactory.Load(vertexShader2, fragmentShader2, Shader::Hint::NONE, 0u, "", shaderHash3);
  DALI_TEST_CHECK(shaderHash3 != 0u);
  DALI_TEST_EQUALS(shaderData3.Get()->HasBinary(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(shaderData3->GetBufferSize(), dummyBuffer.Count(), TEST_LOCATION);
  for(size_t i = 0; i < dummyBuffer.Count(); ++i)
  {
    DALI_TEST_EQUALS(shaderData3->GetBuffer()[i], dummyBuffer[i], TEST_LOCATION);
  }

  DALI_TEST_EQUALS(platformAbstraction.WasCalled(TestPlatformAbstraction::SaveShaderBinaryFileFunc), false, TEST_LOCATION);
  DALI_TEST_EQUALS(platformAbstraction.WasCalled(TestPlatformAbstraction::LoadShaderBinaryFileFunc), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInternalShaderSaveAndLoad02(void)
{
  TestApplication application;

  std::string vertexShader1   = "some vertex code\n";
  std::string fragmentShader1 = "some fragment code\n";

  std::string vertexShader2   = "some another vertex code\n";
  std::string fragmentShader2 = "some another fragment code\n";

  Dali::Vector<uint8_t> dummyBuffer(5);
  for(size_t i = 0; i < 5; i++)
  {
    dummyBuffer[i] = static_cast<uint8_t>(i + 21);
  }

  // Make save and load failed successful
  auto& platformAbstraction = application.GetPlatform();
  platformAbstraction.SetLoadFileResult(false, dummyBuffer);

  // Test version number in the shader data
  Dali::Internal::ThreadLocalStorage& tls           = Dali::Internal::ThreadLocalStorage::Get();
  Dali::Internal::ShaderFactory&      shaderFactory = tls.GetShaderFactory();

  tet_printf("Load shader1. It should be cached at string container\n");
  size_t                  shaderHash1 = 0u;
  Internal::ShaderDataPtr shaderData1 = shaderFactory.Load(vertexShader1, fragmentShader1, Shader::Hint::NONE, 0u, "", shaderHash1);
  DALI_TEST_CHECK(shaderHash1 != 0u);

  DALI_TEST_EQUALS(platformAbstraction.WasCalled(TestPlatformAbstraction::SaveShaderBinaryFileFunc), false, TEST_LOCATION);
  DALI_TEST_EQUALS(platformAbstraction.WasCalled(TestPlatformAbstraction::LoadShaderBinaryFileFunc), true, TEST_LOCATION);

  // Reset trace callstack
  platformAbstraction.GetTrace().Reset();

  tet_printf("Load shader2. It also should be cached at string container\n");
  size_t                  shaderHash2 = 0u;
  Internal::ShaderDataPtr shaderData2 = shaderFactory.Load(vertexShader2, fragmentShader2, Shader::Hint::NONE, 0u, "", shaderHash2);
  DALI_TEST_CHECK(shaderHash2 != 0u);

  DALI_TEST_EQUALS(platformAbstraction.WasCalled(TestPlatformAbstraction::SaveShaderBinaryFileFunc), false, TEST_LOCATION);
  DALI_TEST_EQUALS(platformAbstraction.WasCalled(TestPlatformAbstraction::LoadShaderBinaryFileFunc), true, TEST_LOCATION);

  // Reset trace callstack
  platformAbstraction.GetTrace().Reset();

  tet_printf("Both shader 1 and 2 don't have binary now.\n");
  DALI_TEST_EQUALS(shaderData1.Get()->HasBinary(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(shaderData2.Get()->HasBinary(), false, TEST_LOCATION);

  // Copy the memory of dummy
  shaderData1.Get()->AllocateBuffer(5);
  for(size_t i = 0; i < 5; i++)
  {
    shaderData1.Get()->GetBuffer()[i] = static_cast<uint8_t>(i + 1);
  }

  DALI_TEST_EQUALS(shaderData1.Get()->HasBinary(), true, TEST_LOCATION);

  tet_printf("Save shaderData1 as binary, but failed.\n");
  shaderFactory.SaveBinary(shaderData1);

  tet_printf("Check shader saved\n");
  DALI_TEST_EQUALS(platformAbstraction.WasCalled(TestPlatformAbstraction::SaveShaderBinaryFileFunc), true, TEST_LOCATION);
  DALI_TEST_EQUALS(platformAbstraction.WasCalled(TestPlatformAbstraction::LoadShaderBinaryFileFunc), false, TEST_LOCATION);

  // Reset trace callstack
  platformAbstraction.GetTrace().Reset();

  tet_printf("Save shaderData1 as binary, and success now.\n");
  platformAbstraction.SetSaveFileResult(true);
  shaderFactory.SaveBinary(shaderData1);

  tet_printf("Check shader saved\n");
  DALI_TEST_EQUALS(platformAbstraction.WasCalled(TestPlatformAbstraction::SaveShaderBinaryFileFunc), true, TEST_LOCATION);
  DALI_TEST_EQUALS(platformAbstraction.WasCalled(TestPlatformAbstraction::LoadShaderBinaryFileFunc), false, TEST_LOCATION);

  // Reset trace callstack
  platformAbstraction.GetTrace().Reset();

  tet_printf("Load shader with same code as shaderData1\n");
  size_t                  shaderHash3 = 0u;
  Internal::ShaderDataPtr shaderData3 = shaderFactory.Load(vertexShader1, fragmentShader1, Shader::Hint::NONE, 0u, "", shaderHash3);

  tet_printf("Check shaderData1 cached\n");
  DALI_TEST_EQUALS(shaderHash3, shaderHash1, TEST_LOCATION);
  DALI_TEST_EQUALS(shaderData3.Get()->HasBinary(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(shaderData3.Get(), shaderData1.Get(), TEST_LOCATION);

  DALI_TEST_EQUALS(platformAbstraction.WasCalled(TestPlatformAbstraction::SaveShaderBinaryFileFunc), false, TEST_LOCATION);
  DALI_TEST_EQUALS(platformAbstraction.WasCalled(TestPlatformAbstraction::LoadShaderBinaryFileFunc), false, TEST_LOCATION);

  // Reset trace callstack
  platformAbstraction.GetTrace().Reset();

  tet_printf("Load shader with same code as shaderData2\n");
  size_t                  shaderHash4 = 0u;
  Internal::ShaderDataPtr shaderData4 = shaderFactory.Load(vertexShader2, fragmentShader2, Shader::Hint::NONE, 0u, "", shaderHash4);

  tet_printf("Check shaderData2 cached\n");
  DALI_TEST_EQUALS(shaderHash4, shaderHash2, TEST_LOCATION);
  DALI_TEST_EQUALS(shaderData4.Get(), shaderData2.Get(), TEST_LOCATION);
  DALI_TEST_EQUALS(shaderData4.Get()->HasBinary(), false, TEST_LOCATION);

  DALI_TEST_EQUALS(platformAbstraction.WasCalled(TestPlatformAbstraction::SaveShaderBinaryFileFunc), false, TEST_LOCATION);
  DALI_TEST_EQUALS(platformAbstraction.WasCalled(TestPlatformAbstraction::LoadShaderBinaryFileFunc), false, TEST_LOCATION);

  // Reset trace callstack
  platformAbstraction.GetTrace().Reset();

  tet_printf("Allow to load shader binary\n");
  platformAbstraction.SetLoadFileResult(true, dummyBuffer);

  tet_printf("Load shader same as shaderData1, but difference render pass tag\n");
  size_t                  shaderHash5 = 0u;
  Internal::ShaderDataPtr shaderData5 = shaderFactory.Load(vertexShader1, fragmentShader1, Shader::Hint::NONE, 1u, "", shaderHash5);

  tet_printf("Check shaderData1 and shaderData5 have same hash, but deferent buffer\n");
  DALI_TEST_EQUALS(shaderHash5, shaderHash1, TEST_LOCATION);
  DALI_TEST_CHECK(shaderData5.Get() != shaderData1.Get());
  DALI_TEST_EQUALS(shaderData5.Get()->HasBinary(), true, TEST_LOCATION);

  tet_printf("Check shaderData5 binary same as dummy buffer\n");
  DALI_TEST_EQUALS(shaderData5.Get()->GetBufferSize(), dummyBuffer.Count(), TEST_LOCATION);
  for(size_t i = 0; i < 5; ++i)
  {
    DALI_TEST_EQUALS(shaderData5.Get()->GetBuffer()[i], dummyBuffer[i], TEST_LOCATION);
  }

  DALI_TEST_EQUALS(platformAbstraction.WasCalled(TestPlatformAbstraction::SaveShaderBinaryFileFunc), false, TEST_LOCATION);
  DALI_TEST_EQUALS(platformAbstraction.WasCalled(TestPlatformAbstraction::LoadShaderBinaryFileFunc), true, TEST_LOCATION);
  END_TEST;
}
