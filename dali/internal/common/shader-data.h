#ifndef DALI_INTERNAL_SHADER_DATA_H
#define DALI_INTERNAL_SHADER_DATA_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <string>

// INTERNAL INCLUDES
#include <dali/graphics-api/graphics-types.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/rendering/shader.h> // ShaderHints

namespace Dali
{
namespace Internal
{
class ShaderData;
using ShaderDataPtr = IntrusivePtr<ShaderData>;

namespace
{
static const std::vector<char> emptyShader;

inline std::vector<char> StringToVector(const std::string& str)
{
  auto retval = std::vector<char>{};
  retval.insert(retval.begin(), str.begin(), str.end());
  retval.push_back('\0');
  return retval;
}

} // namespace

/**
 * ShaderData class.
 * A container for shader source code and compiled binary byte code.
 */
class ShaderData : public Dali::RefObject
{
public:
  /**
   * Constructor
   * @param[in] vertexSource   Source code for vertex program
   * @param[in] fragmentSource Source code for fragment program
   * @param[in] hints          Hints for rendering
   */
  ShaderData(std::string vertexSource, std::string fragmentSource, const Dali::Shader::Hint::Value hints)
  : mShaderHash(-1),
    mVertexShader(StringToVector(vertexSource)),
    mFragmentShader(StringToVector(fragmentSource)),
    mHints(hints),
    mSourceMode(Graphics::ShaderSourceMode::TEXT)
  {
  }

  /**
   * Creates a shader data containing binary content
   * @param[in] vertexSource   Source code for vertex program
   * @param[in] fragmentSource Source code for fragment program
   * @param[in] hints          Hints for rendering
   */
  ShaderData(std::vector<char>& vertexSource, std::vector<char>& fragmentSource, const Dali::Shader::Hint::Value hints)
  : mShaderHash(-1),
    mVertexShader(vertexSource),
    mFragmentShader(fragmentSource),
    mHints(hints),
    mSourceMode(Graphics::ShaderSourceMode::BINARY)
  {
  }

protected:
  /**
   * Protected Destructor
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~ShaderData() override
  {
    // vector releases its data
  }

public: // API
  /**
   * Set hash value which is created with vertex and fragment shader code
   * @param [in] shaderHash  hash key created with vertex and fragment shader code
   */
  void SetHashValue(size_t shaderHash)
  {
    DALI_ASSERT_DEBUG(shaderHash != size_t(-1));
    mShaderHash = shaderHash;
  }

  /**
   * Get hash value which is created with vertex and fragment shader code
   * @return shaderHash  hash key created with vertex and fragment shader code
   */
  size_t GetHashValue() const
  {
    DALI_ASSERT_DEBUG(mShaderHash != size_t(-1));
    return mShaderHash;
  }

  /**
   * @return the vertex shader
   */
  const char* GetVertexShader() const
  {
    return &mVertexShader[0];
  }

  /**
   * @return the vertex shader
   */
  const char* GetFragmentShader() const
  {
    return &mFragmentShader[0];
  }

  /**
   * Returns a std::vector containing the shader code associated with particular pipeline stage
   * @param[in] the graphics pipeline stage
   * @return the shader code
   */
  const std::vector<char>& GetShaderForPipelineStage(Graphics::PipelineStage stage) const
  {
    if(stage == Graphics::PipelineStage::VERTEX_SHADER)
    {
      return mVertexShader;
    }
    else if(stage == Graphics::PipelineStage::FRAGMENT_SHADER)
    {
      return mFragmentShader;
    }
    else
    {
      //      DALI_LOG_ERROR("Unsupported shader stage\n");
      return emptyShader;
    }
  }

  /**
   * @return the hints
   */
  Dali::Shader::Hint::Value GetHints() const
  {
    return mHints;
  }
  /**
   * Check whether there is a compiled binary available
   * @return true if this objects contains a compiled binary
   */
  bool HasBinary() const
  {
    return 0 != mBuffer.Size();
  }

  /**
   * Allocate a buffer for the compiled binary bytecode
   * @param[in] size  The size of the buffer in bytes
   */
  void AllocateBuffer(std::size_t size)
  {
    mBuffer.Resize(size);
  }

  /**
   * Get the program buffer
   * @return reference to the buffer
   */
  std::size_t GetBufferSize() const
  {
    return mBuffer.Size();
  }

  /**
   * Get the data that the buffer points to
   * @return raw pointer to the buffer data
   */
  uint8_t* GetBufferData()
  {
    DALI_ASSERT_DEBUG(mBuffer.Size() > 0);
    return &mBuffer[0];
  }

  /**
   * Get the data that the buffer points to
   * @return raw pointer to the buffer data
   */
  Dali::Vector<uint8_t>& GetBuffer()
  {
    return mBuffer;
  }

  /**
   * Get the source mode of shader data
   * @return the source mode of shader data ( text or binary )
   */
  Graphics::ShaderSourceMode GetSourceMode() const
  {
    return mSourceMode;
  }

private:                                        // Not implemented
  ShaderData(const ShaderData& other);          ///< no copying of this object
  ShaderData& operator=(const ShaderData& rhs); ///< no copying of this object

private:                                      // Data
  std::size_t                mShaderHash;     ///< hash key created with vertex and fragment shader code
  std::vector<char>          mVertexShader;   ///< source code for vertex program
  std::vector<char>          mFragmentShader; ///< source code for fragment program
  Dali::Shader::Hint::Value  mHints;          ///< take a hint
  Dali::Vector<uint8_t>      mBuffer;         ///< buffer containing compiled binary bytecode
  Graphics::ShaderSourceMode mSourceMode;     ///< Source mode of shader data ( text or binary )
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_SHADER_DATA_H
