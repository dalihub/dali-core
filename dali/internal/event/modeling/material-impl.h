#ifndef __DALI_INTERNAL_MATERIAL_H__
#define __DALI_INTERNAL_MATERIAL_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/common/constants.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/images/image.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/modeling/material.h>
#include <dali/public-api/object/base-object.h>

namespace Dali
{
class Image;

namespace Internal
{
class Material;
class EventToUpdate;

typedef std::vector<Dali::Material>       MaterialContainer;
typedef MaterialContainer::iterator       MaterialIter;
typedef MaterialContainer::const_iterator MaterialConstIter;

namespace SceneGraph
{
class Material;
class UpdateManager;
}


struct MaterialProperties
{
  float         mOpacity;           ///< opacity (0 = transparent, 1 = opaque [default: 1]
  float         mShininess;         ///< value between 0 and 128 [default: 0]
  Vector4       mAmbientColor;      ///< Ambient color [default: 0.2 0.2 0.2 1.0]
  Vector4       mDiffuseColor;      ///< Diffuse color [default: 0.8 0.8 0.8 1.0]
  Vector4       mSpecularColor;     ///< Specular color [default: 0.0 0.0 0.0 1.0]
  Vector4       mEmissiveColor;     ///< Emissive color [default: 0.0 0.0 0.0 1.0]
  unsigned int  mMapU;              ///< mapping mode for U texture coordinates
  unsigned int  mMapV;              ///< mapping mode for V texture coordinates
  unsigned int  mDiffuseUVIndex;    ///< Index into bound mesh's array of UV's for diffuse texture coordinates
  unsigned int  mOpacityUVIndex;    ///< Index into bound mesh's array of UV's for opacity texture coordinates
  unsigned int  mNormalUVIndex;     ///< Index into bound mesh's array of UV's for normal/height map texture coordinates
  bool          mHasHeightMap;      ///< Determines if the normal map (if supplied) contains normals or height information

  MaterialProperties();

  MaterialProperties(const MaterialProperties& rhs);

  MaterialProperties& operator=(const MaterialProperties& rhs);
};

/**
 * @copydoc Dali::Material
 *
 * A Material object can be created by the Adaptor when loading a model
 * resource, in which case it becomes part of the ModelData resource, or it can
 * be created directly by an application.
 *
 * An Internal::Material is tracked by an Internal::MeshAttachment.  When the
 * mesh attachment puts its scene graph equivalent on the stage, the
 * Internal::Material is informed through the Connect method. If this is the
 * first connection, then this creates a SceneGraph::Material and passes
 * ownership to the UpdateManager, keeping a pointer to it, and also returning a
 * pointer to the MeshAttachment.
  */
class Material : public BaseObject
{
public:

  enum SHADING_MODE
  {
    SHADING_FLAT =          0x1,            ///< Flat (faceted) shading.
    SHADING_Gouraud =       0x2,            ///< Simple gouraud shading.
    SHADING_Phong =         0x3,            ///< Phong shading
    SHADING_Blinn =         0x4,            ///< Phong-Blinn shading
    SHADING_Toon =          0x5,            ///< Toon (comic) shading
    SHADING_OrenNayar =     0x6,            ///< Extension to standard Lambertian shading, taking the roughness of the material into account
    SHADING_Minnaert =      0x7,            ///< Extension to standard Lambertian shading, taking the "darkness" of the material into account
    SHADING_CookTorrance =  0x8,            ///< Shader for metallic surfaces.
    SHADING_None =          0x9,            ///< No shading, constant light influence of 1.0.
    SHADING_Fresnel =       0xa,            ///< Fresnel shading
  };

  enum MAPPING_MODE
  {
    MAPPING_MODE_WRAP =     0x0,            ///< A texture coordinate u|v is translated to u%1|v%1
    MAPPING_MODE_CLAMP =    0x1,            ///< Texture coordinates outside [0...1] are clamped to the nearest valid value
    MAPPING_MODE_MIRROR =   0x2,            ///< A texture coordinate u|v becomes u%1|v%1 if (u-(u%1))%2 is zero and 1-(u%1)|1-(v%1) otherwise
    MAPPING_MODE_DECAL =    0x3,            ///< If the texture coordinates for a pixel are outside [0...1] the texture is not applied to that pixel
  };

