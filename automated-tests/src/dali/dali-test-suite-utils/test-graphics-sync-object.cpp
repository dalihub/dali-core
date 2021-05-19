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
 */

#include "test-graphics-sync-object.h"

namespace Dali
{
TestGraphicsSyncObject::TestGraphicsSyncObject(TestGraphicsSyncImplementation& syncImpl, const Graphics::SyncObjectCreateInfo& createInfo)
: mSyncImplementation(syncImpl),
  mSyncObject(nullptr),
  mCreateInfo(createInfo)
{
}

TestGraphicsSyncObject::~TestGraphicsSyncObject()
{
  mSyncImplementation.DestroySyncObject(mSyncObject);
}

void TestGraphicsSyncObject::InitializeResource()
{
  mSyncObject = static_cast<TestSyncObject*>(mSyncImplementation.CreateSyncObject());
}

bool TestGraphicsSyncObject::IsSynced()
{
  bool synced = false;
  if(mSyncObject)
  {
    synced = mSyncObject->IsSynced();
  }
  return synced;
}

} // namespace Dali
