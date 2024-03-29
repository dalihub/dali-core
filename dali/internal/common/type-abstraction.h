#ifndef DALI_INTERNAL_TYPE_ABSTRACTION_H
#define DALI_INTERNAL_TYPE_ABSTRACTION_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <climits>

// INTERNAL INCLUDES
#include <dali/internal/common/owner-pointer.h>
#include <dali/public-api/object/ref-object.h>

namespace Dali
{
namespace Internal
{
/**
 * Template helpers to strip of const reference and reference to prevent anyone
 * from passing references as message parameters
 */

// For basic types, they are always pass by copy
template<typename Type>
struct BasicType
{
  using HolderType  = Type;
  using PassingType = Type;
};

// For complex types that are copied into the message,
// they are passed as const reference when they don't need to be copied
template<typename Type>
struct ComplexType
{
  using HolderType  = Type;
  using PassingType = const Type&;
};

// For complex types that are owned by the message,
// They are passed and hold in an OwnerPointer
template<typename Type>
struct OwnedType
{
  using HolderType  = OwnerPointer<Type>;
  using PassingType = OwnerPointer<Type>&;
};

// Default for Vector3 and other structures
template<class T>
struct ParameterType : public ComplexType<T>
{
};

// For message owned parameters
template<class T>
struct ParameterType<OwnerPointer<T> > : public OwnedType<T>
{
};

// Basic types types
template<typename T>
struct ParameterType<T*> : public BasicType<T*>
{
};
template<typename T>
struct ParameterType<const T*> : public BasicType<const T*>
{
};
template<>
struct ParameterType<int> : public BasicType<int>
{
};
template<>
struct ParameterType<unsigned int> : public BasicType<unsigned int>
{
};
template<>
struct ParameterType<float> : public BasicType<float>
{
};
template<>
struct ParameterType<bool> : public BasicType<bool>
{
};
template<>
struct ParameterType<short int> : public BasicType<short int>
{
};
template<>
struct ParameterType<unsigned short int> : public BasicType<unsigned short int>
{
};

#if SCHAR_MAX != SHRT_MAX
template<>
struct ParameterType<char> : public BasicType<char>
{
};
template<>
struct ParameterType<unsigned char> : public BasicType<unsigned char>
{
};
#endif

#if INT_MAX != LONG_MAX
template<>
struct ParameterType<long> : public BasicType<long>
{
};
template<>
struct ParameterType<unsigned long> : public BasicType<unsigned long>
{
};
#endif

//TODO: Passing intrusive pointers through messages is potentially dangerous,
//      this should be checked
template<typename T>
struct ParameterType<IntrusivePtr<T> >
: public BasicType<IntrusivePtr<T> >
{
};

// poorly constructed types, types should not be defined as references
// this will trigger a compilation error.
template<class U>
struct ParameterType<U&>
{
};
template<class U>
struct ParameterType<const U&>
{
};

} //namespace Internal

} //namespace Dali

#endif // DALI_INTERNAL_TYPE_ABSTRACTION_H
