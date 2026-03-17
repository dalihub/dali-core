/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali/public-api/animation/alpha-function.h>

// EXTERNAL INCLUDES
#include <variant>

namespace
{

Dali::SpringData GetSpringDefaultData(Dali::AlphaFunction::SpringType springType)
{
  switch(springType)
  {
    case Dali::AlphaFunction::SpringType::GENTLE:
    {
      return {100.0f, 15.0f, 1.0f};
    }
    case Dali::AlphaFunction::SpringType::QUICK:
    {
      return {300.0f, 20.0f, 1.0f};
    }
    case Dali::AlphaFunction::SpringType::BOUNCY:
    {
      return {600.0f, 15.0f, 1.0f};
    }
    case Dali::AlphaFunction::SpringType::SLOW:
    {
      return {94.0f, 18.5f, 1.0f};
    }
    default:
    {
      return {100.0f, 15.0f, 1.0f};
    }
  }
}
} //namespace

namespace Dali
{

struct AlphaFunction::Impl
{
  /**
   * @brief Default constructor.
   */
  Impl()
  : mMode(BUILTIN_FUNCTION),
    mBuiltin(DEFAULT),
    mVariantData(SpringData{100.0f, 15.0f, 1.0f})
  {
  }

  /**
   * @brief Constructor for built-in function.
   */
  Impl(BuiltinFunction builtin)
  : mMode(BUILTIN_FUNCTION),
    mBuiltin(builtin),
    mVariantData(SpringData{100.0f, 15.0f, 1.0f})
  {
  }

  /**
   * @brief Constructor for custom function.
   */
  Impl(AlphaFunctionPrototype customFunction)
  : mMode(CUSTOM_FUNCTION),
    mBuiltin(DEFAULT),
    mVariantData(customFunction)
  {
  }

  /**
   * @brief Constructor for bezier function.
   */
  Impl(const Vector2& controlPoint0, const Vector2& controlPoint1)
  : mMode(BEZIER),
    mBuiltin(DEFAULT),
    mVariantData(
      Vector4(Clamp(controlPoint0.x, 0.0f, 1.0f),
              controlPoint0.y,
              Clamp(controlPoint1.x, 0.0f, 1.0f),
              controlPoint1.y))
  {
  }

  /**
   * @brief Constructor for spring type.
   */
  Impl(SpringType springType)
  : mMode(SPRING),
    mBuiltin(DEFAULT),
    mVariantData(GetSpringDefaultData(springType))
  {
  }

  /**
   * @brief Constructor for custom spring data.
   */
  Impl(const SpringData& springData)
  : mMode(CUSTOM_SPRING),
    mBuiltin(DEFAULT),
    mVariantData(springData)
  {
  }

  /**
   * @brief Copy constructor.
   */
  Impl(const Impl& rhs)
  : mMode(rhs.mMode),
    mBuiltin(rhs.mBuiltin),
    mVariantData(rhs.mVariantData)
  {
  }

  ~Impl()
  {
  }

  AlphaFunction::Mode            mMode;    ///< Enum indicating the functioning mode of the AlphaFunction
  AlphaFunction::BuiltinFunction mBuiltin; ///< Enum indicating the built-in alpha function

  std::variant<Vector4, AlphaFunctionPrototype, SpringData> mVariantData;
};

AlphaFunction::AlphaFunction()
: mImpl(new Impl())
{
}

AlphaFunction::AlphaFunction(BuiltinFunction function)
: mImpl(new Impl(function))
{
}

AlphaFunction::AlphaFunction(AlphaFunctionPrototype function)
: mImpl(new Impl(function))
{
}

AlphaFunction::AlphaFunction(const Vector2& controlPoint0, const Vector2& controlPoint1)
: mImpl(new Impl(controlPoint0, controlPoint1))
{
}

AlphaFunction::AlphaFunction(SpringType springType)
: mImpl(new Impl(springType))
{
}

AlphaFunction::AlphaFunction(const Dali::SpringData& springData)
: mImpl(new Impl(springData))
{
}

AlphaFunction::~AlphaFunction()
{
  delete mImpl;
}

AlphaFunction::AlphaFunction(const AlphaFunction& rhs)
: mImpl(new Impl(*rhs.mImpl))
{
}

AlphaFunction::AlphaFunction(AlphaFunction&& rhs) noexcept
: mImpl(rhs.mImpl)
{
  rhs.mImpl = nullptr;
}

AlphaFunction& AlphaFunction::operator=(const AlphaFunction& rhs)
{
  if(this != &rhs)
  {
    delete mImpl;
    mImpl = new Impl(*rhs.mImpl);
  }
  return *this;
}

AlphaFunction& AlphaFunction::operator=(AlphaFunction&& rhs) noexcept
{
  if(this != &rhs)
  {
    delete mImpl;
    mImpl     = rhs.mImpl;
    rhs.mImpl = nullptr;
  }
  return *this;
}

Vector4 AlphaFunction::GetBezierControlPoints() const
{
  auto* value = std::get_if<Vector4>(&mImpl->mVariantData);
  return value ? *value : Vector4::ZERO;
}

AlphaFunctionPrototype AlphaFunction::GetCustomFunction() const
{
  auto* value = std::get_if<AlphaFunctionPrototype>(&mImpl->mVariantData);
  return value ? *value : nullptr;
}

AlphaFunction::BuiltinFunction AlphaFunction::GetBuiltinFunction() const
{
  return mImpl->mBuiltin;
}

AlphaFunction::Mode AlphaFunction::GetMode() const
{
  return mImpl->mMode;
}

const Dali::SpringData& AlphaFunction::GetSpringData() const
{
  auto* value = std::get_if<SpringData>(&mImpl->mVariantData);
  if(value)
  {
    return *value;
  }

  static const SpringData defaultSpringData{};
  return defaultSpringData;
}

} // namespace Dali
