#ifndef DALI_INTERNAL_RENDER_SAMPLER_MESSAGES_H
#define DALI_INTERNAL_RENDER_SAMPLER_MESSAGES_H

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
 */

// INTERNAL INCLUDES
#include <dali/internal/common/message.h>
#include <dali/internal/common/type-abstraction-enums.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/render/renderers/render-sampler.h>

namespace Dali::Internal::Render
{
inline void SetFilterModeMessage(EventThreadServices& eventThreadServices, Render::Sampler& sampler, Dali::FilterMode::Type minFilterMode, Dali::FilterMode::Type magFilterMode)
{
  using LocalType = MessageValue2<Render::Sampler, Dali::FilterMode::Type, Dali::FilterMode::Type>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&sampler, &Render::Sampler::SetFilterMode, minFilterMode, magFilterMode);
}

inline void SetWrapModeMessage(EventThreadServices& eventThreadServices, Render::Sampler& sampler, Dali::WrapMode::Type rWrapMode, Dali::WrapMode::Type sWrapMode, Dali::WrapMode::Type tWrapMode)
{
  using LocalType = MessageValue3<Render::Sampler, Dali::WrapMode::Type, Dali::WrapMode::Type, Dali::WrapMode::Type>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&sampler, &Render::Sampler::SetWrapMode, rWrapMode, sWrapMode, tWrapMode);
}
} // namespace Dali::Internal::Render

#endif //  DALI_INTERNAL_RENDER_SAMPLER_MESSAGES_H
