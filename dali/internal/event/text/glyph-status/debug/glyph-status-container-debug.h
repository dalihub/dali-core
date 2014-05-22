#ifndef __DALI_INTERNAL_GLYPH_STATUS_CONTAINER_DEBUG_H__
#define __DALI_INTERNAL_GLYPH_STATUS_CONTAINER_DEBUG_H__

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

// un-comment to enable
//#define DEBUG_GLYPH_STATUS_CONTAINER

#ifdef DEBUG_GLYPH_STATUS_CONTAINER

// INTERNAL INCLUDES
#include <dali/public-api/common/set-wrapper.h>
#include <dali/internal/event/text/glyph-status/glyph-status.h>

namespace Dali
{

namespace Internal
{

// typedefs are only defined if  DEBUG_GLYPH_STATUS_CONTAINER is enabled
typedef std::set< GlyphStatus, GlyphStatus::Comparator > StatusSet;
typedef std::set< const GlyphStatus*, GlyphStatus::PointerComparator > StatusPointerSet;

/**
 * If DEBUG_GLYPH_STATUS_CONTAINER is defined, it will display
 * the following whenever an item is inserted / reference /un-referenced / deleted
 *
 * ----- Glyph Status Container ---
 * Character 46 ".", Font 0, High Q Loaded , Ref count: 0 : ~DEAD~
 * Character 72 "H", Font 0, High Q Loaded , Ref count: 1
 * Character 78 "N", Font 0, High Q Loaded , Ref count: 1
 * Character 97 "a", Font 0, High Q Loaded , Ref count: 1
 * Character 100 "d", Font 0, High Q Loaded , Ref count: 1
 * Character 101 "e", Font 0, High Q Loaded , Ref count: 2
 * Character 108 "l", Font 0, High Q Loaded , Ref count: 2
 * Character 111 "o", Font 0, High Q Loaded , Ref count: 3
 * Character 112 "p", Font 0, High Q Loaded , Ref count: 1
 * Character 114 "r", Font 0, High Q Loaded , Ref count: 0 : ~DEAD~
 * Character 116 "t", Font 0, High Q Loaded , Ref count: 1
 * Character 119 "w", Font 0, High Q Loaded , Ref count: 1
 * -------------------
 * -----Dead List------
 * Character 46 ".", Font 0, Ref count: 0, High Q Loaded , Dead Time: 0
 * Character 114 "r", Font 0, Ref count: 0, High Q Loaded , Dead Time: 1
 * -------------------
 * Atlas Size: 16, Used Space 10, Dead size (cached): 2
 * -------------------
 *
 * @param atlasSize, the size of the container
 * @param statusSet glyph status set
 * @param deadSet set of dead glyphs
 */
void DebugGlyphContainer( unsigned int atlasSize,
                          StatusSet statusSet,
                          StatusPointerSet deadSet);


} // namespace Internal

} // namespace Dali

#endif // DEBUG_GLYPH_STATUS_CONTAINER
#endif // __DALI_INTERNAL_GLYPH_STATUS_CONTAINER_H__
