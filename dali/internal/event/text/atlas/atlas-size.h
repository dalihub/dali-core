#ifndef __DALI_INTERNAL_ATLAS_SIZES_H__
#define __DALI_INTERNAL_ATLAS_SIZES_H__

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
namespace Dali
{

namespace Internal
{

namespace GlyphAtlasSize
{

/**
 * Given an atlas size, return the maximum amount of characters it can hold
 * @return maximum of number of characters
 */
unsigned int GetAtlasCharacterCount( unsigned int size );

/**
 * Get the initial size of an atlas, which will at least fit
 * character count passed in.
 * @return initial size of the atlas
 */
unsigned int GetInitialSize( unsigned int initialiCharacterCount);

/**
 * Gets the next biggest atlas size
 * @return the next atlas size
 */
unsigned int GetNextSize( unsigned int currentSize);

/**
 * This returns the maximum atlas size.
 * @return the maximum atlas size
 */
unsigned int GetMaxSize();

/**
 * Return the atlas block size.
 * @return block size
 */
unsigned int GetBlockSize();

} // namespace GlyphAtlasSize

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_ATLAS_SIZES_H__
