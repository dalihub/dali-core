#ifndef DALI_TEST_ACTOR_UTILS_H
#define DALI_TEST_ACTOR_UTILS_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <dali/public-api/rendering/texture-set.h>
#include <dali/public-api/rendering/texture.h>
#include <string>

namespace Dali
{
class Actor;
class Image;
class Texture;

/**
 * @brief Creates a simple renderable-actor with solid colored quad.
 * @return An actor with a renderer.
 */
Actor CreateRenderableActor();

/**
 * @brief Creates a renderable-actor with a texture.
 * @param[in] texture Texture to set.
 * @return An actor with a renderer.
 */
Actor CreateRenderableActor(Texture texture);

/**
 * @brief Creates a renderable-actor with a texture and custom shaders.
 * @param[in] texture Texture to set.
 * @param[in] vertexShader The vertex-shader.
 * @param[in] fragmentShader The fragment-shader.
 * @return An actor with a renderer.
 */
Actor CreateRenderableActor(Texture texture, const std::string& vertexShader, const std::string& fragmentShader);

/**
 * @brief Creates a renderable-actor with a texture and custom shaders.
 * @param[in] textures TextureSet to set.
 * @param[in] vertexShader The vertex-shader.
 * @param[in] fragmentShader The fragment-shader.
 * @return An actor with a renderer.
 */
Actor CreateRenderableActor2(TextureSet textures, const std::string& vertexShader, const std::string& fragmentShader);

Texture    CreateTexture(TextureType::Type type, Pixel::Format format, int width, int height);
TextureSet CreateTextureSet(Pixel::Format format, int width, int height);

} // namespace Dali

#endif // DALI_TEST_ACTOR_UTILS_H
