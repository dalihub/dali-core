#ifndef DALI_GRAPHICS_API_BASE_FACTORY_H
#define DALI_GRAPHICS_API_BASE_FACTORY_H
/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <memory>
#include <tuple>
#include <utility>

namespace Dali
{
namespace Graphics
{
namespace API
{

template< typename T, typename... Params >
class BaseFactory
{
public:
  using Type        = T;
  using PointerType = std::unique_ptr< T >;

  BaseFactory(Params&&... params) : mParams(std::forward< Params >(params)...)
  {
  }
  virtual ~BaseFactory() = default;

  virtual PointerType Create(const Params&...) const = 0;

protected:
  BaseFactory(const BaseFactory&) = delete;
  BaseFactory& operator=(const BaseFactory&) = delete;

  BaseFactory(BaseFactory&&) = default;
  BaseFactory& operator=(BaseFactory&&) = default;

private:
  std::tuple< Params... > mParams;
};

} // namespace API
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_API_BASE_FACTORY_H
