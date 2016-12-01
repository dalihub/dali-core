#ifndef __DALI_INTERNAL_SHADER_FACTORY_H__
#define __DALI_INTERNAL_SHADER_FACTORY_H__

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>
#include <dali/internal/common/message.h>
#include <dali/internal/common/shader-data.h>
#include <dali/internal/common/shader-saver.h>

namespace Dali
{

namespace Internal
{

class ShaderData;
typedef IntrusivePtr<ShaderData> ShaderDataPtr;

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
  virtual ~ShaderFactory();

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
   * @param [out] shaderHash    Hash key created from vertex and fragment shader code
   * @return                    ShaderData containing the source and hash value, and additionally,
   *                            a compiled shader program binary if one could be found, else an
   *                            empty binary buffer cleared to size zero.
   */
  Internal::ShaderDataPtr Load( const std::string& vertexSource, const std::string& fragmentSource, const Dali::Shader::Hint::Value hints, size_t& shaderHash );

  /**
   * @brief Saves shader to memory cache and filesystem.
   *
   * This is called when a shader binary is ready to be saved to the memory cache file system.
   * Shaders that pass through here become available to subsequent invocations of Load.
   * @param[in] shader The data to be saved.
   * @sa Load
   */
  virtual void SaveBinary( Internal::ShaderDataPtr shader );

private:

  void MemoryCacheInsert( Internal::ShaderData& shaderData );

  // Undefined
  ShaderFactory( const ShaderFactory& );

  // Undefined
  ShaderFactory& operator=( const ShaderFactory& rhs );

private:
  Dali::Vector< Internal::ShaderData* > mShaderBinaryCache; ///< Cache of pre-compiled shaders.

}; // class ShaderFactory

inline MessageBase* ShaderCompiledMessage( ShaderSaver& factory, Internal::ShaderDataPtr shaderData )
{
  return new MessageValue1< ShaderSaver, Internal::ShaderDataPtr >( &factory,
                                                            &ShaderSaver::SaveBinary,
                                                            shaderData );
}

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SHADER_FACTORY_H__