  enum ImageType
  {
    DIFFUSE_TEXTURE,
    OPACITY_TEXTURE,
    NORMAL_MAP
  };

  static Material* New(const std::string& name);

  Material(const std::string& name);

private:
  /**
   * Don't allow copy constructor
   */
  Material(const Material& rhs);

  /**
   * Don't allow use of copy operator
   */
  Material& operator=(const Material& rhs);

  virtual ~Material();

public:
  /**
   * @copydoc Dali::Material::SetName
   */
  void SetName(const std::string& name)
  {
    mName = name;
  }

  /**
   * @copydoc Dali::Material::GetName
   */
  const std::string& GetName() const
  {
    return mName;
  }

  /**
   * @copydoc Dali::Material::SetOpacity
   */
  void SetOpacity(const float opacity);

  /**
   * @copydoc Dali::Material::GetOpacity
   */
  float GetOpacity() const;

  /**
   * @copydoc Dali::Material::SetShininess
   */
  void SetShininess(const float shininess);

  /**
   * @copydoc Dali::Material::GetShininess
   */
  float GetShininess() const;

  /**
   * @copydoc Dali::Material::SetAmbientColor
   */
  void SetAmbientColor(const Vector4& color);

  /**
   * @copydoc Dali::Material::GetAmbientColor
   */
  const Vector4& GetAmbientColor() const;

  /**
   * @copydoc Dali::Material::SetDiffuseColor
   */
  void SetDiffuseColor(const Vector4& color);

  /**
   * @copydoc Dali::Material::GetDiffuseColor
   */
  const Vector4& GetDiffuseColor() const;

  /**
   * @copydoc Dali::Material::SetSpecularColor
   */
  void SetSpecularColor(const Vector4& color);

  /**
   * @copydoc Dali::Material::GetSpecularColor
   */
  const Vector4& GetSpecularColor() const;

  /**
   * @copydoc Dali::Material::SetEmissiveColor
   */
  void SetEmissiveColor(const Vector4& color);

  /**
   * @copydoc Dali::Material::GetEmissiveColor
   */
  const Vector4& GetEmissiveColor() const;

  /**
   * @copydoc Dali::Material::SetDiffuseTexture(Dali::Image)
   */
  void SetDiffuseTexture(Dali::Image image);

  /**
   * @copydoc Dali::Material::SetDiffuseTextureFileName()
   */
  void SetDiffuseTextureFileName(const std::string filename);

  /**
   * @copydoc Dali::Material::GetDiffuseTexture
   */
  Dali::Image GetDiffuseTexture() const;

  /**
   * @copydoc Dali::Material::GetDiffuseFileName
   */
  const std::string& GetDiffuseTextureFileName() const;

  /**
   * @copydoc Dali::Material::SetOpacityTexture
   */
  void SetOpacityTexture(Dali::Image image);
  void SetOpacityTextureFileName(const std::string filename);

  /**
   * @copydoc Dali::Material::GetOpacityTexture
   */
  Dali::Image GetOpacityTexture() const;
  const std::string& GetOpacityTextureFileName() const;

  /**
   * @copydoc Dali::Material::SetNormalMap
   */
  void SetNormalMap(Dali::Image image);
  void SetNormalMapFileName(const std::string image);

  /**
   * @copydoc Dali::Material::GetNormalMap
   */
  Dali::Image GetNormalMap() const;
  const std::string& GetNormalMapFileName() const;

  /**
   * @copydoc Dali::Material::SetMapU
   */
  void SetMapU(const unsigned int map);

  /**
   * @copydoc Dali::Material::GetMapU
   */
  unsigned int GetMapU() const;

