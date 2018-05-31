/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

int UtcDaliPropertyTypesGetNameP(void)
{
  DALI_TEST_EQUALS( "NONE",               Dali::PropertyTypes::GetName(Property::NONE               ), TEST_LOCATION );
  DALI_TEST_EQUALS( "BOOLEAN",            Dali::PropertyTypes::GetName(Property::BOOLEAN            ), TEST_LOCATION );
  DALI_TEST_EQUALS( "FLOAT",              Dali::PropertyTypes::GetName(Property::FLOAT              ), TEST_LOCATION );
  DALI_TEST_EQUALS( "INTEGER",            Dali::PropertyTypes::GetName(Property::INTEGER            ), TEST_LOCATION );
  DALI_TEST_EQUALS( "VECTOR2",            Dali::PropertyTypes::GetName(Property::VECTOR2            ), TEST_LOCATION );
  DALI_TEST_EQUALS( "VECTOR3",            Dali::PropertyTypes::GetName(Property::VECTOR3            ), TEST_LOCATION );
  DALI_TEST_EQUALS( "VECTOR4",            Dali::PropertyTypes::GetName(Property::VECTOR4            ), TEST_LOCATION );
  DALI_TEST_EQUALS( "MATRIX3",            Dali::PropertyTypes::GetName(Property::MATRIX3            ), TEST_LOCATION );
  DALI_TEST_EQUALS( "MATRIX",             Dali::PropertyTypes::GetName(Property::MATRIX             ), TEST_LOCATION );
  DALI_TEST_EQUALS( "RECTANGLE",          Dali::PropertyTypes::GetName(Property::RECTANGLE          ), TEST_LOCATION );
  DALI_TEST_EQUALS( "ROTATION",           Dali::PropertyTypes::GetName(Property::ROTATION           ), TEST_LOCATION );
  DALI_TEST_EQUALS( "STRING",             Dali::PropertyTypes::GetName(Property::STRING             ), TEST_LOCATION );
  DALI_TEST_EQUALS( "ARRAY",              Dali::PropertyTypes::GetName(Property::ARRAY              ), TEST_LOCATION );
  DALI_TEST_EQUALS( "MAP",                Dali::PropertyTypes::GetName(Property::MAP                ), TEST_LOCATION );
  END_TEST;
}

int UtcDaliPropertyTypesGet02P(void)
{
  DALI_TEST_CHECK( Dali::PropertyTypes::Get<bool>() == Property::BOOLEAN );
  END_TEST;
}

int UtcDaliPropertyTypesGet03P(void)
{
  DALI_TEST_CHECK( Dali::PropertyTypes::Get<float>() == Property::FLOAT );
  END_TEST;
}

int UtcDaliPropertyTypesGet04P(void)
{
  DALI_TEST_CHECK( Dali::PropertyTypes::Get<int>() == Property::INTEGER );
  END_TEST;
}

int UtcDaliPropertyTypesGet06P(void)
{
  DALI_TEST_CHECK( Dali::PropertyTypes::Get<Dali::Vector2>() == Property::VECTOR2 );
  END_TEST;
}

int UtcDaliPropertyTypesGet07P(void)
{
  DALI_TEST_CHECK( Dali::PropertyTypes::Get<Dali::Vector3>() == Property::VECTOR3 );
  END_TEST;
}

int UtcDaliPropertyTypesGet08P(void)
{
  DALI_TEST_CHECK( Dali::PropertyTypes::Get<Dali::Vector4>() == Property::VECTOR4 );
  END_TEST;
}

int UtcDaliPropertyTypesGet09P(void)
{
  DALI_TEST_CHECK( Dali::PropertyTypes::Get<Dali::Matrix3>() == Property::MATRIX3 );
  END_TEST;
}

int UtcDaliPropertyTypesGet10(void)
{
  DALI_TEST_CHECK( Dali::PropertyTypes::Get<Dali::Matrix>() == Property::MATRIX );
  END_TEST;
}

int UtcDaliPropertyTypesGet11P(void)
{
  DALI_TEST_CHECK( Dali::PropertyTypes::Get<Dali::AngleAxis>() == Property::ROTATION );
  END_TEST;
}

int UtcDaliPropertyTypesGet12P(void)
{
  DALI_TEST_CHECK( Dali::PropertyTypes::Get<Dali::Quaternion>() == Property::ROTATION );
  END_TEST;
}

int UtcDaliPropertyTypesGet13P(void)
{
  DALI_TEST_CHECK( Dali::PropertyTypes::Get<std::string>() == Property::STRING );
  END_TEST;
}

int UtcDaliPropertyTypesGet14P(void)
{
  DALI_TEST_CHECK( Dali::PropertyTypes::Get<Dali::Rect<int> >() == Property::RECTANGLE );
  END_TEST;
}

int UtcDaliPropertyTypesGet15P(void)
{
  DALI_TEST_CHECK( Dali::PropertyTypes::Get<Dali::Property::Map>() == Property::MAP );
  END_TEST;
}

int UtcDaliPropertyTypesGet16P(void)
{
  DALI_TEST_CHECK( Dali::PropertyTypes::Get<Dali::Property::Array>() == Property::ARRAY );
  END_TEST;
}
