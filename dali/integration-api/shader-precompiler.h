#ifndef DALI_INTEGRATION_SHADER_PRECOMPILER_H
#define DALI_INTEGRATION_SHADER_PRECOMPILER_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

// INTERNAL HEADER
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/devel-api/threading/conditional-wait.h>
#include <dali/devel-api/threading/mutex.h>

// EXTERNAL HEDAER
#include <string_view>
#include <memory>
#include <mutex>

namespace Dali
{
namespace Internal DALI_INTERNAL
{
class ShaderPrecompiler;
}

struct RawShaderData
{
  int shaderCount;
  std::vector<std::string_view> vertexPrefix;
  std::vector<std::string_view> fragmentPrefix;
  std::string_view vertexShader;
  std::string_view fragmentShader;
};

namespace Integration
{
/**
 * ShaderPrecompiler  is used to precompile shaders.
 * The precompiled shaders are stored in a file.
 * @SINCE_2_2.45
 */
class DALI_CORE_API ShaderPrecompiler : public BaseHandle
{
public:
  /**
   * @brief Gets the singleton of ShaderPrecompiler object.
   *
   * @SINCE_2_2.45
   * @return A handle to the ShaderPrecompiler
   */
  static ShaderPrecompiler& Get();

  /**
   * @brief Get the precompile shader list.
   *
   *  @SINCE_2_2.45
   *  @param[in] shaders shader data for precompile
  */
  void GetPrecompileShaderList(RawShaderData& shaders);

  /**
   * @brief Save the precompile shader list.
   *
   * @SINCE_2_2.45
   * @param[in] shaders shader data for precompile
  */
  void SavePrecomipleShaderList(RawShaderData& shaders);

  /**
   * @brief Check precompile list is ready or not
   *
   * @SINCE_2_2.45
   * @return true if precompile list is ready
  */
  bool IsReady() const;

  /**
   * @brief Enable the feature of precompile
   *
   * @SINCE_2_2.45
  */
  void Enable();

  /**
   * @brief Check the feature of precompile is enabled or not
   *
   * @SINCE_2_2.45
   * @return true if the feature of precompile is enabled
  */
  bool IsEnable();

  /**
   * Construct a new ShaderPrecompiler.
   */
  ShaderPrecompiler();

  // Undefined
  ShaderPrecompiler(const ShaderPrecompiler&) = delete;

  // Undefined
  ShaderPrecompiler& operator=(const ShaderPrecompiler& rhs) = delete;

private:
  static std::unique_ptr<ShaderPrecompiler> mInstance;
  static std::once_flag mOnceFlag;
  RawShaderData mRawShaderData;
  ConditionalWait mConditionalWait;
  bool mPrecompiled;
  bool mEnabled;
};

} // namespace Integration

} // namespace Dali

#endif // DALI_INTEGRATION_SHADER_PRECOMPILER_H