  /**
   * @copydoc Dali::Material::SetMapV
   */
  void SetMapV(const unsigned int map);

  /**
   * @copydoc Dali::Material::GetMapV
   */
  unsigned int GetMapV() const;

  /**
   * @copydoc Dali::Material::SetDiffuseUVIndex
   */
  void SetDiffuseUVIndex(const int index);

  /**
   * @copydoc Dali::Material::GetDiffuseUVIndex
   */
  unsigned int GetDiffuseUVIndex() const;

  /**
   * @copydoc Dali::Material::SetOpacityUVIndex
   */
  void SetOpacityUVIndex(const int index);

  /**
   * @copydoc Dali::Material::GetOpacityUVIndex
   */
  unsigned int GetOpacityUVIndex() const;

  /**
   * @copydoc Dali::Material::SetNormalUVIndex
   */
  void SetNormalUVIndex(const int index);

  /**
   * @copydoc Dali::Material::GetNormalUVIndex
   */
  unsigned int GetNormalUVIndex() const;

  /**
   * @copydoc Dali::Material::SetHasHeightMap
   */
  void SetHasHeightMap(const bool flag);

  /**
   * @copydoc Dali::Material::GetHasHeightMap
   */
  bool GetHasHeightMap() const;

  const MaterialProperties& GetProperties() const
  {
    return mProperties;
  }

  void SetProperties(const MaterialProperties& properties)
  {
    mProperties = properties;
  }

  /**
   * Returns an existing scene object, or if none exists, creates one and returns it.
   * Note, the existing scene object may already be staged.
   * @return The associated scene object
   */
  const SceneGraph::Material* GetSceneObject();

  /**
   * Connect scene graph object to stage if it isn't already.
   * Track number of connections (may have single material used by multiple meshes/attachments)
   */
  void Connect();

  /**
   * Disconnect scene graph object from mesh/attachment. Disconnect from stage when this count
   * reaches zero
   */
  void Disconnect();

  /**
   * Set or send message to set properties on the scene object
   */
  void SendPropertiesToSceneObject();

  /**
   * Set or send message to set diffuse texture image on the scene object
   */
  void SendDiffuseImageToSceneObject(Dali::Image image);

  /**
   * Set or send message to set opacity image on the scene object
   */
  void SendOpacityImageToSceneObject(Dali::Image image);

  /**
   * Set or send message to set normal map on the scene object
   */
  void SendNormalMapToSceneObject(Dali::Image image);


private:
  /**
   * Create the scene object and return it.
   * @return the scene object
   */
  Internal::SceneGraph::Material* CreateSceneObject();

  // Attributes
private:
  std::string        mName;              ///< Material's name
  MaterialProperties mProperties;        ///< Material properties
  Dali::Image        mDiffuseImage;      ///< Diffuse image handle
  Dali::Image        mOpacityImage;      ///< opacity image handle
  Dali::Image        mNormalMap;         ///< Normal/Height map image handle

  std::string        mDiffuseName;       ///< Diffuse texture file name
  std::string        mOpacityName;       ///< Opacity Texture file name
  std::string        mNormalMapName;     ///< Normap map file name

private:
  SceneGraph::Material*       mSceneObject;               ///< The associated scene graph object
  EventToUpdate*              mEventToUpdate;             ///< Used to send messages to update-thread
  SceneGraph::UpdateManager*  mUpdateManager;             ///< The Update manager
  unsigned int                mConnectionCount;           ///< Number of on-stage meshes connected to this material
}; // class Material

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::Material& GetImplementation(Dali::Material& object)
{
  DALI_ASSERT_ALWAYS( object && "Material handle is empty" );

  BaseObject& handle = object.GetBaseObject();

  return static_cast<Internal::Material&>(handle);
}

inline const Internal::Material& GetImplementation(const Dali::Material& object)
{
  DALI_ASSERT_ALWAYS( object && "Material handle is empty" );

  const BaseObject& handle = object.GetBaseObject();

  return static_cast<const Internal::Material&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_MATERIAL_H__
