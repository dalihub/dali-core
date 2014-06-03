#ifndef __DALI_INTEGRATION_SHADER_DATA_H__
#define __DALI_INTEGRATION_SHADER_DATA_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/vector-wrapper.h>

namespace Dali
{

namespace Integration
{

class ShaderData;

typedef IntrusivePtr<ShaderData> ShaderDataPtr;

/**
 * ShaderData class.
 * A container for shader source code and compiled binary byte code
 */
class DALI_IMPORT_API ShaderData : public Dali::RefObject
{
public:
  /**
   * Constructor
   * @param[in] vertexSource   Source code for vertex program
   * @param[in] fragmentSource Source code for fragment program
   */
  ShaderData(const std::string& vertexSource, const std::string& fragmentSource);

protected:
  /**
   * Protected Destructor
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ShaderData();

public:
  /**
   * Check whether there is a compiled binary available
   * @return true if this objects contains a compiled binary
   */
  bool HasBinary() const;

  /**
   * Allocate a buffer for the compiled binary bytecode
   * @param[in] size  The size of the buffer in bytes
   */
  void AllocateBuffer(const unsigned int size);

  /**
   * Set hash value which is created with vertex and fragment shader code
   * @param [in] shaderHash  hash key created with vertex and fragment shader code
   */
  void SetHashValue(size_t shaderHash) { mShaderHash = shaderHash; }

  /**
   * Get hash value which is created with vertex and fragment shader code
   * @return shaderHash  hash key created with vertex and fragment shader code
   */
  size_t GetHashValue() { return mShaderHash; }

private:
  ShaderData(const ShaderData& other);            ///< defined private to prevent use
  ShaderData& operator= (const ShaderData& rhs);  ///< defined private to prevent use

public: // Attributes
  size_t                     mShaderHash;         ///< hash key created with vertex and fragment shader code
  const std::string          vertexShader;        ///< source code for vertex program
  const std::string          fragmentShader;      ///< source code for fragment program
  std::vector<unsigned char> buffer;              ///< buffer containing compiled binary bytecode
};

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_SHADER_DATA_H__
