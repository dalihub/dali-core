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

// CLASS HEADER
#include <dali/internal/update/common/property-base.h>

// INTERNAL INCLUDES
#include <dali/internal/update/common/resetter-manager.h> ///< For RequestPropertyBaseResetToBaseValue

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
namespace
{
ResetterManager* gResetterManager = nullptr;
} // unnamed namespace

void PropertyBase::RegisterResetterManager(ResetterManager& manager)
{
  DALI_ASSERT_ALWAYS(gResetterManager == nullptr && "PropertyBase::RegisterResetterManager called twice!");

  gResetterManager = &manager;
}

void PropertyBase::UnregisterResetterManager()
{
  DALI_ASSERT_ALWAYS(gResetterManager && "PropertyBase::RegisterResetterManager not be called before!");

  gResetterManager = nullptr;
}

void PropertyBase::RequestResetToBaseValue()
{
  DALI_ASSERT_ALWAYS(gResetterManager && "PropertyBase::RequestResetToBaseValue called without RegisterResetterManager!!");

  gResetterManager->RequestPropertyBaseResetToBaseValue(this);
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
