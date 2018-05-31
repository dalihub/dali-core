#ifndef __TEST_NATIVE_IMAGE_H__
#define __TEST_NATIVE_IMAGE_H__

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

  inline int GetEglImageTextureTarget(){return GL_TEXTURE_EXTERNAL_OES;}

};

class DALI_CORE_API TestNativeImage : public Dali::NativeImageInterface
{
public:
  static TestNativeImagePointer New(int width, int height);

  inline void SetGlExtensionCreateResult(bool result){ createResult = result;}
  inline virtual bool GlExtensionCreate() { ++mExtensionCreateCalls; return createResult;};
  inline virtual void GlExtensionDestroy() { ++mExtensionDestroyCalls; };
  inline virtual GLenum TargetTexture() { ++mTargetTextureCalls; return 0;};
  inline virtual void PrepareTexture() {};
  inline virtual unsigned int GetWidth() const {return mWidth;};
  inline virtual unsigned int GetHeight() const {return mHeight;};
  inline virtual bool RequiresBlending() const {return true;};
  inline virtual Dali::NativeImageInterface::Extension* GetExtension() {return mExtension;}

private:
  TestNativeImage(int width, int height);
  virtual ~TestNativeImage();

  int mWidth;
  int mHeight;
public:
  int mExtensionCreateCalls;
  int mExtensionDestroyCalls;
  int mTargetTextureCalls;

  bool createResult;
  TestNativeImageExtension* mExtension;
};


class DALI_CORE_API TestNativeImageNoExt : public Dali::NativeImageInterface
{
public:
  static TestNativeImageNoExtPointer New(int width, int height);

  inline void SetGlExtensionCreateResult(bool result){ createResult = result;}
  inline virtual bool GlExtensionCreate() { ++mExtensionCreateCalls; return createResult;};
  inline virtual void GlExtensionDestroy() { ++mExtensionDestroyCalls; };
  inline virtual GLenum TargetTexture() { ++mTargetTextureCalls; return 1;};
  inline virtual void PrepareTexture() {};
  inline virtual unsigned int GetWidth() const {return mWidth;};
  inline virtual unsigned int GetHeight() const {return mHeight;};
  inline virtual bool RequiresBlending() const {return true;};

private:
  TestNativeImageNoExt(int width, int height);
  virtual ~TestNativeImageNoExt();

  int mWidth;
  int mHeight;
public:
  int mExtensionCreateCalls;
  int mExtensionDestroyCalls;
  int mTargetTextureCalls;
  bool createResult;
};

} // Dali

#endif
