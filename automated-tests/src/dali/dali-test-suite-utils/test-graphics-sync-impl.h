#ifndef TEST_SYNC_IMPLEMENTATION_H
#define TEST_SYNC_IMPLEMENTATION_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/graphics-api/graphics-sync-object-create-info.h>
#include <dali/graphics-api/graphics-sync-object.h>
#include <dali/integration-api/core.h>
#include <dali/integration-api/graphics-sync-abstraction.h>

#include "test-trace-call-stack.h"

namespace Dali
{
class TestGraphicsSyncImplementation;

class TestSyncObject : public Integration::GraphicsSyncAbstraction::SyncObject
{
public:
  TestSyncObject(TraceCallStack& trace);
  ~TestSyncObject() override;
  bool IsSynced() override;
  void Wait() override;
  void ClientWait() override;

  bool            synced;
  TraceCallStack& mTrace;
};

/**
 * Class to emulate the gpu sync functions with tracing
 */
class TestGraphicsSyncImplementation : public Integration::GraphicsSyncAbstraction
{
public:
  /**
   * Constructor
   */
  TestGraphicsSyncImplementation();

  /**
   * Destructor
   */
  virtual ~TestGraphicsSyncImplementation();

  /**
   * Initialize the sync objects
   */
  void Initialize();

  /**
   * Create a sync object that can be polled
   */
  GraphicsSyncAbstraction::SyncObject* CreateSyncObject() override;

  /**
   * Destroy a sync object
   */
  void DestroySyncObject(GraphicsSyncAbstraction::SyncObject* syncObject) override;

public: // TEST FUNCTIONS
  GraphicsSyncAbstraction::SyncObject* GetLastSyncObject();

  /**
   * Test method to trigger the object sync behaviour.
   * @param[in]
   * @param[in] sync The sync value to set
   */
  void SetObjectSynced(GraphicsSyncAbstraction::SyncObject* syncObject, bool sync);

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

  TestGraphicsSyncImplementation(const TestGraphicsSyncImplementation&) = delete;
  TestGraphicsSyncImplementation& operator=(const TestGraphicsSyncImplementation&) = delete;

private:
  typedef std::vector<TestSyncObject*> SyncContainer;
  typedef SyncContainer::iterator      SyncIter;
  SyncContainer                        mSyncObjects;       ///< The sync objects
  TraceCallStack                       mTrace{true, "gl"}; ///< the trace call stack for testing
};

} // namespace Dali

#endif // TEST_GL_SYNC_ABSTRACTION_H
