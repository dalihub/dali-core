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
TEST_FUNCTION( UtcDaliDegreeConstructors01,        POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliDegreeComparison01,          POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliDegreeCastOperators01,       POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliDegreeCastOperatorEquals,    POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliDegreeCastOperatorNotEquals, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliDegreeCastOperatorLessThan,  POSITIVE_TC_IDX );

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}


// Positive test case for constructors
static void UtcDaliDegreeConstructors01()
{
  TestApplication application;

  // Default constructor, does not initialise the value
  Degree degree0( 0.0f );

  // Test float assignment operator
  degree0 = 180.0f;
  DALI_TEST_EQUALS( float(degree0), 180.0f, 0.001f, TEST_LOCATION );

  // Constructor from float value
  Degree degree1( 180.0f );
  DALI_TEST_EQUALS( float(degree1), 180.0f, 0.001f, TEST_LOCATION );

  // Constructor from a Radian
  Degree degree2( Radian( Math::PI ) );
  DALI_TEST_EQUALS( float(degree2), 180.0f, 0.001f, TEST_LOCATION );

  // Assignment from Radian
  Degree degree3( 0.0f );
  degree3 = Radian( Math::PI );
  DALI_TEST_EQUALS( float(degree3), 180.0f, 0.001f, TEST_LOCATION );
}

// Positive test case for comparison
static void UtcDaliDegreeComparison01()
{
  TestApplication application;

  // Comparison between radians
  Degree degree0( 90.0f );
  Degree degree1( 90.0f );
  Degree degree2( 180.0f );

  DALI_TEST_CHECK( degree0 == degree1 );
  DALI_TEST_CHECK( degree0 != degree2 );

  // Comparison between radian to degree
  Degree degree3( 180.0f );
  Degree degree4( 90.0f );
  Radian radian0( Math::PI );

  DALI_TEST_CHECK( degree3 == radian0 );
  DALI_TEST_CHECK( degree4 != radian0 );

  // Comparison with float
  Degree degree5( 90.0f );

  DALI_TEST_CHECK( degree5 == 90.0f );
  DALI_TEST_CHECK( degree5 != 180.0f );

}


// test case for cast operators
static void UtcDaliDegreeCastOperators01()
{
  TestApplication application;  // Exceptions require TestApplication

  Degree degree0( 180.0f );

  const float& value0( degree0 );
  DALI_TEST_EQUALS( value0, 180.0f, 0.001f, TEST_LOCATION );

  degree0 = 90.0f;
  DALI_TEST_EQUALS( value0, 90.0f, 0.001f, TEST_LOCATION );

  float& value1( degree0 );
  DALI_TEST_EQUALS( value1, 90.0f, 0.001f, TEST_LOCATION );

  value1 = 180.0f;
  DALI_TEST_EQUALS( float(degree0), 180.0f, 0.001f, TEST_LOCATION );
}



static void UtcDaliDegreeCastOperatorEquals()
{
  TestApplication application;

  Degree a(90.0f);
  Degree b(90.0f);
  Degree c(180.0f);

  DALI_TEST_EQUALS(a == a, true, TEST_LOCATION);
  DALI_TEST_EQUALS(a == b, true, TEST_LOCATION);
  DALI_TEST_EQUALS(a == c, false, TEST_LOCATION);
}

static void UtcDaliDegreeCastOperatorNotEquals()
{
  TestApplication application;

  Degree a(90.0f);
  Degree b(90.0f);
  Degree c(180.0f);

  DALI_TEST_EQUALS(a != a, false, TEST_LOCATION);
  DALI_TEST_EQUALS(a != b, false, TEST_LOCATION);
  DALI_TEST_EQUALS(a != c, true, TEST_LOCATION);
}

static void UtcDaliDegreeCastOperatorLessThan()
{
  TestApplication application;

  Degree a(45.0f);
  Degree b(90.0f);
  Degree c(180.0f);
  Degree d(360.0f);
  Degree e(-180.0f);

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
}
