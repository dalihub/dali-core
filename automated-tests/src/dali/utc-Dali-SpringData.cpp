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
 *
 */

#include <dali-test-suite-utils.h>
#include <dali/public-api/dali-core.h>
#include <stdlib.h>

#include <iostream>

using namespace Dali;

void utc_dali_spring_data_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_spring_data_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliSpringDataDefaultConstructorP(void)
{
  TestApplication application;
  SpringData      springData;

  //Should return default values
  DALI_TEST_EQUALS(springData.GetStiffness(), 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(springData.GetDamping(), 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(springData.GetMass(), 0.1f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSpringDataConstructorWithParametersP(void)
{
  TestApplication application;

  float stiffness = 100.0f;
  float damping   = 20.0f;
  float mass      = 1.0f;

  SpringData springData(stiffness, damping, mass);

  DALI_TEST_EQUALS(springData.GetStiffness(), stiffness, TEST_LOCATION);
  DALI_TEST_EQUALS(springData.GetDamping(), damping, TEST_LOCATION);
  DALI_TEST_EQUALS(springData.GetMass(), mass, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSpringDataConstructorWithParametersN(void)
{
  TestApplication application;

  float stiffness = -10.0f;
  float damping   = -5.0f;
  float mass      = -1.0f;

  SpringData springData(stiffness, damping, mass);

  //Values should be clamped to minimum 0.1f
  DALI_TEST_EQUALS(springData.GetStiffness(), 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(springData.GetDamping(), 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(springData.GetMass(), 0.1f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSpringDataSetStiffnessP(void)
{
  TestApplication application;
  SpringData      springData(100.0f, 20.0f, 1.0f);

  float newStiffness = 300.0f;
  springData.SetStiffness(newStiffness);

  DALI_TEST_EQUALS(springData.GetStiffness(), newStiffness, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSpringDataSetStiffnessN(void)
{
  TestApplication application;
  SpringData      springData(100.0f, 20.0f, 1.0f);

  float newStiffness = -10.0f;
  springData.SetStiffness(newStiffness);

  //Value should be clamped to minimum 0.1f
  DALI_TEST_EQUALS(springData.GetStiffness(), 0.1f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSpringDataSetDampingP(void)
{
  TestApplication application;
  SpringData      springData(100.0f, 20.0f, 1.0f);

  float newDamping = 30.0f;
  springData.SetDamping(newDamping);

  DALI_TEST_EQUALS(springData.GetDamping(), newDamping, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSpringDataSetDampingN(void)
{
  TestApplication application;
  SpringData      springData(100.0f, 20.0f, 1.0f);

  float newDamping = -5.0f;
  springData.SetDamping(newDamping);

  //Value should be clamped to minimum 0.1f
  DALI_TEST_EQUALS(springData.GetDamping(), 0.1f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSpringDataSetMassP(void)
{
  TestApplication application;
  SpringData      springData(100.0f, 20.0f, 1.0f);

  float newMass = 2.0f;
  springData.SetMass(newMass);

  DALI_TEST_EQUALS(springData.GetMass(), newMass, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSpringDataSetMassN(void)
{
  TestApplication application;
  SpringData      springData(100.0f, 20.0f, 1.0f);

  float newMass = -1.0f;
  springData.SetMass(newMass);

  //Value should be clamped to minimum 0.1f
  DALI_TEST_EQUALS(springData.GetMass(), 0.1f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSpringDataGetDurationP(void)
{
  TestApplication application;
  SpringData      springData(100.0f, 20.0f, 1.0f);

  float duration = springData.GetDuration();

  //Duration should be a positive value
  DALI_TEST_CHECK(duration > 0.0f);

  END_TEST;
}

int UtcDaliSpringDataCopyConstructorP(void)
{
  TestApplication application;

  SpringData springData1(100.0f, 20.0f, 1.0f);
  SpringData springData2(springData1);

  //Check that values were copied correctly
  DALI_TEST_EQUALS(springData2.GetStiffness(), springData1.GetStiffness(), TEST_LOCATION);
  DALI_TEST_EQUALS(springData2.GetDamping(), springData1.GetDamping(), TEST_LOCATION);
  DALI_TEST_EQUALS(springData2.GetMass(), springData1.GetMass(), TEST_LOCATION);

  //Modify original and check that copy is independent
  springData1.SetStiffness(200.0f);
  DALI_TEST_EQUALS(springData2.GetStiffness(), 100.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSpringDataMoveConstructorP(void)
{
  TestApplication application;

  SpringData springData1(100.0f, 20.0f, 1.0f);
  SpringData springData2(std::move(springData1));

  //Check that values were moved correctly
  DALI_TEST_EQUALS(springData2.GetStiffness(), 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(springData2.GetDamping(), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(springData2.GetMass(), 1.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSpringDataCopyAssignmentP(void)
{
  TestApplication application;

  SpringData springData1(100.0f, 20.0f, 1.0f);
  SpringData springData2(200.0f, 30.0f, 2.0f);

  springData2 = springData1;

  //Check that values were copied correctly
  DALI_TEST_EQUALS(springData2.GetStiffness(), springData1.GetStiffness(), TEST_LOCATION);
  DALI_TEST_EQUALS(springData2.GetDamping(), springData1.GetDamping(), TEST_LOCATION);
  DALI_TEST_EQUALS(springData2.GetMass(), springData1.GetMass(), TEST_LOCATION);

  //Modify original and check that copy is independent
  springData1.SetStiffness(200.0f);
  DALI_TEST_EQUALS(springData2.GetStiffness(), 100.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSpringDataMoveAssignmentP(void)
{
  TestApplication application;

  SpringData springData1(100.0f, 20.0f, 1.0f);
  SpringData springData2(200.0f, 30.0f, 2.0f);

  springData2 = std::move(springData1);

  //Check that values were moved correctly
  DALI_TEST_EQUALS(springData2.GetStiffness(), 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(springData2.GetDamping(), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(springData2.GetMass(), 1.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSpringDataSelfAssignmentP(void)
{
  TestApplication application;

  SpringData springData(100.0f, 20.0f, 1.0f);

  //Self assignment should not cause issues
  springData = springData;

  //Check that values remain unchanged
  DALI_TEST_EQUALS(springData.GetStiffness(), 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(springData.GetDamping(), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(springData.GetMass(), 1.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSpringDataGetDurationUnderdampedP(void)
{
  TestApplication application;

  // Underdamped case: zeta < 1.0 (oscillatory motion)
  // For zeta = 0.3, we need damping such that: damping < 2 * sqrt(stiffness * mass)
  SpringData springData(100.0f, 5.0f, 1.0f);

  float duration = springData.GetDuration();

  //Duration should be positive for underdamped case
  DALI_TEST_CHECK(duration > 0.0f);

  END_TEST;
}

int UtcDaliSpringDataGetDurationCriticallyDampedP(void)
{
  TestApplication application;

  // Critically damped case: zeta = 1.0
  // For zeta = 1.0, we need: damping = 2 * sqrt(stiffness * mass)
  float stiffness = 100.0f;
  float mass      = 1.0f;
  float damping   = 2.0f * std::sqrt(stiffness * mass);

  SpringData springData(stiffness, damping, mass);

  float duration = springData.GetDuration();

  //Duration should be positive for critically damped case
  DALI_TEST_CHECK(duration > 0.0f);

  END_TEST;
}

int UtcDaliSpringDataGetDurationOverdampedP(void)
{
  TestApplication application;

  // Overdamped case: zeta > 1.0 (no oscillation, slow settling)
  // For zeta = 2.0, we need: damping > 2 * sqrt(stiffness * mass)
  SpringData springData(100.0f, 40.0f, 1.0f);

  float duration = springData.GetDuration();

  //Duration should be positive for overdamped case
  DALI_TEST_CHECK(duration > 0.0f);

  END_TEST;
}

int UtcDaliSpringDataSetDampingBoundaryValuesP(void)
{
  TestApplication application;
  SpringData      springData(100.0f, 20.0f, 1.0f);

  // Test setting damping to minimum value
  springData.SetDamping(0.1f);
  DALI_TEST_EQUALS(springData.GetDamping(), 0.1f, TEST_LOCATION);

  // Test setting damping to value just above minimum
  springData.SetDamping(0.11f);
  DALI_TEST_EQUALS(springData.GetDamping(), 0.11f, TEST_LOCATION);

  // Test setting damping to value just below minimum (should clamp)
  springData.SetDamping(0.09f);
  DALI_TEST_EQUALS(springData.GetDamping(), 0.1f, TEST_LOCATION);

  // Test setting damping to very small positive value (should clamp)
  springData.SetDamping(0.001f);
  DALI_TEST_EQUALS(springData.GetDamping(), 0.1f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSpringDataSetMassLargeValuesP(void)
{
  TestApplication application;
  SpringData      springData(100.0f, 20.0f, 1.0f);

  // Test setting mass to large value
  springData.SetMass(1000.0f);
  DALI_TEST_EQUALS(springData.GetMass(), 1000.0f, TEST_LOCATION);

  // Test setting mass to very large value
  springData.SetMass(10000.0f);
  DALI_TEST_EQUALS(springData.GetMass(), 10000.0f, TEST_LOCATION);

  // Test setting mass back to normal value
  springData.SetMass(5.0f);
  DALI_TEST_EQUALS(springData.GetMass(), 5.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSpringDataExtremeValuesP(void)
{
  TestApplication application;

  // Test with extremely high stiffness
  SpringData highStiffness(10000.0f, 20.0f, 1.0f);
  DALI_TEST_EQUALS(highStiffness.GetStiffness(), 10000.0f, TEST_LOCATION);
  DALI_TEST_CHECK(highStiffness.GetDuration() > 0.0f);

  // Test with extremely high damping
  SpringData highDamping(100.0f, 1000.0f, 1.0f);
  DALI_TEST_EQUALS(highDamping.GetDamping(), 1000.0f, TEST_LOCATION);
  DALI_TEST_CHECK(highDamping.GetDuration() > 0.0f);

  // Test with combination of extreme values
  SpringData extremeCombo(10000.0f, 1000.0f, 10.0f);
  DALI_TEST_CHECK(extremeCombo.GetDuration() > 0.0f);

  END_TEST;
}
