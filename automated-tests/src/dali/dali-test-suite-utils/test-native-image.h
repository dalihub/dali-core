#ifndef TEST_NATIVE_IMAGE_H
#define TEST_NATIVE_IMAGE_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/images/native-image-interface.h>
#include <dali/devel-api/images/native-image-interface-extension.h>
#include <dali/integration-api/gl-defines.h>

namespace Dali
{
class TestNativeImage;
class TestNativeImageNoExt;
typedef IntrusivePtr<TestNativeImage> TestNativeImagePointer;
typedef IntrusivePtr<TestNativeImageNoExt> TestNativeImageNoExtPointer;

class DALI_CORE_API TestNativeImageExtension: public Dali::NativeImageInterface::Extension
{
public:
  inline const char* GetCustomFragmentPreFix(){return "#extension GL_OES_EGL_image_external:require\n";}
  inline const char* GetCustomSamplerTypename(){return "samplerExternalOES";}

  inline int32_t GetEglImageTextureTarget(){return GL_TEXTURE_EXTERNAL_OES;}

};

class DALI_CORE_API TestNativeImage : public Dali::NativeImageInterface
{
public:
  static TestNativeImagePointer New(uint32_t width, uint32_t height);

  inline void SetGlExtensionCreateResult(bool result){ createResult = result;}
  inline virtual bool GlExtensionCreate() { ++mExtensionCreateCalls; return createResult;};
  inline virtual void GlExtensionDestroy() { ++mExtensionDestroyCalls; };
  inline virtual GLenum TargetTexture() { ++mTargetTextureCalls; return 0;};
  inline virtual void PrepareTexture() {};
  inline virtual uint32_t GetWidth() const {return mWidth;};
  inline virtual uint32_t GetHeight() const {return mHeight;};
  inline virtual bool RequiresBlending() const {return true;};
  inline virtual Dali::NativeImageInterface::Extension* GetExtension() {return mExtension;}

private:
  TestNativeImage(uint32_t width, uint32_t height);
  virtual ~TestNativeImage();

  uint32_t mWidth;
  uint32_t mHeight;
public:
  int32_t mExtensionCreateCalls;
  int32_t mExtensionDestroyCalls;
  int32_t mTargetTextureCalls;

  bool createResult;
  TestNativeImageExtension* mExtension;
};


class DALI_CORE_API TestNativeImageNoExt : public Dali::NativeImageInterface
{
public:
  static TestNativeImageNoExtPointer New(uint32_t width, uint32_t height);

  inline void SetGlExtensionCreateResult(bool result){ createResult = result;}
  inline virtual bool GlExtensionCreate() { ++mExtensionCreateCalls; return createResult;};
  inline virtual void GlExtensionDestroy() { ++mExtensionDestroyCalls; };
  inline virtual GLenum TargetTexture() { ++mTargetTextureCalls; return 1;};
  inline virtual void PrepareTexture() {};
  inline virtual uint32_t GetWidth() const {return mWidth;};
  inline virtual uint32_t GetHeight() const {return mHeight;};
  inline virtual bool RequiresBlending() const {return true;};

private:
  TestNativeImageNoExt(uint32_t width, uint32_t height);
  virtual ~TestNativeImageNoExt();

  uint32_t mWidth;
  uint32_t mHeight;
public:
  int32_t mExtensionCreateCalls;
  int32_t mExtensionDestroyCalls;
  int32_t mTargetTextureCalls;
  bool createResult;
};

} // Dali

#endif // TEST_NATIVE_IMAGE_H
