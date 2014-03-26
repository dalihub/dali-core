#ifndef __DALI_INTEGRATION_RESOURCE_TYPES_H__
#define __DALI_INTEGRATION_RESOURCE_TYPES_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// EXTERNAL INCLUDES
#include <stdint.h>
#include <vector>

// INTERNAL INCLUDES
#include <dali/public-api/images/image-attributes.h>
#include <dali/integration-api/resource-declarations.h>

namespace Dali
{

namespace Integration
{

// Resource Types

/**
 * Extendable set of resource type identifiers used by ResourceType subclasses.
 */
enum ResourceTypeId
{
  ResourceImageData,   ///< A buffer of image data with dimension and type metadata.
  ResourceNativeImage,
  ResourceTargetImage,
  ResourceShader,
  ResourceModel,
  ResourceMesh,
  ResourceText,
  ResourceTexture,     ///< Used to pass through a request for a GLES texture to be allocated on the render thread.
  ResourceAppBitmap    ///< Used in Core to tag Bitmaps that belong to BitmapImages.
};

/**
 * @brief A ResourceType-derived class is just a data bucket.
 *
 * Each ResourceType derived class carries a bundle of data specific to one or
 * more resource request types that are unambiguously specified by the
 * <code>id</code> member of this base class.
 * Dispatch on that ResourceTypeId to know exactly which kind of resource
 * request is associated with a particular instance.
 */
struct DALI_IMPORT_API ResourceType
{
  /**
   * Constructor.
   * @param[in] typeId resource type id
   */
  ResourceType(ResourceTypeId typeId)
  : id(typeId) {}

  virtual ~ResourceType() {}

  /**
   * Create a copy of the resource type with the same attributes.
   * @return pointer to the new ResourceType.
   */
  virtual ResourceType* Clone() const = 0;

  const ResourceTypeId id;

private:

  // Undefined copy constructor.
  ResourceType( const ResourceType& type );

  // Undefined assignment operator.
  ResourceType& operator=( const ResourceType& rhs );
};

/**
 * ImageResourceType can be used to identify a a request as pertaining
 * to images and carries some image attributes to modify the request.
 * @todo Replace all uses of other image-related ResourceTypes with
 * this one as they are duplicates.
 */
struct DALI_IMPORT_API ImageResourceType : public ResourceType
{
  /**
   * Constructor.
   * @param[in] typeId  One of the image ResourceTypeIds.
   * @param[in] attribs parameters for image loading request
   */
  ImageResourceType(const ResourceTypeId typeId, const ImageAttributes& attribs)
  : ResourceType(typeId),
    imageAttributes(attribs)
  {
    DALI_ASSERT_DEBUG( typeId == ResourceTexture || typeId == ResourceImageData || typeId == ResourceNativeImage || typeId == ResourceTargetImage || typeId == ResourceAppBitmap );
  }

  /**
   * @copydoc ResourceType::Clone
   */
  virtual ResourceType* Clone() const
  {
    return new ImageResourceType( id, imageAttributes );
  }

  /**
   * Attributes are copied from the request.
   */
  ImageAttributes imageAttributes;
};

/**
 * NativeImageResourceType describes a native image resource, which can be injected
 * through ResourceManager::AddNativeImage() or requested through ResourceLoader::LoadResource().
 * If the adaptor does not support NativeImages, it can fall back to Bitmap type.
 */
struct DALI_IMPORT_API NativeImageResourceType : public ResourceType
{
  /**
   * Default constructor initialises the base class with the correct ResourceTypeId.
   */
  NativeImageResourceType()
  : ResourceType(ResourceNativeImage) {}

  /**
   * Constructor.
   * @param[in] attribs parameters for image loading request
   */
  NativeImageResourceType(const ImageAttributes& attribs)
  : ResourceType(ResourceNativeImage),
    imageAttributes(attribs) {}

 /**
  * @copydoc ResourceType::Clone
  */
  virtual ResourceType* Clone() const
  {
    return new NativeImageResourceType(imageAttributes);
  }

  /**
   * Attributes are copied from the request (if supplied).
   */
  ImageAttributes imageAttributes;
};

/**
 * RenderTargetResourceType describes a bitmap resource, which can injected
 * through ResourceManager::AddTargetImage()
 */
struct DALI_IMPORT_API RenderTargetResourceType : public ResourceType
{
  /**
   * Default constructor initialises the base class with the correct ResourceTypeId.
   */
  RenderTargetResourceType()
  : ResourceType(ResourceTargetImage) {}

  /**
   * Constructor.
   * @param[in] attribs parameters for image loading request
   */
  RenderTargetResourceType(const ImageAttributes& attribs)
  : ResourceType(ResourceTargetImage),
    imageAttributes(attribs) {}

  /**
   * @copydoc ResourceType::Clone
   */
  virtual ResourceType* Clone() const
  {
    return new RenderTargetResourceType(imageAttributes);
  }

  /**
   * Attributes are copied from the request.
   */
  ImageAttributes imageAttributes;
};

/**
 * ShaderResourceType describes a shader program resource, which can be requested
 * from PlatformAbstraction::LoadResource()
 */
struct DALI_IMPORT_API ShaderResourceType : public ResourceType
{
  ShaderResourceType(size_t shaderHash, const std::string& vertexSource, const std::string& fragmentSource)
  : ResourceType(ResourceShader),
    hash(shaderHash),
    vertexShader(vertexSource),
    fragmentShader(fragmentSource)
  {
  }

