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
#include "lockless-buffer.h"

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Integration
{

LocklessBuffer::LocklessBuffer( uint32_t size )
: mState( R0W1 ),
  mSize( size )
{
  // allocate memory to speed up operation
  mBuffer[0] = new uint8_t[size];
  mBuffer[1] = new uint8_t[size];

  memset (mBuffer[0], 0, size );
  memset (mBuffer[1], 0, size );
}

LocklessBuffer::~LocklessBuffer()
{
  delete[] mBuffer[0];
  delete[] mBuffer[1];
}

void LocklessBuffer::Write( const uint8_t *src, uint32_t size )
{
  DALI_ASSERT_ALWAYS( size <= mSize );

  // set WRITING bit
  BufferState currentState( __sync_fetch_and_or( &mState, WRITING ) );
  DALI_ASSERT_DEBUG( !(currentState & WRITING_MASK) ); // WRITING bit should never be set when we get here

  // copy data to current write buffer, negate state to get actual index (recap: R0W1 = 0, R1W0 = 1)
  const unsigned int index = (currentState & WRITE_BUFFER_MASK);
  memcpy( mBuffer[index], src, size );

  // unset WRITING bit, set UPDATED bit
  BufferState checkState = __sync_val_compare_and_swap( &mState,
                                                        static_cast<BufferState>(currentState | WRITING),
                                                        static_cast<BufferState>(index | UPDATED) );

  DALI_ASSERT_DEBUG( checkState & WRITING );
  (void)checkState; // Avoid unused variable warning
}

const uint8_t* LocklessBuffer::Read()
{
  // current state (only to avoid multiple memory reads with volatile variable)
  BufferState currentState( mState );
  BufferState currentWriteBuf( static_cast<BufferState>( currentState & WRITE_BUFFER_MASK ) );

  if( currentState & UPDATED_MASK )
  {
    // Try to swap buffers.
    // This will set mState to 1 if readbuffer 0 was updated, 0 if readbuffer 1 was updated and fail if WRITING is set
    if( __sync_bool_compare_and_swap( &mState,
                                      static_cast<BufferState>(currentWriteBuf | UPDATED),
                                      static_cast<BufferState>(!currentWriteBuf) )  )
    {
      // swap successful
      return mBuffer[currentWriteBuf];
    }
  }

  // UPDATE bit wasn't set or WRITING bit was set in other thread
  // swap failed, read from current readbuffer
  return mBuffer[!currentWriteBuf];
}

uint32_t LocklessBuffer::GetSize() const
{
  return static_cast<unsigned int>(mSize);
}

} // Internal

} // Dali
