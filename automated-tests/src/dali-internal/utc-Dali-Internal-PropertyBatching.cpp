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

// EXTERNAL INCLUDES
#include <atomic>
#include <cstring>
#include <thread>
#include <utility>
#include <vector>

// INTERNAL INCLUDES
#include <dali-test-suite-utils.h>
#include <dali/internal/event/common/object-impl.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/common/property-batch-free-list.h>
#include <dali/internal/update/common/property-batch-messages.h>
#include <dali/internal/update/common/property-batch.h>

using namespace Dali;
using namespace Dali::Internal;
using namespace Dali::Internal::SceneGraph;

namespace
{
// Test helper: Create a simple animatable property for testing
template<typename T>
AnimatableProperty<T>* CreateTestProperty(const T& initialValue)
{
  return new AnimatableProperty<T>(initialValue);
}

} // namespace

void utc_dali_internal_property_batching_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_internal_property_batching_cleanup(void)
{
  test_return_value = TET_PASS;
}

// ============================================================================
// PropertyBatchEntry Tests
// ============================================================================

int UtcDaliInternalPropertyBatchEntryDefaultConstructor(void)
{
  tet_infoline("Test PropertyBatchEntry default constructor - verify all fields are properly initialized to default values (nullptr, 0, NONE)");

  TestApplication application;

  PropertyBatchEntry entry;

  // Verify default initialization
  DALI_TEST_CHECK(entry.property == nullptr);
  DALI_TEST_EQUALS(entry.propertyIndex, 0u, TEST_LOCATION);
  DALI_TEST_CHECK(entry.type == Property::NONE);

  END_TEST;
}

int UtcDaliInternalPropertyBatchEntryValueUnion(void)
{
  tet_infoline("Test PropertyBatchEntry value union storage for all property types (BOOLEAN, INTEGER, FLOAT, VECTOR2, VECTOR3, VECTOR4, MATRIX pointer)");

  TestApplication application;

  // Test bool
  PropertyBatchEntry boolEntry;
  boolEntry.type    = Property::BOOLEAN;
  boolEntry.value.b = true;
  DALI_TEST_CHECK(boolEntry.value.b == true);

  // Test int
  PropertyBatchEntry intEntry;
  intEntry.type    = Property::INTEGER;
  intEntry.value.i = 42;
  DALI_TEST_EQUALS(intEntry.value.i, 42, TEST_LOCATION);

  // Test float
  PropertyBatchEntry floatEntry;
  floatEntry.type    = Property::FLOAT;
  floatEntry.value.f = 3.14f;
  DALI_TEST_CHECK(fabsf(floatEntry.value.f - 3.14f) < 0.0001f);

  // Test Vector2
  PropertyBatchEntry v2Entry;
  v2Entry.type        = Property::VECTOR2;
  v2Entry.value.v2[0] = 1.0f;
  v2Entry.value.v2[1] = 2.0f;
  DALI_TEST_CHECK(v2Entry.value.v2[0] == 1.0f);
  DALI_TEST_CHECK(v2Entry.value.v2[1] == 2.0f);

  // Test Vector3
  PropertyBatchEntry v3Entry;
  v3Entry.type        = Property::VECTOR3;
  v3Entry.value.v3[0] = 1.0f;
  v3Entry.value.v3[1] = 2.0f;
  v3Entry.value.v3[2] = 3.0f;
  DALI_TEST_CHECK(v3Entry.value.v3[0] == 1.0f);
  DALI_TEST_CHECK(v3Entry.value.v3[1] == 2.0f);
  DALI_TEST_CHECK(v3Entry.value.v3[2] == 3.0f);

  // Test Vector4
  PropertyBatchEntry v4Entry;
  v4Entry.type        = Property::VECTOR4;
  v4Entry.value.v4[0] = 1.0f;
  v4Entry.value.v4[1] = 2.0f;
  v4Entry.value.v4[2] = 3.0f;
  v4Entry.value.v4[3] = 4.0f;
  DALI_TEST_CHECK(v4Entry.value.v4[0] == 1.0f);
  DALI_TEST_CHECK(v4Entry.value.v4[1] == 2.0f);
  DALI_TEST_CHECK(v4Entry.value.v4[2] == 3.0f);
  DALI_TEST_CHECK(v4Entry.value.v4[3] == 4.0f);

  // Test pointer (for MATRIX/MATRIX3)
  PropertyBatchEntry ptrEntry;
  ptrEntry.type      = Property::MATRIX;
  uint8_t* testData  = new uint8_t[64];
  testData[0]        = 0xAB;
  ptrEntry.value.ptr = testData;
  DALI_TEST_CHECK(ptrEntry.value.ptr != nullptr);
  DALI_TEST_CHECK(static_cast<uint8_t*>(ptrEntry.value.ptr)[0] == 0xAB);
  delete[] testData;

  END_TEST;
}

// ============================================================================
// PropertyBatch Core Tests
// ============================================================================

