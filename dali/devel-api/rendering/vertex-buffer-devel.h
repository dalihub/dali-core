#ifndef DALI_VERTEX_BUFFER_DEVEL_H
#define DALI_VERTEX_BUFFER_DEVEL_H

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
#include <dali/devel-api/signals/vertex-buffer-update-callback.h>
#include <dali/public-api/common/unique-ptr.h>
#include <dali/public-api/rendering/vertex-buffer.h>

namespace Dali
{
namespace DevelVertexBuffer
{
/**
 * @brief Sets VertexBufferUpdateCallback
 *
 * Function takes over the ownership over the callback.
 *
 * Developers must make sure the lifetime of used objects within the callback
 * will remain valid as long as the callback exists.
 *
 * @param[in] vertexBuffer The vertex buffer to attach update callback.
 * @param[in] updateCallback Valid VertexBufferUpdateCallback object
 */
void DALI_CORE_API SetVertexBufferUpdateCallback(Dali::VertexBuffer vertexBuffer, UniquePtr<VertexBufferUpdateCallback>&& updateCallback);

/**
 * @brief Clears attached vertex buffer update callback
 *
 * This function provides implicit thread safety.
 * @param[in] vertexBuffer The vertex buffer to remove update callback.
 */
void DALI_CORE_API ClearVertexBufferUpdateCallback(Dali::VertexBuffer vertexBuffer);
} // namespace DevelVertexBuffer
} // namespace Dali

#endif // DALI_VERTEX_BUFFER_DEVEL_H
