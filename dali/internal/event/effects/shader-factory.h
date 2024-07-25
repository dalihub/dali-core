#ifndef DALI_INTERNAL_SHADER_FACTORY_H
#define DALI_INTERNAL_SHADER_FACTORY_H

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

// EXTERNAL INCLUDES
#include <dali/devel-api/common/map-wrapper.h>

// INTERNAL INCLUDES
#include <dali/internal/common/message.h>
#include <dali/internal/common/shader-data.h>
#include <dali/internal/common/shader-saver.h>
#include <dali/public-api/common/dali-vector.h>

namespace Dali
{
namespace Internal
{
class ShaderData;
using ShaderDataPtr = IntrusivePtr<ShaderData>;

/**
 * @brief ShaderFactory loads and saves shader binaries synchronously.
 *
 * Binaries loaded or saved are also cached by the ShaderFactory.
 */
class ShaderFactory : public ShaderSaver
{
public:
  /**
   * Default constructor
   */
  ShaderFactory();

  /**
   * Destructor
   */
  ~ShaderFactory() override;

  /**
   * @brief Looks for precompiled binary version of shader program in memory and file caches.
   *
   * Tries to load a binary version of a shader program identified by a hash over the two source
   * files, checking an in-memory cache first.
   * If the cache hits or the load succeeds, the buffer member of the returned ShaderData will
   * contain a precompiled shader binary program which can be uploaded directly to GLES.
   *
   * @param [in] vertexSource   The vertex shader source code
   * @param [in] fragmentSource The fragment shader source code
   * @param [in] renderPassTag  RenderPass the shaders are executed
   * @param [in] name           The name of shaders are executed
   * @param [out] shaderHash    Hash key created from vertex and fragment shader code
   * @return                    ShaderData containing the source and hash value, and additionally,
   *                            a compiled shader program binary if one could be found, else an
   *                            empty binary buffer cleared to size zero.
   */
  Internal::ShaderDataPtr Load(std::string_view vertexSource, std::string_view fragmentSource, const Dali::Shader::Hint::Value hints, uint32_t renderPassTag, std::string_view name, size_t& shaderHash);

  /**
   * @brief Saves shader to memory cache and filesystem.
   *
   * This is called when a shader binary is ready to be saved to the memory cache file system.
   * Shaders that pass through here become available to subsequent invocations of Load.
   * @param[in] shader The data to be saved.
   * @sa Load
   */
  void SaveBinary(Internal::ShaderDataPtr shader) override;

private:
  void MemoryCacheInsert(Internal::ShaderData& shaderData, const bool isBinaryCached);

  /**
   * @brief Removes the string shader data from the cache.
   * @param[in] shaderData The shader data to remove.
   */
  void RemoveStringShaderData(Internal::ShaderData& shaderData);

  // Undefined
  ShaderFactory(const ShaderFactory&) = delete;

  // Undefined
  ShaderFactory& operator=(const ShaderFactory& rhs) = delete;

private:
  using ShaderCacheContainer = std::map<size_t, Dali::Vector<Internal::ShaderData*>>; ///< Container for the shader cache. Key is hash of shader code.

  ShaderCacheContainer mShaderBinaryCache; ///< Cache of pre-compiled shaders.
  ShaderCacheContainer mShaderStringCache; ///< Cache of non-pre-compiled shaders. (TODO : Could we clean up this cache by user management?)

}; // class ShaderFactory

inline MessageBase* ShaderCompiledMessage(ShaderSaver& factory, Internal::ShaderDataPtr shaderData)
{
  return new MessageValue1<ShaderSaver, Internal::ShaderDataPtr>(&factory,
                                                                 &ShaderSaver::SaveBinary,
                                                                 shaderData);
}

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_SHADER_FACTORY_H
