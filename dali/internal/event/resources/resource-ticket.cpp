//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// CLASS HEADER
#include <dali/internal/event/resources/resource-ticket.h>

// EXTERNAL INCLUDES
#include <algorithm>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/images/image.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/resources/resource-ticket-observer.h>
#include <dali/internal/event/resources/resource-ticket-lifetime-observer.h>

using namespace std;

namespace // unnamed namespace
{

// predicate for std::remove_if algorithm for compressing observer list
bool isNULL(void* ptr)
{
  return ptr == NULL;
}

} // unnamed namespace

namespace Dali
{

namespace Internal
{

ResourceTicket::ResourceTicket( ResourceTicketLifetimeObserver& observer, unsigned int id, ResourceTypePath& typePath )
: mLifetimeObserver( &observer ),
  mId( id ),
  mTypePath( typePath ),
  mLoadingState( ResourceLoading )
{
}

ResourceTicket::~ResourceTicket()
{
  // Guard to allow handle destruction after Core has been destroyed
  if ( Stage::IsInstalled() &&
       mLifetimeObserver )
  {
    mLifetimeObserver->ResourceTicketDiscarded( *this );
  }
}

void ResourceTicket::AddObserver(ResourceTicketObserver& observer)
{
  mObservers.push_back(&observer);
}

void ResourceTicket::RemoveObserver(ResourceTicketObserver& observer)
{
  ObserverIter iter = find(mObservers.begin(), mObservers.end(), &observer);
  DALI_ASSERT_DEBUG( mObservers.end() != iter );
  if( mObservers.end() != iter )
  {
    (*iter) = NULL;
  }
}

void ResourceTicket::LoadingSucceeded()
{
  DALI_ASSERT_DEBUG(mLoadingState == ResourceLoading);

  mLoadingState = ResourceLoadingSucceeded;

  // Using array operator as the call back out to application code might call back in
  // and corrupt the mObservers list. Presumption is the operator gets the current address
  // and adds an offset so a push_back() triggered reallocation should still work.
  size_t count = mObservers.size();
  for(size_t i = 0; i < count; i++)
  {
    if( mObservers[i] != NULL)
    {
      mObservers[i]->ResourceLoadingSucceeded(*this);
    }
  }

  // Move NULL pointers to the end...
  ObserverIter endIter = remove_if( mObservers.begin(), mObservers.end(), isNULL );

  // ...and remove them
  mObservers.erase( endIter, mObservers.end() );
}

void ResourceTicket::LoadingFailed()
{
  DALI_ASSERT_DEBUG(mLoadingState == ResourceLoading);

  mLoadingState = ResourceLoadingFailed;

  // Using array operator as the call back out to application code might call back in
  // and corrupt the mObservers list. Presumption is the operator gets the current address
  // and adds an offset so a push_back() triggered reallocation should still work.
  size_t count = mObservers.size();
  for(size_t i = 0; i < count; i++)
  {
    if( mObservers[i] != NULL)
    {
      mObservers[i]->ResourceLoadingFailed(*this);
    }
  }

  // Move NULL pointers to the end...
  ObserverIter endIter = remove_if( mObservers.begin(), mObservers.end(), isNULL );

  // ...and remove them
  mObservers.erase( endIter, mObservers.end() );
}

void ResourceTicket::Loading()
{
  mLoadingState = ResourceLoading;

  // Move NULL pointers to the end...
  ObserverIter endIter = remove_if( mObservers.begin(), mObservers.end(), isNULL );

  // ...and remove them
  mObservers.erase( endIter, mObservers.end() );
}

void ResourceTicket::Uploaded()
{
  // Using array operator as the call back out to application code might call back in
  // and corrupt the mObservers list. Presumption is the operator gets the current address
  // and adds an offset so a push_back() triggered reallocation should still work.
  size_t count = mObservers.size();
  for(size_t i = 0; i < count; i++)
  {
    if( mObservers[i] != NULL)
    {
      mObservers[i]->ResourceUploaded(*this);
    }
  }

  // Move NULL pointers to the end...
  ObserverIter endIter = remove_if( mObservers.begin(), mObservers.end(), isNULL );

  // ...and remove them
  mObservers.erase( endIter, mObservers.end() );
}

void ResourceTicket::SavingSucceeded()
{
  // Using array operator as the call back out to application code might call back in
  // and corrupt the mObservers list. Presumption is the operator gets the current address
  // and adds an offset so a push_back() triggered reallocation should still work.
  size_t count = mObservers.size();
  for(size_t i = 0; i < count; i++)
  {
    if( mObservers[i] != NULL)
    {
      mObservers[i]->ResourceSavingSucceeded(*this);
    }
  }

  // Move NULL pointers to the end...
  ObserverIter endIter = remove_if( mObservers.begin(), mObservers.end(), isNULL );

  // ...and remove them
  mObservers.erase( endIter, mObservers.end() );
}

void ResourceTicket::SavingFailed()
{
  // Using array operator as the call back out to application code might call back in
  // and corrupt the mObservers list. Presumption is the operator gets the current address
  // and adds an offset so a push_back() triggered reallocation should still work.
  size_t count = mObservers.size();
  for(size_t i = 0; i < count; i++)
  {
    if( mObservers[i] != NULL)
    {
      mObservers[i]->ResourceSavingFailed(*this);
    }
  }

  // Move NULL pointers to the end...
  ObserverIter endIter = remove_if( mObservers.begin(), mObservers.end(), isNULL );

  // ...and remove them
  mObservers.erase( endIter, mObservers.end() );
}

void ResourceTicket::StopLifetimeObservation()
{
  mLifetimeObserver = NULL;
}

} // namespace Internal

} // namespace Dali

