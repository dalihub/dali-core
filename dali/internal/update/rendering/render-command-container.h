#ifndef DALI_INTERNAL_SCENE_GRAPH_RENDER_COMMAND_CONTAINER_H
#define DALI_INTERNAL_SCENE_GRAPH_RENDER_COMMAND_CONTAINER_H

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

#include <dali/internal/update/rendering/render-command.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class RenderInstruction;

struct IndexedRenderCommand
{
  IndexedRenderCommand()
  : renderInstruction(nullptr),
    mRenderCommand{new RenderCommand()},
    updateBufferIndex(0)
  {
  }

  const RenderInstruction* renderInstruction; // Key
  std::unique_ptr<RenderCommand> mRenderCommand;
  BufferIndex updateBufferIndex;
};

class RenderCommandContainer
{
public:

  RenderCommand& AllocRenderCommand( RenderInstruction* renderInstruction,
                                     Graphics::Controller& controller,
                                     BufferIndex updateBufferIndex )
  {
    RenderCommand* renderCommand;
    if( !Find( renderInstruction, renderCommand, updateBufferIndex ) )
    {
      IndexedRenderCommand indexedRenderCommand;
      indexedRenderCommand.renderInstruction = renderInstruction;
      indexedRenderCommand.mRenderCommand = std::make_unique< RenderCommand >();
      indexedRenderCommand.updateBufferIndex = updateBufferIndex;

      mRenderCommands.emplace_back( std::move( indexedRenderCommand ) );
      renderCommand = mRenderCommands.back().mRenderCommand.get();
    }
    // renderCommand is set to the render command. Ensure the graphics render command is also allocated:
    renderCommand->AllocateGfxRenderCommand( controller, updateBufferIndex );
    return *renderCommand;
  }

  RenderCommand& GetRenderCommand( RenderInstruction* renderInstruction,
                                   BufferIndex updateBufferIndex )
  {
    RenderCommand* renderCommand;
    if( !Find( renderInstruction, renderCommand, updateBufferIndex ) )
    {
      DALI_ASSERT_DEBUG( 0 && "Can't find render command for render instruction" );
    }

    return *renderCommand;
  }

  void BindTextures( std::vector<Graphics::TextureBinding>& textureBindings )
  {
    for( auto& elem : mRenderCommands )
    {
      elem.mRenderCommand->BindTextures( textureBindings );
    }
  }

  void ClearUniformBindings()
  {
    for( auto& elem : mRenderCommands )
    {
      elem.mRenderCommand->mUboBindings.clear();
    }
  }

  void DestroyRenderCommand( RenderInstruction* renderInstruction )
  {
    for( auto iter = mRenderCommands.begin(); iter != mRenderCommands.end() ; ++iter )
    {
      if( iter->renderInstruction == renderInstruction )
      {
        mRenderCommands.erase(iter);
        break;
      }
    }
  }

  /**
   * Find a render command for the given render instruction
   * @param[in] renderInstruction
   * @return An iterator to the requested render command, or the end() of the vector
   */
  bool Find( RenderInstruction* renderInstruction, RenderCommand*& renderCommand, BufferIndex updateBufferIndex )
  {
    for( auto& element : mRenderCommands )
    {
      if( element.renderInstruction == renderInstruction && element.updateBufferIndex == updateBufferIndex )
      {
        renderCommand = element.mRenderCommand.get();
        return true;
      }
    }

    renderCommand = nullptr;
    return false;
  }

  void DestroyAll()
  {
    mRenderCommands.clear();
  }

private:
  std::vector<IndexedRenderCommand> mRenderCommands;
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif //DALI_INTERNAL_SCENE_GRAPH_RENDER_COMMAND_CONTAINER_H
