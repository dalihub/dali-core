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

// CLASS HEADER
#include <dali/public-api/modeling/material.h>

// INTERNAL INCLUDES
#include <dali/internal/event/modeling/material-impl.h>

namespace Dali
{

const float   Material::DEFAULT_OPACITY   = 1.0f;
const float   Material::DEFAULT_SHININESS = 0.5f;
const Vector4 Material::DEFAULT_AMBIENT_COLOR(0.2f, 0.2f, 0.2f, 1.0f);
const Vector4 Material::DEFAULT_DIFFUSE_COLOR (0.8f, 0.8f, 0.8f, 1.0f);
const Vector4 Material::DEFAULT_SPECULAR_COLOR(0.0f, 0.0f, 0.0f, 1.0f);
const Vector4 Material::DEFAULT_EMISSIVE_COLOR(0.0f, 0.0f, 0.0f, 1.0f);
const Material::MappingMode  Material::DEFAULT_MAPPING_MODE = Material::MAPPING_MODE_WRAP;
const size_t  Material::DEFAULT_DIFFUSE_UV_INDEX = 0;
const size_t  Material::DEFAULT_OPACITY_UV_INDEX = 0;
const size_t  Material::DEFAULT_NORMAL_UV_INDEX  = 0;
const bool    Material::DEFAULT_HAS_HEIGHT_MAP   = false;


Material Material::New(const std::string& name)
{
  Internal::Material* internal = Internal::Material::New(name);

  return Material(internal);
}

Material Material::DownCast( BaseHandle handle )
{
  return Material( dynamic_cast<Dali::Internal::Material*>(handle.GetObjectPtr()) );
}

Material::Material(Internal::Material* internal)
: BaseHandle(internal)
{
}

Material::Material()
{
}

Material::~Material()
{
}

Material::Material(const Material& handle)
: BaseHandle(handle)
{
}

Material& Material::operator=(const Material& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

void Material::SetName(const std::string& name)
{
  GetImplementation(*this).SetName(name);
}

const std::string& Material::GetName() const
{
  return GetImplementation(*this).GetName();
}

void Material::SetOpacity(const float opacity)
{
  GetImplementation(*this).SetOpacity(opacity);
}

float Material::GetOpacity() const
{
  return GetImplementation(*this).GetOpacity();
}

void Material::SetShininess(const float shininess)
{
  GetImplementation(*this).SetShininess(shininess);
}

float Material::GetShininess() const
{
  return GetImplementation(*this).GetShininess();
}

void Material::SetAmbientColor(const Vector4& color)
{
  GetImplementation(*this).SetAmbientColor(color);
}

const Vector4& Material::GetAmbientColor() const
{
  return GetImplementation(*this).GetAmbientColor();
}

void Material::SetDiffuseColor(const Vector4& color)
{
  GetImplementation(*this).SetDiffuseColor(color);
}

const Vector4& Material::GetDiffuseColor() const
{
  return GetImplementation(*this).GetDiffuseColor();
}

void Material::SetSpecularColor(const Vector4& color)
{
  GetImplementation(*this).SetSpecularColor(color);
}

const Vector4& Material::GetSpecularColor() const
{
  return GetImplementation(*this).GetSpecularColor();
}

void Material::SetEmissiveColor(const Vector4& color)
{
  GetImplementation(*this).SetEmissiveColor(color);
}

const Vector4& Material::GetEmissiveColor() const
{
  return GetImplementation(*this).GetEmissiveColor();
}

void Material::SetDiffuseTexture(Image image)
{
  GetImplementation(*this).SetDiffuseTexture(image);
}

Image Material::GetDiffuseTexture() const
{
  return GetImplementation(*this).GetDiffuseTexture();
}

void Material::SetOpacityTexture(Image image)
{
  GetImplementation(*this).SetOpacityTexture(image);
}

Image Material::GetOpacityTexture() const
{
  return GetImplementation(*this).GetOpacityTexture();
}

void Material::SetNormalMap(Image image)
{
  GetImplementation(*this).SetNormalMap(image);
}

Image Material::GetNormalMap() const
{
  return GetImplementation(*this).GetNormalMap();
}

void Material::SetMapU(const unsigned int map)
{
  GetImplementation(*this).SetMapU(map);
}

unsigned int Material::GetMapU() const
{
  return GetImplementation(*this).GetMapU();
}

void Material::SetMapV(const unsigned int map)
{
  GetImplementation(*this).SetMapV(map);
}

unsigned int Material::GetMapV() const
{
  return GetImplementation(*this).GetMapV();
}

void Material::SetDiffuseUVIndex(const int index)
{
  GetImplementation(*this).SetDiffuseUVIndex(index);
}

unsigned int Material::GetDiffuseUVIndex() const
{
  return GetImplementation(*this).GetDiffuseUVIndex();
}

void Material::SetOpacityUVIndex(const int index)
{
  GetImplementation(*this).SetOpacityUVIndex(index);
}

unsigned int Material::GetOpacityUVIndex() const
{
  return GetImplementation(*this).GetOpacityUVIndex();
}

void Material::SetNormalUVIndex(const int index)
{
  GetImplementation(*this).SetNormalUVIndex(index);
}

unsigned int Material::GetNormalUVIndex() const
{
  return GetImplementation(*this).GetNormalUVIndex();
}

void Material::SetHasHeightMap(const bool flag)
{
  GetImplementation(*this).SetHasHeightMap(flag);
}

bool Material::GetHasHeightMap() const
{
  return GetImplementation(*this).GetHasHeightMap();
}


void Material::SetDiffuseTextureFileName(const std::string filename)
{
  GetImplementation(*this).SetDiffuseTextureFileName(filename);
}
const std::string& Material::GetDiffuseFileName() const
{
  return GetImplementation(*this).GetDiffuseTextureFileName();
}
void Material::SetOpacityTextureFileName(const std::string filename)
{
  GetImplementation(*this).SetOpacityTextureFileName(filename);
}
const std::string& Material::GetOpacityTextureFileName() const
{
  return GetImplementation(*this).GetOpacityTextureFileName();
}
void Material::SetNormalMapFileName(const std::string filename)
{
  GetImplementation(*this).SetNormalMapFileName(filename);
}
const std::string& Material::GetNormalMapFileName() const
{
  return GetImplementation(*this).GetNormalMapFileName();
}

} // namespace Dali
