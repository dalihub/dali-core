#ifndef __DALI_MAP_H__
#define __DALI_MAP_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>


#ifndef HIDE_DALI_INTERNALS

# include <map>

#else

// ensure that default visibility is used with any class that is used as an exception type
# include <memory>
# include <new>
# include <stdexcept>

# include <bits/c++config.h>
# include <bits/stl_tree.h>
# undef _GLIBCXX_VISIBILITY_ATTR
# define _GLIBCXX_VISIBILITY_ATTR(V) __attribute__ ((__visibility__ ("hidden")))
# include <map>
# undef _GLIBCXX_VISIBILITY_ATTR
# define _GLIBCXX_VISIBILITY_ATTR(V) __attribute__ ((__visibility__ (#V))) // restore `_GLIBCXX_VISIBILITY_ATTR`

#endif //ifndef HIDE_DALI_INTERNALS


#endif /* __DALI_MAP_H__ */
