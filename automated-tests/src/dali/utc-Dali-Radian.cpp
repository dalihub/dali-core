#include <iostream>

#include <stdlib.h>
#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>

using namespace Dali;


void utc_dali_radian_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_radian_cleanup(void)
{
  test_return_value = TET_PASS;
}


// Positive test case for constructors
int UtcDaliRadianConstructors01(void)
{
  TestApplication application;

  // Default constructor, does not initialise the value
  Radian radian0( 0.0f );

  // Test float assignment operator
  radian0 = Math::PI;
  DALI_TEST_EQUALS( float(radian0), Math::PI, 0.001f, TEST_LOCATION );

  // Constructor from float value
  Radian radian1( Math::PI );
  DALI_TEST_EQUALS( float(radian1), Math::PI, 0.001f, TEST_LOCATION );

  // Constructor from a Degree
  Radian radian2( Degree( 180.0f ) );
  DALI_TEST_EQUALS( float(radian2), Math::PI, 0.001f, TEST_LOCATION );

  // Assignment from Degree
  Radian radian3( 0.0f );
  radian3 = Degree( 180.0f );
  DALI_TEST_EQUALS( float(radian3), Math::PI, 0.001f, TEST_LOCATION );
  END_TEST;
}

// Positive test case for comparison
int UtcDaliRadianComparison01(void)
{
  TestApplication application;

  // Comparison between radians
  Radian radian0( Math::PI_2 );
  Radian radian1( Math::PI_2 );
  Radian radian2( Math::PI );

  DALI_TEST_CHECK( radian0 == radian1 );
  DALI_TEST_CHECK( radian0 != radian2 );

  // Comparison between radian to degree
  Radian radian3( Math::PI );
  Radian radian4( Math::PI_2 );
  Degree degree0( 180.0f );

  DALI_TEST_CHECK( radian3 == Radian(degree0) );
  DALI_TEST_CHECK( radian4 != Radian(degree0) );

  // Comparison with float
  Radian radian5( Math::PI_2 );

  DALI_TEST_CHECK( radian5 == Math::PI_2 );
  DALI_TEST_CHECK( radian5 != Math::PI );

  END_TEST;
}


// test case for cast operators
int UtcDaliRadianCastOperators01(void)
{
  TestApplication application;  // Exceptions require TestApplication

  Radian radian0( Math::PI );

  const float& value0( radian0.radian );
  DALI_TEST_EQUALS( value0, Math::PI, 0.001f, TEST_LOCATION );

  radian0 = Math::PI_2;
  DALI_TEST_EQUALS( value0, Math::PI_2, 0.001f, TEST_LOCATION );

  float value1( radian0 );
  DALI_TEST_EQUALS( value1, Math::PI_2, 0.001f, TEST_LOCATION );

  radian0 = Math::PI;
  DALI_TEST_EQUALS( float(radian0), Math::PI, 0.001f, TEST_LOCATION );
  END_TEST;
}


int UtcDaliRadianCastOperatorEquals(void)
{
  TestApplication application;

  Radian a(Math::PI_2);
  Radian b(Math::PI_2);
  Radian c(Math::PI);

  DALI_TEST_EQUALS( a == a, true, TEST_LOCATION );
  DALI_TEST_EQUALS( a == b, true, TEST_LOCATION );
  DALI_TEST_EQUALS( a == c, false, TEST_LOCATION );
  DALI_TEST_EQUALS( Degree(c) == c, true, TEST_LOCATION );
  END_TEST;
}

int UtcDaliRadianCastOperatorNotEquals(void)
{
  TestApplication application;

  Radian a(Math::PI_2);
  Radian b(Math::PI_2);
  Radian c(Math::PI);

  DALI_TEST_EQUALS( a != a, false, TEST_LOCATION );
  DALI_TEST_EQUALS( a != b, false, TEST_LOCATION );
  DALI_TEST_EQUALS( a != c, true, TEST_LOCATION );
  DALI_TEST_EQUALS( Degree(a) != c, true, TEST_LOCATION );
  END_TEST;
}

int UtcDaliRadianCastOperatorLessThan(void)
{
  TestApplication application;

  Radian a(Math::PI_4);
  Radian b(Math::PI_2);
  Radian c(Math::PI);
  Radian d(2.0f*Math::PI);
  Radian e(-Math::PI);

  DALI_TEST_EQUALS(a < a, false, TEST_LOCATION);
  DALI_TEST_EQUALS(a < b, true, TEST_LOCATION);
  DALI_TEST_EQUALS(a < c, true, TEST_LOCATION);
  DALI_TEST_EQUALS(a < d, true, TEST_LOCATION);
  DALI_TEST_EQUALS(a < e, false, TEST_LOCATION);

  DALI_TEST_EQUALS(b < a, false, TEST_LOCATION);
  DALI_TEST_EQUALS(b < b, false, TEST_LOCATION);
  DALI_TEST_EQUALS(c < b, false, TEST_LOCATION);
  DALI_TEST_EQUALS(d < b, false, TEST_LOCATION);
  DALI_TEST_EQUALS(e < b, true, TEST_LOCATION);

  DALI_TEST_EQUALS( Radian(Math::PI_2) < Degree(180.0f), true,  TEST_LOCATION);
  DALI_TEST_EQUALS( Radian(Math::PI_2) < Degree(90.0f),  false, TEST_LOCATION);
  DALI_TEST_EQUALS( Radian(Math::PI_2) > Degree(45.0f),  true,  TEST_LOCATION);

  DALI_TEST_EQUALS( Degree(180.0f) > Radian(Math::PI_2), true,  TEST_LOCATION);
  DALI_TEST_EQUALS( Degree(90.0f)  > Radian(Math::PI_2), false, TEST_LOCATION);
  DALI_TEST_EQUALS( Degree(45.0f)  < Radian(Math::PI_2), true,  TEST_LOCATION);

  END_TEST;
}
