/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali/internal/render/renderers/render-texture.h>

// EXTERNAL INCLUDES
#include <math.h> //floor, log2

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/common/memory-pool-object-allocator.h>
#include <dali/internal/render/common/render-manager.h>
#include <dali/internal/update/common/scene-graph-memory-pool-collection.h>

namespace Dali
{
namespace Internal
{
namespace Render
{
namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gTextureFilter = Debug::Filter::New(Debug::Concise, false, "LOG_TEXTURE");
#endif

Dali::Internal::SceneGraph::MemoryPoolCollection*                                 gMemoryPoolCollection = nullptr;
static constexpr Dali::Internal::SceneGraph::MemoryPoolCollection::MemoryPoolType gMemoryPoolType       = Dali::Internal::SceneGraph::MemoryPoolCollection::MemoryPoolType::RENDER_TEXTURE;
} //Unnamed namespace

TextureKey Texture::NewKey(Type type, Pixel::Format format, ImageDimensions size)
{
  DALI_ASSERT_DEBUG(gMemoryPoolCollection && "Texture::RegisterMemoryPoolCollection not called!");
  void* ptr = gMemoryPoolCollection->AllocateRawThreadSafe(gMemoryPoolType);
  auto  key = gMemoryPoolCollection->GetKeyFromPtr(gMemoryPoolType, ptr);
  new(ptr) Texture(type, format, size);

  return TextureKey(key);
}

TextureKey Texture::NewKey(NativeImageInterfacePtr nativeImageInterface)
{
  DALI_ASSERT_DEBUG(gMemoryPoolCollection && "Texture::RegisterMemoryPoolCollection not called!");
  void* ptr = gMemoryPoolCollection->AllocateRawThreadSafe(gMemoryPoolType);
  auto  key = gMemoryPoolCollection->GetKeyFromPtr(gMemoryPoolType, ptr);
  new(ptr) Texture(nativeImageInterface);

  return TextureKey(key);
}

TextureKey Texture::NewKey(Type type, uint32_t resourceId)
{
  DALI_ASSERT_DEBUG(gMemoryPoolCollection && "Texture::RegisterMemoryPoolCollection not called!");
  void* ptr = gMemoryPoolCollection->AllocateRawThreadSafe(gMemoryPoolType);
  auto  key = gMemoryPoolCollection->GetKeyFromPtr(gMemoryPoolType, ptr);
  new(ptr) Texture(type, resourceId);

  return TextureKey(key);
}

void Texture::RegisterMemoryPoolCollection(SceneGraph::MemoryPoolCollection& memoryPoolCollection)
{
  gMemoryPoolCollection = &memoryPoolCollection;
}

void Texture::UnregisterMemoryPoolCollection()
{
  gMemoryPoolCollection = nullptr;
}

Texture::Texture(Type type, Pixel::Format format, ImageDimensions size)
: mGraphicsController(nullptr),
  mRenderManager(nullptr),
  mGraphicsTexture(nullptr),
  mNativeImage(),
  mSampler(),
  mPixelFormat(format),
  mWidth(size.GetWidth()),
  mHeight(size.GetHeight()),
  mResourceId(0u),
  mType(type),
  mHasAlpha(HasAlpha(format)),
  mUpdated(true),
  mUseUploadedParameter(mWidth == 0u && mHeight == 0u && mPixelFormat == Pixel::INVALID)
{
}

Texture::Texture(NativeImageInterfacePtr nativeImageInterface)
: mGraphicsController(nullptr),
  mRenderManager(nullptr),
  mGraphicsTexture(nullptr),
  mNativeImage(nativeImageInterface),
  mSampler(),
  mPixelFormat(Pixel::RGBA8888),
  mWidth(static_cast<uint16_t>(nativeImageInterface->GetWidth())),   // ignoring overflow, not happening in practice
  mHeight(static_cast<uint16_t>(nativeImageInterface->GetHeight())), // ignoring overflow, not happening in practice
  mResourceId(0u),
  mType(TextureType::TEXTURE_2D),
  mHasAlpha(nativeImageInterface->RequiresBlending()),
  mUpdated(true),
  mUseUploadedParameter(false)
{
}

Texture::Texture(Type type, uint32_t resourceId)
: mGraphicsController(nullptr),
  mRenderManager(nullptr),
  mGraphicsTexture(nullptr),
  mNativeImage(),
  mSampler(),
  mPixelFormat(Pixel::INVALID),
  mWidth(0u),
  mHeight(0u),
  mResourceId(resourceId),
  mType(type),
  mHasAlpha(true), ///< Since we don't assume what kind of texture will be uploaded in this case.
  mUpdated(true),
  mUseUploadedParameter(true)
{
}

Texture::~Texture() = default;

void Texture::operator delete(void* ptr)
{
  DALI_ASSERT_DEBUG(gMemoryPoolCollection && "Texture::RegisterMemoryPoolCollection not called!");
  gMemoryPoolCollection->FreeThreadSafe(gMemoryPoolType, ptr);
}

Render::Texture* Texture::Get(TextureKey::KeyType key)
{
  DALI_ASSERT_DEBUG(gMemoryPoolCollection && "Texture::RegisterMemoryPoolCollection not called!");
  return static_cast<Render::Texture*>(gMemoryPoolCollection->GetPtrFromKey(gMemoryPoolType, key));
}

void Texture::Initialize(Graphics::Controller& graphicsController, SceneGraph::RenderManager& renderManager)
{
  mGraphicsController = &graphicsController;
  mRenderManager      = &renderManager;
  if(mNativeImage)
  {
    Create(static_cast<uint32_t>(Graphics::TextureUsageFlagBits::SAMPLE));
  }

  if(mResourceId != 0u && mGraphicsController->GetTextureFromResourceId(mResourceId))
  {
    // Take ownership from graphics controller
    mGraphicsTexture = std::move(mGraphicsController->ReleaseTextureFromResourceId(mResourceId));
    // Now we take on the Graphics::Texture ownership. We don't need to keep mResourceId anymore.
    mResourceId = 0u;
  }
}

void Texture::Destroy()
{
  if(mResourceId != 0u && mGraphicsTexture.get() == nullptr)
  {
    // We still don't take texture ownership from controller before.
    // Discard graphics object now.
    mGraphicsController->DiscardTextureFromResourceId(mResourceId);
    mResourceId = 0u;
  }

  mGraphicsTexture.reset();
}

Graphics::Texture* Texture::GetGraphicsObject()
{
  if(mResourceId != 0u && mGraphicsTexture.get() == nullptr)
  {
    // The ownership of Graphics::Texture is on Graphics::Controller. Ask to controller.
    Graphics::Texture* graphicsTexturePtr = nullptr;
    if(mGraphicsController)
    {
      graphicsTexturePtr = mGraphicsController->GetTextureFromResourceId(mResourceId);

      if(graphicsTexturePtr)
      {
        // Take ownership from graphics controller
        mGraphicsTexture = std::move(mGraphicsController->ReleaseTextureFromResourceId(mResourceId));
        // (Partial update) Do not make mResourceId as 0 until we update mLatestUsedGraphicsTexture.
        NotifyTextureUpdated();
      }
    }
    DALI_LOG_INFO(gTextureFilter, Debug::General, "Render::Texture(%p)::GetGraphicsObject() = %p [with resource id : %u]\n", this, graphicsTexturePtr, mResourceId);

    return graphicsTexturePtr;
  }
  else
  {
    DALI_LOG_INFO(gTextureFilter, Debug::General, "Render::Texture(%p)::GetGraphicsObject() = %p\n", this, mGraphicsTexture.get());

    return mGraphicsTexture.get();
  }
}

void Texture::Create(Graphics::TextureUsageFlags usage)
{
  DALI_ASSERT_ALWAYS(mResourceId == 0u);

  Create(usage, Graphics::TextureAllocationPolicy::CREATION);
}

void Texture::Create(Graphics::TextureUsageFlags usage, Graphics::TextureAllocationPolicy allocationPolicy)
{
  CreateWithData(usage, allocationPolicy, nullptr, 0u);
}

void Texture::CreateWithData(Graphics::TextureUsageFlags usage, Graphics::TextureAllocationPolicy allocationPolicy, uint8_t* data, uint32_t size)
{
  auto createInfo = Graphics::TextureCreateInfo();
  createInfo
    .SetTextureType(Dali::Graphics::ConvertTextureType(mType))
    .SetUsageFlags(usage)
    .SetFormat(Dali::Graphics::ConvertPixelFormat(mPixelFormat))
    .SetSize({mWidth, mHeight})
    .SetLayout(Graphics::TextureLayout::LINEAR)
    .SetAllocationPolicy(allocationPolicy)
    .SetData(data)
    .SetDataSize(size)
    .SetNativeImage(mNativeImage)
    .SetMipMapFlag(Graphics::TextureMipMapFlag::DISABLED);

  mGraphicsTexture = mGraphicsController->CreateTexture(createInfo, std::move(mGraphicsTexture));
}

void Texture::Upload(PixelDataPtr pixelData, const Graphics::UploadParams& params)
{
  DALI_ASSERT_ALWAYS(!mNativeImage);
  DALI_ASSERT_ALWAYS(mResourceId == 0u);

  const uint32_t srcStrideBytes = pixelData->GetStrideBytes();
  uint32_t       srcOffset      = 0u;
  uint32_t       srcSize        = pixelData->GetBufferSize();

  // Cache uploaded data
  const auto uploadedDataWidth   = params.dataWidth;
  const auto uploadedDataHeight  = params.dataHeight;
  const auto uploadedPixelFormat = pixelData->GetPixelFormat();

  const bool requiredSubPixelData = (!Pixel::IsCompressed(uploadedPixelFormat)) &&
                                    ((params.dataXOffset != 0) ||
                                     (params.dataYOffset != 0) ||
                                     (uploadedDataWidth != pixelData->GetWidth()) ||
                                     (uploadedDataHeight != pixelData->GetHeight()));

  const uint32_t bytePerPixel = Pixel::GetBytesPerPixel(uploadedPixelFormat);

  if(requiredSubPixelData)
  {
    /**
     * TextureUpdateInfo use byte scaled offset / size.
     *
     * To make we only use sub-data of inputed PixelData, make srcOffset as 'start of SubPixelData.
     *
     *   |---- dataStrideByte -----|
     *   |-----| <-- dataXOffsetByte
     *   ...........................
     *   ......A-----------+........
     *   ......|           |........
     *   ......|           |........
     *   ......+-----------+C.......
     *   ......B....................
     *
     * A) Start of SubPixelData. offsetByte = dataStrideByte * dataYOffset + dataXOffsetByte.
     * B) offsetByte = A).offsetByte + dataStrideByte * dataHeight. Note, It can be out of original PixelData boundary.
     * C) End of SubPixelData. offsetByte = B).offsetByte - dataStrideByte + dataWidthByte.
     *
     * srcOffset = A).offsetByte;
     * srcSize = ( C).offsetByte - A).offsetByte );
     */
    const uint32_t dataStrideByte  = srcStrideBytes ? srcStrideBytes : (pixelData->GetWidth() * bytePerPixel);
    const uint32_t dataXOffsetByte = params.dataXOffset * bytePerPixel;
    const uint32_t dataWidthByte   = static_cast<uint32_t>(uploadedDataWidth) * bytePerPixel;

    srcOffset = params.dataYOffset * dataStrideByte + dataXOffsetByte;
    srcSize   = static_cast<uint32_t>(uploadedDataHeight) * dataStrideByte - (dataStrideByte - dataWidthByte);
  }

  // Update render texture value as uploaded data.
  if(mUseUploadedParameter)
  {
    mWidth       = params.xOffset + uploadedDataWidth;
    mHeight      = params.yOffset + uploadedDataHeight;
    mPixelFormat = uploadedPixelFormat;
  }

  // Always create new graphics texture object if we use uploaded parameter as texture.
  if(!mGraphicsTexture || mUseUploadedParameter)
  {
    const bool isSubImage(params.xOffset != 0u || params.yOffset != 0u ||
                          uploadedDataWidth != (mWidth >> params.mipmap) ||
                          uploadedDataHeight != (mHeight >> params.mipmap));
    Create(static_cast<Graphics::TextureUsageFlags>(Graphics::TextureUsageFlagBits::SAMPLE), isSubImage ? Graphics::TextureAllocationPolicy::CREATION : Graphics::TextureAllocationPolicy::UPLOAD);
  }

  Graphics::TextureUpdateInfo info{};

  info.dstTexture   = mGraphicsTexture.get();
  info.dstOffset2D  = {params.xOffset, params.yOffset};
  info.layer        = params.layer;
  info.level        = params.mipmap;
  info.srcReference = 0;
  info.srcExtent2D  = {uploadedDataWidth, uploadedDataHeight};
  info.srcOffset    = srcOffset;
  info.srcSize      = srcSize;
  info.srcStride    = bytePerPixel ? srcStrideBytes / bytePerPixel : 0u; ///< Note : Graphics stride use pixel scale!
  info.srcFormat    = Dali::Graphics::ConvertPixelFormat(uploadedPixelFormat);

  mUpdatedArea = Rect<uint16_t>(params.xOffset, params.yOffset, params.width, params.height);

  Graphics::TextureUpdateSourceInfo updateSourceInfo{};
  updateSourceInfo.sourceType                = Graphics::TextureUpdateSourceInfo::Type::PIXEL_DATA;
  updateSourceInfo.pixelDataSource.pixelData = Dali::PixelData(pixelData.Get());

  mGraphicsController->UpdateTextures({info}, {updateSourceInfo});

  SetUpdated(true);
}

bool Texture::HasAlphaChannel() const
{
  bool alpha = mHasAlpha;
  if(mNativeImage)
  {
    alpha = mNativeImage->RequiresBlending();
  }
  return alpha;
}

bool Texture::IsGraphicsObjectChanged()
{
  return mLatestUsedGraphicsTexture != GetGraphicsObject();
}

void Texture::NotifyTextureUpdated()
{
  if(mRenderManager)
  {
    DALI_ASSERT_DEBUG(gMemoryPoolCollection && "Texture::RegisterMemoryPoolCollection not called!");
    mRenderManager->SetTextureUpdated(TextureKey(gMemoryPoolCollection->GetKeyFromPtr(gMemoryPoolType, this)));
  }
}

void Texture::GenerateMipmaps()
{
  DALI_ASSERT_ALWAYS(mResourceId == 0u);

  // Compressed pixel doesn't support mipmap generation.
  if(Pixel::IsCompressed(mPixelFormat))
  {
    return;
  }

  if(!mGraphicsTexture)
  {
    Create(static_cast<Graphics::TextureUsageFlags>(Graphics::TextureUsageFlagBits::SAMPLE));
  }

  mGraphicsController->GenerateTextureMipmaps(*mGraphicsTexture.get());
}

void Texture::OnRenderFinished()
{
  SetUpdated(false);

  if(mResourceId != 0u)
  {
    mLatestUsedGraphicsTexture = GetGraphicsObject();
    if(mLatestUsedGraphicsTexture != nullptr)
    {
      // We don't need to keep mResourceId anymore.
      mResourceId = 0u;
    }
  }

  mUpdatedArea = Rect<uint16_t>{0, 0, 0, 0};
}

Rect<uint16_t> Texture::GetUpdatedArea()
{
  if(mNativeImage)
  {
    Rect<uint32_t> rect = mNativeImage->GetUpdatedArea();
    mUpdatedArea.x      = static_cast<uint16_t>(rect.x);
    mUpdatedArea.y      = static_cast<uint16_t>(rect.y);
    mUpdatedArea.width  = static_cast<uint16_t>(rect.width);
    mUpdatedArea.height = static_cast<uint16_t>(rect.height);
  }
  return mUpdatedArea;
}

} // namespace Render

} // namespace Internal

} // namespace Dali
