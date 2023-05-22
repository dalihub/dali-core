/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

#include <dali-test-suite-utils.h>
#include <mesh-builder.h>
#include <stdlib.h>
#include <memory>

#include <dali/internal/render/renderers/gpu-buffer.h>

// access private members
#define private public
#include <dali/internal/render/renderers/pipeline-cache.h>

#include <dlfcn.h>

using namespace Dali;

template<class Object, class... Args>
void InvokeNext(Object* obj, Args... args)
{
  auto    addr = __builtin_return_address(0);
  Dl_info info;
  dladdr(addr, &info);
  auto func = dlsym(RTLD_NEXT, info.dli_sname);
  typedef void (*FuncPtr)(void*, Args...);
  auto memb = FuncPtr(func);
  memb(obj, args...);
}

template<class Ret, class Object, class... Args>
Ret InvokeReturnNext(Object* obj, Args... args)
{
  auto    addr = __builtin_return_address(0);
  Dl_info info;
  dladdr(addr, &info);
  auto func = dlsym(RTLD_NEXT, info.dli_sname);
  typedef Ret (*FuncPtr)(void*, Args...);
  auto memb = FuncPtr(func);
  return memb(obj, args...);
}

namespace Dali
{
namespace Internal
{
namespace Render
{
} // namespace Render
} // namespace Internal
} // namespace Dali

int UtcDaliCoreGpuBufferDiscardWritePolicy(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Internal::Render::GpuBuffer WritePolicy::DISCARD");

  TestGraphicsController& controller = application.GetGraphicsController();

  // Create GPU Buffer with Discard policy and as a vertex buffer
  std::unique_ptr<Internal::GpuBuffer> buffer = std::make_unique<Internal::GpuBuffer>(controller,
                                                                                      0u | Graphics::BufferUsage::VERTEX_BUFFER,
                                                                                      Internal::GpuBuffer::WritePolicy::DISCARD);

  // should be 0 as unset yet
  DALI_TEST_EQUALS(buffer->GetBufferSize(), 0, TEST_LOCATION);

  std::vector<uint8_t> data;
  data.resize(1000000);
  buffer->UpdateDataBuffer(controller, 1000000, data.data());

  // we should have valid Graphics::Buffer now
  auto ptr0    = buffer->GetGraphicsObject();
  auto result0 = (ptr0 != nullptr);
  DALI_TEST_EQUALS(result0, true, TEST_LOCATION);

  // now write again, with DISCARD policy, we should get totally new Graphics object
  buffer->UpdateDataBuffer(controller, 1000000, data.data());

  auto ptr1    = buffer->GetGraphicsObject();
  auto result1 = (ptr0 != nullptr);
  DALI_TEST_EQUALS(result1, true, TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(ptr0, ptr1, 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliCoreGpuBufferRetainWritePolicy(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Internal::Render::GpuBuffer WritePolicy::RETAIN");

  TestGraphicsController& controller = application.GetGraphicsController();

  // Create GPU Buffer with Discard policy and as a vertex buffer
  std::unique_ptr<Internal::GpuBuffer> buffer = std::make_unique<Internal::GpuBuffer>(controller,
                                                                                      0u | Graphics::BufferUsage::VERTEX_BUFFER,
                                                                                      Internal::GpuBuffer::WritePolicy::RETAIN);

  // should be 0 as unset yet
  DALI_TEST_EQUALS(buffer->GetBufferSize(), 0, TEST_LOCATION);

  std::vector<uint8_t> data;
  data.resize(1000000);
  buffer->UpdateDataBuffer(controller, 1000000, data.data());

  // we should have valid Graphics::Buffer now
  auto ptr0    = buffer->GetGraphicsObject();
  auto result0 = (ptr0 != nullptr);
  DALI_TEST_EQUALS(result0, true, TEST_LOCATION);

  // with RETAIN policy, the buffer shouldn't change as long it uses same spec)
  buffer->UpdateDataBuffer(controller, 1000000, data.data());

  auto ptr1    = buffer->GetGraphicsObject();
  auto result1 = (ptr0 != nullptr);
  DALI_TEST_EQUALS(result1, true, TEST_LOCATION);
  DALI_TEST_EQUALS(ptr0, ptr1, 0, TEST_LOCATION);

  END_TEST;
}