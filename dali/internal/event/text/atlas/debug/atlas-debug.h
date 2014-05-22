#ifndef __DALI_INTERNAL_ATLAS_DEBUG_H__
#define __DALI_INTERNAL_ATLAS_DEBUG_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// un-comment to enable
//#define DEBUG_ATLAS


// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>

namespace Dali
{
namespace Internal
{
#if defined(DEBUG_ENABLED)
extern Debug::Filter* gTextAtlasLogFilter;
#endif
}
}

#ifdef DEBUG_ATLAS

#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/common/map-wrapper.h>

namespace Dali
{
namespace Internal
{

// typedefs below are only defined if DEBUG_ATLAS is defined

/**
 * lookup typedef between a block and a character code.
 */
typedef std::map< unsigned int /* used defined id */, unsigned int /* block num */ >   BlockLookup;

/**
 * array of bytes. Each bit represents an allocation block
 */
typedef std::vector<unsigned char> FreeBlocks;

/**
 * Print the atlas. E.g. for a 4x4 atlas print
 * 1 1 1 1
 * 1 1 _ _
 * _ _ _ _
 * _ _ _ _
 *
 * The 1's represent an allocated block
 * @param blocks Array of bytes, each bit set represents an allocated block
 * @param blocksPerRow how blocks per row. E.g. for a 4x4 atlas, it is 4 blocks per row
 */
void DebugPrintAtlas( const FreeBlocks& blocks,
                      unsigned int blocksPerRow);

/**
 * Print the atlas with the character id next to each allocated block
 * E.g.
 * 1 (34) 1(65) 1(13)  _
 * _      _     _      _
 * @param blocks Array of bytes, each bit set represents an allocated block
 * @param blockLookup lookup that maps a block id, to a character id
 * @param blocksPerRow how blocks per row. E.g. for a 4x4 atlas, it is 4 blocks per row
 */
void DebugPrintAtlasWithIds( const FreeBlocks& blocks,
                             const BlockLookup& blockLookup,
                             unsigned int blocksPerRow);


} // namespace Internal

} // namespace Dali

#endif //DEBUG_ATLAS

#endif // __DALI_INTERNAL_ATLAS_DEBUG_H__
