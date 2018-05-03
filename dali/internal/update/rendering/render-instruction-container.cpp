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

// INTERNAL INCLUDES
#include <dali/internal/update/rendering/render-instruction.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

RenderInstructionContainer::RenderInstructionContainer()
{
  // array initialisation in ctor initializer list not supported until C++ 11
  mIndex[ 0 ] = 0u;
  mIndex[ 1 ] = 0u;
}

RenderInstructionContainer::~RenderInstructionContainer()
{
  // OwnerContainer deletes the instructions
}

void RenderInstructionContainer::ResetAndReserve( BufferIndex bufferIndex, size_t capacityRequired )
{
  mIndex[ bufferIndex ] = 0u;
  size_t oldcapacity = mInstructions[ bufferIndex ].Capacity();
  if( oldcapacity < capacityRequired )
  {
    mInstructions[ bufferIndex ].Reserve( capacityRequired );
    // add N new elements
    for( ; oldcapacity < capacityRequired; ++oldcapacity )
    {
      mInstructions[ bufferIndex ].PushBack( new RenderInstruction );
    }
  }
  // Note that we may have spare elements in the list, we don't remove them as that would
  // decrease the capacity of our container and lead to possibly reallocating, which we hate
  // RenderInstruction holds a lot of data so we keep them and recycle instead of new & delete
}

size_t RenderInstructionContainer::Count( BufferIndex bufferIndex )
{
  // mIndex contains the number of instructions that have been really prepared and updated
  // (from UpdateManager through GetNextInstruction)
  return mIndex[ bufferIndex ];
}

RenderInstruction& RenderInstructionContainer::GetNextInstruction( BufferIndex bufferIndex )
{
  // At protects against running out of space
  return At( bufferIndex, mIndex[ bufferIndex ]++ );
}

RenderInstruction& RenderInstructionContainer::At( BufferIndex bufferIndex, size_t index )
{
  DALI_ASSERT_DEBUG( index < mInstructions[ bufferIndex ].Count() );

  return *mInstructions[ bufferIndex ][ index ];
}


} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
