#ifndef __DALI_INTERNAL_ATLAS_RANK_GENERATOR_H__
#define __DALI_INTERNAL_ATLAS_RANK_GENERATOR_H__

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

// INTERNAL INCLUDES

#include <dali/internal/event/text/atlas/atlas-ranking.h>
#include <dali/internal/event/text/glyph-status/glyph-status-container.h>


namespace Dali
{

namespace Internal
{

 /**
  * Helper to generate the ranking for an atlas.
  * @param[in] text the text to match against
  * @param[in] fontId font id
  * @param[in] container glyph status container
  * @param[in] atlasResizable whether the atlas is resizable
  * @return atlas ranking
  */
 AtlasRanking GetAtlasRanking(  const Integration::TextArray& text,
                                FontId fontId,
                                const GlyphStatusContainer&  container,
                                bool atlasResizable);



} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_ATLAS_RANK_GENERATOR_H__
