#ifndef __DALI_INTERNAL_FONT_LOOKUP_INTERFACE_H__
#define __DALI_INTERNAL_FONT_LOOKUP_INTERFACE_H__

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
#include <dali/internal/event/text/resource/font-id.h>

namespace Dali
{

namespace Internal
{

/**
 * Abstract interface for requesting font information given a unique font id.
 *
 */
class FontLookupInterface
{

public:

  /**
   * Given a font id, get the font information.
   * @param[in] fontId font id
   * @param[out] family font family
   * @param[out] style font style
   * @param[out] maxGlyphWidth maximum glyph width
   * @param[out] maxGlyphHeight maximum glyph height
   */
  virtual void GetFontInformation( FontId fontId,
                                   std::string& family,
                                   std::string& style,
                                   float& maxGlyphWidth,
                                   float& maxGlyphHeight ) const = 0;


protected:

  /**
   * Constructor
   */
  FontLookupInterface()
  {
  }

  /**
   * Virtual Destructor.
   */
  virtual ~FontLookupInterface()
  {
  }

  // Undefined copy constructor.
  FontLookupInterface( const FontLookupInterface& );

  // Undefined assignment operator.
  FontLookupInterface& operator=( const FontLookupInterface& );

};


} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_FONT_LOOKUP_INTERFACE_H__
