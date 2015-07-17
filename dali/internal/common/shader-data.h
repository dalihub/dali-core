#ifndef __DALI_INTERNAL_SHADER_DATA_H__
#define __DALI_INTERNAL_SHADER_DATA_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/common/dali-vector.h>

namespace Dali
{

namespace Internal
{

class ShaderData;
typedef IntrusivePtr<ShaderData> ShaderDataPtr;

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
   */
  ShaderData(const std::string& vertexSource, const std::string& fragmentSource)
  : mShaderHash( -1 ),
    mVertexShader(vertexSource),
    mFragmentShader(fragmentSource)
  { }

protected:
  /**
   * Protected Destructor
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ShaderData()
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
    return mVertexShader.c_str();
  }

  /**
   * @return the vertex shader
   */
  const char* GetFragmentShader() const
  {
    return mFragmentShader.c_str();
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

private: // Not implemented

  ShaderData(const ShaderData& other);            ///< no copying of this object
  ShaderData& operator= (const ShaderData& rhs);  ///< no copying of this object

private: // Data

  size_t                      mShaderHash;     ///< hash key created with vertex and fragment shader code
  std::string                 mVertexShader;   ///< source code for vertex program
  std::string                 mFragmentShader; ///< source code for fragment program
  Dali::Vector<unsigned char> mBuffer;         ///< buffer containing compiled binary bytecode
};

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTERNAL_SHADER_DATA_H__
