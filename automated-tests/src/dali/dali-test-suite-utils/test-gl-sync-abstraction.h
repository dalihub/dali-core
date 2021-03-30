#ifndef TEST_GL_SYNC_ABSTRACTION_H
#define TEST_GL_SYNC_ABSTRACTION_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <map>
#include <sstream>
#include <string>

// INTERNAL INCLUDES
#include <dali/integration-api/core.h>
#include <dali/integration-api/gl-sync-abstraction.h>

#include "test-trace-call-stack.h"

namespace Dali
{
class DALI_CORE_API TestSyncObject : public Integration::GlSyncAbstraction::SyncObject
{
public:
  TestSyncObject(TraceCallStack& trace);
  ~TestSyncObject() override;
  bool            IsSynced() override;
  bool            synced;
  TraceCallStack& mTrace;
};

/**
 * Class to emulate the GL sync functions with tracing
 */
class DALI_CORE_API TestGlSyncAbstraction : public Integration::GlSyncAbstraction
{
public:
  /**
   * Constructor
   */
  TestGlSyncAbstraction();

  /**
   * Destructor
   */
  ~TestGlSyncAbstraction() override;

  /**
   * Initialize the sync objects - clear down the map
   */
  void Initialize();

  /**
   * Create a sync object
   * @return the sync object
   */
  Integration::GlSyncAbstraction::SyncObject* CreateSyncObject() override;

  /**
   * Destroy a sync object
   * @param[in] syncObject The object to destroy
   */
  void DestroySyncObject(Integration::GlSyncAbstraction::SyncObject* syncObject) override;

public: // TEST FUNCTIONS
  Integration::GlSyncAbstraction::SyncObject* GetLastSyncObject();

  /**
   * Test method to trigger the object sync behaviour.
   * @param[in]
   * @param[in] sync The sync value to set
   */
  void SetObjectSynced(Integration::GlSyncAbstraction::SyncObject* syncObject, bool sync);

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
  int32_t GetNumberOfSyncObjects();

private:
  TestGlSyncAbstraction(const TestGlSyncAbstraction&);            ///< Undefined
  TestGlSyncAbstraction& operator=(const TestGlSyncAbstraction&); ///< Undefined

  typedef std::vector<TestSyncObject*> SyncContainer;
  typedef SyncContainer::iterator      SyncIter;
  SyncContainer                        mSyncObjects;       ///< The sync objects
  TraceCallStack                       mTrace{true, "gl"}; ///< the trace call stack for testing
};

} // namespace Dali

#endif // TEST_GL_SYNC_ABSTRACTION_H
