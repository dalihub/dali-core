#include <iostream>

#include <stdlib.h>
#include <tet_api.h>

#include <dali/public-api/dali-core.h>

#include <dali-test-suite-utils.h>

using namespace Dali;

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

#define MAX_NUMBER_OF_TESTS 10000
extern "C" {
  struct tet_testlist tet_testlist[MAX_NUMBER_OF_TESTS];
}

// Add test functionality for all APIs in the class (Positive and Negative)
TEST_FUNCTION( UtcDaliRadianConstructors01,        POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliRadianComparison01,          POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliRadianCastOperators01,       POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliRadianCastOperatorEquals,    POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliRadianCastOperatorNotEquals, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliRadianCastOperatorLessThan,  POSITIVE_TC_IDX );


// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}


// Positive test case for constructors
static void UtcDaliRadianConstructors01()
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
}

// Positive test case for comparison
static void UtcDaliRadianComparison01()
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

  DALI_TEST_CHECK( radian3 == degree0 );
  DALI_TEST_CHECK( radian4 != degree0 );

  // Comparison with float
  Radian radian5( Math::PI_2 );

  DALI_TEST_CHECK( radian5 == Math::PI_2 );
  DALI_TEST_CHECK( radian5 != Math::PI );

}


// test case for cast operators
static void UtcDaliRadianCastOperators01()
{
  TestApplication application;  // Exceptions require TestApplication

  Radian radian0( Math::PI );

  const float& value0( radian0 );
  DALI_TEST_EQUALS( value0, Math::PI, 0.001f, TEST_LOCATION );

  radian0 = Math::PI_2;
  DALI_TEST_EQUALS( value0, Math::PI_2, 0.001f, TEST_LOCATION );

  float& value1( radian0 );
  DALI_TEST_EQUALS( value1, Math::PI_2, 0.001f, TEST_LOCATION );

  value1 = Math::PI;
  DALI_TEST_EQUALS( float(radian0), Math::PI, 0.001f, TEST_LOCATION );
}


static void UtcDaliRadianCastOperatorEquals()
{
  TestApplication application;

  Radian a(Math::PI_2);
  Radian b(Math::PI_2);
  Radian c(Math::PI);

  DALI_TEST_EQUALS(a == a, true, TEST_LOCATION);
  DALI_TEST_EQUALS(a == b, true, TEST_LOCATION);
  DALI_TEST_EQUALS(a == c, false, TEST_LOCATION);
}

static void UtcDaliRadianCastOperatorNotEquals()
{
  TestApplication application;

  Radian a(Math::PI_2);
  Radian b(Math::PI_2);
  Radian c(Math::PI);

  DALI_TEST_EQUALS(a != a, false, TEST_LOCATION);
  DALI_TEST_EQUALS(a != b, false, TEST_LOCATION);
  DALI_TEST_EQUALS(a != c, true, TEST_LOCATION);
}

static void UtcDaliRadianCastOperatorLessThan()
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

  DALI_TEST_EQUALS(Radian(Math::PI_2) < Degree(180.0f), true,  TEST_LOCATION);
  DALI_TEST_EQUALS(Radian(Math::PI_2) < Degree(90.0f),  false, TEST_LOCATION);
  DALI_TEST_EQUALS(Radian(Math::PI_2) < Degree(45.0f),  false, TEST_LOCATION);
}
