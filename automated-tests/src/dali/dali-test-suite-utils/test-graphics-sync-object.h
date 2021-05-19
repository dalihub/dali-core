#ifndef DALI_TEST_GRAPHICS_SYNC_OBJECT_H_
#define DALI_TEST_GRAPHICS_SYNC_OBJECT_H_

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

#include <dali/graphics-api/graphics-sync-object-create-info.h>
#include <dali/graphics-api/graphics-sync-object.h>

#include <test-graphics-sync-impl.h>

namespace Dali
{
class TestGraphicsSyncObject : public Graphics::SyncObject
{
public:
  TestGraphicsSyncObject(TestGraphicsSyncImplementation& syncImpl, const Graphics::SyncObjectCreateInfo& createInfo);
  ~TestGraphicsSyncObject() override;
  void InitializeResource();
  bool IsSynced() override;

public:
  TestGraphicsSyncImplementation& mSyncImplementation;
  TestSyncObject*                 mSyncObject;
  Graphics::SyncObjectCreateInfo  mCreateInfo;
};

} // namespace Dali

#endif //DALI_TEST_GRAPHICS_SYNC_OBJECT_H
