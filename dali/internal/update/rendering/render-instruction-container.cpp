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

// CLASS HEADER
#include <dali/internal/update/rendering/render-instruction-container.h>

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include <dali/internal/update/rendering/render-instruction.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

RenderInstructionContainer::RenderInstructionContainer()
: mInstructions{}
{
}

RenderInstructionContainer::~RenderInstructionContainer()
{
}

void RenderInstructionContainer::ResetAndReserve( BufferIndex bufferIndex, uint32_t capacityRequired )
{
  // Only re-allocate if necessary.
  uint32_t oldCapacity = static_cast<uint32_t>( mInstructions[ bufferIndex ].size() ); // uint32_t is large enough in practice
  if( oldCapacity < capacityRequired )
  {
    mInstructions[bufferIndex].reserve( capacityRequired );
  }
  mInstructions[bufferIndex].clear();
}

uint32_t RenderInstructionContainer::Count( BufferIndex bufferIndex ) const
{
  return static_cast<uint32_t>( mInstructions[bufferIndex].size() );
}

RenderInstruction& RenderInstructionContainer::At( BufferIndex bufferIndex, uint32_t index )
{
  DALI_ASSERT_DEBUG( index < mInstructions[bufferIndex].size() );

  return *mInstructions[bufferIndex][ index ];
}

const RenderInstruction& RenderInstructionContainer::At( BufferIndex bufferIndex, uint32_t index ) const
{
  DALI_ASSERT_DEBUG( index < mInstructions[bufferIndex].size() );

  return *mInstructions[bufferIndex][ index ];
}

void RenderInstructionContainer::PushBack( BufferIndex index, RenderInstruction* renderInstruction )
{
  mInstructions[index].push_back( renderInstruction );
}

void RenderInstructionContainer::DiscardCurrentInstruction( BufferIndex updateBufferIndex )
{
  mInstructions[updateBufferIndex].pop_back();
}

void RenderInstructionContainer::Shutdown()
{
  for( auto instructionPtr : mInstructions[0] )
  {
    instructionPtr->Shutdown();
  }
  for( auto instructionPtr : mInstructions[1] )
  {
    instructionPtr->Shutdown();
  }
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
