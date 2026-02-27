#ifndef DALI_INTERNAL_SHADER_FACTORY_H
#define DALI_INTERNAL_SHADER_FACTORY_H

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

// EXTERNAL INCLUDES
#include <dali/devel-api/common/map-wrapper.h>

// INTERNAL INCLUDES
#include <dali/internal/common/message.h>
#include <dali/internal/common/shader-data.h>
#include <dali/public-api/common/dali-vector.h>

namespace Dali
{
namespace Internal
{
class ShaderData;
using ShaderDataPtr = IntrusivePtr<ShaderData>;

/**
 * @brief ShaderFactory manages shader data caching in memory.
 *
 * The ShaderFactory caches shader source code in memory to avoid redundant loading.
 */
class ShaderFactory
{
public:
  /**
   * Default constructor
   */
  ShaderFactory();

  /**
   * Destructor
   */
  ~ShaderFactory();

  /**
   * @brief Loads shader data from in-memory cache or creates a new instance.
   *
   * Checks an in-memory cache for a shader program identified by a hash over the source files,
   * hints, and render pass tag. If the cache misses, creates a new ShaderData instance with
   * the source code and caches it.
   *
   * @param [in] vertexSource   The vertex shader source code
   * @param [in] fragmentSource The fragment shader source code
   * @param [in] hints          Shader compilation hints
   * @param [in] renderPassTag  RenderPass the shaders are executed
   * @param [in] name           The name of shaders are executed
   * @param [out] shaderHash    Hash key created from vertex and fragment shader code
   * @return                    ShaderData containing the source and hash value
   */
  Internal::ShaderDataPtr Load(std::string_view vertexSource, std::string_view fragmentSource, const Dali::Shader::Hint::Value hints, uint32_t renderPassTag, std::string_view name, size_t& shaderHash);

  /**
   * @brief Removes the string shader data from the cache.
   * @param[in] shaderData The shader data to remove.
   */
  void Remove(Internal::ShaderData& shaderData);

private:
  /**
   * @brief Remove all the string shader data from the cache.
   */
  void ResetStringShaderData();

  // Undefined
  ShaderFactory(const ShaderFactory&) = delete;

  // Undefined
  ShaderFactory& operator=(const ShaderFactory& rhs) = delete;

private:
  using ShaderCacheContainer = std::map<size_t, Dali::Vector<Internal::ShaderData*>>; ///< Container for the shader cache. Key is hash of shader code.

  ShaderCacheContainer mShaderStringCache; ///< Cache of non-pre-compiled shaders. (TODO : Could we clean up this cache by user management?)

  uint32_t mTotalStringCachedShadersCount{0u}; ///< Total number of cached shaders that are not pre-compiled.

}; // class ShaderFactory

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_SHADER_FACTORY_H
