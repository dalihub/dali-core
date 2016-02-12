#ifndef __DALI_SAMPLING_H__
#define __DALI_SAMPLING_H__

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
#include <dali/public-api/common/dali-common.h>

namespace Dali
{
/**
 * @addtogroup dali_core_actors
 * @{
 */

namespace FilterMode
{
/**
 * @brief Texture filtering mode.
 * @SINCE_1_0.0
 * @remarks This is an experimental feature and might not be supported in the next release.
 * We do recommend not to use it.
 */
enum Type
{
  NONE = 0,   ///< Use GL defaults (minification NEAREST_MIPMAP_LINEAR, magnification LINEAR) @SINCE_1_0.0
  DEFAULT,    ///< Use Dali defaults (minification LINEAR, magnification LINEAR) @SINCE_1_0.0
  NEAREST,    ///< Filter nearest @SINCE_1_0.0
  LINEAR      ///< Filter linear @SINCE_1_0.0
};

} //namespace FilterMode

namespace WrapMode
{

enum Type
{
  DEFAULT = 0,    ///< Clamp to edge @SINCE_1_0.0
  CLAMP_TO_EDGE,
  REPEAT,
  MIRRORED_REPEAT
};

} //namespace WrapMode

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_SAMPLING_H__
