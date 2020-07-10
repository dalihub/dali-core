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
#include <dali/integration-api/gl-defines.h>

namespace Dali
{
class TestNativeImage;
typedef IntrusivePtr<TestNativeImage> TestNativeImagePointer;

class DALI_CORE_API TestNativeImage : public Dali::NativeImageInterface
{
public:
  static TestNativeImagePointer New(uint32_t width, uint32_t height);

  inline void SetGlExtensionCreateResult(bool result){ createResult = result;}
  inline virtual bool CreateResource() { ++mExtensionCreateCalls; return createResult;};
  inline virtual void DestroyResource() { ++mExtensionDestroyCalls; };
  inline virtual GLenum TargetTexture() { ++mTargetTextureCalls; return mTargetTextureError;};
  inline virtual void PrepareTexture() {};
  inline virtual uint32_t GetWidth() const {return mWidth;};
  inline virtual uint32_t GetHeight() const {return mHeight;};
  inline virtual bool RequiresBlending() const {return true;};
  inline virtual int GetTextureTarget() const {return GL_TEXTURE_EXTERNAL_OES;};
  inline virtual const char* GetCustomFragmentPrefix() const {return "#extension GL_OES_EGL_image_external:require\n";};
  inline const char* GetCustomSamplerTypename() const override { return "samplerExternalOES"; };

  inline Any GetNativeImageHandle() const override { return nullptr; };
  inline bool SourceChanged() const override { return false; };

  inline virtual Dali::NativeImageInterface::Extension* GetExtension() {return nullptr;}

private:
  TestNativeImage(uint32_t width, uint32_t height);
  virtual ~TestNativeImage();

  uint32_t mWidth;
  uint32_t mHeight;
public:
  int32_t mExtensionCreateCalls;
  int32_t mExtensionDestroyCalls;
  int32_t mTargetTextureCalls;
  uint32_t mTargetTextureError=0u;
  bool createResult;
};



} // Dali

#endif // TEST_NATIVE_IMAGE_H
