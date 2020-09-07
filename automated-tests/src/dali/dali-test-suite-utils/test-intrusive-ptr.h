#ifndef TEST_INTRUSIVE_PTR_H
#define TEST_INTRUSIVE_PTR_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali-test-suite-utils.h>
#include <dali/public-api/dali-core.h>
#include <stdlib.h>

#include <iostream>

namespace Dali
{
template<typename T>
struct UtcCoverageIntrusivePtr
{
  typedef IntrusivePtr<T> (*Creator)();

  void Check(Creator creator)
  {
    IntrusivePtr<T> a = creator();
    IntrusivePtr<T> b = creator();

    DALI_TEST_CHECK(a.Get());

    a.Reset();

    T* pB = b.Detach();

    a.Reset(pB);

    DALI_TEST_CHECK(a);

    a.Reset();
  };
};

} // namespace Dali

#endif // TEST_INTRUSIVE_PTR_H
