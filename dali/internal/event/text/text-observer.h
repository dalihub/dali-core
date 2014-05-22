#ifndef __DALI_INTERNAL_TEXT_OBSERVER_H__
#define __DALI_INTERNAL_TEXT_OBSERVER_H__

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

namespace Dali
{

namespace Internal
{

/**
 * Text observers are notified when characters have been loaded for a particular font.
 *
 * Note, the specific characters loaded are not specified.
 * The internal user of this class has to use Font::TextAvailable to
 * find if the the glyphs for a text string is loaded.
 */
class TextObserver
{
public:


  /**
   * Called when the text has been loaded.
   */
   virtual void TextLoaded() = 0;

protected:

   /**
    * constructor
    */
   TextObserver()
   {
   }

   /**
    * Virtual destructor.
    */
   virtual ~TextObserver()
   {
   }

   /**
    * Undefined copy constructor.
    */
   TextObserver( const TextObserver& );

   /**
    * Undefined assignment operator
    */
   TextObserver& operator=( const TextObserver&  );

};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_TEXT_OBSERVER_H__