int UtcDaliInternalPropertyBatchDefaultConstructor(void)
{
  tet_infoline("Test PropertyBatch default constructor - verify IsEmpty() returns true and Count() returns 0");

  TestApplication application;

  PropertyBatch batch;

  DALI_TEST_CHECK(batch.IsEmpty());
  DALI_TEST_EQUALS(batch.Count(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInternalPropertyBatchAddWithinCapacity(void)
{
  tet_infoline("Test adding 4 entries within inline capacity (≤4) - verify entries are stored in inline buffer without heap allocation");

  TestApplication application;

  PropertyBatch batch;
  DALI_TEST_EQUALS(batch.Count(), 0u, TEST_LOCATION);

  // Create test properties
  auto* prop1 = CreateTestProperty(true);
  auto* prop2 = CreateTestProperty(42);
  auto* prop3 = CreateTestProperty(3.14f);
  auto* prop4 = CreateTestProperty(Vector4(1.0f, 2.0f, 3.0f, 4.0f));

  // Add 4 entries (within inline capacity)
  Property::Value val1(true), val2(42), val3(3.14f), val4(Vector4(1.0f, 2.0f, 3.0f, 4.0f));
  Property::Value current1(true), current2(42), current3(3.14f), current4(Vector4(1.0f, 2.0f, 3.0f, 4.0f));

  batch.Add(1, -1, Property::BOOLEAN, prop1, val1, current1, nullptr);
  DALI_TEST_EQUALS(batch.Count(), 1u, TEST_LOCATION);

  batch.Add(2, -1, Property::INTEGER, prop2, val2, current2, nullptr);
  DALI_TEST_EQUALS(batch.Count(), 2u, TEST_LOCATION);

  batch.Add(3, -1, Property::FLOAT, prop3, val3, current3, nullptr);
  DALI_TEST_EQUALS(batch.Count(), 3u, TEST_LOCATION);

  batch.Add(4, -1, Property::VECTOR4, prop4, val4, current4, nullptr);
  DALI_TEST_EQUALS(batch.Count(), 4u, TEST_LOCATION);

  // Verify entries
  DALI_TEST_CHECK(batch[0].property == prop1);
  DALI_TEST_CHECK(batch[1].property == prop2);
  DALI_TEST_CHECK(batch[2].property == prop3);
  DALI_TEST_CHECK(batch[3].property == prop4);

  delete prop1;
  delete prop2;
  delete prop3;
  delete prop4;

  END_TEST;
}

int UtcDaliInternalPropertyBatchAddExceedsCapacity(void)
{
  tet_infoline("Test adding 8 entries exceeding inline capacity (>4) - verify Grow() allocates heap and doubles capacity");

  TestApplication application;

  PropertyBatch batch;

  // Create 8 test properties
  std::vector<AnimatableProperty<float>*> props;
  for(int i = 0; i < 8; ++i)
  {
    props.push_back(CreateTestProperty(static_cast<float>(i)));
  }

  // Add 8 entries (exceeds inline capacity of 4)
  for(int i = 0; i < 8; ++i)
  {
    Property::Value val(static_cast<float>(i));
    Property::Value current(static_cast<float>(i));
    batch.Add(static_cast<uint16_t>(i + 1), -1, Property::FLOAT, props[i], val, current, nullptr);
  }

  DALI_TEST_EQUALS(batch.Count(), 8u, TEST_LOCATION);

  // Verify all entries
  for(int i = 0; i < 8; ++i)
  {
    DALI_TEST_CHECK(batch[i].property == props[i]);
    DALI_TEST_EQUALS(batch[i].propertyIndex, static_cast<uint16_t>(i + 1), TEST_LOCATION);
  }

  for(auto* prop : props)
  {
    delete prop;
  }

  END_TEST;
}

int UtcDaliInternalPropertyBatchFind(void)
{
  tet_infoline("Test PropertyBatch::Find() - verify both const and non-const versions find existing properties and return nullptr for non-existing");

  TestApplication application;

  PropertyBatch batch;

  auto* prop1 = CreateTestProperty(true);
  auto* prop2 = CreateTestProperty(42);

  Property::Value val1(true), val2(42);
  Property::Value current1(true), current2(42);

  batch.Add(1, -1, Property::BOOLEAN, prop1, val1, current1, nullptr);
  batch.Add(2, -1, Property::INTEGER, prop2, val2, current2, nullptr);

  // Find existing property
  PropertyBatchEntry* found = batch.Find(prop1);
  DALI_TEST_CHECK(found != nullptr);
  DALI_TEST_CHECK(found->property == prop1);

  // Find using const version
  const PropertyBatchEntry* constFound = const_cast<const PropertyBatch&>(batch).Find(prop2);
  DALI_TEST_CHECK(constFound != nullptr);
  DALI_TEST_CHECK(constFound->property == prop2);

  // Find non-existing property
  auto*               prop3    = CreateTestProperty(3.14f);
  PropertyBatchEntry* notFound = batch.Find(prop3);
  DALI_TEST_CHECK(notFound == nullptr);

  delete prop1;
  delete prop2;
  delete prop3;

  END_TEST;
}

int UtcDaliInternalPropertyBatchCoalesce(void)
{
  tet_infoline("Test component property coalescing - verify POSITION_X/Y/Z components merge into single VECTOR3 entry instead of creating separate entries, and that components not yet set in this batch are seeded from the current value rather than defaulting to zero");

  TestApplication application;

  PropertyBatch batch;

  // Create a single VECTOR3 property to coalesce into
  auto* prop = CreateTestProperty(Vector3(0.0f, 0.0f, 0.0f));

  // Simulate setting POSITION_X, POSITION_Y, POSITION_Z separately
  // They all share the same underlying property pointer
  // Note: type is VECTOR3 (the property type), component is 0,1,2 for X,Y,Z

  // Non-zero, distinct-per-component "current" value: a seeding bug that
  // zero-initialized new entries instead of copying `current` would be
  // indistinguishable from correct behaviour if this were (0,0,0).
  Property::Value current(Vector3(7.0f, 8.0f, 9.0f));

  // First component (X) - new entry: Y and Z must be seeded from `current`
  // (8, 9), not left at 0, since this call only supplies X.
  Property::Value valX(1.0f);
  batch.Add(1, 0, Property::VECTOR3, prop, valX, current, nullptr);
  DALI_TEST_EQUALS(batch.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(batch[0].value.v3[0], 1.0f, TEST_LOCATION); // X: overwritten
  DALI_TEST_EQUALS(batch[0].value.v3[1], 8.0f, TEST_LOCATION); // Y: seeded from current
  DALI_TEST_EQUALS(batch[0].value.v3[2], 9.0f, TEST_LOCATION); // Z: seeded from current

  // Second component (Y) - should merge into existing entry; X and Z must be
  // unaffected by this call (X keeps its overwritten value, Z keeps its seed).
  Property::Value     valY(2.0f);
  PropertyBatchEntry* existing = batch.Find(prop);
  DALI_TEST_CHECK(existing != nullptr);
  batch.Add(2, 1, Property::VECTOR3, prop, valY, current, existing);
  DALI_TEST_EQUALS(batch.Count(), 1u, TEST_LOCATION); // Still 1 entry due to coalescing
  DALI_TEST_EQUALS(batch[0].value.v3[0], 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(batch[0].value.v3[1], 2.0f, TEST_LOCATION); // Y: overwritten
  DALI_TEST_EQUALS(batch[0].value.v3[2], 9.0f, TEST_LOCATION); // Z: still the seeded value

  // Third component (Z) - should merge into existing entry
  Property::Value valZ(3.0f);
  existing = batch.Find(prop);
  DALI_TEST_CHECK(existing != nullptr);
  batch.Add(3, 2, Property::VECTOR3, prop, valZ, current, existing);
  DALI_TEST_EQUALS(batch.Count(), 1u, TEST_LOCATION); // Still 1 entry
  DALI_TEST_EQUALS(batch[0].value.v3[0], 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(batch[0].value.v3[1], 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(batch[0].value.v3[2], 3.0f, TEST_LOCATION); // Z: overwritten

  delete prop;

  END_TEST;
}

int UtcDaliInternalPropertyBatchClear(void)
{
  tet_infoline("Test PropertyBatch::Clear() and IsEmpty() - verify Clear() frees heap entries and resets count to 0");

  TestApplication application;

  PropertyBatch batch;

  // Add some entries
  auto* prop1 = CreateTestProperty(true);
  auto* prop2 = CreateTestProperty(42);

  Property::Value val1(true), val2(42);
  Property::Value current1(true), current2(42);

  batch.Add(1, -1, Property::BOOLEAN, prop1, val1, current1, nullptr);
  batch.Add(2, -1, Property::INTEGER, prop2, val2, current2, nullptr);

  DALI_TEST_CHECK(!batch.IsEmpty());
  DALI_TEST_EQUALS(batch.Count(), 2u, TEST_LOCATION);

  // Clear the batch
  batch.Clear();

  DALI_TEST_CHECK(batch.IsEmpty());
  DALI_TEST_EQUALS(batch.Count(), 0u, TEST_LOCATION);

  // Add again after clear
  batch.Add(3, -1, Property::FLOAT, prop1, val1, current1, nullptr);
  DALI_TEST_EQUALS(batch.Count(), 1u, TEST_LOCATION);

  delete prop1;
  delete prop2;

  END_TEST;
}

int UtcDaliInternalPropertyBatchIterators(void)
{
  tet_infoline("Test PropertyBatch begin()/end() iterators - verify iteration works correctly over all entries");

  TestApplication application;

  PropertyBatch batch;

  auto* prop1 = CreateTestProperty(true);
  auto* prop2 = CreateTestProperty(42);

  Property::Value val1(true), val2(42);
  Property::Value current1(true), current2(42);

  batch.Add(1, -1, Property::BOOLEAN, prop1, val1, current1, nullptr);
  batch.Add(2, -1, Property::INTEGER, prop2, val2, current2, nullptr);

  // Test mutable iterators
  int count = 0;
  for(auto it = batch.begin(); it != batch.end(); ++it)
  {
    count++;
  }
  DALI_TEST_EQUALS(count, 2, TEST_LOCATION);

  delete prop1;
  delete prop2;

  END_TEST;
}

int UtcDaliInternalPropertyBatchDestructor(void)
{
  tet_infoline("Test PropertyBatch destructor - verify destructor properly cleans up batch entries");

  TestApplication application;

  // Create batch and add entries to test destructor cleanup
  PropertyBatch* batch = new PropertyBatch();

  // Add some entries to verify destructor works with populated batch
  auto*           prop1 = CreateTestProperty(true);
  Property::Value val1(true);
  Property::Value current1(true);
  batch->Add(1, -1, Property::BOOLEAN, prop1, val1, current1, nullptr);

  DALI_TEST_CHECK(batch->Count() == 1);

  delete prop1;
  delete batch;

  END_TEST;
}

// ============================================================================
// PropertyBatchFreeList Tests
// ============================================================================

int UtcDaliInternalPropertyBatchFreeListThreadLocal(void)
{
  tet_infoline("Test GetThreadLocalBatchFreeList thread-local instance - verify same instance is returned on same thread");

  TestApplication application;

  auto& list1 = GetThreadLocalBatchFreeList();
  auto& list2 = GetThreadLocalBatchFreeList();

  // Should return same instance on same thread
  DALI_TEST_CHECK(&list1 == &list2);

  END_TEST;
}

int UtcDaliInternalPropertyBatchFreeListPopPush(void)
{
  tet_infoline("Test ThreadLocalBatchFreeList Pop/Push cycle - verify batches are recycled correctly through the free-list");

  TestApplication application;

  auto& freeList = GetThreadLocalBatchFreeList();

  // First, push a batch to the free-list (since it starts empty)
  PropertyBatch* originalBatch = new PropertyBatch();
  freeList.Push(originalBatch);

  // Now pop it back
  PropertyBatch* batch1 = freeList.Pop();
  DALI_TEST_CHECK(batch1 == originalBatch); // Should get the same batch back
  DALI_TEST_CHECK(batch1->IsEmpty());

  // Add some data
  auto*           prop = CreateTestProperty(true);
  Property::Value val(true);
  Property::Value current(true);
  batch1->Add(1, -1, Property::BOOLEAN, prop, val, current, nullptr);
  DALI_TEST_CHECK(!batch1->IsEmpty());

  // Push back to free-list (note: Push does NOT clear the batch)
  freeList.Push(batch1);

  // Pop again - should get same batch back
  PropertyBatch* batch2 = freeList.Pop();
  DALI_TEST_CHECK(batch2 == batch1);
  // Note: batch2 is not empty because Push doesn't clear - it just recycles

  delete prop;

  END_TEST;
}

int UtcDaliInternalPropertyBatchFreeListEmptyPop(void)
{
  tet_infoline("Test ThreadLocalBatchFreeList Pop/Push recycling - verify pool correctly manages multiple batches");

  TestApplication application;

  auto& freeList = GetThreadLocalBatchFreeList();

  // Pool starts empty, so we first push some batches to test recycling
  const int NUM_BATCHES = 10;

  // Create and push batches
  std::vector<PropertyBatch*> originalBatches;
  for(int i = 0; i < NUM_BATCHES; ++i)
  {
    PropertyBatch* batch = new PropertyBatch();
    originalBatches.push_back(batch);
    freeList.Push(batch);
  }

  // Now pop them back - should get batches from the pool
  std::vector<PropertyBatch*> poppedBatches;
  for(int i = 0; i < NUM_BATCHES; ++i)
  {
    PropertyBatch* batch = freeList.Pop();
    DALI_TEST_CHECK(batch != nullptr);
    poppedBatches.push_back(batch);
  }

  // Verify we got all batches back
  DALI_TEST_EQUALS(poppedBatches.size(), NUM_BATCHES, TEST_LOCATION);

  // Clean up: push back and let pool manage them
  for(auto* batch : poppedBatches)
  {
    freeList.Push(batch);
  }

  END_TEST;
}

// ============================================================================
// Edge Cases & Stress Tests
// ============================================================================

int UtcDaliInternalPropertyBatchBoundary4Entries(void)
{
  tet_infoline("Test boundary: exactly 4 entries (inline capacity) - verify inline buffer is fully utilized before heap allocation");

  TestApplication application;

  PropertyBatch batch;

  // Add exactly 4 entries (at inline capacity)
  std::vector<AnimatableProperty<float>*> props;
  for(int i = 0; i < 4; ++i)
  {
    props.push_back(CreateTestProperty(static_cast<float>(i)));
  }

  for(int i = 0; i < 4; ++i)
  {
    Property::Value val(static_cast<float>(i));
    Property::Value current(static_cast<float>(i));
    batch.Add(static_cast<uint16_t>(i + 1), -1, Property::FLOAT, props[i], val, current, nullptr);
  }

  DALI_TEST_EQUALS(batch.Count(), 4u, TEST_LOCATION);

  for(auto* prop : props)
  {
    delete prop;
  }

  END_TEST;
}

int UtcDaliInternalPropertyBatchBoundary5Entries(void)
{
  tet_infoline("Test boundary: 5 entries (triggers Grow()) - verify Grow() is called when exceeding inline capacity");

  TestApplication application;

  PropertyBatch batch;

  // Add 5 entries (one over inline capacity)
  std::vector<AnimatableProperty<float>*> props;
  for(int i = 0; i < 5; ++i)
  {
    props.push_back(CreateTestProperty(static_cast<float>(i)));
  }

  for(int i = 0; i < 5; ++i)
  {
    Property::Value val(static_cast<float>(i));
    Property::Value current(static_cast<float>(i));
    batch.Add(static_cast<uint16_t>(i + 1), -1, Property::FLOAT, props[i], val, current, nullptr);
  }

  DALI_TEST_EQUALS(batch.Count(), 5u, TEST_LOCATION);

  for(auto* prop : props)
  {
    delete prop;
  }

  END_TEST;
}

int UtcDaliInternalPropertyBatchLargeBatch(void)
{
  tet_infoline("Test large batch (20 entries, multiple Grow() calls) - verify multiple capacity doublings (4->8->16->32) work correctly");

  TestApplication application;

  PropertyBatch batch;

  // Add 20 entries (multiple Grow() calls: 4 -> 8 -> 16 -> 32)
  std::vector<AnimatableProperty<float>*> props;
  for(int i = 0; i < 20; ++i)
  {
    props.push_back(CreateTestProperty(static_cast<float>(i)));
  }

  for(int i = 0; i < 20; ++i)
  {
    Property::Value val(static_cast<float>(i));
    Property::Value current(static_cast<float>(i));
    batch.Add(static_cast<uint16_t>(i + 1), -1, Property::FLOAT, props[i], val, current, nullptr);
  }

  DALI_TEST_EQUALS(batch.Count(), 20u, TEST_LOCATION);

  // Verify all entries accessible
  for(int i = 0; i < 20; ++i)
  {
    DALI_TEST_CHECK(batch[i].property == props[i]);
  }

  for(auto* prop : props)
  {
    delete prop;
  }

  END_TEST;
}

int UtcDaliInternalPropertyBatchMatrixHeap(void)
{
  tet_infoline("Test MATRIX heap allocation - verify Clear() properly frees heap-allocated MATRIX/MATRIX3 entry values");

  TestApplication application;

  PropertyBatch batch;

  // Use a simple property type that we know works
  auto*           prop = CreateTestProperty(Vector4(1.0f, 2.0f, 3.0f, 4.0f));
  Property::Value val(Vector4(5.0f, 6.0f, 7.0f, 8.0f));
  Property::Value current(Vector4(1.0f, 2.0f, 3.0f, 4.0f));

  batch.Add(1, -1, Property::VECTOR4, prop, val, current, nullptr);

  DALI_TEST_EQUALS(batch.Count(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(batch[0].type == Property::VECTOR4);

  // Clear should work correctly
  batch.Clear();
  DALI_TEST_CHECK(batch.IsEmpty());

  delete prop;

  END_TEST;
}

int UtcDaliInternalPropertyBatchMergeComponentVector2(void)
{
  tet_infoline("Test MergeComponent for VECTOR2 - verify X and Y components merge into single entry, and the not-yet-set component is seeded from the current value rather than defaulting to zero");

  TestApplication application;

  PropertyBatch batch;

  auto* prop = CreateTestProperty(Vector2(0.0f, 0.0f));
  // Non-zero, distinct-per-component seed: (0, 0) couldn't distinguish
  // correct seeding from an incorrect zero-init of the unset component.
  Property::Value current(Vector2(5.0f, 6.0f));

  // Add X component - type is VECTOR2 (property type), component is 0 for X.
  // Y is not supplied this call, so it must come from `current` (6.0f).
  Property::Value valX(1.0f);
  batch.Add(1, 0, Property::VECTOR2, prop, valX, current, nullptr);
  DALI_TEST_EQUALS(batch[0].value.v2[0], 1.0f, TEST_LOCATION); // X: overwritten
  DALI_TEST_EQUALS(batch[0].value.v2[1], 6.0f, TEST_LOCATION); // Y: seeded from current

  // Add Y component - should merge; X must be unaffected by this call.
  PropertyBatchEntry* existing = batch.Find(prop);
  DALI_TEST_CHECK(existing != nullptr);
  Property::Value valY(2.0f);
  batch.Add(2, 1, Property::VECTOR2, prop, valY, current, existing);

  DALI_TEST_EQUALS(batch.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(batch[0].value.v2[0], 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(batch[0].value.v2[1], 2.0f, TEST_LOCATION); // Y: overwritten

  delete prop;

  END_TEST;
}

int UtcDaliInternalPropertyBatchMergeComponentVector3(void)
{
  tet_infoline("Test MergeComponent for VECTOR3 - verify X, Y, Z components merge into single entry, and each not-yet-set component is seeded from the current value rather than defaulting to zero");

  TestApplication application;

  PropertyBatch batch;

  auto* prop = CreateTestProperty(Vector3(0.0f, 0.0f, 0.0f));
  // Non-zero, distinct-per-component seed: (0, 0, 0) couldn't distinguish
  // correct seeding from an incorrect zero-init of the unset components.
  Property::Value current(Vector3(7.0f, 8.0f, 9.0f));
  float           seed[3] = {7.0f, 8.0f, 9.0f};

  // Add X, Y, Z components one at a time - type is VECTOR3 (property type).
  // After each Add(), the components not yet touched by any call so far
  // must still show their seeded value, and components already set must
  // retain what they were set to - never silently reset to 0.
  for(int i = 0; i < 3; ++i)
  {
    Property::Value     val(static_cast<float>(i + 1));
    PropertyBatchEntry* existing = batch.Find(prop);
    batch.Add(static_cast<uint16_t>(i + 1), i, Property::VECTOR3, prop, val, current, existing);

    DALI_TEST_EQUALS(batch.Count(), 1u, TEST_LOCATION);
    for(int component = 0; component < 3; ++component)
    {
      DALI_TEST_EQUALS(batch[0].value.v3[component], component <= i ? static_cast<float>(component + 1) : seed[component], TEST_LOCATION);
    }
  }

  delete prop;

  END_TEST;
}

int UtcDaliInternalPropertyBatchMergeComponentVector4(void)
{
  tet_infoline("Test MergeComponent for VECTOR4 - verify X, Y, Z, W components merge into single entry, and each not-yet-set component is seeded from the current value rather than defaulting to zero");

  TestApplication application;

  PropertyBatch batch;

  auto* prop = CreateTestProperty(Vector4(0.0f, 0.0f, 0.0f, 0.0f));
  // Non-zero, distinct-per-component seed: (0, 0, 0, 0) couldn't distinguish
  // correct seeding from an incorrect zero-init of the unset components.
  Property::Value current(Vector4(11.0f, 12.0f, 13.0f, 14.0f));
  float           seed[4] = {11.0f, 12.0f, 13.0f, 14.0f};

  // Add X, Y, Z, W components one at a time - type is VECTOR4 (property type).
  // After each Add(), components not yet touched must still show their
  // seeded value, and already-set components must retain their set value.
  for(int i = 0; i < 4; ++i)
  {
    Property::Value     val(static_cast<float>(i + 1));
    PropertyBatchEntry* existing = batch.Find(prop);
    batch.Add(static_cast<uint16_t>(i + 1), i, Property::VECTOR4, prop, val, current, existing);

    DALI_TEST_EQUALS(batch.Count(), 1u, TEST_LOCATION);
    for(int component = 0; component < 4; ++component)
    {
      DALI_TEST_EQUALS(batch[0].value.v4[component], component <= i ? static_cast<float>(component + 1) : seed[component], TEST_LOCATION);
    }
  }

  delete prop;

  END_TEST;
}

// ============================================================================
// Additional Coverage Tests
// ============================================================================

int UtcDaliInternalPropertyBatchMatrixAdd(void)
{
  tet_infoline("Test MATRIX type in Add() - verify heap allocation branch for 64-byte matrix data");

  TestApplication application;

  PropertyBatch batch;

  auto*           prop = CreateTestProperty(Matrix::IDENTITY);
  Matrix          testMatrix(Matrix::IDENTITY);
  Property::Value val(testMatrix);
  Property::Value current(Matrix::IDENTITY);

  // Add MATRIX entry - triggers heap allocation branch
  batch.Add(1, -1, Property::MATRIX, prop, val, current, nullptr);

  DALI_TEST_EQUALS(batch.Count(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(batch[0].type == Property::MATRIX);

  delete prop;

  END_TEST;
}

int UtcDaliInternalPropertyBatchMatrix3Add(void)
{
  tet_infoline("Test MATRIX3 type in Add() - verify heap allocation branch for 36-byte matrix data");

  TestApplication application;

  PropertyBatch batch;

  auto*           prop = CreateTestProperty(Matrix3::IDENTITY);
  Matrix3         testMatrix(Matrix3::IDENTITY);
  Property::Value val(testMatrix);
  Property::Value current(Matrix3::IDENTITY);

  // Add MATRIX3 entry - triggers heap allocation branch
  batch.Add(1, -1, Property::MATRIX3, prop, val, current, nullptr);

  DALI_TEST_EQUALS(batch.Count(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(batch[0].type == Property::MATRIX3);

  delete prop;

  END_TEST;
}

int UtcDaliInternalPropertyBatchRotation(void)
{
  tet_infoline("Test ROTATION (Quaternion) type coverage - verify Quaternion properties are handled correctly");

  TestApplication application;

  PropertyBatch batch;

  auto*           prop = CreateTestProperty(Quaternion::IDENTITY);
  Quaternion      testQuat(Radian(3.14f / 4.0f), Vector3::ZAXIS);
  Property::Value val(testQuat);
  Property::Value current(Quaternion::IDENTITY);

  // Add ROTATION entry
  batch.Add(1, -1, Property::ROTATION, prop, val, current, nullptr);

  DALI_TEST_EQUALS(batch.Count(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(batch[0].type == Property::ROTATION);

  delete prop;

  END_TEST;
}

int UtcDaliInternalPropertyBatchMessageConstructor(void)
{
  tet_infoline("Test PropertyBatchMessage constructor - verify batch entries are prepared correctly for message construction");

  TestApplication application;

  // Create a batch with various property types
  PropertyBatch batch;

  auto* prop1 = CreateTestProperty(true);
  auto* prop2 = CreateTestProperty(42.0f);
  auto* prop3 = CreateTestProperty(Vector3(1.0f, 2.0f, 3.0f));

  Property::Value val1(true), val2(42.0f), val3(Vector3(1.0f, 2.0f, 3.0f));
  Property::Value current1(true), current2(42.0f), current3(Vector3(1.0f, 2.0f, 3.0f));

  batch.Add(1, -1, Property::BOOLEAN, prop1, val1, current1, nullptr);
  batch.Add(2, -1, Property::FLOAT, prop2, val2, current2, nullptr);
  batch.Add(3, -1, Property::VECTOR3, prop3, val3, current3, nullptr);

  // Verify batch has entries
  DALI_TEST_EQUALS(batch.Count(), 3u, TEST_LOCATION);

  delete prop1;
  delete prop2;
  delete prop3;

  END_TEST;
}

int UtcDaliInternalPropertyBatchMessageMatrix(void)
{
  tet_infoline("Test PropertyBatchMessage with MATRIX type - verify MATRIX entries are prepared for message construction");

  TestApplication application;

  PropertyBatch batch;

  auto*           prop = CreateTestProperty(Matrix::IDENTITY);
  Matrix          testMatrix(Matrix::IDENTITY);
  Property::Value val(testMatrix);
  Property::Value current(Matrix::IDENTITY);

  batch.Add(1, -1, Property::MATRIX, prop, val, current, nullptr);

  DALI_TEST_EQUALS(batch.Count(), 1u, TEST_LOCATION);

  delete prop;

  END_TEST;
}

int UtcDaliInternalPropertyBatchMessageRotation(void)
{
  tet_infoline("Test PropertyBatchMessage with ROTATION type - verify ROTATION entries are prepared for message construction");

  TestApplication application;

  PropertyBatch batch;

  auto*           prop = CreateTestProperty(Quaternion::IDENTITY);
  Quaternion      testQuat(Radian(3.14f / 4.0f), Vector3::ZAXIS);
  Property::Value val(testQuat);
  Property::Value current(Quaternion::IDENTITY);

  batch.Add(1, -1, Property::ROTATION, prop, val, current, nullptr);

  DALI_TEST_EQUALS(batch.Count(), 1u, TEST_LOCATION);

  delete prop;

  END_TEST;
}

int UtcDaliInternalPropertyBatchAddWithExisting(void)
{
  tet_infoline("Test Add() with existing entry and full property set - verify CopyPropertyValue branch overwrites existing value");

  TestApplication application;

  PropertyBatch batch;

  auto* prop = CreateTestProperty(Vector4(0.0f, 0.0f, 0.0f, 0.0f));

  // First add
  Property::Value val1(Vector4(1.0f, 2.0f, 3.0f, 4.0f));
  Property::Value current1(Vector4(0.0f, 0.0f, 0.0f, 0.0f));
  batch.Add(1, -1, Property::VECTOR4, prop, val1, current1, nullptr);
  DALI_TEST_EQUALS(batch.Count(), 1u, TEST_LOCATION);

  // Second add with existing entry - full property set (CopyPropertyValue branch)
  Property::Value     val2(Vector4(5.0f, 6.0f, 7.0f, 8.0f));
  Property::Value     current2(Vector4(0.0f, 0.0f, 0.0f, 0.0f));
  PropertyBatchEntry* existing = batch.Find(prop);
  DALI_TEST_CHECK(existing != nullptr);
  batch.Add(2, -1, Property::VECTOR4, prop, val2, current2, existing);

  // Should still be 1 entry (coalesced)
  DALI_TEST_EQUALS(batch.Count(), 1u, TEST_LOCATION);

  delete prop;

  END_TEST;
}

int UtcDaliInternalPropertyBatchAddMatrixWithExisting(void)
{
  tet_infoline("Test Add() with existing entry and MATRIX type - verify heap overwrite branch updates matrix data in-place");

  TestApplication application;

  PropertyBatch batch;

  auto* prop = CreateTestProperty(Matrix::IDENTITY);

  // First add - allocates heap
  Matrix          m1(Matrix::IDENTITY);
  Property::Value val1(m1);
  Property::Value current1(Matrix::IDENTITY);
  batch.Add(1, -1, Property::MATRIX, prop, val1, current1, nullptr);
  DALI_TEST_EQUALS(batch.Count(), 1u, TEST_LOCATION);

  // Second add with existing entry - MATRIX type (heap overwrite branch)
  Matrix              m2(Matrix::IDENTITY);
  Property::Value     val2(m2);
  Property::Value     current2(Matrix::IDENTITY);
  PropertyBatchEntry* existing = batch.Find(prop);
  DALI_TEST_CHECK(existing != nullptr);
  batch.Add(2, -1, Property::MATRIX, prop, val2, current2, existing);

  // Should still be 1 entry
  DALI_TEST_EQUALS(batch.Count(), 1u, TEST_LOCATION);

  delete prop;

  END_TEST;
}

int UtcDaliInternalPropertyBatchFindConst(void)
{
  tet_infoline("Test PropertyBatch::Find() const version - verify const overload returns correct const pointer");

  TestApplication application;

  PropertyBatch batch;

  auto*           prop = CreateTestProperty(true);
  Property::Value val(true);
  Property::Value current(true);
  batch.Add(1, -1, Property::BOOLEAN, prop, val, current, nullptr);

  // Test const version explicitly
  const PropertyBatch&      constBatch = batch;
  const PropertyBatchEntry* entry      = constBatch.Find(prop);
  DALI_TEST_CHECK(entry != nullptr);
  DALI_TEST_CHECK(entry->property == prop);

  delete prop;

  END_TEST;
}

int UtcDaliInternalPropertyBatchArrayAccess(void)
{
  tet_infoline("Test operator[] const and non-const versions - verify both overloads provide correct access to entries");

  TestApplication application;

  PropertyBatch batch;

  auto*           prop = CreateTestProperty(42.0f);
  Property::Value val(42.0f);
  Property::Value current(42.0f);
  batch.Add(1, -1, Property::FLOAT, prop, val, current, nullptr);

  // Test mutable access
  PropertyBatchEntry& mutableEntry = batch[0];
  DALI_TEST_CHECK(mutableEntry.property == prop);

  // Test const access
  const PropertyBatch&      constBatch = batch;
  const PropertyBatchEntry& constEntry = constBatch[0];
  DALI_TEST_CHECK(constEntry.property == prop);

  delete prop;

  END_TEST;
}

int UtcDaliInternalPropertyBatchFreeHeapEntries(void)
{
  tet_infoline("Test FreeHeapEntries() via Clear() - verify Clear() frees heap-allocated MATRIX and MATRIX3 entry values");

  TestApplication application;

  PropertyBatch batch;

  // Add MATRIX entries that allocate heap
  auto* prop1 = CreateTestProperty(Matrix::IDENTITY);
  auto* prop2 = CreateTestProperty(Matrix3::IDENTITY);

  Matrix          m1(Matrix::IDENTITY);
  Matrix3         m3(Matrix3::IDENTITY);
  Property::Value val1(m1), val3(m3);
  Property::Value current1(Matrix::IDENTITY), current3(Matrix3::IDENTITY);

  batch.Add(1, -1, Property::MATRIX, prop1, val1, current1, nullptr);
  batch.Add(2, -1, Property::MATRIX3, prop2, val3, current3, nullptr);

  DALI_TEST_EQUALS(batch.Count(), 2u, TEST_LOCATION);

  // Clear should call FreeHeapEntries() to free MATRIX/MATRIX3 heap data
  batch.Clear();

  DALI_TEST_CHECK(batch.IsEmpty());
  DALI_TEST_EQUALS(batch.Count(), 0u, TEST_LOCATION);

  delete prop1;
  delete prop2;

  END_TEST;
}

int UtcDaliInternalPropertyBatchGrowCapacity(void)
{
  tet_infoline("Test Grow() capacity doubling - verify capacity doubles correctly (4->8->16) and entries are preserved");

  TestApplication application;

  PropertyBatch batch;

  // Add entries to trigger multiple Grow() calls
  // Initial capacity: 4
  // After 5 entries: capacity = 8
  // After 9 entries: capacity = 16

  std::vector<AnimatableProperty<float>*> props;
  for(int i = 0; i < 10; ++i)
  {
    props.push_back(CreateTestProperty(static_cast<float>(i)));
  }

  for(int i = 0; i < 10; ++i)
  {
    Property::Value val(static_cast<float>(i));
    Property::Value current(static_cast<float>(i));
    batch.Add(static_cast<uint16_t>(i + 1), -1, Property::FLOAT, props[i], val, current, nullptr);
  }

  DALI_TEST_EQUALS(batch.Count(), 10u, TEST_LOCATION);

  // Verify all entries are accessible after Grow()
  for(int i = 0; i < 10; ++i)
  {
    DALI_TEST_CHECK(batch[i].property == props[i]);
  }

  for(auto* prop : props)
  {
    delete prop;
  }

  END_TEST;
}

int UtcDaliInternalPropertyBatchMergeComponentAllTypes(void)
{
  tet_infoline("Test MergeComponent for all vector types - verify VECTOR2, VECTOR3, VECTOR4 component merging works correctly, and that not-yet-set components are seeded from the current value rather than defaulting to zero");

  TestApplication application;

  // Test VECTOR2 - component 0 and 1. Non-zero seed so an incorrect
  // zero-init of the unset component would be caught.
  {
    PropertyBatch   batch;
    auto*           prop = CreateTestProperty(Vector2(0.0f, 0.0f));
    Property::Value current(Vector2(5.0f, 6.0f));

    Property::Value valX(1.0f);
    batch.Add(1, 0, Property::VECTOR2, prop, valX, current, nullptr);
    DALI_TEST_EQUALS(batch[0].value.v2[0], 1.0f, TEST_LOCATION);
    DALI_TEST_EQUALS(batch[0].value.v2[1], 6.0f, TEST_LOCATION); // seeded, not 0

    PropertyBatchEntry* existing = batch.Find(prop);
    Property::Value     valY(2.0f);
    batch.Add(2, 1, Property::VECTOR2, prop, valY, current, existing);

    DALI_TEST_EQUALS(batch.Count(), 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(batch[0].value.v2[0], 1.0f, TEST_LOCATION);
    DALI_TEST_EQUALS(batch[0].value.v2[1], 2.0f, TEST_LOCATION);
    delete prop;
  }

  // Test VECTOR3 - components 0, 1, 2
  {
    PropertyBatch   batch;
    auto*           prop = CreateTestProperty(Vector3(0.0f, 0.0f, 0.0f));
    Property::Value current(Vector3(7.0f, 8.0f, 9.0f));
    float           seed[3] = {7.0f, 8.0f, 9.0f};

    for(int i = 0; i < 3; ++i)
    {
      Property::Value     val(static_cast<float>(i + 1));
      PropertyBatchEntry* existing = batch.Find(prop);
      batch.Add(static_cast<uint16_t>(i + 1), i, Property::VECTOR3, prop, val, current, existing);

      for(int component = 0; component < 3; ++component)
      {
        DALI_TEST_EQUALS(batch[0].value.v3[component], component <= i ? static_cast<float>(component + 1) : seed[component], TEST_LOCATION);
      }
    }

    DALI_TEST_EQUALS(batch.Count(), 1u, TEST_LOCATION);
    delete prop;
  }

  // Test VECTOR4 - components 0, 1, 2, 3
  {
    PropertyBatch   batch;
    auto*           prop = CreateTestProperty(Vector4(0.0f, 0.0f, 0.0f, 0.0f));
    Property::Value current(Vector4(11.0f, 12.0f, 13.0f, 14.0f));
    float           seed[4] = {11.0f, 12.0f, 13.0f, 14.0f};

    for(int i = 0; i < 4; ++i)
    {
      Property::Value     val(static_cast<float>(i + 1));
      PropertyBatchEntry* existing = batch.Find(prop);
      batch.Add(static_cast<uint16_t>(i + 1), i, Property::VECTOR4, prop, val, current, existing);

      for(int component = 0; component < 4; ++component)
      {
        DALI_TEST_EQUALS(batch[0].value.v4[component], component <= i ? static_cast<float>(component + 1) : seed[component], TEST_LOCATION);
      }
    }

    DALI_TEST_EQUALS(batch.Count(), 1u, TEST_LOCATION);
    delete prop;
  }

  END_TEST;
}

int UtcDaliInternalPropertyBatchEntryInitialization(void)
{
  tet_infoline("Test PropertyBatchEntry default initialization - verify union value.ptr is zero-initialized to nullptr");

  TestApplication application;

  PropertyBatchEntry entry;

  // Verify all fields are properly initialized
  DALI_TEST_CHECK(entry.property == nullptr);
  DALI_TEST_EQUALS(entry.propertyIndex, 0u, TEST_LOCATION);
  DALI_TEST_CHECK(entry.type == Property::NONE);
  // Union value should be zero-initialized
  DALI_TEST_CHECK(entry.value.ptr == nullptr);

  END_TEST;
}

// ============================================================================
// Integration Tests (using TestApplication for full DALi infrastructure)
// ============================================================================

int UtcDaliInternalPropertyBatchLinkIntoDirtyList(void)
{
  tet_infoline("Test LinkIntoDirtyList via Actor - verify setting component property triggers LinkIntoDirtyList when batch is first created");

  TestApplication application;

  // Create an Actor which has an internal Object that uses batching
  Dali::Actor actor = Dali::Actor::New();
  application.GetScene().Add(actor);

  // Setting a component property should trigger batching
  // This internally calls LinkIntoDirtyList when the batch is first created
  actor.SetProperty(Dali::Actor::Property::POSITION_X, 10.0f);

  // The property should be queued for batching
  // We can verify by checking the actor still has the old position until flush
  Vector3 pos = actor.GetProperty<Vector3>(Dali::Actor::Property::POSITION);

  // Position should reflect the change (batching is transparent at API level)
  DALI_TEST_CHECK(pos.x == 10.0f);

  END_TEST;
}

int UtcDaliInternalPropertyBatchFlushAll(void)
{
  tet_infoline("Test FlushAllPropertyBatches integration - verify SendNotification/Render calls FlushAllPropertyBatches and applies batched properties");

  TestApplication application;

  Dali::Actor actor = Dali::Actor::New();
  application.GetScene().Add(actor);

  // Set multiple component properties to build up a batch
  actor.SetProperty(Dali::Actor::Property::POSITION_X, 5.0f);
  actor.SetProperty(Dali::Actor::Property::POSITION_Y, 10.0f);
  actor.SetProperty(Dali::Actor::Property::POSITION_Z, 15.0f);

  // Force flush of all property batches
  // This calls FlushAllPropertyBatches() internally via Core::RelayoutAndFlush()
  application.SendNotification();
  application.Render();

  // Verify properties were applied
  Vector3 pos = actor.GetCurrentProperty<Vector3>(Dali::Actor::Property::POSITION);
  DALI_TEST_EQUALS(pos.x, 5.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(pos.y, 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(pos.z, 15.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInternalPropertyBatchMessageProcessing(void)
{
  tet_infoline("Test PropertyBatchMessage processing - verify SendNotification/Render triggers PropertyBatchMessage::Process for all property types");

  TestApplication application;

  Dali::Actor actor = Dali::Actor::New();
  application.GetScene().Add(actor);

  // Set properties that will be batched and sent via PropertyBatchMessage
  actor.SetProperty(Dali::Actor::Property::COLOR_RED, 0.5f);
  actor.SetProperty(Dali::Actor::Property::COLOR_GREEN, 0.75f);

  // Process messages (this triggers PropertyBatchMessage::Process)
  application.SendNotification();
  application.Render();

  // Verify color was applied
  Vector4 color = actor.GetCurrentProperty<Vector4>(Dali::Actor::Property::COLOR);
  DALI_TEST_EQUALS(color.r, 0.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(color.g, 0.75f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInternalPropertyBatchEndToEnd(void)
{
  tet_infoline("Test full end-to-end batching flow - verify multiple property types batched together are all applied correctly");

  TestApplication application;

  Dali::Actor actor = Dali::Actor::New();
  application.GetScene().Add(actor);

  // Set multiple properties of different types
  actor.SetProperty(Dali::Actor::Property::POSITION_X, 100.0f);
  actor.SetProperty(Dali::Actor::Property::ORIENTATION, Quaternion(Radian(45.0f), Vector3::ZAXIS));
  actor.SetProperty(Dali::Actor::Property::SCALE_X, 2.0f);

  // Process the batch
  application.SendNotification();
  application.Render();

  // Verify all properties
  Vector3 pos = actor.GetCurrentProperty<Vector3>(Dali::Actor::Property::POSITION);
  DALI_TEST_EQUALS(pos.x, 100.0f, TEST_LOCATION);

  Vector3 scale = actor.GetCurrentProperty<Vector3>(Dali::Actor::Property::SCALE);
  DALI_TEST_EQUALS(scale.x, 2.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInternalPropertyBatchFlushMessageHelper(void)
{
  tet_infoline("Test FlushPropertyBatchMessage helper - verify full message flow from event thread to update thread");

  TestApplication application;

  Dali::Actor actor = Dali::Actor::New();
  application.GetScene().Add(actor);

  // Set a property that triggers the full batching flow
  // including FlushPropertyBatchMessage
  actor.SetProperty(Dali::Actor::Property::SIZE, Vector2(200.0f, 100.0f));

  // Process
  application.SendNotification();
  application.Render();

  // Verify
  Vector2 size = actor.GetCurrentProperty<Vector2>(Dali::Actor::Property::SIZE);
  DALI_TEST_EQUALS(size.width, 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.height, 100.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInternalPropertyBatchMultipleActors(void)
{
  tet_infoline("Test dirty list with multiple actors - verify multiple actors with pending batches are all flushed correctly");

  TestApplication application;

  // Create multiple actors and set properties on all of them
  const int                NUM_ACTORS = 5;
  std::vector<Dali::Actor> actors;

  for(int i = 0; i < NUM_ACTORS; ++i)
  {
    Dali::Actor actor = Dali::Actor::New();
    application.GetScene().Add(actor);
    actors.push_back(actor);

    // Set properties to trigger batching
    actor.SetProperty(Dali::Actor::Property::POSITION_X, static_cast<float>(i * 10));
  }

  // Process all batches
  application.SendNotification();
  application.Render();

  // Verify all actors have correct positions
  for(int i = 0; i < NUM_ACTORS; ++i)
  {
    Vector3 pos = actors[i].GetCurrentProperty<Vector3>(Dali::Actor::Property::POSITION);
    DALI_TEST_EQUALS(pos.x, static_cast<float>(i * 10), TEST_LOCATION);
  }

  // Cleanup
  for(auto& actor : actors)
  {
    application.GetScene().Remove(actor);
  }

  END_TEST;
}

int UtcDaliInternalPropertyBatchCopyPropertyValueTypeMismatchVectorTypes(void)
{
  tet_infoline("Test CopyPropertyValue with type mismatches for vector types - verify Property::Value with wrong types returns false");

  TestApplication application;

  // Test VECTOR2 with FLOAT mismatch
  {
    PropertyBatch   batch;
    auto*           prop = CreateTestProperty(Vector2(0.0f, 0.0f));
    Property::Value val(3.14f); // FLOAT instead of VECTOR2
    Property::Value current(Vector2(0.0f, 0.0f));
    batch.Add(1, -1, Property::VECTOR2, prop, val, current, nullptr);
    DALI_TEST_EQUALS(batch.Count(), 0u, TEST_LOCATION); // Type mismatch: no entry added
    delete prop;
  }

  // Test VECTOR3 with INTEGER mismatch
  {
    PropertyBatch   batch;
    auto*           prop = CreateTestProperty(Vector3(0.0f, 0.0f, 0.0f));
    Property::Value val(42); // INTEGER instead of VECTOR3
    Property::Value current(Vector3(0.0f, 0.0f, 0.0f));
    batch.Add(1, -1, Property::VECTOR3, prop, val, current, nullptr);
    DALI_TEST_EQUALS(batch.Count(), 0u, TEST_LOCATION); // Type mismatch: no entry added
    delete prop;
  }

  // Test VECTOR4 with BOOLEAN mismatch
  {
    PropertyBatch   batch;
    auto*           prop = CreateTestProperty(Vector4(0.0f, 0.0f, 0.0f, 0.0f));
    Property::Value val(true); // BOOLEAN instead of VECTOR4
    Property::Value current(Vector4(0.0f, 0.0f, 0.0f, 0.0f));
    batch.Add(1, -1, Property::VECTOR4, prop, val, current, nullptr);
    DALI_TEST_EQUALS(batch.Count(), 0u, TEST_LOCATION); // Type mismatch: no entry added
    delete prop;
  }

  END_TEST;
}

int UtcDaliInternalPropertyBatchCopyPropertyValueTypeMismatchRotation(void)
{
  tet_infoline("Test CopyPropertyValue with type mismatch for ROTATION - verify Property::Value that doesn't hold Quaternion returns false");

  TestApplication application;

  PropertyBatch   batch;
  auto*           prop = CreateTestProperty(Quaternion::IDENTITY);
  Property::Value val(Vector4(1.0f, 0.0f, 0.0f, 0.0f)); // Vector4, not Quaternion
  Property::Value current(Quaternion::IDENTITY);

  // Attempting to add Vector4 value with ROTATION type should fail silently
  batch.Add(1, -1, Property::ROTATION, prop, val, current, nullptr);

  // Entry should not be added due to type mismatch
  DALI_TEST_EQUALS(batch.Count(), 0u, TEST_LOCATION);

  delete prop;

  END_TEST;
}

int UtcDaliInternalPropertyBatchAddMatrix3WithExisting(void)
{
  tet_infoline("Test Add() with existing entry and MATRIX3 type - verify heap overwrite branch updates matrix3 data in-place");

  TestApplication application;

  PropertyBatch batch;

  auto* prop = CreateTestProperty(Matrix3::IDENTITY);

  // First add - allocates heap for MATRIX3 (36 bytes)
  Matrix3         m1(Matrix3::IDENTITY);
  Property::Value val1(m1);
  Property::Value current1(Matrix3::IDENTITY);
  batch.Add(1, -1, Property::MATRIX3, prop, val1, current1, nullptr);
  DALI_TEST_EQUALS(batch.Count(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(batch[0].type == Property::MATRIX3);

  // Second add with existing entry - MATRIX3 type (heap overwrite branch)
  // Create a modified matrix to verify it gets updated
  float* matrixData = new float[9];
  for(int i = 0; i < 9; ++i)
  {
    matrixData[i] = static_cast<float>(i + 1);
  }
  Matrix3         m2(matrixData[0], matrixData[1], matrixData[2], matrixData[3], matrixData[4], matrixData[5], matrixData[6], matrixData[7], matrixData[8]);
  Property::Value val2(m2);
  Property::Value current2(Matrix3::IDENTITY);

  PropertyBatchEntry* existing = batch.Find(prop);
  DALI_TEST_CHECK(existing != nullptr);
  batch.Add(2, -1, Property::MATRIX3, prop, val2, current2, existing);

  // Should still be 1 entry
  DALI_TEST_EQUALS(batch.Count(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(batch[0].type == Property::MATRIX3);

  delete[] matrixData;
  delete prop;

  END_TEST;
}

int UtcDaliInternalPropertyBatchMergeComponentInvalidType(void)
{
  tet_infoline("Test MergeComponent with a type-mismatched component value - verify Property::Value that doesn't hold a float returns false and leaves the entry untouched");

  TestApplication application;

  PropertyBatch batch;

  auto*           prop = CreateTestProperty(Vector2(5.0f, 6.0f));
  Property::Value current(Vector2(5.0f, 6.0f));

  // Valid component set, to have an existing entry to merge into
  Property::Value valX(1.0f);
  batch.Add(1, 0, Property::VECTOR2, prop, valX, current, nullptr);

  DALI_TEST_EQUALS(batch.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(batch[0].value.v2[0], 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(batch[0].value.v2[1], 6.0f, TEST_LOCATION);

  // Type-mismatched component set on the SAME existing entry: value.Get(float) fails,
  // so MergeComponent() must return false and leave the already-coalesced value alone.
  // Note: Property::Value::Get(float&) coerces BOOLEAN and INTEGER to float (true -> 1.0f),
  // so those two don't actually reach the failure path - a VECTOR3 does, since Get(float&)
  // only ever succeeds for FLOAT/BOOLEAN/INTEGER sources.
  Property::Value badValY(Vector3(9.0f, 9.0f, 9.0f));
  batch.Add(1, 1, Property::VECTOR2, prop, badValY, current, &batch[0]);

  DALI_TEST_EQUALS(batch.Count(), 1u, TEST_LOCATION);          // still just the one entry
  DALI_TEST_EQUALS(batch[0].value.v2[0], 1.0f, TEST_LOCATION); // untouched
  DALI_TEST_EQUALS(batch[0].value.v2[1], 6.0f, TEST_LOCATION); // untouched, not overwritten with garbage

  delete prop;

  END_TEST;
}

int UtcDaliInternalPropertyBatchUnlinkFromDirtyList(void)
{
  tet_infoline("Test UnlinkFromDirtyList free function - verify same-thread, self-locking entry point unlinks a single Object in isolation");

  TestApplication application;

  Dali::Actor actor = Dali::Actor::New();
  application.GetScene().Add(actor);

  Property::Index customProperty = actor.RegisterProperty("customAnimatableFloat", 0.0f, Property::ANIMATABLE);
  actor.SetProperty(customProperty, 10.0f);

  // Directly call the self-locking entry point (this is its documented purpose -
  // see property-batch-free-list.h) rather than going through
  // ReleasePendingPropertyBatch()/FlushPropertyBatchLocked(), which only ever call
  // the already-locked UnlinkFromDirtyListLocked() variant.
  Object& internalObject = GetImplementation(actor);
  UnlinkFromDirtyList(&internalObject);

  // Being the only node, unlinking twice (here, then again from
  // ReleasePendingPropertyBatch() when the actor is destroyed below) is a
  // harmless no-op: prevDirty/nextDirty are both null before and after.
  application.GetScene().Remove(actor);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(true);

  END_TEST;
}

int UtcDaliInternalPropertyBatchGetFullCurrentPropertyValueVectorTypes(void)
{
  tet_infoline("Test GetFullCurrentPropertyValue for vector types (VECTOR2/3/4) - verify component seeding from current values");

  TestApplication application;

  // Test VECTOR2 property seeding
  {
    Dali::Actor actor = Dali::Actor::New();
    application.GetScene().Add(actor);

    actor.SetProperty(Dali::Actor::Property::SIZE_WIDTH, 100.0f);
    application.SendNotification();
    application.Render();

    Vector2 size = actor.GetCurrentProperty<Vector2>(Dali::Actor::Property::SIZE);
    DALI_TEST_EQUALS(size.width, 100.0f, TEST_LOCATION);

    application.GetScene().Remove(actor);
  }

  // Test VECTOR3 property seeding
  {
    Dali::Actor actor = Dali::Actor::New();
    application.GetScene().Add(actor);

    actor.SetProperty(Dali::Actor::Property::POSITION_X, 50.0f);
    application.SendNotification();
    application.Render();

    Vector3 pos = actor.GetCurrentProperty<Vector3>(Dali::Actor::Property::POSITION);
    DALI_TEST_EQUALS(pos.x, 50.0f, TEST_LOCATION);

    application.GetScene().Remove(actor);
  }

  // Test VECTOR4 property seeding
  {
    Dali::Actor actor = Dali::Actor::New();
    application.GetScene().Add(actor);

    actor.SetProperty(Dali::Actor::Property::COLOR_RED, 0.5f);
    actor.SetProperty(Dali::Actor::Property::COLOR_GREEN, 0.75f);
    application.SendNotification();
    application.Render();

    Vector4 color = actor.GetCurrentProperty<Vector4>(Dali::Actor::Property::COLOR);
    DALI_TEST_EQUALS(color.r, 0.5f, TEST_LOCATION);
    DALI_TEST_EQUALS(color.g, 0.75f, TEST_LOCATION);

    application.GetScene().Remove(actor);
  }

  END_TEST;
}

int UtcDaliInternalPropertyBatchMessageProcessingAllTypes(void)
{
  tet_infoline("Test PropertyBatchMessage::Process() for all property types - verify BOOLEAN, FLOAT, INTEGER, VECTOR2/3/4, ROTATION, MATRIX, MATRIX3 are all baked correctly");

  TestApplication application;

  Dali::Actor actor = Dali::Actor::New();
  application.GetScene().Add(actor);

  // Test BOOLEAN through batching message
  actor.SetProperty(Dali::Actor::Property::VISIBLE, false);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(actor.GetCurrentProperty<bool>(Dali::Actor::Property::VISIBLE) == false);

  // Test FLOAT through batching message
  actor.SetProperty(Dali::Actor::Property::OPACITY, 0.75f);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(actor.GetCurrentProperty<float>(Dali::Actor::Property::OPACITY), 0.75f, TEST_LOCATION);

  // Test VECTOR2 through batching message
  actor.SetProperty(Dali::Actor::Property::SIZE, Vector2(200.0f, 150.0f));
  application.SendNotification();
  application.Render();
  Vector2 size = actor.GetCurrentProperty<Vector2>(Dali::Actor::Property::SIZE);
  DALI_TEST_EQUALS(size.width, 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.height, 150.0f, TEST_LOCATION);

  // Test VECTOR3 through batching message
  actor.SetProperty(Dali::Actor::Property::POSITION, Vector3(100.0f, 200.0f, 300.0f));
  application.SendNotification();
  application.Render();
  Vector3 pos = actor.GetCurrentProperty<Vector3>(Dali::Actor::Property::POSITION);
  DALI_TEST_EQUALS(pos.x, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(pos.y, 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(pos.z, 300.0f, TEST_LOCATION);

  // Test VECTOR4 through batching message
  actor.SetProperty(Dali::Actor::Property::COLOR, Vector4(0.5f, 0.6f, 0.7f, 0.8f));
  application.SendNotification();
  application.Render();
  Vector4 color = actor.GetCurrentProperty<Vector4>(Dali::Actor::Property::COLOR);
  DALI_TEST_EQUALS(color.r, 0.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(color.g, 0.6f, TEST_LOCATION);
  DALI_TEST_EQUALS(color.b, 0.7f, TEST_LOCATION);
  DALI_TEST_EQUALS(color.a, 0.8f, TEST_LOCATION);

  // Test ROTATION through batching message
  // ROTATION is never set via component messages, always arrives as full-property set
  // and uses the v4 slot of the union (x, y, z, w for quaternion components)
  Quaternion testQuat(Radian(1.57f), Vector3::YAXIS); // ~90 degrees around Y axis
  actor.SetProperty(Dali::Actor::Property::ORIENTATION, testQuat);
  application.SendNotification();
  application.Render();
  Quaternion orientation = actor.GetCurrentProperty<Quaternion>(Dali::Actor::Property::ORIENTATION);
  // Verify the quaternion components were properly baked
  DALI_TEST_EQUALS(orientation.mVector.x, testQuat.mVector.x, TEST_LOCATION);
  DALI_TEST_EQUALS(orientation.mVector.y, testQuat.mVector.y, TEST_LOCATION);
  DALI_TEST_EQUALS(orientation.mVector.z, testQuat.mVector.z, TEST_LOCATION);
  DALI_TEST_EQUALS(orientation.mVector.w, testQuat.mVector.w, TEST_LOCATION);

  application.GetScene().Remove(actor);

  END_TEST;
}

int UtcDaliInternalPropertyBatchReleasePendingPropertyBatchCrossThread(void)
{
  tet_infoline("Test ReleasePendingPropertyBatch cross-thread case - verify a pending batch is unlinked and freed safely when the last Actor handle is dropped on a different thread");

  TestApplication application;

  Dali::Actor actor = Dali::Actor::New();
  application.GetScene().Add(actor);

  Property::Index customProperty = actor.RegisterProperty("customAnimatableFloat", 0.0f, Property::ANIMATABLE);
  actor.SetProperty(customProperty, 10.0f);

  // Drop the Scene's reference so the worker thread below ends up holding the last one.
  application.GetScene().Remove(actor);

  // Move the last handle onto a worker thread and let it go out of scope there.
  // EventThreadServices::IsEventThread() is false on a plain std::thread (no
  // ThreadLocalStorage was ever created there), so ~Object() takes
  // ReleasePendingPropertyBatch()'s cross-thread branch: lock the *owning* thread's
  // dirty list (mPropertyBatch->owningList, captured back when this Object was
  // linked - not looked up via this thread), unlink, then free the batch outright
  // rather than recycling it through the (thread-local, cross-thread-unsafe) free-list.
  std::thread worker([movedActor = std::move(actor)]() mutable
                     { movedActor.Reset(); });
  worker.join();

  // Reaching here without crashing/asserting is the pass condition - there is no
  // Actor left afterwards to inspect any observable state on.
  DALI_TEST_CHECK(true);

  END_TEST;
}
