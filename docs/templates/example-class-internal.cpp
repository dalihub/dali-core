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

// Class header
#include "example-class-internal.h"

// System includes
#include <algorithm>

// Dali includes
#include "actor.h"

namespace // unnamed namespace start
{
const int gExampleConstant = 0;

void ExampleNonMemberFunction()
{
}

} // namespace

namespace Dali
{
namespace Internal
{
ExampleClass::ExampleClass()
{
}

ExampleClass::ExampleClass(const string& name)
: mName(name)
{
}

ExampleClass::~ExampleClass()
{
}

void ExampleClass::SetName(const string& name)
{
  mName = name;

  // Notify derived classes
  OnNameSet();
}

void ExampleClass::HelperMember(ExampleEnum exampleParam)
{
  // TODO
}

} // namespace Internal

} // namespace Dali
