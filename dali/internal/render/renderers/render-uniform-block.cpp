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

// CLASS HEADER
#include <dali/internal/render/renderers/render-uniform-block.h>

// EXTERNAL HEADER
#include <dali/devel-api/common/hash.h>
#include <dali/integration-api/debug.h>

// INTERNAL HEADER
#include <dali/internal/render/renderers/uniform-buffer-view.h>
#include <dali/internal/render/shaders/program.h>

namespace Dali::Internal::Render
{
UniformBlock::~UniformBlock() = default;

void UniformBlock::WriteUniforms(BufferIndex renderBufferIndex, const Program& program, Render::UniformBufferView& ubo)
{
  if(mUpdateMaps)
  {
    auto& uniformMap = GetUniformMap();

    const uint32_t mapCount = uniformMap.Count();
    mUniformIndexMap.clear();
    mUniformIndexMap.resize(mapCount);

    // Copy uniform map into mUniformIndexMap
    uint32_t mapIndex = 0;
    for(; mapIndex < mapCount; ++mapIndex)
    {
      mUniformIndexMap[mapIndex].propertyValue          = uniformMap[mapIndex].propertyPtr;
      mUniformIndexMap[mapIndex].uniformName            = uniformMap[mapIndex].uniformName;
      mUniformIndexMap[mapIndex].uniformNameHash        = uniformMap[mapIndex].uniformNameHash;
      mUniformIndexMap[mapIndex].uniformNameHashNoArray = uniformMap[mapIndex].uniformNameHashNoArray;
      mUniformIndexMap[mapIndex].arrayIndex             = uniformMap[mapIndex].arrayIndex;
    }

    mUpdateMaps = false;
  }

  for(auto& iter : mUniformIndexMap)
  {
    auto& uniform = iter;

    if(!uniform.initialized)
    {
      auto uniformInfo  = Graphics::UniformInfo{};
      auto uniformFound = program.GetUniform(uniform.uniformName.GetStringView(),
                                             uniform.uniformNameHash,
                                             uniform.uniformNameHashNoArray,
                                             uniformInfo);

      if(!uniformFound)
      {
        continue;
      }

      uniform.uniformOffset     = uniformInfo.offset;
      uniform.uniformLocation   = int16_t(uniformInfo.location);
      uniform.uniformBlockIndex = uniformInfo.bufferIndex;
      uniform.initialized       = true;

      const auto typeSize        = iter.propertyValue->GetValueSize();
      uniform.arrayElementStride = uniformInfo.elementCount > 0 ? (uniformInfo.elementStride ? uniformInfo.elementStride : typeSize) : typeSize;
      uniform.matrixStride       = uniformInfo.matrixStride;

      WriteDynUniform(iter.propertyValue, uniform, ubo, renderBufferIndex);
    }
    else
    {
      WriteDynUniform(iter.propertyValue, uniform, ubo, renderBufferIndex);
    }
  }
}

void UniformBlock::OnMappingChanged()
{
  mUpdateMaps = true;
}

void UniformBlock::WriteDynUniform(
  const PropertyInputImpl* propertyValue,
  UniformIndexMap&         uniform,
  UniformBufferView&       ubo,
  BufferIndex              renderBufferIndex)
{
  const auto dest = uniform.uniformOffset + uniform.arrayElementStride * uniform.arrayIndex;

  const auto valueAddress = propertyValue->GetValueAddress(renderBufferIndex);

  if((propertyValue->GetType() == Property::MATRIX3 || propertyValue->GetType() == Property::VECTOR4) &&
     uniform.matrixStride != uint32_t(-1) &&
     uniform.matrixStride > 0)
  {
    // If the property is Vector4 type and matrixStride is valid integer, then we should treat it as mat2 type uniforms.
    const uint32_t matrixRow = (propertyValue->GetType() == Property::MATRIX3) ? 3 : 2;
    for(uint32_t i = 0; i < matrixRow; ++i)
    {
      ubo.Write(reinterpret_cast<const float*>(valueAddress) + i * matrixRow,
                sizeof(float) * matrixRow,
                dest + (i * uniform.matrixStride));
    }
  }
  else
  {
    const auto typeSize = propertyValue->GetValueSize();
    ubo.Write(valueAddress, typeSize, dest);
  }
}

} // namespace Dali::Internal::Render
