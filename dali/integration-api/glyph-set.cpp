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

// CLASS HEADER
#include <dali/integration-api/glyph-set.h>

namespace Dali
{

namespace Integration
{

GlyphSet::GlyphSet()
: mFontHash(0),
  mLineHeight(0.0f),
  mAscender(0.0f),
  mUnitsPerEM(0.0f),
  mUnderlinePosition(0.0f),
  mUnderlineThickness(0.0f),
  mPadAdjust(0.0f)
{
}

GlyphSet::~GlyphSet()
{
  mCharacterList.clear();
}

void GlyphSet::AddCharacter(ImageDataPtr imageData, const GlyphMetrics& glyphMetrics)
{
  mCharacterList.push_back(Character(imageData, glyphMetrics));
}

void GlyphSet::AddCharacter(const Character& character)
{
  mCharacterList.push_back(character);
}

const GlyphSet::CharacterList& GlyphSet::GetCharacterList() const
{
  return mCharacterList;
}

bool GlyphSet::HasCharacter(const uint32_t charCode) const
{
  for (CharacterConstIter iter = mCharacterList.begin(), endIter = mCharacterList.end(); iter != endIter; ++iter)
  {
    if (iter->second.code == charCode)
    {
      return true;
    }
  }
  return false;
}

bool GlyphSet::HasCharacter(const Character& character) const
{
  return HasCharacter(character.second.code);
}

ResourceId GlyphSet::GetAtlasResourceId() const
{
  return mAtlasId;
}

void GlyphSet::SetAtlasResourceId(ResourceId resourceId)
{
  mAtlasId = resourceId;
}

} //namespace Integration

} //namespace Dali
