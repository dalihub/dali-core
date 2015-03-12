#ifndef __DALI_INTERPOLATOR_FUNCTIONS_H__
#define __DALI_INTERPOLATOR_FUNCTIONS_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/any.h>
#include <dali/public-api/object/property.h>
#include <dali/public-api/signals/callback.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

/**
 * @brief Retrieve an interpolator function for a property.
 *
 * @param[in] type The property type for which to get an interpolator
 * @return The interpolator function.
 *
 * @note This returns dynamically allocated memory. The caller is responsible for deleting it.
 */
CallbackBase* GetDefaultInterpolator( Property::Type type );

} // namespace Internal

} //namespace SceneGraph

} // namespace Dali

#endif // __DALI_INTERPOLATOR_FUNCTIONS_H__
