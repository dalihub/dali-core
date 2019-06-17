#ifndef DALI_VIEW_MODE_H
#define DALI_VIEW_MODE_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
/**
 * @addtogroup dali_core_common
 * @{
 */

/**
 * @brief Enumeration for stereoscopic view modes.
 * @SINCE_1_0.0
 */
enum ViewMode
{
  MONO,              ///< Monoscopic (single camera). This is the default. @SINCE_1_0.0
  STEREO_HORIZONTAL, ///< @DEPRECATED_1_3_39 Stereoscopic. This mode presents the left image on the top half of the screen and the right image on the bottom half. @SINCE_1_0.0
  STEREO_VERTICAL    ///< @DEPRECATED_1_3_39 Stereoscopic. This mode renders the left image on the left half of the screen and the right image on the right half. @SINCE_1_0.0
};

/**
 * @}
 */
} // namespace Dali

#endif // DALI_VIEW_MODE_H
