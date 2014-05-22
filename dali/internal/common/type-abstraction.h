#ifndef __DALI_INTERNAL_TYPE_ABSTRACTION_H__
#define __DALI_INTERNAL_TYPE_ABSTRACTION_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/ref-object.h>
#include <dali/internal/common/owner-pointer.h>

namespace Dali
{

namespace Internal
{

/**
 * Template helpers to strip of const reference and reference to prevent anyone
 * from passing references as message parameters
 */

// For basic types, they are always pass by copy
template <typename Type>
struct BasicType
{
  typedef Type HolderType;
  typedef Type PassingType;
  static PassingType PassObject( PassingType object ) { return object; }
};

// For complex types that are copied into the message,
// they are passed as const reference when they don't need to be copied
template <typename Type>
struct ComplexType
{
  typedef Type HolderType;
  typedef const Type& PassingType;
  static PassingType PassObject( PassingType object ) { return object; }
};

// For complex types that are owned by the message,
// They are passed as raw pointer and hold in an OwnerPointer
template <typename Type>
struct OwnedType
{
  typedef OwnerPointer<Type> HolderType;
  typedef Type* PassingType;
  static PassingType PassObject( HolderType& object ) { return object.Release(); }
};

// Default for Vector3 and other structures
template <class T> struct ParameterType : public ComplexType< T > {};

// For message owned parameters
template <class T> struct ParameterType< OwnerPointer<T> > : public OwnedType< T > {};

// Basic types types
template <typename T> struct ParameterType< T* > : public BasicType< T* > {};
template <typename T> struct ParameterType< const T* > : public BasicType< const T* > {};
template <> struct ParameterType< int > : public BasicType< int > {};
template <> struct ParameterType< unsigned int > : public BasicType< unsigned int > {};
template <> struct ParameterType< float > : public BasicType< float > {};
template <> struct ParameterType< bool > : public BasicType< bool > {};
template <> struct ParameterType< short int > : public BasicType< short int > {};

#if INT_MAX != LONG_MAX
template <> struct ParameterType< long > : public BasicType< long > {};
template <> struct ParameterType< unsigned long > : public BasicType< unsigned long > {};
#endif

//TODO: Passing intrusive pointers through messages is potentially dangerous,
//      this should be checked
template <typename T> struct ParameterType< IntrusivePtr<T> >
: public BasicType< IntrusivePtr<T> > {};

// poorly constructed types, types should not be defined as references
// this will trigger a compilation error.
template <class U> struct ParameterType< U& > {};
template <class U> struct ParameterType< const U& > {};

} //namespace Internal

} //namespace Dali

#endif // __DALI_INTERNAL_TYPE_ABSTRACTION_H__
