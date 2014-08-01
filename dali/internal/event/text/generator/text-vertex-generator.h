#ifndef __DALI_INTERNAL_TEXT_VERTEX_GENERATOR_H__
#define __DALI_INTERNAL_TEXT_VERTEX_GENERATOR_H__

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

// INTERNAL INCLUDES
#include <dali/internal/common/text-array.h>
#include <dali/internal/event/text/text-format.h>
#include <dali/internal/common/text-vertex-buffer.h>
#include <dali/internal/event/text/font-metrics-interface.h>
#include <dali/internal/event/text/atlas/atlas-uv-interface.h>
#include <dali/internal/event/text/resource/font-id.h>

namespace Dali
{

namespace Internal
{

/**
 * Creates the vertex data for a string of text.
 *
 */
namespace TextVertexGenerator
{

/**
 * Get the vertex buffer to draw the text.
 * The caller takes ownership of the buffer and is responsible for
 * deleting it.
 * @param[in] text text array
 * @param[in] format text format
 * @param[in] metrics interface to get metric information
 * @param[in] uvInterface interface to get uv co-ordinates of each characters
 * @param[in] fontId the font id
 * @return text vertex buffer
 */
 TextVertexBuffer* Generate(const TextArray& text,
                            const TextFormat& format,
                            const FontMetricsInterface& metrics,
                            const AtlasUvInterface& uvInterface,
                            FontId fontId);



};



} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_TEXT_VERTEX_GENERATOR_H__
