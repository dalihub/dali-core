#ifndef DALI_CUSTOM_ACTOR_DEVEL_H
#define DALI_CUSTOM_ACTOR_DEVEL_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/actors/custom-actor.h>

namespace Dali
{

namespace DevelCustomActor
{

/**
 * Get the type info for the CustomActor.
 * This is useful when the type information has been set via
 * DevelHandle::SetTypeInfo() for this object.
 * @param[in] actor The custom actor of which to get the type information
 * @return the TypeInfo of this actor
 */
DALI_CORE_API const Dali::TypeInfo GetTypeInfo( CustomActor actor );

} // namespace DevelCustomActor

} // namespace Dali

#endif // DALI_CUSTOM_ACTOR_DEVEL_H
