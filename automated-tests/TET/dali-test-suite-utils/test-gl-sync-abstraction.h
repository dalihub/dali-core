#ifndef __TEST_GL_SYNC_ABSTRACTION_H__
#define __TEST_GL_SYNC_ABSTRACTION_H__

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

// EXTERNAL INCLUDES
#include <sstream>
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/common/map-wrapper.h>
#include <dali/public-api/dali-core.h>
#include <dali/integration-api/core.h>
#include <dali/integration-api/gl-sync-abstraction.h>
#include <test-trace-call-stack.h>

namespace Dali
{

class DALI_IMPORT_API TestSyncObject : public Integration::GlSyncAbstraction::SyncObject
{
public:
  TestSyncObject(TraceCallStack& trace)
  : synced(false),
    mTrace(trace)
  {
  }

  virtual ~TestSyncObject()
  {
  }

  virtual bool IsSynced()
  {
    mTrace.PushCall("SyncObject::IsSynced", ""); // Trace the method
    return synced;
  }
  bool synced;
  TraceCallStack& mTrace;
};

/**
 * Class to emulate the GL sync functions with tracing
 */
class DALI_IMPORT_API TestGlSyncAbstraction: public Integration::GlSyncAbstraction
{
public:
  /**
   * Constructor
   */
  TestGlSyncAbstraction()
  {
    Initialize();
  }

  /**
   * Destructor
   */
  ~TestGlSyncAbstraction()
  {
    for( SyncIter iter=mSyncObjects.begin(), end=mSyncObjects.end(); iter != end; ++iter )
    {
      delete *iter;
    }
  }

  /**
   * Initialize the sync objects - clear down the map
   */
  void Initialize()
  {
    mSyncObjects.clear();
  }

  /**
   * Create a sync object
   * @return the sync object
   */
  virtual Integration::GlSyncAbstraction::SyncObject* CreateSyncObject( )
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
  virtual void DestroySyncObject( Integration::GlSyncAbstraction::SyncObject* syncObject )
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


public: // TEST FUNCTIONS
  Integration::GlSyncAbstraction::SyncObject* GetLastSyncObject( )
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
  void SetObjectSynced( Integration::GlSyncAbstraction::SyncObject* syncObject, bool sync )
  {
    TestSyncObject* testSyncObject = static_cast<TestSyncObject*>(syncObject);
    testSyncObject->synced = sync;
  }

  /**
   * Turn trace on
   */
  void EnableTrace(bool enable) { mTrace.Enable(enable); }

  /**
   * Reset the trace callstack
   */
  void ResetTrace() { mTrace.Reset(); }

  /**
   * Get the trace object (allows test case to find methods on it)
   */
  TraceCallStack& GetTrace() { return mTrace; }

private:
  typedef std::vector<TestSyncObject*>   SyncContainer;
  typedef SyncContainer::iterator SyncIter;
  SyncContainer mSyncObjects;  ///< The sync objects
  TraceCallStack mTrace; ///< the trace call stack for testing
};

} // Dali

#endif // __TEST_GL_SYNC_ABSTRACTION_H__
