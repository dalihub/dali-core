#ifndef __VECTOR_WRAPPER_H__
#define __VECTOR_WRAPPER_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <cstddef>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

#ifndef HIDE_DALI_INTERNALS

#include <vector>

#else

// ensure that default visibility is used with any class that is used as an exception type
#include <memory>
#include <new>
#include <stdexcept>

#if defined(__clang__)

#undef _LIBCPP_INLINE_VISIBILITY
#define _LIBCPP_INLINE_VISIBILITY
#undef _LIBCPP_EXTERN_TEMPLATE
#define _LIBCPP_EXTERN_TEMPLATE(...)

#include <vector>

#undef _LIBCPP_INLINE_VISIBILITY
#define _LIBCPP_INLINE_VISIBILITY __attribute__ ((__visibility__("hidden"), __always_inline__))
#undef _LIBCPP_EXTERN_TEMPLATE
#define _LIBCPP_EXTERN_TEMPLATE(...) extern template __VA_ARGS__;

#else

#include <bits/c++config.h>
#undef _GLIBCXX_VISIBILITY_ATTR
#define _GLIBCXX_VISIBILITY_ATTR(V) __attribute__ ((__visibility__ ("hidden")))
#include <vector>
#undef _GLIBCXX_VISIBILITY_ATTR
#define _GLIBCXX_VISIBILITY_ATTR(V) __attribute__ ((__visibility__ (#V))) // restore `_GLIBCXX_VISIBILITY_ATTR`

#endif //ifdef __clang__

#endif //ifndef HIDE_DALI_INTERNALS

#endif /* __VECTOR_WRAPPER__H__ */
