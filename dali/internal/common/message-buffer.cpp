/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
const unsigned int INCREMENT_NUMERATOR   = 3u;
const unsigned int INCREMENT_DENOMINATOR = 2u;

const unsigned int MESSAGE_SIZE_FIELD = 1u; // Size required to mark the message size
const unsigned int MESSAGE_END_FIELD  = 1u; // Size required to mark the end of messages

const unsigned int MESSAGE_SIZE_PLUS_END_FIELD = MESSAGE_SIZE_FIELD + MESSAGE_END_FIELD;

const unsigned int MAX_DIVISION_BY_WORD_REMAINDER = sizeof(unsigned int) - 1u; // For word alignment on ARM

} // unnamed namespace

namespace Dali
{

namespace Internal
{

MessageBuffer::MessageBuffer( std::size_t initialCapacity )
: mInitialCapacity( initialCapacity / sizeof(unsigned int) ),
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

unsigned int* MessageBuffer::ReserveMessageSlot( std::size_t size )
{
  DALI_ASSERT_DEBUG( 0 != size );

  std::size_t requestedSize = (size + MAX_DIVISION_BY_WORD_REMAINDER) / sizeof(unsigned int);
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
  unsigned int* slot = mNextSlot;

  *slot++ = requestedSize; // Object size marker is stored in first word

  mSize += requestedSize + MESSAGE_SIZE_FIELD;
  mNextSlot = mData + mSize;

  // End marker
  *mNextSlot = 0;

  return slot;
}

std::size_t MessageBuffer::GetCapacity() const
{
  return mCapacity * sizeof(unsigned int);
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

    unsigned int* oldData = mData;
    mData = reinterpret_cast<unsigned int*>( realloc( mData, newCapacity * sizeof(unsigned int) ) );

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
    mData = reinterpret_cast<unsigned int*>( malloc( newCapacity * sizeof(unsigned int) ) );
  }
  DALI_ASSERT_ALWAYS( NULL != mData );

  mCapacity = newCapacity;
  mNextSlot = mData + mSize;
}

MessageBuffer::Iterator::Iterator(unsigned int* current)
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
