#ifndef __DALI_ACTIVE_CONSTRAINT_DECLARATIONS_H__
#define __DALI_ACTIVE_CONSTRAINT_DECLARATIONS_H__

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

// EXTERNAL INCLUDES
#include <boost/function.hpp>

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{

class ActiveConstraint;

typedef Signal< void (ActiveConstraint& constraint) > ActiveConstraintSignalType;

typedef boost::function<void (ActiveConstraint& constraint)> ActiveConstraintCallbackType;

} // namespace Dali

#endif // __DALI_ACTIVE_CONSTRAINT_DECLARATIONS_H__
