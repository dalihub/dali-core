/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include "mesh-builder.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/images/texture-set-image.h>

namespace Dali
{

Shader CreateShader()
{
  return Shader::New( "vertexSrc", "fragmentSrc" );
}

TextureSet CreateTextureSet()
{
  return TextureSet::New();
}

TextureSet CreateTextureSet( Image image )
{
  TextureSet textureSet = TextureSet::New();
  TextureSetImage( textureSet, 0u, image );
  return textureSet;
}

PropertyBuffer CreatePropertyBuffer()
{
  Property::Map texturedQuadVertexFormat;
  texturedQuadVertexFormat["aPosition"] = Property::VECTOR2;
  texturedQuadVertexFormat["aVertexCoord"] = Property::VECTOR2;

  PropertyBuffer vertexData = PropertyBuffer::New( texturedQuadVertexFormat );
  return vertexData;
}

} // namespace Dali
