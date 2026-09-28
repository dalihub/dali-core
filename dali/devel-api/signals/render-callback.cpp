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
#include <dali/devel-api/signals/render-callback.h>

namespace Dali
{
// Constructing the object is what needs the vtable, and this is the only place it is
// done. That keeps the vtable inside the library, which is what a client linking against
// an exported class needs: it cannot emit one of its own.
RenderCallbackPtr RenderCallback::New(Dali::CallbackBase* callback, ExecutionMode executionMode)
{
  return RenderCallbackPtr(new RenderCallback(callback, executionMode));
}

RenderCallback::RenderCallback(Dali::CallbackBase* callback, ExecutionMode executionMode)
: mCallback(callback),
  mDispatchCallback(MakeCallback(this, &RenderCallback::Dispatch)),
  mExecutionMode(executionMode)
{
}

RenderCallback::~RenderCallback() = default;

} // namespace Dali
