#ifndef __DALI_ACTIVE_CONSTRAINT_DECLARATIONS_H__
#define __DALI_ACTIVE_CONSTRAINT_DECLARATIONS_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

/**
 * @addtogroup CAPI_DALI_FRAMEWORK
 * @{
 */

// EXTERNAL INCLUDES
#include <boost/function.hpp>

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/signals/dali-signal-v2.h>

namespace Dali DALI_IMPORT_API
{

class ActiveConstraint;

typedef SignalV2< void (ActiveConstraint& constraint) > ActiveConstraintSignalV2;

typedef boost::function<void (ActiveConstraint& constraint)> ActiveConstraintCallbackType;

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_ACTIVE_CONSTRAINT_DECLARATIONS_H__
