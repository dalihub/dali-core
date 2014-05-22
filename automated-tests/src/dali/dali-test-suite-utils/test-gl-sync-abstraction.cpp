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

#include "test-gl-sync-abstraction.h"

namespace Dali
{

TestSyncObject::TestSyncObject(TraceCallStack& trace)
: synced(false),
  mTrace(trace)
{
}

TestSyncObject::~TestSyncObject()
{
}

bool TestSyncObject::IsSynced()
{
  mTrace.PushCall("SyncObject::IsSynced", ""); // Trace the method
  return synced;
}



TestGlSyncAbstraction::TestGlSyncAbstraction()
{
  Initialize();
}

/**
 * Destructor
 */
TestGlSyncAbstraction::~TestGlSyncAbstraction()
{
  for( SyncIter iter=mSyncObjects.begin(), end=mSyncObjects.end(); iter != end; ++iter )
  {
    delete *iter;
  }
}

/**
 * Initialize the sync objects - clear down the map
 */
void TestGlSyncAbstraction::Initialize()
{
  mSyncObjects.clear();
}

/**
 * Create a sync object
 * @return the sync object
 */
Integration::GlSyncAbstraction::SyncObject* TestGlSyncAbstraction::CreateSyncObject( )
{
  mTrace.PushCall("CreateSyncObject", ""); // Trace the method

  TestSyncObject* syncObject = new TestSyncObject(mTrace);
  mSyncObjects.push_back( syncObject );
  return syncObject;
}

/**
 * Destroy a sync object
 * @param[in] syncObject The object to destroy
 */
void TestGlSyncAbstraction::DestroySyncObject( Integration::GlSyncAbstraction::SyncObject* syncObject )
{
  std::stringstream out;
  out << syncObject;
  mTrace.PushCall("DestroySyncObject", out.str()); // Trace the method

  for( SyncIter iter=mSyncObjects.begin(), end=mSyncObjects.end(); iter != end; ++iter )
  {
    if( *iter == syncObject )
    {
      delete *iter;
      mSyncObjects.erase(iter);
      break;
    }
  }
}


Integration::GlSyncAbstraction::SyncObject* TestGlSyncAbstraction::GetLastSyncObject( )
{
  if( !mSyncObjects.empty() )
  {
    return mSyncObjects.back();
  }
  return NULL;
}

/**
 * Test method to trigger the object sync behaviour.
 * @param[in]
 * @param[in] sync The sync value to set
 */
void TestGlSyncAbstraction::SetObjectSynced( Integration::GlSyncAbstraction::SyncObject* syncObject, bool sync )
{
  TestSyncObject* testSyncObject = static_cast<TestSyncObject*>(syncObject);
  testSyncObject->synced = sync;
}

/**
 * Turn trace on
 */
void TestGlSyncAbstraction::EnableTrace(bool enable) { mTrace.Enable(enable); }

/**
 * Reset the trace callstack
 */
void TestGlSyncAbstraction::ResetTrace() { mTrace.Reset(); }

/**
 * Get the trace object (allows test case to find methods on it)
 */
TraceCallStack& TestGlSyncAbstraction::GetTrace() { return mTrace; }


} // Dali
