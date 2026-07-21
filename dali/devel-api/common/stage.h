#ifndef DALI_STAGE_H
#define DALI_STAGE_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/base-handle.h>

namespace Dali
{
struct Vector2;

/**
 * @brief DO NOT USE.
 *
 * Minimal interface kept only for legacy applications.
 */
class DALI_CORE_API Stage : public BaseHandle
{
public:
  /**
   * @brief Allows the creation of an empty stage handle.
   */
  Stage();

  /**
   * @brief Gets the current Stage.
   *
   * @return An (empty) stage handle. Kept for binary compatibility only.
   */
  static Stage GetCurrent();

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~Stage();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param[in] handle A reference to the copied handle
   */
  Stage(const Stage& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  Stage& operator=(const Stage& rhs);

  /**
   * @brief This move constructor is required for (smart) pointer semantics.
   *
   * @param[in] handle A reference to the moved handle
   */
  Stage(Stage&& handle) noexcept;

  /**
   * @brief This move assignment operator is required for (smart) pointer semantics.
   *
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this
   */
  Stage& operator=(Stage&& rhs) noexcept;

  /**
   * @brief Retrieves the DPI of the display device to which the stage is connected.
   *
   * @return The horizontal and vertical DPI
   */
  Vector2 GetDpi() const;

  /**
   * @brief Keep rendering for at least the given amount of time.
   *
   * @param[in] durationSeconds Time to keep rendering, 0 means render at least one more frame
   */
  void KeepRendering(float durationSeconds);
};

} // namespace Dali

#endif // DALI_STAGE_H
