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
#include <dali/public-api/object/property-notification.h>

// INTERNAL INCLUDES
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/math/degree.h>
#include <dali/public-api/math/radian.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/object/handle.h>
#include <dali/internal/event/common/property-notification-impl.h>
#include <stdio.h>

namespace Dali
{

PropertyNotification::PropertyNotification()
{
}

PropertyNotification::PropertyNotification(Internal::PropertyNotification* propertyNotification)
: BaseHandle(propertyNotification)
{
}

PropertyNotification PropertyNotification::DownCast( BaseHandle handle )
{
  return PropertyNotification( dynamic_cast<Dali::Internal::PropertyNotification*>(handle.GetObjectPtr()) );
}

PropertyNotification::~PropertyNotification()
{
}

PropertyCondition PropertyNotification::GetCondition()
{
  return GetImplementation(*this).GetCondition();
}

const PropertyCondition& PropertyNotification::GetCondition() const
{
  return GetImplementation(*this).GetCondition();
}

Dali::Handle PropertyNotification::GetTarget() const
{
  return GetImplementation(*this).GetTarget();
}

Property::Index PropertyNotification::GetTargetProperty() const
{
  return GetImplementation(*this).GetTargetProperty();
}

void PropertyNotification::SetNotifyMode( NotifyMode mode )
{
  GetImplementation(*this).SetNotifyMode( mode );
}

PropertyNotification::NotifyMode PropertyNotification::GetNotifyMode()
{
  return GetImplementation(*this).GetNotifyMode();
}

bool PropertyNotification::GetNotifyResult() const
{
  return GetImplementation(*this).GetNotifyResult();
}

PropertyNotifySignalV2& PropertyNotification::NotifySignal()
{
  return GetImplementation(*this).NotifySignal();
}

} // namespace Dali
