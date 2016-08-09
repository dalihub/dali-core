#ifndef DALI_INTERNAL_TYPE_ABSTRACTION_ENUMS_H
#define DALI_INTERNAL_TYPE_ABSTRACTION_ENUMS_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/rendering/renderer.h>
#include <dali/internal/common/type-abstraction.h>

namespace Dali
{

namespace Internal
{

template <> struct ParameterType< Dali::FaceCullingMode::Type >  : public BasicType< Dali::FaceCullingMode::Type > {};
template <> struct ParameterType< Dali::BlendMode::Type >        : public BasicType< Dali::BlendMode::Type > {};
template <> struct ParameterType< Dali::DepthWriteMode::Type >   : public BasicType< Dali::DepthWriteMode::Type > {};
template <> struct ParameterType< Dali::DepthTestMode::Type >    : public BasicType< Dali::DepthTestMode::Type > {};
template <> struct ParameterType< Dali::DepthFunction::Type >    : public BasicType< Dali::DepthFunction::Type > {};
template <> struct ParameterType< Dali::StencilMode::Type >      : public BasicType< Dali::StencilMode::Type > {};
template <> struct ParameterType< Dali::StencilFunction::Type >  : public BasicType< Dali::StencilFunction::Type > {};
template <> struct ParameterType< Dali::StencilOperation::Type > : public BasicType< Dali::StencilOperation::Type > {};

} //namespace Internal

} //namespace Dali

#endif // DALI_INTERNAL_TYPE_ABSTRACTION_ENUMS_H
