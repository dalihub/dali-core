#ifndef DALI_GRAPHICS_API_RENDER_COMMAND_H
#define DALI_GRAPHICS_API_RENDER_COMMAND_H

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

// EXTERNAL INCLUDES
#include <tuple>
#include <utility>
#include <vector>

// INTERNAL INCLUDES
#include <dali/graphics-api/graphics-api-generic-buffer.h>
#include <dali/graphics-api/utility/utility-builder.h>
#include <dali/graphics-api/utility/utility-strong-type.h>
#include <dali/graphics-api/graphics-api-shader-details.h>
#include <dali/graphics-api/graphics-api-accessor.h>

namespace Dali
{
namespace Graphics
{
namespace API
{
class Shader;
class Texture;
using PrimitiveCount = Utility::StrongType<size_t, struct PrimitiveCountTag>;
using BufferInfo     = std::unique_ptr<GenericBufferBase>;
using BufferList     = Utility::StrongType<std::vector<BufferInfo>, struct BufferListTag>;
using ShaderList     = std::vector<Accessor<Shader>>;
using TextureList    = std::vector<Accessor<Texture>>;

/**
 * @brief Interface class for RenderCommand types in the graphics API.
 */
class RenderCommand final
{
public:
  RenderCommand( PrimitiveCount primitiveCount, BufferList&& bufferList,
  TextureList&& textureList )
  : mPrimitiveCount{primitiveCount}, mBufferList{std::move( bufferList )}, mTextureList{std::move( textureList )}
  {
  }

  // derived types should not be moved direcly to prevent slicing
  RenderCommand( RenderCommand&& ) = default;
  RenderCommand& operator=( RenderCommand&& ) = default;

  // not copyable
  RenderCommand( const RenderCommand& ) = delete;
  RenderCommand& operator=( const RenderCommand& ) = delete;

  ~RenderCommand() = default;

  PrimitiveCount GetPrimitiveCount() const
  {
    return mPrimitiveCount;
  }

  const BufferList& GetBufferList() const
  {
    return mBufferList;
  }

  const TextureList& GetTextures() const
  {
    return mTextureList;
  }

  Accessor<Shader> GetShader( ShaderDetails::PipelineStage shaderStage ) const
  {
    using ShaderDetails::PipelineStage;

    auto retval = Accessor<Shader>{nullptr};

    size_t index = mShaders.size();
    switch( shaderStage )
    {
      case PipelineStage::VERTEX:
      {
        index = 0;
        break;
      }
      case PipelineStage::FRAGMENT:
      {
        index = 1;
        break;
      }
      case PipelineStage::GEOMETRY:
      case PipelineStage::COMPUTE:
      case PipelineStage::TESSELATION_CONTROL:
      case PipelineStage::TESSELATION_EVALUATION:
      {
        break;
      }
    }
    if(index < mShaders.size())
    {
      retval = mShaders[index];
    }
    return retval;
  }

private:
  PrimitiveCount mPrimitiveCount;
  BufferList     mBufferList;
  TextureList    mTextureList;
  ShaderList     mShaders;
};

using RenderCommandBuilder = Utility::Builder<RenderCommand, PrimitiveCount, BufferList, TextureList>;

} // namespace API
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_API_RENDER_COMMAND_H
