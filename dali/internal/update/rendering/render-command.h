#ifndef DALI_INTERNAL_SCENEGRAPH_RENDER_COMMAND_H
#define DALI_INTERNAL_SCENEGRAPH_RENDER_COMMAND_H

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
 */

#include <dali/internal/common/buffer-index.h>
#include <dali/graphics-api/graphics-api-render-command.h>
#include <dali/graphics-api/graphics-api-pipeline.h>
#include <dali/graphics-api/graphics-api-controller.h>
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
  : mGfxRenderCommand{ nullptr, nullptr },
    mGfxPipeline{ nullptr, nullptr },
    mUboBindings{ {}, {} }
  {
  }

  Graphics::API::RenderCommand& AllocateGfxRenderCommand( Graphics::API::Controller& controller,
                                                          BufferIndex updateBufferIndex )
  {
    if (!mGfxRenderCommand[updateBufferIndex])
    {
      mGfxRenderCommand[updateBufferIndex] = controller.AllocateRenderCommand();
    }
    return *mGfxRenderCommand[updateBufferIndex].get();
  }

  Graphics::API::RenderCommand& GetGfxRenderCommand( BufferIndex bufferIndex )
  {
    DALI_ASSERT_ALWAYS( mGfxRenderCommand[bufferIndex] != nullptr );
    return *mGfxRenderCommand[bufferIndex].get();
  }

  void BindPipeline( std::unique_ptr<Graphics::API::Pipeline> pipeline, BufferIndex updateBufferIndex )
  {
    mGfxPipeline[updateBufferIndex] = std::move(pipeline);
    mGfxRenderCommand[updateBufferIndex]->BindPipeline( mGfxPipeline[updateBufferIndex].get() );
  }

  std::unique_ptr<Graphics::API::Pipeline> ReleaseGraphicsPipeline( BufferIndex updateBufferIndex )
  {
    return std::move( mGfxPipeline[updateBufferIndex] );
  }

  void BindTextures( std::vector<Graphics::API::RenderCommand::TextureBinding>& textureBindings,
                     BufferIndex updateBufferIndex )
  {
    if( !mGfxRenderCommand[updateBufferIndex]->GetTextureBindings() )
    {
      mGfxRenderCommand[updateBufferIndex]->BindTextures( &textureBindings );
    }
  }

  void BindTextures( std::vector<Graphics::API::RenderCommand::TextureBinding>& textureBindings )
  {
    for( auto& cmd : mGfxRenderCommand ) // For both buffers
    {
      if( cmd )
      {
        cmd->BindTextures( &textureBindings );
      }
    }
  }

  std::unique_ptr<Graphics::API::RenderCommand> mGfxRenderCommand[ MAX_GRAPHICS_DATA_BUFFER_COUNT ];
  std::unique_ptr<Graphics::API::Pipeline> mGfxPipeline[ MAX_GRAPHICS_DATA_BUFFER_COUNT ];
  std::vector<Graphics::API::RenderCommand::UniformBufferBinding> mUboBindings[ MAX_GRAPHICS_DATA_BUFFER_COUNT ];
};

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif //DALI_INTERNAL_SCENEGRAPH_RENDER_COMMAND_H_H
