#ifndef __DALI_INTERNAL_CONSTANTS_H__
#define __DALI_INTERNAL_CONSTANTS_H__

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

namespace Dali
{

namespace Internal
{

// Constants used by renderers and actors to determine if something is visible.
extern const float FULLY_OPAQUE;      ///< Alpha values must drop below this, before an object is considered to be transparent.
extern const float FULLY_TRANSPARENT; ///< Alpha values must rise above this, before an object is considered to be visible.

} // namespace Internal

} // namespace Dali


#endif // __DALI_INTERNAL_CONSTANTS_H__
