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