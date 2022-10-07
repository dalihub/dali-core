/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

#include <dali/internal/event/common/object-impl.h>
#include <dali/public-api/object/handle.h>
#include <dali/public-api/object/indirect-value.h>

namespace Dali
{
IndirectValue::IndirectValue(Handle& handle, Property::Index index)
: mHandle(handle.GetObjectPtr()),
  mIndex(index),
  mExtension(nullptr)
{
}

void IndirectValue::operator=(Property::Value value)
{
  Handle(static_cast<Dali::Internal::Object*>(mHandle.Get())).SetProperty(mIndex, value);
}

Property::Value IndirectValue::GetProperty()
{
  return Handle(static_cast<Dali::Internal::Object*>(mHandle.Get())).GetProperty(mIndex);
}

IndirectValue& IndirectValue::operator=(IndirectValue&&) noexcept = default;

IndirectValue::IndirectValue(IndirectValue&&) noexcept = default;

} // namespace Dali
