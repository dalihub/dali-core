#ifndef DALI_GRAPHICS_API_GRAPHICS_SYNC_OBJECT_H
#define DALI_GRAPHICS_API_GRAPHICS_SYNC_OBJECT_H

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

namespace Dali::Graphics
{
class SyncObject
{
public:
  SyncObject()          = default;
  virtual ~SyncObject() = default;

  // Definitely not copyable:
  SyncObject(const SyncObject&) = delete;
  SyncObject& operator=(const SyncObject&) = delete;

  /**
   * Determine if the synchronisation object has been signalled.
   *
   * @return false if the sync object has not been signalled, true if it has been signalled (and
   * can now be destroyed)
   */
  virtual bool IsSynced() = 0;

protected:
  SyncObject(SyncObject&&) = default;
  SyncObject& operator=(SyncObject&&) = default;
};

} // namespace Dali::Graphics

#endif //DALI_GRAPHICS_API_GRAPHICS_SYNC_OBJECT_H
