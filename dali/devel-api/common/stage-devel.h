#ifndef DALI_STAGE_DEVEL_H
#define DALI_STAGE_DEVEL_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/stage.h>

namespace Dali
{

class FrameCallbackInterface;

namespace DevelStage
{

/**
 * @brief The DALi rendering behavior.
 */
enum class Rendering
{
  IF_REQUIRED,  ///< Default. Will only render if required to do so.
  CONTINUOUSLY, ///< Will render continuously.
};

typedef Signal< bool (const KeyEvent&) > KeyEventGeneratedSignalType;      ///< Stage key event generated signal type

/**
 * @brief The user would connect to this signal to get a KeyEvent when KeyEvent is generated.
 *
 * @param[in] stage The stage to emit a signal
 * @return The return is true if KeyEvent is consumed, otherwise false.
 */
DALI_CORE_API KeyEventGeneratedSignalType& KeyEventGeneratedSignal( Dali::Stage stage );

/**
 * @brief Gives the user the ability to set the rendering behavior of DALi.
 *
 * @param[in] stage The stage
 * @param[in] renderingBehavior The rendering behavior required.
 *
 * @note By default, DALi uses Rendering::IF_REQUIRED.
 * @see Rendering
 */
DALI_CORE_API void SetRenderingBehavior( Dali::Stage stage, Rendering renderingBehavior );

/**
 * @brief Retrieves the rendering behavior of DALi.
 *
 * @param[in] stage The stage
 * @return The rendering behavior of DALi.
 */
DALI_CORE_API Rendering GetRenderingBehavior( Dali::Stage stage );

/*
 * @brief The FrameCallbackInterface implementation added gets called on every frame from the update-thread.
 *
 * @param[in] stage The stage to set the FrameCallbackInterface implementation on
 * @param[in] frameCallback An implementation of the FrameCallbackInterface
 * @param[in] rootActor The root-actor in the scene that the callback applies to
 *
 * @note The frameCallback cannot be added more than once. This will assert if that is attempted.
 * @note Only the rootActor and it's children will be parsed by the UpdateProxy.
 * @note If the rootActor is destroyed, then the callback is automatically removed
 * @see FrameCallbackInterface
 */
DALI_CORE_API void AddFrameCallback( Dali::Stage stage, FrameCallbackInterface& frameCallback, Actor rootActor );

/**
 * @brief Removes the specified FrameCallbackInterface implementation from being called on every frame.
 *
 * @param[in] stage The stage to clear the FrameCallbackInterface on
 * @param[in] frameCallback The FrameCallbackInterface implementation to remove
 *
 * @note This function will block if the FrameCallbackInterface::Update method is being processed in the update-thread.
 * @note If the callback implementation has already been removed, then this is a no-op.
 */
DALI_CORE_API void RemoveFrameCallback( Dali::Stage stage, FrameCallbackInterface& frameCallback );

} // namespace DevelStage

} // namespace Dali

#endif // DALI_STAGE_DEVEL_H
