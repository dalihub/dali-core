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
#include <dali/public-api/events/hit-test-algorithm.h>

// INTERNAL INCLUDES
#include <dali/internal/event/events/hit-test-algorithm-impl.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>

namespace Dali
{

namespace HitTestAlgorithm
{

bool HitTest( Stage stage, const Vector2& screenCoordinates, Results& results, HitTestFunction func )
{
  return Internal::HitTestAlgorithm::HitTest( GetImplementation(stage), screenCoordinates, results, func );
}

bool HitTest( RenderTask& renderTask, const Vector2& screenCoordinates, Results& results, HitTestFunction func )
{
  Stage stage = Stage::GetCurrent();
  return Internal::HitTestAlgorithm::HitTest( GetImplementation( stage ), GetImplementation(renderTask), screenCoordinates, results, func );
}

} // namespace HitTestAlgorithm

} // namespace Dali
