#ifndef __DALI_INTERNAL_TEXT_REQUEST_HELPER_H__
#define __DALI_INTERNAL_TEXT_REQUEST_HELPER_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/object/ref-object.h>
#include <dali/internal/common/text-array.h>
#include <dali/internal/event/text/font-declarations.h>
#include <dali/internal/event/text/text-format.h>
#include <dali/internal/common/text-vertex-buffer.h>
#include <dali/internal/event/text/resource/glyph-texture-observer.h>

namespace Dali
{

namespace Internal
{

/**
 * Used to control text requests passed to a font object.
 * Characters are reference counted by the font object. This
 * class ensures the correct characters are reference / unreferenced.
 *
 */
class TextRequestHelper
{

public:

  /**
   * constructor
   * @param[in] observer glyph texture observer
   */
  TextRequestHelper( GlyphTextureObserver& observer );

  /**
   * destructor
   */
  ~TextRequestHelper();


  /**
   * Set the text to request
   * @param [in] text text string
   * @param [in] format text format
   * @return the vertex data required to draw the text
   */
  TextVertexBuffer* SetText(const TextArray& text, const TextFormat& format );

  /**
   * Set the font
   * @param [in] font
   * @param [in] format text format
   * @return the vertex data required to draw the text
   */
  TextVertexBuffer* SetFont( const FontPointer&  font, const TextFormat& format );

  /**
   * Set the text and font to request
   * @param [in] text the text string
   * @param [in] font new font to request from
   * @param [in] format text format
   * @return the vertex data required to draw the text
   */
  TextVertexBuffer* SetTextAndFont(const TextArray& text, const FontPointer& font, const TextFormat& format );

  /**
   * Called when the texture id has changed
   * @param[in] old texture id
   * @param[in] new texture id
   */
  void TextureChanged( unsigned int oldTextureId, unsigned int newTextureId );

  /**
   * Check if the text is loaded
   * @return true if text is loaded
   */
  bool IsTextLoaded() const;

private:

  /**
   * Tell the font we have stopped using a string of characters
   */
  void TextNotRequired();

  /**
   * Tell the font we require some text
   * @param[in] format the text format
   * @return the vertex data required to draw the text
   */
  TextVertexBuffer* TextRequired(  const TextFormat& format  );

  /**
   * start observing global atlas changes such as atlas resize / split
   */
  void AddTextureObserver();

  unsigned int mTextureId;                ///< the texture id of the glyph atlas being used
  TextArray mText;                        ///< text string
  TextFormat mFormat;                     ///< text format
  FontPointer mFont;                      ///< the font
  bool mTextureObserverInstalled:1;       ///< whether texture observer is installed
  GlyphTextureObserver& mTextureObserver; ///< texture observer

};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_TEXT_REQUEST_HELPER_H__
