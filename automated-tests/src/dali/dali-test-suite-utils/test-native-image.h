#ifndef TEST_NATIVE_IMAGE_H
#define TEST_NATIVE_IMAGE_H

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

  inline void SetGlExtensionCreateResult(bool result)
  {
    createResult = result;
  }
  inline virtual bool CreateResource()
  {
    ++mExtensionCreateCalls;
    mCallStack.PushCall("CreateResource", "");
    return createResult;
  };
  inline virtual void DestroyResource()
  {
    ++mExtensionDestroyCalls;
    mCallStack.PushCall("DestroyResource", "");
  };
  inline virtual GLenum TargetTexture()
  {
    ++mTargetTextureCalls;
    mCallStack.PushCall("TargetTexture", "");
    return mTargetTextureError > 0u ? mTargetTextureError-- : 0u;
  };
  inline virtual Dali::NativeImageInterface::PrepareTextureResult PrepareTexture()
  {
    mCallStack.PushCall("PrepareTexture", "");
    return mPrepareTextureResult;
  };
  inline virtual uint32_t GetWidth() const
  {
    mCallStack.PushCall("GetWidth", "");
    return mWidth;
  };
  inline virtual uint32_t GetHeight() const
  {
    mCallStack.PushCall("GetHeight", "");
    return mHeight;
  };
  inline virtual bool RequiresBlending() const
  {
    mCallStack.PushCall("RequiresBlending", "");
    return true;
  };
  inline virtual int GetTextureTarget() const
  {
    mCallStack.PushCall("GetTextureTarget", "");
    return GL_TEXTURE_EXTERNAL_OES;
  };
  inline virtual bool ApplyNativeFragmentShader(std::string& shader) override
  {
    return ApplyNativeFragmentShader(shader, 1);
  }
  inline virtual bool ApplyNativeFragmentShader(std::string& shader, int count) override
  {
    mCallStack.PushCall("ApplyNativeFragmentShader", "");
    shader = "#extension GL_OES_EGL_image_external:require\n" + shader;

    // Get custom sampler type name
    const char* customSamplerTypename = GetCustomSamplerTypename();
    if(customSamplerTypename)
    {
      size_t pos           = 0;
      int    replacedCount = 0;

      while((pos = shader.find("sampler2D", pos)) != std::string::npos)
      {
        if(count >= 0 && replacedCount >= count)
        {
          break;
        }
        shader.replace(pos, strlen("sampler2D"), customSamplerTypename);
        pos += strlen(customSamplerTypename);
        replacedCount++;
      }
    }
    return true;
  };
  inline const char* GetCustomSamplerTypename() const override
  {
    mCallStack.PushCall("GetCustomSamplerTypename", "");
    return "samplerExternalOES";
  };

  inline Any GetNativeImageHandle() const override
  {
    return nullptr;
  };
  inline bool SourceChanged() const override
  {
    return true;
  };

  inline Rect<uint32_t> GetUpdatedArea() override
  {
    return mUpdatedArea;
  }

  inline void PostRender() override
  {
    mCallStack.PushCall("PostRender", "");
  }

  inline virtual Dali::NativeImageInterface::Extension* GetExtension()
  {
    return nullptr;
  }

  inline void SetUpdatedArea(const Rect<uint32_t>& updatedArea)
  {
    mUpdatedArea = updatedArea;
  }

private:
  TestNativeImage(uint32_t width, uint32_t height);
  virtual ~TestNativeImage();

  uint32_t       mWidth;
  uint32_t       mHeight;
  Rect<uint32_t> mUpdatedArea{};

public:
  int32_t  mExtensionCreateCalls;
  int32_t  mExtensionDestroyCalls;
  int32_t  mTargetTextureCalls;
  uint32_t mTargetTextureError{0u};
  bool     createResult;

  Dali::NativeImageInterface::PrepareTextureResult mPrepareTextureResult{Dali::NativeImageInterface::PrepareTextureResult::NO_ERROR};

  mutable TraceCallStack mCallStack;
};

} // namespace Dali

#endif // TEST_NATIVE_IMAGE_H
