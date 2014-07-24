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

// CLASS HEADER
#include <dali/internal/event/text/resource/debug/glyph-resource-debug.h>

// INTERNAL HEADERS

// EXTERNAL HEADERS
#include <sstream>

namespace Dali
{
namespace Internal
{

#if defined(DEBUG_ENABLED)
Debug::Filter* gTextLogFilter = Debug::Filter::New(Debug::Concise, false, "LOG_GLYPH_RESOURCE");

std::string DebugCharacterString(const GlyphResourceRequest::CharacterList& characterList)
{
  std::string textString;

  for(std::size_t i=0, length=characterList.size(); i<length; i++)
  {
    std::ostringstream oss;
    oss << static_cast<char>(characterList[i].character) << "(" << characterList[i].xPosition << "," << characterList[i].yPosition << ") ";
    textString.append(oss.str());
  }
  return textString;
}

std::string DebugCharacterString(const Integration::GlyphSet& glyphSet)
{
  std::string textString;
  const Integration::GlyphSet::CharacterList& charList = glyphSet.GetCharacterList();

  for(std::size_t i=0, length=charList.size(); i<length; i++)
  {
    std::ostringstream oss;
    const Integration::GlyphMetrics& gm = charList[i].second;
    oss << static_cast<char>(gm.code) << "(" << gm.xPosition << "," << gm.yPosition
        << " " << (gm.quality?"H":"L") << ") ";
    textString.append(oss.str());
  }
  return textString;
}

#endif

} // namespace Internal
} // namespace Dali