  /**
   * @copydoc ResourceType::Clone
   */
  virtual ResourceType* Clone() const
  {
    return new ShaderResourceType(hash, vertexShader, fragmentShader);
  }

public: // Attributes
  size_t            hash;              ///< Hash of the vertex/fragment sources
  const std::string vertexShader;      ///< source code for vertex program
  const std::string fragmentShader;    ///< source code for fragment program
};

/**
 * TextResourceType describes a font resource, which can be requested.
 * from PlatformAbstraction::LoadResource()  No font atlas is created.
 */
struct DALI_IMPORT_API TextResourceType : public ResourceType
{
  /**
   *  Text quality enum
   */
  enum TextQuality
  {
    TextQualityLow,       ///< Request lower quality text
    TextQualityHigh       ///< Request higher quality text
  };

  /**
   * Structure for requesting character to be loaded from file with atlas position
   * for automatic texture upload
   */
  struct GlyphPosition
  {
    GlyphPosition(unsigned int chr, unsigned int xPos, unsigned int yPos)
    : character(chr),
      quality(0),
      loaded(0),
      xPosition(xPos),
      yPosition(yPos)
    {
    }

    /** \addtogroup GlyphPositionPackedWord
     * We have 32 bits available for this data because of the alignment restrictions
     * on the 32 bit words that follow so rather than using the minimum number of
     * bits for each, we give "loaded" a whole 8 bits and push it to a byte-aligned
     * address to make access possible via a plain byte load instead of a load,
     * mask, shift sequence. The naive bitwidths before this modification are as follows:<code>
     *    character:21;
     *    quality:1;
     *    loaded:1;</code>
     *  @{
     */
    uint32_t character:21;       ///< character code (UTF-32), max value of 0x10ffff (21 bits)
    uint32_t quality:3;          ///< Loaded quality 0 = low quality, 1 = high quality
    uint32_t loaded:8;           ///< true if Loaded
    /** @}*/

    uint32_t xPosition;      ///< X Position in atlas
    uint32_t yPosition;      ///< Y Position in atlas

    /**
     * Used by ResourceTypeCompare
     */
    friend bool operator==(const GlyphPosition& lhs, const GlyphPosition& rhs);
  };

  typedef std::vector< GlyphPosition > CharacterList;      ///< List of glyphs requested

  enum GlyphCacheMode
  {
    GLYPH_CACHE_READ,    ///< Doesn't cache glyphs.
    GLYPH_CACHE_WRITE,   ///< Caches glyphs.
  };

  /**
   * Text resource type constructor
   * @param [in] hash           The resourceHash for the FontAtlas and FontMetrics
   * @param [in] style          The font style
   * @param [in] characterList  The requested text as a vector or UTF-32 codes
   * @param [in] textureAtlasId The resource ID of the texture atlas
   * @param [in] quality        A boolean, set to true to request high quality glyph bitmaps.
   * @param [in] maxGlyphSize   The size of the largest glyph in the font.
   * @param [in] cache          Whether text glyph should be cached or not.
   */
  TextResourceType( const size_t hash,
                    const std::string& style,
                    const CharacterList& characterList,
                    ResourceId textureAtlasId,
                    TextQuality quality = TextQualityLow,
                    Vector2 maxGlyphSize = Vector2::ONE,
                    GlyphCacheMode cache = GLYPH_CACHE_READ )
  : ResourceType(ResourceText),
    mFontHash(hash),
    mStyle(style),
    mCharacterList(characterList),
    mTextureAtlasId(textureAtlasId),
    mQuality(quality),
    mMaxGlyphSize(maxGlyphSize),
    mCache( cache )
  {
  }

  /**
   * @copydoc ResourceType::Clone
   */
  virtual ResourceType* Clone() const
  {
    return new TextResourceType(mFontHash, mStyle, mCharacterList, mTextureAtlasId, mQuality, mMaxGlyphSize, mCache);
  }

  /**
   * Font resource hash.
   */
  const size_t mFontHash;

  /**
   * Font style.
   */
  const std::string mStyle;

  /**
   * Displayed text (UTF-32 codes)
   */

  CharacterList mCharacterList; ///< List of characters

  ResourceId mTextureAtlasId; ///< Resource ID of the texture atlas this request is for

  TextQuality mQuality;  ///< Text quality setting

  Vector2 mMaxGlyphSize;  ///< Max glyph size for font

  GlyphCacheMode mCache; ///< Whether text glyphs should be cached.
};

/**
 * ModelResourceType describes a model resource, which can be requested
 * from PlatformAbstraction::LoadResource()
 */
struct DALI_IMPORT_API ModelResourceType : public ResourceType
{
  /**
   * Default constructor initialises the base class with the correct ResourceTypeId.
   */
  ModelResourceType()
    : ResourceType(ResourceModel)
  {
  }

  /**
   * @copydoc ResourceType::Clone
   */
  virtual ResourceType* Clone() const
  {
    return new ModelResourceType();
  }
};


/**
 * MeshResourceType describes a mesh program resource, which can be created
 * using ResourceManager::AllocateMesh.
 */
struct DALI_IMPORT_API MeshResourceType : public ResourceType
{
  /**
   * Default constructor initialises the base class with the correct ResourceTypeId.
   */
  MeshResourceType()
  : ResourceType(ResourceMesh) {}

  /**
   * @copydoc ResourceType::Clone
   */
  virtual ResourceType* Clone() const
  {
    return new MeshResourceType();
  }
};

inline bool operator==(const TextResourceType::GlyphPosition& lhs, const TextResourceType::GlyphPosition& rhs)
{
  return lhs.character == rhs.character && lhs.xPosition == rhs.xPosition && lhs.yPosition == rhs.yPosition && lhs.quality == rhs.quality;
}

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_RESOURCE_TYPES_H__

