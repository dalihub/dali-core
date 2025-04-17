#ifndef DALI_INTERNAL_RENDER_RENDER_UNIFORM_BLOCK_H
#define DALI_INTERNAL_RENDER_RENDER_UNIFORM_BLOCK_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
 */

// INTERNAL INCLUDES
#include <dali/devel-api/common/hash.h>
#include <dali/internal/update/common/property-owner.h>

namespace Dali::Internal
{
class Program;
namespace Render
{
class UniformBufferView;
/**
 * This property owner enables registration of properties as uniforms,
 * specifically within a named uniform block of a shader program.
 *
 * The named uniform block can be connected to any number of shaders,
 * and if the name matches the program reflection, then a singular
 * area of memory is reserved in the current frame's uniform buffer
 * for this block, and only properties registered with this uniform
 * block are written to that memory area.
 */
class UniformBlock : public SceneGraph::PropertyOwner
{
public:
  UniformBlock(std::string&& blockName)
  : mName(std::move(blockName)),
    mNameHash(CalculateHash(std::string_view(mName))),
    mUniformIndexMap(),
    mUpdateMaps(true)
  {
  }
  ~UniformBlock() override;

  const std::string& GetName() const
  {
    return mName;
  }

  size_t GetHash() const
  {
    return mNameHash;
  }

  void WriteUniforms(BufferIndex renderBufferIndex, const Program& program, UniformBufferView& ubo);

protected: // From SceneGraph::PropertyOwner
  void OnMappingChanged() override;

private:
  // Copy of Render::Renderer::UniformIndexMap;
  using Hash = std::size_t;
  struct UniformIndexMap
  {
    enum class State : uint8_t
    {
      INITIALIZE_REQUIRED,
      INITIALIZED,
      NOT_USED,
    };
    ConstString              uniformName;            ///< The uniform name
    const PropertyInputImpl* propertyValue{nullptr}; ///< The property value
    Hash                     uniformNameHash{0u};
    Hash                     uniformNameHashNoArray{0u};
    int32_t                  arrayIndex{-1};         ///< The array index
    uint32_t                 arrayElementStride{0u}; ///< The stride for element of an array (0 - tightly packed)
    uint32_t                 matrixStride{0u};       ///< The stride for a matrix row

    int16_t  uniformLocation{0u};
    uint16_t uniformOffset{0u};
    uint16_t uniformBlockIndex{0u};
    State    state{State::INITIALIZE_REQUIRED};
  };

  void WriteDynUniform(const PropertyInputImpl* propertyValue,
                       UniformIndexMap&         uniform,
                       UniformBufferView&       ubo,
                       BufferIndex              renderBufferIndex);

private:
  std::string mName;
  const Hash  mNameHash{0u};

  using UniformIndexMappings = std::vector<UniformIndexMap>;
  UniformIndexMappings mUniformIndexMap;

  bool mUpdateMaps : 1;
};
} // namespace Render

} // namespace Dali::Internal

#endif //DALI_INTERNAL_RENDER_RENDER_UNIFORM_BLOCK_H
