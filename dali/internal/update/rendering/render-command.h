#ifndef DALI_INTERNAL_SCENEGRAPH_RENDER_COMMAND_H
#define DALI_INTERNAL_SCENEGRAPH_RENDER_COMMAND_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

#include <dali/internal/common/buffer-index.h>
#include <dali/graphics-api/graphics-command-buffer.h>
#include <dali/graphics-api/graphics-controller.h>
#include <dali/graphics-api/graphics-pipeline.h>
#include <memory>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

struct RenderCommand
{
  RenderCommand()
  : mGfxRenderCommand{ nullptr },
    mGfxPipeline{ nullptr },
    mUboBindings{ {}, {} }
  {
  }

  Graphics::RenderCommand& AllocateGfxRenderCommand( Graphics::Controller& controller,
                                                     BufferIndex updateBufferIndex )
  {
    if (!mGfxRenderCommand)
    {
      mGfxRenderCommand = controller.AllocateRenderCommand();
    }
    return *mGfxRenderCommand.get();
  }

  Graphics::CommandBuffer& GetGfxRenderCommand( BufferIndex bufferIndex )
  {
    DALI_ASSERT_ALWAYS( mGfxRenderCommand != nullptr );
    return *mGfxRenderCommand.get();
  }

  void BindPipeline( std::unique_ptr<Graphics::Pipeline> pipeline )
  {
    mGfxPipeline = std::move(pipeline); //@query
    mGfxRenderCommand->BindPipeline( *mGfxPipeline.get() );
  }

  // std::unique_ptr<Graphics::Pipeline> ReleaseGraphicsPipeline( BufferIndex updateBufferIndex )
  // {
  //   return std::move( mGfxPipeline );
  //}

  void BindTextures( std::vector<Graphics::TextureBinding>& textureBindings )
  {
    if( mGfxRenderCommand )
    {
      mGfxRenderCommand->BindTextures( &textureBindings );
    }
  }

  std::unique_ptr<Graphics::CommandBuffer> mGfxRenderCommand;
  std::unique_ptr<Graphics::Pipeline> mGfxPipeline;
  std::vector<Graphics::UniformBufferBinding> mUboBindings;
};

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif //DALI_INTERNAL_SCENEGRAPH_RENDER_COMMAND_H_H
