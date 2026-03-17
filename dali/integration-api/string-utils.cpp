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
 */

#include <dali/integration-api/dali-string-impl.h>
#include <dali/integration-api/string-utils.h>
#include <dali/public-api/common/dali-common.h>

namespace Dali::Integration
{

std::string ToStdString(String&& string)
{
  return Internal::StringImpl(std::move(string)).Extract();
}

std::string ToStdString(const Property::Value& value)
{
  DALI_ASSERT_DEBUG(value.GetType() == Property::STRING);
  Dali::String str;
  if(value.Get(str))
  {
    return Internal::StringImpl(std::move(str)).Extract();
  }
  return "";
}

bool GetStdString(const Property::Value& value, std::string& out)
{
  Dali::String str;
  if(value.Get(str))
  {
    out = Internal::StringImpl(std::move(str)).Extract();
    return true;
  }
  return false;
}

String ToDaliString(std::string&& string)
{
  return Internal::StringImpl::ToDaliString(std::move(string));
}

} // namespace Dali::Integration
