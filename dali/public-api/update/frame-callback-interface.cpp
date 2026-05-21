/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/update/frame-callback-interface.h>

// INTERNAL INCLUDES
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/update/frame-callback-interface-impl.h>

namespace Dali
{
FrameCallbackInterface::FrameCallbackInterface()
: mImpl(new Impl)
{
}

FrameCallbackInterface::~FrameCallbackInterface()
{
  if(Internal::ThreadLocalStorage::Created())
  {
    // This will be a no-op if the callback has already been removed
    Internal::ThreadLocalStorage::Get().RemoveFrameCallback(*this);
  }

  delete mImpl;
}

} // namespace Dali
