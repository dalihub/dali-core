/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

#ifndef DALI_GRAPHICS_VULKAN_SPIRV_SPIRV_H
#define DALI_GRAPHICS_VULKAN_SPIRV_SPIRV_H

#include <cstdint>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
namespace SpirV
{

// include C header
#include "spirv.h"

using SPIRVWord = uint32_t;

/**
 * Opcode storage, doesn't expose direct interface, must be used within
 * SPIRVShader context
 */
struct SPIRVOpCode;

/**
 * Defines SPIRVShader program
 */
class SPIRVShader
{
  friend class SPIRVUtils;

public:

  SPIRVShader( SPIRVShader&& shader ) noexcept;
  SPIRVShader( const SPIRVShader& shader ) = delete;
  ~SPIRVShader();

  /**
   * Generates descriptor set layout info from the supplied shader
   * @note This function does not create the layout itself!
   * @return create info structure
   */
  vk::DescriptorSetLayoutCreateInfo GenerateDescriptorSetLayoutCreateInfo();

  /**
   * Returns total number of OpCodes
   * @return
   */
  uint32_t GetOpCodeCount() const;

  /**
   * Returns OpCode at specified index
   * @param index
   * @return
   */
  const SPIRVOpCode* GetOpCodeAt( uint32_t index ) const;

  /**
   * Returns OpCode associated with specified result id
   * @param resultId
   * @return
   */
  const SPIRVOpCode* GetOpCodeForResultId( uint32_t resultId ) const;

  /**
   * Helper function that allows to convert SPIRVWords into requested type
   * @tparam T
   * @param index
   * @return
   */
  template<class T>
  T GetOpCodeParameter( uint32_t index ) const
  {
    return *reinterpret_cast<const T*>( GetOpCodeParameterPtr( index ) );
  }

  /**
   * Returns SPIRV parameter as 32bit SPIRVWord
   * @param index
   * @return
   */
  SPIRVWord GetOpCodeParameterWord( uint32_t index ) const;

  /**
   *
   * @param opCode
   * @return
   */
  SpvOp GetOpCodeType( SPIRVOpCode& opCode );

private:

  /**
   * Accesses parameter via pointer
   * @param index
   * @return
   */
  const uint32_t* GetOpCodeParameterPtr( uint32_t index ) const;

  struct Impl;
  std::unique_ptr<Impl> mImpl;

  explicit SPIRVShader( Impl& impl );

  SPIRVShader();

  /**
   * Constructor that should be called only from SPIRVUtils
   * @param code
   */
  explicit SPIRVShader( std::vector<SPIRVWord> code );

public:
  /**
   * Accessor to the implementation object
   * @return
   */
  Impl& GetImplementation() const;
};



/**
 * Simple set
 */
class SPIRVUtils
{
public:

  /**
   * Creates SPIRVShader instance
   * @param data
   * @return
   */
  static SPIRVShader Parse( std::vector<SPIRVWord> data );

private:

  struct Impl;
  std::unique_ptr<Impl> mImpl;
};

}
} // namespace Vulkan
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_VULKAN_SPIRV_SPIRV_H
