#ifndef DALI_DEVEL_API_IMAGES_PIXEL_DATA_MASK_H
#define DALI_DEVEL_API_IMAGES_PIXEL_DATA_MASK_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
 */

#include <dali/public-api/images/pixel-data.h>

namespace Dali
{

/**
 * @brief Apply the mask to this pixel data.
 * If the mask image size is larger, then this will sample from
 * the mask using bilinear filtering.
 * @param[in] image The pixel data of the image
 * @param[in] mask The pixel data of the mask
 */
DALI_IMPORT_API void ApplyMask( PixelData image, PixelData mask );

} // namespace Dali

#endif // DALI_DEVEL_API_IMAGES_PIXEL_DATA_MASK_H
