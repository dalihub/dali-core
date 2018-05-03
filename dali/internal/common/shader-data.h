#ifndef __DALI_INTERNAL_SHADER_DATA_H__
#define __DALI_INTERNAL_SHADER_DATA_H__

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

// EXTERNAL INCLUDES
#include <string>
#include <vector>

// INTERNAL INCLUDES
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/rendering/shader.h> // ShaderHints

namespace Dali
{

namespace Internal
{

class ShaderData;
typedef IntrusivePtr<ShaderData> ShaderDataPtr;

namespace
{
inline std::vector<char> StringToVector( const std::string& str )
{
  auto retval = std::vector<char>{};
  retval.insert( retval.begin(), str.begin(), str.end() );
  retval.push_back( '\0' );
  return retval;
}
}

/**
 * ShaderData class.
 * A container for shader source code and compiled binary byte code.
 */
class ShaderData : public Dali::RefObject
{
public:

  enum class Type
  {
    TEXT,
    BINARY,
  };

  enum class ShaderStage
  {
    VERTEX,
    FRAGMENT
  };
  /**
   * Constructor
   * @param[in] vertexSource   Source code for vertex program
   * @param[in] fragmentSource Source code for fragment program
   */
  ShaderData(const std::string& vertexSource, const std::string& fragmentSource, const Dali::Shader::Hint::Value hints)
  : mShaderHash( uint32_t(-1) ),
    mVertexShader(StringToVector(vertexSource)),
    mFragmentShader(StringToVector(fragmentSource)),
    mHints(hints),
    mType( Type::TEXT )
  { }

  /**
   * Creates a shader data containing binary content
   * @param vertexSource
   * @param fragmentSource
   * @param hints
   */
  ShaderData( std::vector<char>& vertexSource, std::vector<char>& fragmentSource, const Dali::Shader::Hint::Value hints)
    : mShaderHash( uint32_t(-1) ),
      mVertexShader(vertexSource),
      mFragmentShader(fragmentSource),
      mHints(hints),
      mType( Type::BINARY )
  { }

protected:
  /**
   * Protected Destructor
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~ShaderData() override = default;

public: // API

  /**
   * Set hash value which is created with vertex and fragment shader code
   * @param [in] shaderHash  hash key created with vertex and fragment shader code
   */
  void SetHashValue(size_t shaderHash)
  {
    DALI_ASSERT_DEBUG( shaderHash != size_t(-1) );
    mShaderHash = shaderHash;
  }

  /**
   * Get hash value which is created with vertex and fragment shader code
   * @return shaderHash  hash key created with vertex and fragment shader code
   */
  size_t GetHashValue() const
  {
    DALI_ASSERT_DEBUG( mShaderHash != size_t(-1) );
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
   * Returns a std::vector containing shader code associated with particular stage
   * @param stage
   * @return
   */
  const std::vector<char>& GetShaderForStage( ShaderStage stage ) const
  {
    if( stage == ShaderStage::VERTEX )
    {
      return mVertexShader;
    }
    else
    {
      return mFragmentShader;
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
  void AllocateBuffer( size_t size )
  {
    mBuffer.Resize( size );
  }

  /**
   * Get the program buffer
   * @return reference to the buffer
   */
  size_t GetBufferSize() const
  {
    return mBuffer.Size();
  }

  /**
   * Get the data that the buffer points to
   * @return raw pointer to the buffer data
   */
  unsigned char* GetBufferData()
  {
    DALI_ASSERT_DEBUG( mBuffer.Size() > 0 );
    return &mBuffer[0];
  }

  /**
   * Get the data that the buffer points to
   * @return raw pointer to the buffer data
   */
  Dali::Vector<unsigned char>& GetBuffer()
  {
    return mBuffer;
  }

  ShaderData(const ShaderData& other) = delete;            ///< no copying of this object
  ShaderData& operator= (const ShaderData& rhs) = delete;  ///< no copying of this object

private: // Data

  size_t                      mShaderHash;     ///< hash key created with vertex and fragment shader code
  std::vector<char>           mVertexShader;   ///< binary code for vertex program
  std::vector<char>           mFragmentShader; ///< binary code for fragment program
  Dali::Shader::Hint::Value   mHints;          ///< take a hint
  Dali::Vector<unsigned char> mBuffer;         ///< buffer containing compiled binary bytecode
  Type                        mType;           ///< Type of shader data ( text or binary )

};

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTERNAL_SHADER_DATA_H__
