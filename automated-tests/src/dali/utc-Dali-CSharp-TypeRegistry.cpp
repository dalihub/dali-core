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

#include <iostream>
#include <stdlib.h>
#include <limits>
#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>
#include <dali/devel-api/object/csharp-type-info.h>
#include <dali/devel-api/object/csharp-type-registry.h>
#include <dali/internal/event/common/type-registry-impl.h>
#include <dali/public-api/object/property.h>
using namespace Dali;


namespace
{


static bool CreateCustomNamedInitCalled = false;


BaseHandle* CreateCustomNamedInit(const char* const typeName )
{
  CreateCustomNamedInitCalled = true;

  BaseHandle* x = new BaseHandle();

  return x;
}

// Property Registration
bool setPropertyCalled = false;
bool getPropertyCalled = false;
int intPropertyValue = 0;

void SetProperty( BaseObject* object, const char* const propertyName , Property::Value* value )
{

  value->Get( intPropertyValue );

  setPropertyCalled = true;
}

Property::Value* GetProperty( BaseObject* object, const char* const propertyName )
{
  getPropertyCalled = true;
  Property::Value* x = new Property::Value( 10 );
  return x;
}

}

int UtcDaliRegisterCSharpTypeP(void)
{

  TestApplication application;

  CSharpTypeRegistry::RegisterType( "CSharpControl", typeid( Dali::Actor), &CreateCustomNamedInit  );

  Dali::TypeInfo info = Dali::TypeRegistry::Get().GetTypeInfo( "CSharpControl" );

  info.CreateInstance();

  DALI_TEST_EQUALS( CreateCustomNamedInitCalled, true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRegisterCSharpTypeNoInitP(void)
{

  TestApplication application;

  CreateCustomNamedInitCalled = false;

  CSharpTypeRegistry::RegisterType( "CSharpControl", typeid( Dali::Actor), &CreateCustomNamedInit );

  GetImplementation(Dali::TypeRegistry::Get()).CallInitFunctions();

  DALI_TEST_EQUALS( CreateCustomNamedInitCalled, false, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRegisterCSharpTypeN(void)
{
  TestApplication application;

  CSharpTypeRegistry::RegisterType( "CSharpControl", typeid( Dali::Actor), &CreateCustomNamedInit );

  // should cause an assert because we're registering same type twice
  try
  {
    CSharpTypeRegistry::RegisterType( "CSharpControl", typeid( Dali::Actor), &CreateCustomNamedInit );
    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "Duplicate type name in Type Registration", TEST_LOCATION );
  }

  END_TEST;
}

int UtcDaliRegisterCSharpTypeCreateP(void)
{

  TestApplication application;
  CreateCustomNamedInitCalled = false;

  CSharpTypeRegistry::RegisterType( "CSharpControl", typeid( Dali::Actor), &CreateCustomNamedInit );


  TypeInfo info = Dali::TypeRegistry::Get().GetTypeInfo( "CSharpControl");

  BaseHandle handle = info.CreateInstance();

  DALI_TEST_EQUALS( CreateCustomNamedInitCalled, true, TEST_LOCATION );

  END_TEST;
}



int UtcDaliRegisterCSharpPropertyP(void)
{
  TestApplication application;

  CSharpTypeRegistry::RegisterType( "DateControl", typeid( Dali::Actor), &CreateCustomNamedInit );


  bool registered = CSharpTypeRegistry::RegisterProperty( "DateControl",
                    "year",
                    10000 + 1,
                    Property::INTEGER,
                    SetProperty,
                    GetProperty );

  DALI_TEST_EQUALS( registered, true, TEST_LOCATION );


  END_TEST;
}


int UtcDaliRegisterCSharpPropertyN(void)
{
  TestApplication application;

  // register the same property twice
  CSharpTypeRegistry::RegisterType( "DateControl", typeid( Dali::Actor), &CreateCustomNamedInit );


  bool registered = CSharpTypeRegistry::RegisterProperty( "DateControl",
                    "year",
                    10000 + 1,
                    Property::INTEGER,
                    SetProperty,
                    GetProperty );

  DALI_TEST_EQUALS( registered, true, TEST_LOCATION );


  // should fail second time with an assert as the property is already registered
  try
  {
    registered = CSharpTypeRegistry::RegisterProperty( "DateControl",
                      "year",
                      10000 + 1,
                      Property::INTEGER,
                      SetProperty,
                      GetProperty );

    tet_result( TET_FAIL );
  }
  catch ( DaliException& e )
  {
    DALI_TEST_ASSERT( e, "Property index already added to Type", TEST_LOCATION );
  }

  END_TEST;
}

int UtcDaliRegisterCSharpPropertySetP(void)
{
  TestApplication application;

   // register the same property twice
   CSharpTypeRegistry::RegisterType( "DateControl", typeid( Dali::Actor), &CreateCustomNamedInit );;

   Property::Index index(100001);

   CSharpTypeRegistry::RegisterProperty( "DateControl",
                     "year",
                     index,
                     Property::INTEGER,
                     SetProperty,
                     GetProperty );

   TypeRegistry typeRegistry = TypeRegistry::Get();

   TypeInfo typeInfo = TypeRegistry::Get().GetTypeInfo( "DateControl" );


   // Check the property is writable in the type registry
   Internal::TypeInfo& typeInfoImpl = GetImplementation( typeInfo );

   Property::Value value(25);

   typeInfoImpl.SetProperty( NULL, index, value );

   DALI_TEST_EQUALS( 25, intPropertyValue , TEST_LOCATION );

   Property::Value value2(50);

   typeInfoImpl.SetProperty( NULL, "year", value2 );

   DALI_TEST_EQUALS( 50, intPropertyValue , TEST_LOCATION );

   DALI_TEST_EQUALS( setPropertyCalled, true, TEST_LOCATION );

   END_TEST;
}


int UtcDaliRegisterCSharpPropertyGetP(void)
{
  TestApplication application;

   // register the same property twice
   CSharpTypeRegistry::RegisterType( "DateControl", typeid( Dali::Actor), &CreateCustomNamedInit );

   Property::Index index(100001);

   CSharpTypeRegistry::RegisterProperty( "DateControl",
                     "year",
                     index,
                     Property::INTEGER,
                     SetProperty,
                     GetProperty );

   TypeRegistry typeRegistry = TypeRegistry::Get();

   TypeInfo typeInfo = TypeRegistry::Get().GetTypeInfo( "DateControl" );


   // Check the property is writable in the type registry
   Internal::TypeInfo& typeInfoImpl = GetImplementation( typeInfo );

   Property::Value value(1);

   value = typeInfoImpl.GetProperty( NULL, index );

   int propValue;
   value.Get( propValue );

   DALI_TEST_EQUALS( getPropertyCalled, true, TEST_LOCATION );
   DALI_TEST_EQUALS( propValue, 10, TEST_LOCATION );

   value = typeInfoImpl.GetProperty( NULL, "year");
   value.Get( propValue );

   DALI_TEST_EQUALS( propValue, 10, TEST_LOCATION );

   END_TEST;
}

int UtcDaliRegisterCSharpPropertyNotRegisteredN(void)
{
  TestApplication application;

  // control not registered, should fail
  bool registered = CSharpTypeRegistry::RegisterProperty( "DateControl",
                    "year",
                    10000 + 1,
                    Property::INTEGER,
                    SetProperty,
                    GetProperty );

  DALI_TEST_EQUALS( registered, false, TEST_LOCATION );

  END_TEST;
}

