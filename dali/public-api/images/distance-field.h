#ifndef __DALI_DISTANCE_FIELD_H__
#define __DALI_DISTANCE_FIELD_H__

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

struct Vector2;

/**
 * @brief Generate a distance field map from a source image.
 *
 * @param[in]  imagePixels     A pointer to a buffer containing the source image
 * @param[in]  imageSize       The size, width and height, of the source image
 * @param[out] distanceMap     A pointer to a buffer to receive the calculated distance field map.
 *                             Note: This must not overlap with imagePixels for correct distance field map generation.
 * @param[in]  distanceMapSize The size, width and height, of the distance field map
 * @param[in]  fieldBorder     The amount of distance field cells to add around the data (for glow/shadow effects)
 * @param[in]  maxSize         The image is scaled from this size to distanceMapSize
 * @param[in]  highQuality     Set true to generate high quality distance fields
 */
DALI_IMPORT_API void GenerateDistanceFieldMap( const unsigned char* const imagePixels, const Vector2& imageSize,
                                               unsigned char* const distanceMap, const Vector2& distanceMapSize,
                                               const unsigned int fieldBorder,
                                               const Vector2& maxSize,
                                               bool highQuality = true);

} //namespace Dali

#endif // ifndef __DALI_DISTANCE_FIELD_H__
