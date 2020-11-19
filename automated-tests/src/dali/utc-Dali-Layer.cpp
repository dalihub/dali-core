/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

void layer_test_startup(void)
{
  test_return_value = TET_UNDEF;
}

void layer_test_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliLayerNew(void)
{
  TestApplication application;
  Layer           layer = Layer::New();

  DALI_TEST_CHECK(layer);
  END_TEST;
}

int UtcDaliLayerDownCast(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Layer::DownCast()");

  Layer actor1  = Layer::New();
  Actor anActor = Actor::New();
  anActor.Add(actor1);

  Actor child = anActor.GetChildAt(0);
  Layer layer = DownCast<Layer>(child);

  DALI_TEST_CHECK(layer);
  END_TEST;
}

int UtcDaliLayerDownCast2(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Layer::DownCast()");

  Actor actor1  = Actor::New();
  Actor anActor = Actor::New();
  anActor.Add(actor1);

  Actor child = anActor.GetChildAt(0);
  Layer layer = DownCast<Layer>(child);
  DALI_TEST_CHECK(!layer);

  Actor unInitialzedActor;
  layer = Layer::DownCast(unInitialzedActor);
  DALI_TEST_CHECK(!layer);
  END_TEST;
}

int UtcDaliLayerMoveConstructor(void)
{
  TestApplication application;
  Layer           layer = Layer::New();
  DALI_TEST_CHECK(layer);
  DALI_TEST_EQUALS(1, layer.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(0, layer.GetProperty<int>(Layer::Property::DEPTH), TEST_LOCATION);

  application.GetScene().Add(layer);
  DALI_TEST_EQUALS(2, layer.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(1, layer.GetProperty<int>(Layer::Property::DEPTH), TEST_LOCATION);

  Layer move = std::move(layer);
  DALI_TEST_CHECK(move);
  DALI_TEST_EQUALS(2, move.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(1, move.GetProperty<int>(Layer::Property::DEPTH), TEST_LOCATION);
  DALI_TEST_CHECK(!layer);

  END_TEST;
}

int UtcDaliLayerMoveAssignment(void)
{
  TestApplication application;
  Layer           layer = Layer::New();
  DALI_TEST_CHECK(layer);
  DALI_TEST_EQUALS(1, layer.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(0, layer.GetProperty<int>(Layer::Property::DEPTH), TEST_LOCATION);

  application.GetScene().Add(layer);
  DALI_TEST_EQUALS(2, layer.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(1, layer.GetProperty<int>(Layer::Property::DEPTH), TEST_LOCATION);

  Layer move;
  move = std::move(layer);
  DALI_TEST_CHECK(move);
  DALI_TEST_EQUALS(2, move.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(1, move.GetProperty<int>(Layer::Property::DEPTH), TEST_LOCATION);
  DALI_TEST_CHECK(!layer);

  END_TEST;
}

int UtcDaliLayerGetDepth(void)
{
  tet_infoline("Testing Dali::Layer::GetDepth()");
  TestApplication application;
  Layer           layer1 = Layer::New();
  Layer           layer2 = Layer::New();

  // layers are not on scene
  DALI_TEST_EQUALS(layer1.GetProperty<int>(Layer::Property::DEPTH), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(layer2.GetProperty<int>(Layer::Property::DEPTH), 0u, TEST_LOCATION);

  // root depth is 0
  Layer root = application.GetScene().GetLayer(0);
  DALI_TEST_EQUALS(root.GetProperty<int>(Layer::Property::DEPTH), 0u, TEST_LOCATION);

  application.GetScene().Add(layer1);
  application.GetScene().Add(layer2);

  DALI_TEST_EQUALS(root.GetProperty<int>(Layer::Property::DEPTH), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(layer1.GetProperty<int>(Layer::Property::DEPTH), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(layer2.GetProperty<int>(Layer::Property::DEPTH), 2u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayerRaise1(void)
{
  tet_infoline("Testing Dali::Layer::Raise()");
  TestApplication application;
  Layer           layer1 = Layer::New();
  Layer           layer2 = Layer::New();

  application.GetScene().Add(layer1);
  application.GetScene().Add(layer2);
  DALI_TEST_EQUALS(layer1.GetProperty<int>(Layer::Property::DEPTH), 1u, TEST_LOCATION);

  layer1.Raise();
  DALI_TEST_EQUALS(layer1.GetProperty<int>(Layer::Property::DEPTH), 2u, TEST_LOCATION);

  // get root
  Layer root = application.GetScene().GetLayer(0);
  DALI_TEST_EQUALS(root.GetProperty<int>(Layer::Property::DEPTH), 0u, TEST_LOCATION);
  root.Raise();
  DALI_TEST_EQUALS(root.GetProperty<int>(Layer::Property::DEPTH), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(layer1.GetProperty<int>(Layer::Property::DEPTH), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(layer2.GetProperty<int>(Layer::Property::DEPTH), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayerRaise2(void)
{
  tet_infoline("Testing Dali::Layer raise Action");
  TestApplication application;
  Layer           layer1 = Layer::New();
  Layer           layer2 = Layer::New();

  application.GetScene().Add(layer1);
  application.GetScene().Add(layer2);
  DALI_TEST_EQUALS(layer1.GetProperty<int>(Layer::Property::DEPTH), 1u, TEST_LOCATION);

  layer1.Raise();
  DALI_TEST_EQUALS(layer1.GetProperty<int>(Layer::Property::DEPTH), 2u, TEST_LOCATION);

  // get root
  Layer root = application.GetScene().GetLayer(0);
  DALI_TEST_EQUALS(root.GetProperty<int>(Layer::Property::DEPTH), 0u, TEST_LOCATION);

  GetImplementation(root).DoAction("raise", Property::Map());

  DALI_TEST_EQUALS(root.GetProperty<int>(Layer::Property::DEPTH), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(layer1.GetProperty<int>(Layer::Property::DEPTH), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(layer2.GetProperty<int>(Layer::Property::DEPTH), 0u, TEST_LOCATION);
  END_TEST;
}


int UtcDaliLayerLower1(void)
{
  tet_infoline("Testing Dali::Layer::Lower()");
  TestApplication application;
  Layer           layer1 = Layer::New();
  Layer           layer2 = Layer::New();

  application.GetScene().Add(layer1);
  application.GetScene().Add(layer2);
  DALI_TEST_EQUALS(layer2.GetProperty<int>(Layer::Property::DEPTH), 2u, TEST_LOCATION);

  layer2.Lower();
  DALI_TEST_EQUALS(layer2.GetProperty<int>(Layer::Property::DEPTH), 1u, TEST_LOCATION);

  // get root
  Layer root = application.GetScene().GetLayer(0);
  root.Lower();
  DALI_TEST_EQUALS(root.GetProperty<int>(Layer::Property::DEPTH), 0u, TEST_LOCATION);
  layer2.Lower();
  DALI_TEST_EQUALS(root.GetProperty<int>(Layer::Property::DEPTH), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(layer2.GetProperty<int>(Layer::Property::DEPTH), 0u, TEST_LOCATION);
  END_TEST;
}


int UtcDaliLayerLower2(void)
{
  tet_infoline("Testing Dali::Layer lower Action");
  TestApplication application;
  Layer           layer1 = Layer::New();
  Layer           layer2 = Layer::New();

  application.GetScene().Add(layer1);
  application.GetScene().Add(layer2);
  DALI_TEST_EQUALS(layer2.GetProperty<int>(Layer::Property::DEPTH), 2u, TEST_LOCATION);

  layer2.Lower();
  DALI_TEST_EQUALS(layer2.GetProperty<int>(Layer::Property::DEPTH), 1u, TEST_LOCATION);

  // get root
  Layer root = application.GetScene().GetLayer(0);
  GetImplementation(root).DoAction("lower", Property::Map());
  DALI_TEST_EQUALS(root.GetProperty<int>(Layer::Property::DEPTH), 0u, TEST_LOCATION);

  GetImplementation(layer2).DoAction("lower", Property::Map());
  DALI_TEST_EQUALS(root.GetProperty<int>(Layer::Property::DEPTH), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(layer2.GetProperty<int>(Layer::Property::DEPTH), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayerRaiseToTop1(void)
{
  tet_infoline("Testing Dali::Layer::RaiseToTop()");
  TestApplication application;
  Layer           layer1 = Layer::New();
  Layer           layer2 = Layer::New();
  Layer           layer3 = Layer::New();

  application.GetScene().Add(layer1);
  application.GetScene().Add(layer2);
  application.GetScene().Add(layer3);
  Layer root = application.GetScene().GetLayer(0);

  DALI_TEST_EQUALS(root.GetProperty<int>(Layer::Property::DEPTH), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(layer1.GetProperty<int>(Layer::Property::DEPTH), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(layer2.GetProperty<int>(Layer::Property::DEPTH), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(layer3.GetProperty<int>(Layer::Property::DEPTH), 3u, TEST_LOCATION);

  layer1.RaiseToTop();
  DALI_TEST_EQUALS(layer1.GetProperty<int>(Layer::Property::DEPTH), 3u, TEST_LOCATION);

  root.RaiseToTop();
  DALI_TEST_EQUALS(root.GetProperty<int>(Layer::Property::DEPTH), 3u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayerRaiseToTop2(void)
{
  tet_infoline("Testing Dali::Layer raiseToTop Action");
  TestApplication application;
  Layer           layer1 = Layer::New();
  Layer           layer2 = Layer::New();
  Layer           layer3 = Layer::New();

  application.GetScene().Add(layer1);
  application.GetScene().Add(layer2);
  application.GetScene().Add(layer3);
  Layer root = application.GetScene().GetLayer(0);

  DALI_TEST_EQUALS(root.GetProperty<int>(Layer::Property::DEPTH), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(layer1.GetProperty<int>(Layer::Property::DEPTH), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(layer2.GetProperty<int>(Layer::Property::DEPTH), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(layer3.GetProperty<int>(Layer::Property::DEPTH), 3u, TEST_LOCATION);

  GetImplementation(layer1).DoAction("raiseToTop", Property::Map());
  DALI_TEST_EQUALS(layer1.GetProperty<int>(Layer::Property::DEPTH), 3u, TEST_LOCATION);

  GetImplementation(root).DoAction("raiseToTop", Property::Map());
  DALI_TEST_EQUALS(root.GetProperty<int>(Layer::Property::DEPTH), 3u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayerLowerToBottom1(void)
{
  tet_infoline("Testing Dali::Layer::LowerToBottom()");
  TestApplication application;
  Layer           layer1 = Layer::New();
  Layer           layer2 = Layer::New();
  Layer           layer3 = Layer::New();

  application.GetScene().Add(layer1);
  application.GetScene().Add(layer2);
  application.GetScene().Add(layer3);

  DALI_TEST_EQUALS(layer1.GetProperty<int>(Layer::Property::DEPTH), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(layer2.GetProperty<int>(Layer::Property::DEPTH), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(layer3.GetProperty<int>(Layer::Property::DEPTH), 3u, TEST_LOCATION);

  layer3.LowerToBottom();
  DALI_TEST_EQUALS(layer3.GetProperty<int>(Layer::Property::DEPTH), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayerLowerToBottom2(void)
{
  tet_infoline("Testing Dali::Layer lowerToBottom Action");
  TestApplication application;
  Layer           layer1 = Layer::New();
  Layer           layer2 = Layer::New();
  Layer           layer3 = Layer::New();

  application.GetScene().Add(layer1);
  application.GetScene().Add(layer2);
  application.GetScene().Add(layer3);

  DALI_TEST_EQUALS(layer1.GetProperty<int>(Layer::Property::DEPTH), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(layer2.GetProperty<int>(Layer::Property::DEPTH), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(layer3.GetProperty<int>(Layer::Property::DEPTH), 3u, TEST_LOCATION);

  GetImplementation(layer3).DoAction("lowerToBottom", Property::Map());
  DALI_TEST_EQUALS(layer3.GetProperty<int>(Layer::Property::DEPTH), 0u, TEST_LOCATION);
  END_TEST;
}


int UtcDaliLayerSetClipping(void)
{
  tet_infoline("Testing Dali::Layer::SetClipping()");
  TestApplication application;

  Layer layer = Layer::New();
  DALI_TEST_CHECK(!layer.GetProperty<bool>(Layer::Property::CLIPPING_ENABLE));

  layer.SetProperty(Layer::Property::CLIPPING_ENABLE, true);
  DALI_TEST_CHECK(layer.GetProperty<bool>(Layer::Property::CLIPPING_ENABLE));
  END_TEST;
}

int UtcDaliLayerIsClipping(void)
{
  tet_infoline("Testing Dali::Layer::IsClipping()");
  TestApplication application;

  Layer layer = Layer::New();
  DALI_TEST_CHECK(!layer.GetProperty<bool>(Layer::Property::CLIPPING_ENABLE));
  END_TEST;
}

int UtcDaliLayerSetClippingBox(void)
{
  tet_infoline("Testing Dali::Layer::SetClippingBox()");
  TestApplication application;

  ClippingBox testBox(5, 6, 77, 83);

  Layer layer = Layer::New();
  DALI_TEST_CHECK(layer.GetProperty<Rect<int32_t> >(Layer::Property::CLIPPING_BOX) != testBox);
  layer.SetProperty(Layer::Property::CLIPPING_BOX, testBox);
  DALI_TEST_CHECK(layer.GetProperty<Rect<int32_t> >(Layer::Property::CLIPPING_BOX) == testBox);
  END_TEST;
}

int UtcDaliLayerGetClippingBox(void)
{
  tet_infoline("Testing Dali::Layer::GetClippingBox()");
  TestApplication application;

  Layer layer = Layer::New();
  DALI_TEST_CHECK(layer.GetProperty<Rect<int32_t> >(Layer::Property::CLIPPING_BOX) == ClippingBox(0, 0, 0, 0));
  END_TEST;
}

static int gTestSortFunctionCalled;

static float TestSortFunction(const Vector3& /*position*/)
{
  ++gTestSortFunctionCalled;
  return 0.0f;
}

int UtcDaliLayerSetSortFunction(void)
{
  tet_infoline("Testing Dali::Layer::SetSortFunction()");
  TestApplication application;

  // create two transparent actors so there is something to sort
  Actor actor  = CreateRenderableActor();
  Actor actor2 = CreateRenderableActor();
  actor.SetProperty(Actor::Property::SIZE, Vector2(1, 1));
  actor.SetProperty(Actor::Property::COLOR, Vector4(1, 1, 1, 0.5f)); // 50% transparent
  actor2.SetProperty(Actor::Property::SIZE, Vector2(1, 1));
  actor2.SetProperty(Actor::Property::COLOR, Vector4(1, 1, 1, 0.5f)); // 50% transparent

  // add to scene
  application.GetScene().Add(actor);
  application.GetScene().Add(actor2);

  Layer root              = application.GetScene().GetLayer(0);
  gTestSortFunctionCalled = 0;
  root.SetSortFunction(TestSortFunction);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(gTestSortFunctionCalled > 0);
  END_TEST;
}

int UtcDaliLayerRaiseAbove(void)
{
  tet_infoline("Testing Dali::Layer::RaiseAbove()");
  TestApplication application;
  Layer           layer = Layer::New();
  // try to raise above root layer
  Layer root = application.GetScene().GetLayer(0);
  layer.RaiseAbove(root);
  // layer depth is zero as its not on scene
  DALI_TEST_EQUALS(layer.GetProperty<int>(Layer::Property::DEPTH), 0u, TEST_LOCATION);
  // add to scene
  application.GetScene().Add(layer);
  layer.RaiseAbove(root);
  DALI_TEST_EQUALS(layer.GetProperty<int>(Layer::Property::DEPTH), 1u, TEST_LOCATION);
  root.RaiseAbove(layer);
  DALI_TEST_EQUALS(layer.GetProperty<int>(Layer::Property::DEPTH), 0u, TEST_LOCATION);
  layer.RaiseAbove(layer);
  DALI_TEST_EQUALS(layer.GetProperty<int>(Layer::Property::DEPTH), 0u, TEST_LOCATION);

  // make another layer on the scene
  Layer layer2 = Layer::New();
  application.GetScene().Add(layer2);
  layer.RaiseAbove(layer2);
  DALI_TEST_GREATER(layer.GetProperty<int>(Layer::Property::DEPTH), layer2.GetProperty<int>(Layer::Property::DEPTH), TEST_LOCATION);
  layer2.RaiseAbove(layer);
  DALI_TEST_GREATER(layer2.GetProperty<int>(Layer::Property::DEPTH), layer.GetProperty<int>(Layer::Property::DEPTH), TEST_LOCATION);
  root.RaiseAbove(layer2);
  DALI_TEST_GREATER(root.GetProperty<int>(Layer::Property::DEPTH), layer2.GetProperty<int>(Layer::Property::DEPTH), TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayerRaiseBelow(void)
{
  tet_infoline("Testing Dali::Layer::RaiseBelow()");
  TestApplication application;
  Layer           layer = Layer::New();
  // try to lower below root layer
  Layer root = application.GetScene().GetLayer(0);
  layer.LowerBelow(root);
  // layer depth is zero as its not on scene
  DALI_TEST_EQUALS(layer.GetProperty<int>(Layer::Property::DEPTH), 0u, TEST_LOCATION);
  // add to scene
  application.GetScene().Add(layer);
  DALI_TEST_EQUALS(layer.GetProperty<int>(Layer::Property::DEPTH), 1u, TEST_LOCATION);
  layer.LowerBelow(root);
  DALI_TEST_EQUALS(layer.GetProperty<int>(Layer::Property::DEPTH), 0u, TEST_LOCATION);
  root.LowerBelow(layer);
  DALI_TEST_EQUALS(layer.GetProperty<int>(Layer::Property::DEPTH), 1u, TEST_LOCATION);
  layer.LowerBelow(layer);
  DALI_TEST_EQUALS(layer.GetProperty<int>(Layer::Property::DEPTH), 1u, TEST_LOCATION);

  // make another layer on the scene
  Layer layer2 = Layer::New();
  application.GetScene().Add(layer2);
  layer.LowerBelow(layer2);
  DALI_TEST_GREATER(layer2.GetProperty<int>(Layer::Property::DEPTH), layer.GetProperty<int>(Layer::Property::DEPTH), TEST_LOCATION);
  layer2.LowerBelow(layer);
  DALI_TEST_GREATER(layer.GetProperty<int>(Layer::Property::DEPTH), layer2.GetProperty<int>(Layer::Property::DEPTH), TEST_LOCATION);
  root.LowerBelow(layer2);
  DALI_TEST_GREATER(layer2.GetProperty<int>(Layer::Property::DEPTH), root.GetProperty<int>(Layer::Property::DEPTH), TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayerMoveAbove(void)
{
  tet_infoline("Testing Dali::Layer::MoveAbove()");
  TestApplication application;
  Layer           layer = Layer::New();
  // try to raise above root layer
  Layer root = application.GetScene().GetLayer(0);
  layer.MoveAbove(root);
  // layer depth is zero as its not on scene
  DALI_TEST_EQUALS(layer.GetProperty<int>(Layer::Property::DEPTH), 0u, TEST_LOCATION);
  root.MoveAbove(layer);
  // root depth is zero as layer is not on scene
  DALI_TEST_EQUALS(layer.GetProperty<int>(Layer::Property::DEPTH), 0u, TEST_LOCATION);
  // add to scene
  application.GetScene().Add(layer);
  layer.MoveAbove(root);
  DALI_TEST_EQUALS(layer.GetProperty<int>(Layer::Property::DEPTH), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(root.GetProperty<int>(Layer::Property::DEPTH), 0u, TEST_LOCATION);
  root.MoveAbove(layer);
  DALI_TEST_EQUALS(layer.GetProperty<int>(Layer::Property::DEPTH), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(root.GetProperty<int>(Layer::Property::DEPTH), 1u, TEST_LOCATION);

  // make another layer on the scene
  Layer layer2 = Layer::New();
  application.GetScene().Add(layer2);
  layer.MoveAbove(layer2);
  DALI_TEST_EQUALS(layer.GetProperty<int>(Layer::Property::DEPTH), layer2.GetProperty<int>(Layer::Property::DEPTH) + 1u, TEST_LOCATION);
  layer2.MoveAbove(root);
  DALI_TEST_EQUALS(layer2.GetProperty<int>(Layer::Property::DEPTH), root.GetProperty<int>(Layer::Property::DEPTH) + 1u, TEST_LOCATION);
  root.MoveAbove(layer);
  DALI_TEST_EQUALS(root.GetProperty<int>(Layer::Property::DEPTH), layer.GetProperty<int>(Layer::Property::DEPTH) + 1u, TEST_LOCATION);

  Layer layer3 = Layer::New();
  application.GetScene().Add(layer3);
  DALI_TEST_EQUALS(layer3.GetProperty<int>(Layer::Property::DEPTH), 3u, TEST_LOCATION);
  root.MoveAbove(layer3);
  DALI_TEST_EQUALS(root.GetProperty<int>(Layer::Property::DEPTH), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(layer3.GetProperty<int>(Layer::Property::DEPTH), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetScene().GetLayer(0).GetProperty<int>(Layer::Property::DEPTH), 0u, TEST_LOCATION);
  layer3.MoveAbove(application.GetScene().GetLayer(0));
  DALI_TEST_EQUALS(layer3.GetProperty<int>(Layer::Property::DEPTH), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayerMoveBelow(void)
{
  tet_infoline("Testing Dali::Layer::MoveBelow()");
  TestApplication application;
  Layer           layer = Layer::New();
  // try to raise above root layer
  Layer root = application.GetScene().GetLayer(0);
  layer.MoveBelow(root);
  // layer depth is zero as its not on scene
  DALI_TEST_EQUALS(layer.GetProperty<int>(Layer::Property::DEPTH), 0u, TEST_LOCATION);
  root.MoveBelow(layer);
  // root depth is zero as layer is not on scene
  DALI_TEST_EQUALS(layer.GetProperty<int>(Layer::Property::DEPTH), 0u, TEST_LOCATION);
  // add to scene
  application.GetScene().Add(layer);
  layer.MoveBelow(root);
  DALI_TEST_EQUALS(layer.GetProperty<int>(Layer::Property::DEPTH), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(root.GetProperty<int>(Layer::Property::DEPTH), 1u, TEST_LOCATION);
  root.MoveBelow(layer);
  DALI_TEST_EQUALS(layer.GetProperty<int>(Layer::Property::DEPTH), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(root.GetProperty<int>(Layer::Property::DEPTH), 0u, TEST_LOCATION);

  // make another layer on the scene
  Layer layer2 = Layer::New();
  application.GetScene().Add(layer2);
  layer.MoveBelow(layer2);
  DALI_TEST_EQUALS(layer.GetProperty<int>(Layer::Property::DEPTH), layer2.GetProperty<int>(Layer::Property::DEPTH) - 1u, TEST_LOCATION);
  layer2.MoveBelow(root);
  DALI_TEST_EQUALS(layer2.GetProperty<int>(Layer::Property::DEPTH), root.GetProperty<int>(Layer::Property::DEPTH) - 1u, TEST_LOCATION);
  root.MoveBelow(layer);
  DALI_TEST_EQUALS(root.GetProperty<int>(Layer::Property::DEPTH), layer.GetProperty<int>(Layer::Property::DEPTH) - 1u, TEST_LOCATION);

  Layer layer3 = Layer::New();
  application.GetScene().Add(layer3);
  DALI_TEST_EQUALS(layer3.GetProperty<int>(Layer::Property::DEPTH), 3u, TEST_LOCATION);
  root.MoveBelow(layer3);
  DALI_TEST_EQUALS(root.GetProperty<int>(Layer::Property::DEPTH), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(layer3.GetProperty<int>(Layer::Property::DEPTH), 3u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayerDefaultProperties(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Layer DefaultProperties");

  Layer actor = Layer::New();

  std::vector<Property::Index> indices;
  indices.push_back(Layer::Property::CLIPPING_ENABLE);
  indices.push_back(Layer::Property::CLIPPING_BOX);
  indices.push_back(Layer::Property::BEHAVIOR);
  indices.push_back(Layer::Property::DEPTH);
  indices.push_back(Layer::Property::DEPTH_TEST);
  indices.push_back(Layer::Property::CONSUMES_TOUCH);
  indices.push_back(Layer::Property::CONSUMES_HOVER);

  DALI_TEST_CHECK(actor.GetPropertyCount() == (Actor::New().GetPropertyCount() + indices.size()));

  for(std::vector<Property::Index>::iterator iter = indices.begin(); iter != indices.end(); ++iter)
  {
    DALI_TEST_CHECK(*iter == actor.GetPropertyIndex(actor.GetPropertyName(*iter)));
    DALI_TEST_CHECK(*iter == Layer::Property::DEPTH ? !actor.IsPropertyWritable(*iter) : actor.IsPropertyWritable(*iter));
    DALI_TEST_CHECK(!actor.IsPropertyAnimatable(*iter));
    DALI_TEST_CHECK(actor.GetPropertyType(*iter) == actor.GetPropertyType(*iter)); // just checking call succeeds
  }

  // set/get one of them
  actor.SetProperty(Layer::Property::CLIPPING_BOX, ClippingBox(0, 0, 0, 0));

  ClippingBox testBox(10, 20, 30, 40);
  DALI_TEST_CHECK(actor.GetProperty<Rect<int32_t> >(Layer::Property::CLIPPING_BOX) != testBox);

  actor.SetProperty(Layer::Property::CLIPPING_BOX, Property::Value(Rect<int>(testBox)));

  DALI_TEST_CHECK(Property::RECTANGLE == actor.GetPropertyType(Layer::Property::CLIPPING_BOX));

  Property::Value v = actor.GetProperty(Layer::Property::CLIPPING_BOX);
  DALI_TEST_CHECK(v.Get<Rect<int> >() == testBox);

  v = actor.GetCurrentProperty(Layer::Property::CLIPPING_BOX);
  DALI_TEST_CHECK(v.Get<Rect<int> >() == testBox);

  // set the same boundaries, but through a clipping box object
  actor.SetProperty(Layer::Property::CLIPPING_BOX, testBox);
  DALI_TEST_CHECK(actor.GetProperty<Rect<int32_t> >(Layer::Property::CLIPPING_BOX) == testBox);

  actor.SetProperty(Layer::Property::BEHAVIOR, Property::Value(Layer::LAYER_UI));
  DALI_TEST_CHECK(Property::INTEGER == actor.GetPropertyType(Layer::Property::BEHAVIOR));

  Property::Value behavior = actor.GetProperty(Layer::Property::BEHAVIOR);
  DALI_TEST_EQUALS(behavior.Get<Dali::Layer::Behavior>(), Layer::LAYER_UI, TEST_LOCATION);

  behavior = actor.GetCurrentProperty(Layer::Property::BEHAVIOR);
  DALI_TEST_EQUALS(behavior.Get<Dali::Layer::Behavior>(), Layer::LAYER_UI, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLayerSetDepthTestDisabled(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Layer::SetDepthTestDisabled() ");

  Layer actor = Layer::New();
  // Note that Layer::Property::DEPTH_TEST does not depend on layer behavior,
  // as 2D layers can still have depth tests performed on a per-renderer basis.
  // Check default.
  DALI_TEST_CHECK(!actor.GetProperty<bool>(Layer::Property::DEPTH_TEST));

  // Check Set / Unset.
  actor.SetProperty(Layer::Property::DEPTH_TEST, true);
  DALI_TEST_CHECK(actor.GetProperty<bool>(Layer::Property::DEPTH_TEST));
  actor.SetProperty(Layer::Property::DEPTH_TEST, false);
  DALI_TEST_CHECK(!actor.GetProperty<bool>(Layer::Property::DEPTH_TEST));

  END_TEST;
}

int UtcDaliLayerCreateDestroy(void)
{
  tet_infoline("Testing Dali::Layer::CreateDestroy() ");
  Layer* layer = new Layer;
  DALI_TEST_CHECK(layer);
  delete layer;
  END_TEST;
}

int UtcDaliLayerPropertyIndices(void)
{
  TestApplication application;
  Actor           basicActor = Actor::New();
  Layer           layer      = Layer::New();

  Property::IndexContainer indices;
  layer.GetPropertyIndices(indices);
  DALI_TEST_CHECK(indices.Size() > basicActor.GetPropertyCount());
  DALI_TEST_EQUALS(indices.Size(), layer.GetPropertyCount(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayerTouchConsumed(void)
{
  TestApplication application;
  Layer           layer = Layer::New();

  DALI_TEST_EQUALS(layer.GetProperty<bool>(Layer::Property::CONSUMES_TOUCH), false, TEST_LOCATION);
  layer.SetProperty(Layer::Property::CONSUMES_TOUCH, true);
  DALI_TEST_EQUALS(layer.GetProperty<bool>(Layer::Property::CONSUMES_TOUCH), true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayerHoverConsumed(void)
{
  TestApplication application;
  Layer           layer = Layer::New();

  DALI_TEST_EQUALS(layer.GetProperty<bool>(Layer::Property::CONSUMES_HOVER), false, TEST_LOCATION);
  layer.SetProperty(Layer::Property::CONSUMES_HOVER, true);
  DALI_TEST_EQUALS(layer.GetProperty<bool>(Layer::Property::CONSUMES_HOVER), true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayerClippingGLCalls(void)
{
  TestApplication                         application;
  const TestGlAbstraction::ScissorParams& glScissorParams(application.GetGlAbstraction().GetScissorParams());
  Integration::Scene                      scene(application.GetScene());

  ClippingBox testBox(5, 6, 77, 83);
  Layer       layer = application.GetScene().GetRootLayer();
  layer.SetProperty(Layer::Property::CLIPPING_ENABLE, true);
  layer.SetProperty(Layer::Property::CLIPPING_BOX, testBox);

  // Add at least one renderable actor so the GL calls are actually made
  Texture img   = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 1, 1);
  Actor   actor = CreateRenderableActor(img);
  scene.Add(actor);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(testBox.x, glScissorParams.x, TEST_LOCATION);
  DALI_TEST_EQUALS(testBox.y, (int)(scene.GetSize().height - glScissorParams.y - testBox.height), TEST_LOCATION); // GL Coordinates are from bottom left
  DALI_TEST_EQUALS(testBox.width, glScissorParams.width, TEST_LOCATION);
  DALI_TEST_EQUALS(testBox.height, glScissorParams.height, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayerBehaviour(void)
{
  TestApplication application;
  Layer           layer = Layer::New();

  DALI_TEST_EQUALS(layer.GetProperty<Layer::Behavior>(Layer::Property::BEHAVIOR), Dali::Layer::LAYER_UI, TEST_LOCATION);
  layer.SetProperty(Layer::Property::BEHAVIOR, Dali::Layer::LAYER_3D);
  DALI_TEST_EQUALS(layer.GetProperty<Layer::Behavior>(Layer::Property::BEHAVIOR), Dali::Layer::LAYER_3D, TEST_LOCATION);
  END_TEST;
}

Actor CreateActor(bool withAlpha)
{
  Texture texture = Texture::New(TextureType::TEXTURE_2D, withAlpha ? Pixel::Format::RGBA8888 : Pixel::Format::RGB888, 1u, 1u);

  Actor actor = CreateRenderableActor(texture);
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);

  return actor;
}

int UtcDaliLayer3DSort(void)
{
  tet_infoline("Testing LAYER_3D sort coverage test");
  TestApplication    application;
  TestGlAbstraction& glAbstraction       = application.GetGlAbstraction();
  TraceCallStack&    enabledDisableTrace = glAbstraction.GetEnableDisableTrace();

  application.GetScene().GetRootLayer().SetProperty(Layer::Property::BEHAVIOR, Layer::LAYER_3D);

  // Create an actor.
  Actor actor = CreateActor(false);
  application.GetScene().Add(actor);

  // Create child actors.
  Actor child1 = CreateActor(true);
  actor.Add(child1);
  Actor child2 = CreateActor(false);
  child1.Add(child2);

  enabledDisableTrace.Reset();
  enabledDisableTrace.Enable(true);
  application.SendNotification();
  application.Render();
  enabledDisableTrace.Enable(false);

  DALI_TEST_CHECK(enabledDisableTrace.FindMethodAndParams("Enable", "2929")); // 2929 is GL_DEPTH_TEST

  END_TEST;
}

int UtcDaliLayerLowerBelowNegative(void)
{
  TestApplication application;
  Dali::Layer     instance;
  try
  {
    Dali::Layer arg1;
    instance.LowerBelow(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliLayerRaiseAboveNegative(void)
{
  TestApplication application;
  Dali::Layer     instance;
  try
  {
    Dali::Layer arg1;
    instance.RaiseAbove(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliLayerRaiseToTopNegative(void)
{
  TestApplication application;
  Dali::Layer     instance;
  try
  {
    instance.RaiseToTop();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliLayerLowerToBottomNegative(void)
{
  TestApplication application;
  Dali::Layer     instance;
  try
  {
    instance.LowerToBottom();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliLayerSetSortFunctionNegative(void)
{
  TestApplication application;
  Dali::Layer     instance;
  try
  {
    Layer::SortFunctionType function = nullptr;
    instance.SetSortFunction(function);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliLayerLowerNegative(void)
{
  TestApplication application;
  Dali::Layer     instance;
  try
  {
    instance.Lower();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliLayerRaiseNegative(void)
{
  TestApplication application;
  Dali::Layer     instance;
  try
  {
    instance.Raise();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliLayerMoveAboveNegative(void)
{
  TestApplication application;
  Dali::Layer     instance;
  try
  {
    Dali::Layer arg1;
    instance.MoveAbove(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliLayerMoveBelowNegative(void)
{
  TestApplication application;
  Dali::Layer     instance;
  try
  {
    Dali::Layer arg1;
    instance.MoveBelow(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}
