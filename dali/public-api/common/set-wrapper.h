#ifndef __DALI_SET_H__
#define __DALI_SET_H__

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

#ifndef HIDE_DALI_INTERNALS

# include <set>

#else

// ensure that default visibility is used with any class that is used as an exception type
# include <memory>
# include <new>
# include <stdexcept>

# include <bits/c++config.h>
# include <bits/stl_tree.h>
# undef _GLIBCXX_VISIBILITY_ATTR
# define _GLIBCXX_VISIBILITY_ATTR(V) __attribute__ ((__visibility__ ("hidden")))
# include <set>
# undef _GLIBCXX_VISIBILITY_ATTR
# define _GLIBCXX_VISIBILITY_ATTR(V) __attribute__ ((__visibility__ (#V))) // restore `_GLIBCXX_VISIBILITY_ATTR`

#endif //ifndef HIDE_DALI_INTERNALS

#endif /* __DALI_SET_H__ */
