#ifndef __DALI_TEXT_ACTOR_PARAMETERS_H__
#define __DALI_TEXT_ACTOR_PARAMETERS_H__

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
#include <dali/public-api/common/dali-common.h>

namespace Dali DALI_IMPORT_API
{

// Forward declarations
class TextStyle;

/**
 * @brief Encapsulates text-actor parameters.
 */
struct TextActorParameters
{
  /**
   * @brief Enumeration of the automatic font detection medes.
   */
  enum AutomaticFontDetection
  {
    FONT_DETECTION_OFF, ///< Font detection disabled.
    FONT_DETECTION_ON   ///< Font detection enabled.
  };

  /**
   * @brief Default constructor.
   *
   * By default the text style is de default one and the automatic font detection is enabled.
   */
  TextActorParameters();

  /**
   * @brief Constructor.
   *
   * @param[in] style The text style.
   * @param[in] fontDetection Whether to automatically detect if the font support the characters and replace it if not.
   */
  TextActorParameters( const TextStyle& style, AutomaticFontDetection fontDetection );

  /**
   * @brief Destructor.
   *
   * Destroys the internal implementation.
   */
  ~TextActorParameters();

  /**
   * @brief Copy constructor.
   *
   * Reset the internal implementation with new given values.
   * @param[in] parameters The new text-actor parameters.
   */
  TextActorParameters( const TextActorParameters& parameters );

  /**
   * @brief Assignment operator.
   *
   * @param[in] parameters The new text-actor parameters.
   * @return A reference to this
   */
  TextActorParameters& operator=( const TextActorParameters& parameters );

  /**
   * @brief Retrieves the text style.
   *
   * @return the text style.
   */
  const TextStyle& GetTextStyle() const;

  /**
   * @brief Whether the automatic font detection is enabled.
   *
   * @return \e true if the automatic font detection is enabled.
   */
  bool IsAutomaticFontDetectionEnabled() const;

private:
  struct Impl;
  Impl* mImpl;
};

extern TextActorParameters DEFAULT_TEXT_ACTOR_PARAMETERS; ///< By default the text style is the default one and the automatic font detection is enabled.

} // namespace Dali

#endif // __DALI_TEXT_ACTOR_PARAMETERS_H__
