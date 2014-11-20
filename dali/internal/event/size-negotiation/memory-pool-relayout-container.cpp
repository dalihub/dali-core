/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

// FILE HEADER
#include "memory-pool-relayout-container.h"

namespace Dali
{

namespace Internal
{

MemoryPoolRelayoutContainer::MemoryPoolRelayoutContainer( MemoryPoolObjectAllocator< RelayoutInfo >& objectAllocator )
: mAllocator( objectAllocator )
{
}

MemoryPoolRelayoutContainer::~MemoryPoolRelayoutContainer()
{
}

bool MemoryPoolRelayoutContainer::Contains( const Dali::Actor& actor )
{
  for( RelayoutInfoContainer::Iterator it = mRelayoutInfos.Begin(), itEnd = mRelayoutInfos.End(); it != itEnd; ++it )
  {
    RelayoutInfo* info = *it;

    if( info->actor == actor )
    {
      return true;
    }
  }

  return false;
}

void MemoryPoolRelayoutContainer::Add( const Dali::Actor& actor, const Vector2& size )
{
  if( !Contains( actor ) )
  {
    RelayoutInfo* info = mAllocator.Allocate();
    info->actor = actor;
    info->size = size;

    mRelayoutInfos.PushBack( info );
  }
}

void MemoryPoolRelayoutContainer::PopBack()
{
  if( mRelayoutInfos.Size() > 0 )
  {
    RelayoutInfoContainer::Iterator back = mRelayoutInfos.End();
    back--;
    RelayoutInfo* info = *back;
    mAllocator.Free( info );
    mRelayoutInfos.Erase( back );
  }
}

void MemoryPoolRelayoutContainer::Get( size_t index, Dali::Actor& actorOut, Vector2& sizeOut  ) const
{
  DALI_ASSERT_DEBUG( index < Size() );

  RelayoutInfo* info = mRelayoutInfos[ index ];
  actorOut = info->actor;
  sizeOut = info->size;
}

size_t MemoryPoolRelayoutContainer::Size() const
{
  return mRelayoutInfos.Size();
}

void MemoryPoolRelayoutContainer::Reserve( size_t capacity )
{
  mRelayoutInfos.Reserve( capacity );
}

void MemoryPoolRelayoutContainer::Clear()
{
  for( size_t i = 0; i < Size(); ++i )
  {
    RelayoutInfo* info = mRelayoutInfos[ i ];
    mAllocator.Free( info );
  }

  mRelayoutInfos.Clear();
}

} // namespace Internal

} // namespace Dali
