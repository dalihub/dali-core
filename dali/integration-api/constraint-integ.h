#ifndef DALI_CONSTRAINT_INTEG_H
#define DALI_CONSTRAINT_INTEG_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/object/handle.h>

namespace Dali::Integration
{
/**
 * @brief Set tag number for given constraint.
 * It should be called only for internal repositories.
 * Have exclusive relation with SetTag().
 * @warning Assert if we set internal tags which alread set some custom tag by application.
 *
 * @SINCE_2_4.14
 * @param[in] constraint The constraint that want to set tag.
 * @param[in] tag The tag number.
 */
DALI_CORE_API void ConstraintSetInternalTag(Dali::Constraint& constraint, const uint32_t tag);

/**
 * @brief Removes all the constraint from the Object include custom and internal.
 * It should be called only for internal repositories.
 *
 * @SINCE_2_4.14
 * @param[in] handle The handle that want to remove all constraints.
 */
DALI_CORE_API void HandleRemoveAllConstraints(Dali::Handle& handle);

/**
 * @brief Removes all the constraint from the Object with a matching tag.
 * It should be called only for internal repositories.
 *
 * @SINCE_2_4.14
 * @param[in] handle The handle that want to remove constraint by tag.
 * @param[in] tag The tag of the constraints which will be removed. Only internal tag be allowed.
 */
DALI_CORE_API void HandleRemoveConstraints(Dali::Handle& handle, uint32_t tag);

/**
 * @brief Removes all the constraint from the Object with a range of tags [tagBegin tagEnd).
 * It should be called only for internal repositories.
 *
 * @SINCE_2_4.14
 * @param[in] handle The handle that want to remove constraint by tag.
 * @param[in] tagBegin The begin tag of the constraints which will be removed. Only internal tag be allowed. (include)
 * @param[in] tagEnd The end tag of the constraints which will be removed. Only internal tag be allowed. (exclusived)
 */
DALI_CORE_API void HandleRemoveConstraints(Dali::Handle& handle, uint32_t tagBegin, uint32_t tagEnd);

} // namespace Dali::Integration

#endif // DALI_CONSTRAINT_INTEG_H
