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

// HEADER
#include "glyph-status-container-debug.h"

#ifdef DEBUG_GLYPH_STATUS_CONTAINER

// EXTERNAL INCLUDES
#include <iostream>

namespace Dali
{

namespace Internal
{

namespace
{

void OutputStatus( const GlyphStatus& status )
{
  switch ( status.GetLoadState() )
   {
     case GlyphStatus::GLYPH_NOT_LOADED                        : std::cout << ", Not loaded    "; break;
     case GlyphStatus::GLYPH_LOAD_REQUESTED                    : std::cout << ", Load Requested"; break;
     case GlyphStatus::GLYPH_LOW_QUALITY_LOADED_HIGH_REQUESTED : std::cout << ", Low Q  loaded "; break;
     case GlyphStatus::GLYPH_HIGH_QUALITY_LOADED               : std::cout << ", High Q Loaded "; break;
   }
}


unsigned int GetUsedSize( StatusSet statusSet )
{
  unsigned int used = 0;

  for( StatusSet::iterator iter = statusSet.begin(); iter != statusSet.end(); ++iter)
  {
    const GlyphStatus& status( (*iter) );
    if( status.GetRefCount() > 0)
    {
      used++;
    }
  }
  return used;
}

} // un-named namespace

void DebugGlyphContainer( unsigned int atlasSize,
                          StatusSet statusSet,
                          StatusPointerSet deadSet)
{
  std::cout <<"----- Glyph Status Container --- " << std::endl;

  for( StatusSet::iterator iter = statusSet.begin(); iter != statusSet.end(); ++iter)
  {
    const GlyphStatus& status( (*iter) );
    std::cout << " Character " << status.GetCharacterCode() <<" \""<< (char)status.GetCharacterCode() << "\"";
    std::cout << ", Font " << status.GetFontId();

    OutputStatus(status);
    if( status.GetRefCount() )
    {
      std::cout << ", Ref count: "<< status.GetRefCount()  << std::endl;
    }
    else
    {
      std::cout << ", Ref count: 0 : ~DEAD~"  << std::endl;
    }
  }

  std::cout <<" ------------------- " << std::endl;
  std::cout <<" -----Dead List------ " << std::endl;

  for( StatusPointerSet::iterator iter = deadSet.begin(); iter != deadSet.end(); ++iter)
  {
    const GlyphStatus* status( (*iter) );
    std::cout << " Character " << status->GetCharacterCode() <<" \""<< (char)status->GetCharacterCode() << "\"";
    std::cout << ", Font " << status->GetFontId();
    std::cout << ", Ref count: "<< status->GetRefCount();

    OutputStatus(*status);

    std::cout << ", Dead Time: "<< status->GetDeadTime()  << std::endl;
  }
  std::cout <<" ------------------- " << std::endl;
  std::cout <<" Atlas Size: "<< atlasSize << ", Used Space " << GetUsedSize(statusSet);
  std::cout <<", Dead size (cached): "<< deadSet.size() << std::endl;
  std::cout <<" ------------------- " << std::endl;

};

} // namespace Internal

} // namespace Dali

#endif // DEBUG_GLYPH_STATUS_CONTAINER
