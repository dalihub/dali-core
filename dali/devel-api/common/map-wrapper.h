#ifndef DALI_MAP_WRAPPER_H
#define DALI_MAP_WRAPPER_H

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

#ifndef HIDE_DALI_INTERNALS

#include <map>

#else

// Ensure that default visibility is used with any class that is used as an exception type
#include <memory>
#include <new>
#include <stdexcept>

#if defined(__clang__)

#undef _LIBCPP_INLINE_VISIBILITY
#define _LIBCPP_INLINE_VISIBILITY
#undef _LIBCPP_EXTERN_TEMPLATE
#define _LIBCPP_EXTERN_TEMPLATE(...)

#include <map>

#undef _LIBCPP_INLINE_VISIBILITY
#define _LIBCPP_INLINE_VISIBILITY __attribute__ ((__visibility__("hidden"), __always_inline__))
#undef _LIBCPP_EXTERN_TEMPLATE
#define _LIBCPP_EXTERN_TEMPLATE(...) extern template __VA_ARGS__;

#else

#include <bits/c++config.h>
#include <bits/stl_tree.h>
#undef _GLIBCXX_VISIBILITY_ATTR
#undef _GLIBCXX_VISIBILITY
#define _GLIBCXX_VISIBILITY_ATTR(V) __attribute__ ((__visibility__ ("hidden")))
#define _GLIBCXX_VISIBILITY(V) __attribute__ ((__visibility__ ("hidden")))
#include <map>
#undef _GLIBCXX_VISIBILITY_ATTR
#undef _GLIBCXX_VISIBILITY
#define _GLIBCXX_VISIBILITY_ATTR(V) __attribute__ ((__visibility__ (#V))) // restore `_GLIBCXX_VISIBILITY_ATTR`
#define _GLIBCXX_VISIBILITY(V) __attribute__ ((__visibility__ (#V))) // restore `_GLIBCXX_VISIBILITY`

#endif // #ifdef __clang__

#endif // #ifndef HIDE_DALI_INTERNALS

#endif // DALI_MAP_WRAPPER_H
