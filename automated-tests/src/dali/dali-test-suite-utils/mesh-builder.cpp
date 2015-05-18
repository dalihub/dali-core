/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include "mesh-builder.h"

namespace Dali
{

Material CreateMaterial(float opacity)
{
  Shader shader = Shader::New( "vertexSrc", "fragmentSrc" );
  Material material = Material::New(shader);

  Vector4 color = Color::WHITE;
  color.a = opacity;
  material.SetProperty(Material::Property::COLOR, color);
  return material;
}

Material CreateMaterial(float opacity, Image image)
{
  Shader shader = Shader::New( "vertexSrc", "fragmentSrc" );
  Material material = Material::New(shader);

  Vector4 color = Color::WHITE;
  color.a = opacity;
  material.SetProperty(Material::Property::COLOR, color);

  Sampler sampler = Sampler::New( image, "sTexture" );
  material.AddSampler( sampler );

  return material;
}

PropertyBuffer CreatePropertyBuffer()
{
  Property::Map texturedQuadVertexFormat;
  texturedQuadVertexFormat["aPosition"] = Property::VECTOR2;
  texturedQuadVertexFormat["aVertexCoord"] = Property::VECTOR2;

  PropertyBuffer vertexData = PropertyBuffer::New( PropertyBuffer::STATIC,
                                                   texturedQuadVertexFormat, 4 );
  return vertexData;
}

Geometry CreateQuadGeometry(void)
{
  PropertyBuffer vertexData = CreatePropertyBuffer();
  return CreateQuadGeometryFromBuffer( vertexData );
}

Geometry CreateQuadGeometryFromBuffer( PropertyBuffer vertexData )
{
  const float halfQuadSize = .5f;
  struct TexturedQuadVertex { Vector2 position; Vector2 textureCoordinates; };
  TexturedQuadVertex texturedQuadVertexData[4] = {
    { Vector2(-halfQuadSize, -halfQuadSize), Vector2(0.f, 0.f) },
    { Vector2( halfQuadSize, -halfQuadSize), Vector2(1.f, 0.f) },
    { Vector2(-halfQuadSize,  halfQuadSize), Vector2(0.f, 1.f) },
    { Vector2( halfQuadSize,  halfQuadSize), Vector2(1.f, 1.f) } };
  vertexData.SetData(texturedQuadVertexData);

  unsigned short indexData[6] = { 0, 3, 1, 0, 2, 3 };
  Property::Map indexFormat;
  indexFormat["indices"] = Property::UNSIGNED_INTEGER; // Should be Unsigned Short
  PropertyBuffer indices = PropertyBuffer::New( PropertyBuffer::STATIC, indexFormat, 3 );
  indices.SetData(indexData);

  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer( vertexData );
  geometry.SetIndexBuffer( indices );

  return geometry;
}



} // namespace Dali
