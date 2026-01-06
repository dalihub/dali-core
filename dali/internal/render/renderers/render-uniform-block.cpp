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
UniformBlock::~UniformBlock()
{
  // Remove observers.
  ProgramDestroyed(nullptr);
}

void UniformBlock::WriteUniforms(BufferIndex renderBufferIndex, ProgramIndex programIndex, Render::UniformBufferView& ubo)
{
  for(auto& uniform : mUniformIndexMaps[programIndex])
  {
    switch(uniform.state)
    {
      case UniformIndexMap::State::INITIALIZED:
      {
        WriteDynUniform(uniform.propertyValue, uniform, ubo, renderBufferIndex);
        break;
      }
      case UniformIndexMap::State::INITIALIZE_REQUIRED:
      {
        DALI_ASSERT_DEBUG(false && "UniformIndexMap should be initialize at GetProgramIndex()!");
        DALI_FALLTHROUGH;
      }
      default:
      {
        break;
      }
    }
  }
}

void UniformBlock::OnMappingChanged()
{
  // Clear cached mappings
  ProgramDestroyed(nullptr);
}

void UniformBlock::ProgramDestroyed(const Program* program)
{
  if(!mProgramToUniformIndexMap.empty())
  {
    // Destroy whole mProgramToUniformIndexMap and mUniformIndexMaps container.
    // It will be re-created at next render time.
    // Note : Destroy the program will be happened at RenderManager::PostRender().
    //        Also, OnMappingChanged() called at RenderManager::PreRender().
    //        We don't worry about the mProgramToUniformIndexMap and mUniformIndexMaps become invalidated after this call.

    for(auto& iter : mProgramToUniformIndexMap)
    {
      if(iter.first != program)
      {
        Program* mutableProgram = const_cast<Program*>(iter.first);
        mutableProgram->RemoveLifecycleObserver(*this);
      }
    }

    mProgramToUniformIndexMap.clear();
    mProgramToUniformIndexMap.rehash(0u);
    mUniformIndexMaps.clear();
  }
}

UniformBlock::ProgramIndex UniformBlock::GetProgramIndex(const Program& program)
{
  auto it = mProgramToUniformIndexMap.find(&program);
  if(it != mProgramToUniformIndexMap.end())
  {
    return it->second;
  }

  ProgramIndex programIndex = mUniformIndexMaps.size();

  auto& uniformMap = GetUniformMap();

  const uint32_t mapCount = uniformMap.Count();

  // Create index map for program from uniform map
  UniformIndexMappings currentUniformIndexMap;
  currentUniformIndexMap.resize(mapCount);

  // Copy uniform map into uniformIndexMap
  uint32_t mapIndex = 0;
  for(; mapIndex < mapCount; ++mapIndex)
  {
    currentUniformIndexMap[mapIndex].propertyValue          = uniformMap[mapIndex].propertyPtr;
    currentUniformIndexMap[mapIndex].uniformName            = uniformMap[mapIndex].uniformName;
    currentUniformIndexMap[mapIndex].uniformNameHash        = uniformMap[mapIndex].uniformNameHash;
    currentUniformIndexMap[mapIndex].uniformNameHashNoArray = uniformMap[mapIndex].uniformNameHashNoArray;
    currentUniformIndexMap[mapIndex].arrayIndex             = uniformMap[mapIndex].arrayIndex;
  }

  // Initialize uniforms by current program.
  for(auto& uniform : currentUniformIndexMap)
  {
    DALI_ASSERT_ALWAYS(uniform.state == UniformIndexMap::State::INITIALIZE_REQUIRED && "Something wrong!");

    auto uniformInfo  = Graphics::UniformInfo{};
    auto uniformFound = program.GetUniform(uniform.uniformName.GetStringView(),
                                           uniform.uniformNameHash,
                                           uniform.uniformNameHashNoArray,
                                           uniformInfo);

    if(!uniformFound)
    {
      uniform.state = UniformIndexMap::State::NOT_USED;
      continue;
    }

    uniform.uniformOffset = uniformInfo.offset;

    const auto typeSize        = uniform.propertyValue->GetValueSize();
    uniform.arrayElementStride = uniformInfo.elementCount > 0 ? (uniformInfo.elementStride ? uniformInfo.elementStride : typeSize) : typeSize;
    uniform.matrixStride       = uniformInfo.matrixStride;

    uniform.state = UniformIndexMap::State::INITIALIZED;
  }

  for(ProgramIndex cachedUniformIndexMap = 0; cachedUniformIndexMap < mUniformIndexMaps.size(); ++cachedUniformIndexMap)
  {
    // DevNote : Cache miss will be happened very rarely. Most of case, mUniformIndexMaps.size() is 1.
    //           Also, the number of uniform is small enough usually.
    //           So, full-search test is okay

    bool found = true;
    DALI_ASSERT_ALWAYS(mapCount == mUniformIndexMaps[cachedUniformIndexMap].size());
    for(uint32_t i = 0; i < mapCount; ++i)
    {
      const auto& lhs = currentUniformIndexMap[i];
      const auto& rhs = mUniformIndexMaps[cachedUniformIndexMap][i];

      if(!((lhs.uniformOffset == rhs.uniformOffset) &&
           (lhs.arrayElementStride == rhs.arrayElementStride) &&
           (lhs.matrixStride == rhs.matrixStride) &&
           (lhs.state == rhs.state)))
      {
        found = false;
        break;
      }
    }
    if(found)
    {
      programIndex = cachedUniformIndexMap;
      break;
    }
  }

  if(programIndex == mUniformIndexMaps.size())
  {
    // Create first index map from uniform map
    mUniformIndexMaps.emplace_back(currentUniformIndexMap);
  }

  // Add observer to ensure cached index map cleared.
  {
    Program* mutableProgram = const_cast<Program*>(&program);
    mutableProgram->AddLifecycleObserver(*this);
  }
  mProgramToUniformIndexMap.emplace(&program, programIndex);

  return programIndex;
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
