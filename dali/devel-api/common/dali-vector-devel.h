#ifndef DALI_VECTOR_DEVEL_H
#define DALI_VECTOR_DEVEL_H

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

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>

namespace Dali
{
/**
 * Dali::Vector support for C++11 Range-based for loop: for( item : container )
 *
 * @param vector The vector to iterate
 * @return The start iterator
 */
template< class T >
typename T::Iterator begin( T& vector )
{
    return vector.Begin();
}

/**
 * Dali::Vector support for C++11 Range-based for loop: for( item : container )
 *
 * @param vector The vector to iterate
 * @return The end iterator
 */
template< class T >
typename T::Iterator end( T& vector )
{
    return vector.End();
}

} // namespace Dali

#endif /* DALI_VECTOR_DEVEL_H */
