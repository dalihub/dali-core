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


#include "test-application.h"
#include "test-native-image.h"


namespace Dali
{

TestNativeImagePointer TestNativeImage::New(int width, int height)
{
  return new TestNativeImage(width, height);
}

TestNativeImage::TestNativeImage(int width, int height)
: mWidth(width), mHeight(height), mExtensionCreateCalls(0), mExtensionDestroyCalls(0), mTargetTextureCalls(0),createResult(true)
{
  mExtension = new TestNativeImageExtension();
}

TestNativeImage::~TestNativeImage()
{
}

} // namespace dali
