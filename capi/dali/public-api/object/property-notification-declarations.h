#ifndef __DALI_PROPERTY_NOTIFICATION_DECLARATIONS_H__
#define __DALI_PROPERTY_NOTIFICATION_DECLARATIONS_H__

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

/**
 * @addtogroup CAPI_DALI_OBJECT_MODULE
 * @{
 */

// EXTERNAL INCLUDES
#include <boost/function.hpp>

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/signals/dali-signal-v2.h>

namespace Dali DALI_IMPORT_API
{

class PropertyNotification;

/**
 * @brief Signal type for Dali::PropertyNotification::NotifySignal().
 */
typedef SignalV2< void (PropertyNotification& source) > PropertyNotifySignalV2;

/**
 * @brief Function signature for Dali::PropertyNotification::NotifySignal() signal callbacks.
 */
typedef boost::function<void (PropertyNotification& source)> PropertyNotifyCallbackType;

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_PROPERTY_NOTIFICATION_DECLARATIONS_H__
