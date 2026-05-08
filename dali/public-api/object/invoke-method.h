#ifndef DALI_INVOKE_METHOD_H
#define DALI_INVOKE_METHOD_H

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
 */

#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/object/any.h>

namespace Dali
{

/**
 * @brief Container used to pass arguments to InvokeMethod().
 *
 * Each element is a Dali::Any containing one argument for the target method.
 * Arguments must be appended in the same order as the public API method
 * declaration. The generated wrapper uses Dali::AnyCast with the expected value
 * type and fails the invocation if the count or any type does not match.
 *
 * For public DALi APIs, generated wrappers are emitted for supported methods on
 * exported Dali::BaseHandle-derived classes. The target object's runtime type
 * must be registered in the TypeRegistry; otherwise InvokeMethod() fails
 * without invoking anything.
 *
 * Invokable methods should use DALi API types or simple primitive types that can
 * be copied into Dali::Any. Output parameters and non-const reference parameters
 * are not supported by the generated wrappers. APIs using raw pointers,
 * callbacks, functors, std public types, signal objects, or incomplete
 * forward-declared value types are excluded from wrapper generation.
 *
 * For overloaded methods, wrappers select a candidate by argument count and
 * exact Dali::Any value types. Callers must provide values using the same value
 * types expected by the generated wrapper; C++ implicit conversions are not
 * applied at runtime. Trailing default parameters may be omitted when the
 * generated overload set is unambiguous.
 */
using InvokeArguments = Dali::Vector<Dali::Any>;

/**
 * @brief Result value returned from InvokeMethod().
 *
 * If an invoked method has a non-void return type, the generated wrapper stores
 * the returned value in this Dali::Any. Callers should use Dali::AnyCast with
 * the expected return type. For void methods, the result is reset to an empty
 * Dali::Any.
 */
using InvokeResult = Dali::Any;

} // namespace Dali

#endif // DALI_INVOKE_METHOD_H
