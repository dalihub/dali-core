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
#include <dali/public-api/common/dali-vector.h>

// EXTERNAL INCLUDES
#include <cstring> // for memcpy & memmove

namespace Dali
{

VectorBase::VectorBase()
  : mData( NULL )
{
}

VectorBase::~VectorBase()
{
}

VectorBase::SizeType VectorBase::Capacity() const
{
  SizeType capacity = 0u;
  if( mData )
  {
    SizeType* metadata = reinterpret_cast< SizeType* >( mData );
    capacity = *(metadata - 2u);
  }
  return capacity;
}


void VectorBase::Release()
{
  if( mData )
  {
    // adjust pointer to real beginning
    SizeType* metadata = reinterpret_cast< SizeType* >( mData );

    delete [] ( metadata - 2u );
    mData = 0u;
  }
}

void VectorBase::SetCount( SizeType count )
{
  // someone can call Resize( 0u ) before ever populating the vector
  if( mData )
  {
    SizeType* metadata = reinterpret_cast< SizeType* >( mData );
    *(metadata - 1u) = count;
  }
}

void VectorBase::Reserve( SizeType capacity, SizeType elementSize )
{
  SizeType oldCapacity = Capacity();
  SizeType oldCount = Count();
  if( capacity > oldCapacity )
  {
    const SizeType wholeAllocation = sizeof(SizeType) * 2u + capacity * elementSize;
    void* wholeData = reinterpret_cast< void* >( new uint8_t[ wholeAllocation ] );
    DALI_ASSERT_ALWAYS( wholeData && "VectorBase::Reserve - Memory allocation failed" );

#if defined( DEBUG_ENABLED )
    // in debug build this will help identify a vector of uninitialized data
    memset( wholeData, 0xaa, wholeAllocation );
#endif
    SizeType* metaData = reinterpret_cast< SizeType* >( wholeData );
    *metaData++ = capacity;
    *metaData++ = oldCount;
    if( mData )
    {
      // copy over the old data
      memcpy( metaData, mData, oldCount * elementSize );
      // release old buffer
      Release();
    }
    mData = metaData;
  }
}

void VectorBase::Copy( const VectorBase& vector, SizeType elementSize )
{
  // release old data
  Release();
  // reserve space based on source capacity
  const SizeType capacity = vector.Capacity();
  Reserve( capacity, elementSize );
  // copy over whole data
  const SizeType wholeAllocation = sizeof(SizeType) * 2u + capacity * elementSize;
  SizeType* srcData = reinterpret_cast< SizeType* >( vector.mData );
  SizeType* dstData = reinterpret_cast< SizeType* >( mData );
  memcpy( dstData - 2u, srcData - 2u, wholeAllocation );
}

void VectorBase::Swap( VectorBase& vector )
{
  // just swap the data pointers, metadata will swap as side effect
  std::swap( mData, vector.mData );
}

void VectorBase::Erase( uint8_t* address, SizeType elementSize )
{
  // erase can be called on an unallocated vector
  if( mData )
  {
    uint8_t* startAddress = address + elementSize;
    const uint8_t* endAddress = reinterpret_cast< uint8_t* >( mData ) + Count() * elementSize;
    SizeType numberOfBytes = endAddress - startAddress;
    // addresses overlap so use memmove
    memmove( address, startAddress, numberOfBytes );
    SetCount( Count() - 1u );
  }
}

uint8_t* VectorBase::Erase( uint8_t* first, uint8_t* last, SizeType elementSize )
{
  uint8_t* next = NULL;

  if( mData )
  {
    uint8_t* startAddress = last;
    const uint8_t* endAddress = reinterpret_cast< uint8_t* >( mData ) + Count() * elementSize;
    SizeType numberOfBytes = endAddress - startAddress;
    // addresses overlap so use memmove
    memmove( first, startAddress, numberOfBytes );
    SetCount( Count() - ( last - first ) / elementSize );

    next = first;
  }

  return next;
}

void VectorBase::CopyMemory( uint8_t* destination, const uint8_t* source, size_t numberOfBytes )
{
  if( ( ( source < destination ) && ( source + numberOfBytes > destination ) ) ||
      ( ( destination < source ) && ( destination + numberOfBytes > source ) ) )
  {
    // If there is overlap, use memmove.
    memmove( destination, source, numberOfBytes );
  }
  else
  {
    // It's safe to use memcpy if there isn't overlap.
    memcpy( destination, source, numberOfBytes );
  }
}

} // namespace Dali

