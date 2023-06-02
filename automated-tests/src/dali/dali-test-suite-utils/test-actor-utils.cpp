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

// CLASS HEADER
#include "test-actor-utils.h"

// EXTERNAL INCLUDES
#include <dali/public-api/dali-core.h>

// INTERNAL INCLUDES
#include "mesh-builder.h"

namespace Dali
{
namespace
{
const char* VERTEX_SHADER = DALI_COMPOSE_SHADER(
  attribute mediump vec2   aPosition;\n
    uniform mediump mat4   uMvpMatrix;\n
      uniform mediump vec3 uSize;\n
  \n void main()\n {
        \n
          mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);
        \n
          vertexPosition.xyz *= uSize;
        \n
          gl_Position = uMvpMatrix * vertexPosition;
        \n
      }\n);

const char* FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
  uniform lowp vec4 uColor;\n
  \n void main()\n {
    \n
      gl_FragColor = uColor;
    \n
  }\n);

} // unnamed namespace

Actor CreateRenderableActor()
{
  return CreateRenderableActor(Texture(), VERTEX_SHADER, FRAGMENT_SHADER);
}

Actor CreateRenderableActor(Texture texture)
{
  return CreateRenderableActor(texture, VERTEX_SHADER, FRAGMENT_SHADER);
}

Actor CreateRenderableActor(Texture texture, const std::string& vertexShader, const std::string& fragmentShader)
{
  // Create the geometry
  Geometry geometry = CreateQuadGeometry();

  // Create Shader
  Shader shader = Shader::New(vertexShader, fragmentShader);

  // Create renderer from geometry and material
  Renderer renderer = Renderer::New(geometry, shader);

  // Create actor and set renderer
  Actor actor = Actor::New();
  actor.AddRenderer(renderer);

  // If we a texture, then create a texture-set and add to renderer
  if(texture)
  {
    TextureSet textureSet = TextureSet::New();
    textureSet.SetTexture(0u, texture);
    renderer.SetTextures(textureSet);

    // Set actor to the size of the texture if set
    actor.SetProperty(Actor::Property::SIZE, Vector2(texture.GetWidth(), texture.GetHeight()));
  }

  return actor;
}

Actor CreateRenderableActor2(TextureSet textures, const std::string& vertexShader, const std::string& fragmentShader)
{
  // Create the geometry
  Geometry geometry = CreateQuadGeometry();

  // Create Shader
  Shader shader = Shader::New(vertexShader, fragmentShader);

  // Create renderer from geometry and material
  Renderer renderer = Renderer::New(geometry, shader);

  // Create actor and set renderer
  Actor actor = Actor::New();
  actor.AddRenderer(renderer);

  // If we a texture, then create a texture-set and add to renderer
  if(textures)
  {
    renderer.SetTextures(textures);

    auto texture = textures.GetTexture(0);

    // Set actor to the size of the texture if set
    actor.SetProperty(Actor::Property::SIZE, Vector2(texture.GetWidth(), texture.GetHeight()));
  }

  return actor;
}

Texture CreateTexture(TextureType::Type type, Pixel::Format format, int width, int height)
{
  Texture texture = Texture::New(type, format, width, height);

  int       bufferSize = width * height * 2;
  uint8_t*  buffer     = reinterpret_cast<uint8_t*>(malloc(bufferSize));
  PixelData pixelData  = PixelData::New(buffer, bufferSize, width, height, format, PixelData::FREE);
  texture.Upload(pixelData, 0u, 0u, 0u, 0u, width, height);
  return texture;
}

TextureSet CreateTextureSet(Pixel::Format format, int width, int height)
{
  TextureSet textureSet = TextureSet::New();
  textureSet.SetTexture(0u, CreateTexture(TextureType::TEXTURE_2D, format, width, height));
  return textureSet;
}

} // namespace Dali
