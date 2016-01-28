#ifndef __TEST_GL_SYNC_ABSTRACTION_H__
#define __TEST_GL_SYNC_ABSTRACTION_H__

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

// EXTERNAL INCLUDES
#include <sstream>
#include <string>
#include <map>

// INTERNAL INCLUDES
#include <dali/integration-api/core.h>
#include <dali/integration-api/gl-sync-abstraction.h>
#include "test-trace-call-stack.h"

namespace Dali
{

class DALI_IMPORT_API TestSyncObject : public Integration::GlSyncAbstraction::SyncObject
{
public:
  TestSyncObject(TraceCallStack& trace);
  ~TestSyncObject();
  bool IsSynced();
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
  TestGlSyncAbstraction();

  /**
   * Destructor
   */
  ~TestGlSyncAbstraction();

  /**
   * Initialize the sync objects - clear down the map
   */
  void Initialize();

  /**
   * Create a sync object
   * @return the sync object
   */
  virtual Integration::GlSyncAbstraction::SyncObject* CreateSyncObject( );

  /**
   * Destroy a sync object
   * @param[in] syncObject The object to destroy
   */
  virtual void DestroySyncObject( Integration::GlSyncAbstraction::SyncObject* syncObject );


public: // TEST FUNCTIONS
  Integration::GlSyncAbstraction::SyncObject* GetLastSyncObject( );

  /**
   * Test method to trigger the object sync behaviour.
   * @param[in]
   * @param[in] sync The sync value to set
   */
  void SetObjectSynced( Integration::GlSyncAbstraction::SyncObject* syncObject, bool sync );

  /**
   * Turn trace on
   */
  void EnableTrace(bool enable);

  /**
   * Reset the trace callstack
   */
  void ResetTrace();

  /**
   * Get the trace object (allows test case to find methods on it)
   */
  TraceCallStack& GetTrace();

  /**
   * Get the number of sync objects
   *
   * @return the number of sync objects
   */
  int GetNumberOfSyncObjects();

private:
  typedef std::vector<TestSyncObject*>   SyncContainer;
  typedef SyncContainer::iterator SyncIter;
  SyncContainer mSyncObjects;  ///< The sync objects
  TraceCallStack mTrace; ///< the trace call stack for testing
};

} // Dali

#endif // __TEST_GL_SYNC_ABSTRACTION_H__
