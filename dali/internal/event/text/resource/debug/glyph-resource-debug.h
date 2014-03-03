#ifndef _DALI_INTERNAL_GLYPH_RESOURCE_DEBUG_H_
#define _DALI_INTERNAL_GLYPH_RESOURCE_DEBUG_H_
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

#include <dali/integration-api/debug.h>
#include <dali/integration-api/glyph-set.h>
#include <dali/internal/event/text/resource/glyph-resource-request.h>

namespace Dali
{
namespace Internal
{

#if defined(DEBUG_ENABLED)

extern Debug::Filter* gTextLogFilter;

std::string DebugCharacterString(const GlyphResourceRequest::CharacterList& characterList);
std::string DebugCharacterString(const Integration::GlyphSet& glyphSet);

#endif

} // namespace Internal
} // namespace Dali

#endif // _DALI_INTERNAL_GLYPH_RESOURCE_DEBUG_H_
