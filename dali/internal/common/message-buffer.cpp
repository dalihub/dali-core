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
#include <dali/internal/common/message-buffer.h>

// EXTERNAL INCLUDES
#include <limits>
#include <cstdlib>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>

namespace // unnamed namespace
{

// Increase capacity by 1.5 when buffer limit reached
const uint32_t INCREMENT_NUMERATOR   = 3u;
const uint32_t INCREMENT_DENOMINATOR = 2u;

const uint32_t MESSAGE_SIZE_FIELD = 1u; // Size required to mark the message size
const uint32_t MESSAGE_END_FIELD  = 1u; // Size required to mark the end of messages

const uint32_t MESSAGE_SIZE_PLUS_END_FIELD = MESSAGE_SIZE_FIELD + MESSAGE_END_FIELD;

const std::size_t MAX_DIVISION_BY_WORD_REMAINDER = sizeof(Dali::Internal::MessageBuffer::WordType) - 1u; // For word alignment on ARM
const std::size_t WORD_SIZE = sizeof(Dali::Internal::MessageBuffer::WordType);

} // unnamed namespace

namespace Dali
{

namespace Internal
{

MessageBuffer::MessageBuffer( std::size_t initialCapacity )
: mInitialCapacity( initialCapacity / WORD_SIZE ),
  mData( NULL ),
  mNextSlot( NULL ),
  mCapacity( 0 ),
  mSize( 0 )
{
}

MessageBuffer::~MessageBuffer()
{
  free( mData );
}

uint32_t* MessageBuffer::ReserveMessageSlot( std::size_t size )
{
  DALI_ASSERT_DEBUG( 0 != size );

  // Number of aligned words required to handle a message of size in bytes
  std::size_t requestedSize = (size + MAX_DIVISION_BY_WORD_REMAINDER) / WORD_SIZE;
  std::size_t requiredSize = requestedSize + MESSAGE_SIZE_PLUS_END_FIELD;

  // Keep doubling the additional capacity until we have enough
  std::size_t nextCapacity = mCapacity ? mCapacity : mInitialCapacity;

  if ( (nextCapacity - mSize) < requiredSize )
  {
    nextCapacity = nextCapacity * INCREMENT_NUMERATOR / INCREMENT_DENOMINATOR;

    // Something has gone badly wrong if requiredSize is this big
    DALI_ASSERT_DEBUG( (nextCapacity - mSize) > requiredSize );
  }

  if ( nextCapacity > mCapacity )
  {
    IncreaseCapacity( nextCapacity );
  }

  // Now reserve the slot
  WordType* slot = mNextSlot;

  *slot++ = requestedSize; // Object size marker is stored in first word

  mSize += requestedSize + MESSAGE_SIZE_FIELD;
  mNextSlot = mData + mSize;

  // End marker
  *mNextSlot = 0;

  return reinterpret_cast<uint32_t*>(slot);
}

std::size_t MessageBuffer::GetCapacity() const
{
  return mCapacity * WORD_SIZE;
}

MessageBuffer::Iterator MessageBuffer::Begin() const
{
  if ( 0 != mSize )
  {
    return Iterator( mData );
  }

  return Iterator( NULL );
}

void MessageBuffer::Reset()
{
  // All messages have been processed, reset the buffer
  mSize = 0;
  mNextSlot = mData;
}

void MessageBuffer::IncreaseCapacity( std::size_t newCapacity )
{
  DALI_ASSERT_DEBUG( newCapacity > mCapacity );

  if ( mData )
  {
    // Often this avoids the need to copy memory

    WordType* oldData = mData;
    mData = reinterpret_cast<WordType*>( realloc( mData, newCapacity * WORD_SIZE ) );

    // if realloc fails the old data is still valid
    if( !mData )
    {
      // TODO: Process message queue to free up some data?
      free(oldData);
      DALI_ASSERT_DEBUG( false && "Realloc failed we're out of memory!" );
    }
  }
  else
  {
    mData = reinterpret_cast<WordType*>( malloc( newCapacity * WORD_SIZE ) );
  }
  DALI_ASSERT_ALWAYS( NULL != mData );

  mCapacity = newCapacity;
  mNextSlot = mData + mSize;
}

MessageBuffer::Iterator::Iterator(WordType* current)
: mCurrent(current),
  mMessageSize(0)
{
  if( NULL != mCurrent )
  {
    // The first word is the size of the following object
    mMessageSize = *mCurrent++;
  }
}

MessageBuffer::Iterator::Iterator(const Iterator& copy)
: mCurrent( copy.mCurrent ),
  mMessageSize( copy.mMessageSize )
{
}

} // namespace Internal

} // namespace Dali
