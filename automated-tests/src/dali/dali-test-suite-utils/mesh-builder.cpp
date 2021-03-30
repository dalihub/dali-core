/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

namespace Dali
{
Shader CreateShader()
{
  return Shader::New("vertexSrc", "fragmentSrc");
}

TextureSet CreateTextureSet()
{
  return TextureSet::New();
}

TextureSet CreateTextureSet(Texture texture)
{
  TextureSet textureSet = TextureSet::New();
  textureSet.SetTexture(0u, texture);
  return textureSet;
}

VertexBuffer CreateVertexBuffer()
{
  Property::Map texturedQuadVertexFormat;
  texturedQuadVertexFormat["aPosition"] = Property::VECTOR2;
  texturedQuadVertexFormat["aTexCoord"] = Property::VECTOR2;

  VertexBuffer vertexData = VertexBuffer::New(texturedQuadVertexFormat);
  return vertexData;
}

Geometry CreateQuadGeometry(void)
{
  VertexBuffer vertexData   = CreateVertexBuffer();
  const float  halfQuadSize = .5f;
  struct TexturedQuadVertex
  {
    Vector2 position;
    Vector2 textureCoordinates;
  };
  TexturedQuadVertex texturedQuadVertexData[4] = {
    {Vector2(-halfQuadSize, -halfQuadSize), Vector2(0.f, 0.f)},
    {Vector2(halfQuadSize, -halfQuadSize), Vector2(1.f, 0.f)},
    {Vector2(-halfQuadSize, halfQuadSize), Vector2(0.f, 1.f)},
    {Vector2(halfQuadSize, halfQuadSize), Vector2(1.f, 1.f)}};
  vertexData.SetData(texturedQuadVertexData, 4);

  unsigned short indexData[6] = {0, 3, 1, 0, 2, 3};

  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer(vertexData);
  geometry.SetIndexBuffer(indexData, sizeof(indexData) / sizeof(indexData[0]));

  return geometry;
}

Property::Map CreateModelVertexFormat()
{
  Property::Map modelVF;
  modelVF["aPosition"]       = Property::VECTOR3;
  modelVF["aNormal"]         = Property::VECTOR3;
  modelVF["aTexCoord1"]      = Property::VECTOR3;
  modelVF["aTexCoord2"]      = Property::VECTOR3;
  modelVF["aBoneIndex[0]"]   = Property::INTEGER;
  modelVF["aBoneIndex[1]"]   = Property::INTEGER;
  modelVF["aBoneIndex[2]"]   = Property::INTEGER;
  modelVF["aBoneIndex[3]"]   = Property::INTEGER;
  modelVF["aBoneWeights[0]"] = Property::FLOAT;
  modelVF["aBoneWeights[1]"] = Property::FLOAT;
  modelVF["aBoneWeights[2]"] = Property::FLOAT;
  modelVF["aBoneWeights[3]"] = Property::FLOAT;
  return modelVF;
}

Geometry CreateModelGeometry(Property::Map& vf)
{
  VertexBuffer vertexData = VertexBuffer::New(vf);

  struct Vertex
  {
    Vector3 position;
    Vector3 diffuseTexCoords;
    Vector3 metalRoughTexCoords;
    int     boneIndices[4];
    float   boneWeights[4];
  };

  Vertex verts[30];
  vertexData.SetData(verts, 30);
  unsigned short indexData[40];

  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer(vertexData);
  geometry.SetIndexBuffer(indexData, sizeof(indexData) / sizeof(indexData[0]));

  return geometry;
}

} // namespace Dali
