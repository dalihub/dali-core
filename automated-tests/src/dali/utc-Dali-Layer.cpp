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

#include <iostream>

#include <stdlib.h>

#include <dali/public-api/dali-core.h>

#include <dali-test-suite-utils.h>

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
  Layer layer = Layer::New();

  DALI_TEST_CHECK(layer);
  END_TEST;
}

int UtcDaliLayerDownCast(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Layer::DownCast()");

  Layer actor1 = Layer::New();
  Actor anActor = Actor::New();
  anActor.Add(actor1);

  Actor child = anActor.GetChildAt(0);
  Layer layer = DownCast< Layer >(child);

  DALI_TEST_CHECK(layer);
  END_TEST;
}

int UtcDaliLayerDownCast2(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Layer::DownCast()");

  Actor actor1 = Actor::New();
  Actor anActor = Actor::New();
  anActor.Add(actor1);

  Actor child = anActor.GetChildAt(0);
  Layer layer = DownCast< Layer >(child);
  DALI_TEST_CHECK(!layer);

  Actor unInitialzedActor;
  layer = Layer::DownCast( unInitialzedActor );
  DALI_TEST_CHECK(!layer);
  END_TEST;
}


int UtcDaliLayerGetDepth(void)
{
  tet_infoline("Testing Dali::Layer::GetDepth()");
  TestApplication application;
  Layer layer1 = Layer::New();
  Layer layer2 = Layer::New();

  // layers are not on stage
  DALI_TEST_EQUALS(layer1.GetDepth(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(layer2.GetDepth(), 0u, TEST_LOCATION);

  // root depth is 0
  Layer root = Stage::GetCurrent().GetLayer( 0 );
  DALI_TEST_EQUALS(root.GetDepth(), 0u, TEST_LOCATION);

  Stage::GetCurrent().Add(layer1);
  Stage::GetCurrent().Add(layer2);

  DALI_TEST_EQUALS(  root.GetDepth(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(layer1.GetDepth(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(layer2.GetDepth(), 2u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayerRaise(void)
{
  tet_infoline("Testing Dali::Layer::Raise()");
  TestApplication application;
  Layer layer1 = Layer::New();
  Layer layer2 = Layer::New();

  Stage::GetCurrent().Add(layer1);
  Stage::GetCurrent().Add(layer2);
  DALI_TEST_EQUALS(layer1.GetDepth(), 1u, TEST_LOCATION);

  layer1.Raise();
  DALI_TEST_EQUALS(layer1.GetDepth(), 2u, TEST_LOCATION);

  // get root
  Layer root = Stage::GetCurrent().GetLayer( 0 );
  DALI_TEST_EQUALS(  root.GetDepth(), 0u, TEST_LOCATION);
  root.Raise();
  DALI_TEST_EQUALS(  root.GetDepth(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(layer1.GetDepth(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(layer2.GetDepth(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayerLower(void)
{
  tet_infoline("Testing Dali::Layer::Lower()");
  TestApplication application;
  Layer layer1 = Layer::New();
  Layer layer2 = Layer::New();

  Stage::GetCurrent().Add(layer1);
  Stage::GetCurrent().Add(layer2);
  DALI_TEST_EQUALS(layer2.GetDepth(), 2u, TEST_LOCATION);

  layer2.Lower();
  DALI_TEST_EQUALS(layer2.GetDepth(), 1u, TEST_LOCATION);

  // get root
  Layer root = Stage::GetCurrent().GetLayer( 0 );
  root.Lower();
  DALI_TEST_EQUALS(  root.GetDepth(), 0u, TEST_LOCATION);
  layer2.Lower();
  DALI_TEST_EQUALS(  root.GetDepth(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(layer2.GetDepth(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayerRaiseToTop(void)
{
  tet_infoline("Testing Dali::Layer::RaiseToTop()");
  TestApplication application;
  Layer layer1 = Layer::New();
  Layer layer2 = Layer::New();
  Layer layer3 = Layer::New();

  Stage::GetCurrent().Add(layer1);
  Stage::GetCurrent().Add(layer2);
  Stage::GetCurrent().Add(layer3);
  Layer root = Stage::GetCurrent().GetLayer( 0 );

  DALI_TEST_EQUALS(  root.GetDepth(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(layer1.GetDepth(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(layer2.GetDepth(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(layer3.GetDepth(), 3u, TEST_LOCATION);

  layer1.RaiseToTop();
  DALI_TEST_EQUALS(layer1.GetDepth(), 3u, TEST_LOCATION);

  root.RaiseToTop();
  DALI_TEST_EQUALS(  root.GetDepth(), 3u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayerLowerToBottom(void)
{
  tet_infoline("Testing Dali::Layer::LowerToBottom()");
  TestApplication application;
  Layer layer1 = Layer::New();
  Layer layer2 = Layer::New();
  Layer layer3 = Layer::New();

  Stage::GetCurrent().Add(layer1);
  Stage::GetCurrent().Add(layer2);
  Stage::GetCurrent().Add(layer3);

  DALI_TEST_EQUALS(layer1.GetDepth(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(layer2.GetDepth(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(layer3.GetDepth(), 3u, TEST_LOCATION);

  layer3.LowerToBottom();
  DALI_TEST_EQUALS(layer3.GetDepth(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLayerSetClipping(void)
{
  tet_infoline("Testing Dali::Layer::SetClipping()");
  TestApplication application;

  Layer layer = Layer::New();
  DALI_TEST_CHECK(!layer.IsClipping());

  layer.SetClipping(true);
  DALI_TEST_CHECK(layer.IsClipping());
  END_TEST;
}

int UtcDaliLayerIsClipping(void)
{
  tet_infoline("Testing Dali::Layer::IsClipping()");
  TestApplication application;

  Layer layer = Layer::New();
  DALI_TEST_CHECK(!layer.IsClipping());
  END_TEST;
}

int UtcDaliLayerSetClippingBox(void)
{
  tet_infoline("Testing Dali::Layer::SetClippingBox()");
  TestApplication application;

  ClippingBox testBox(5,6, 77,83);

  Layer layer = Layer::New();
  DALI_TEST_CHECK(layer.GetClippingBox() != testBox);

  layer.SetClippingBox(5,6, 77,83);
  DALI_TEST_CHECK(layer.GetClippingBox() == testBox);
  END_TEST;
}

int UtcDaliLayerGetClippingBox(void)
{
  tet_infoline("Testing Dali::Layer::GetClippingBox()");
  TestApplication application;

  Layer layer = Layer::New();
  DALI_TEST_CHECK(layer.GetClippingBox() == ClippingBox(0,0,0,0));
  END_TEST;
}

static int gTestSortFunctionCalled;

static float TestSortFunction(const Vector3& /*position*/, float /*sortModifier*/)
{
  ++gTestSortFunctionCalled;
  return 0.0f;
}

int UtcDaliLayerSetSortFunction(void)
{
  tet_infoline("Testing Dali::Layer::SetSortFunction()");
  TestApplication application;
  BufferImage img = BufferImage::New( 1,1 );
  // create two transparent actors so there is something to sort
  ImageActor actor = ImageActor::New( img );
  ImageActor actor2 = ImageActor::New( img );
  actor.SetSize(1,1);
  actor.SetColor( Vector4(1, 1, 1, 0.5f ) ); // 50% transparent
  actor2.SetSize(1,1);
  actor2.SetColor( Vector4(1, 1, 1, 0.5f ) ); // 50% transparent

  // add to stage
  Stage::GetCurrent().Add( actor );
  Stage::GetCurrent().Add( actor2 );

  Layer root = Stage::GetCurrent().GetLayer( 0 );
  gTestSortFunctionCalled = 0;
  root.SetSortFunction(TestSortFunction);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( gTestSortFunctionCalled > 0 );
  END_TEST;
}


int UtcDaliLayerRaiseAbove(void)
{
  tet_infoline("Testing Dali::Layer::RaiseAbove()");
  TestApplication application;
  Layer layer = Layer::New();
  // try to raise above root layer
  Layer root = Stage::GetCurrent().GetLayer( 0 );
  layer.RaiseAbove( root );
  // layer depth is zero as its not on stage
  DALI_TEST_EQUALS( layer.GetDepth(), 0u, TEST_LOCATION );
  // add to stage
  Stage::GetCurrent().Add( layer );
  layer.RaiseAbove( root );
  DALI_TEST_EQUALS( layer.GetDepth(), 1u, TEST_LOCATION );
  root.RaiseAbove( layer );
  DALI_TEST_EQUALS( layer.GetDepth(), 0u, TEST_LOCATION );
  layer.RaiseAbove( layer );
  DALI_TEST_EQUALS( layer.GetDepth(), 0u, TEST_LOCATION );

  // make another layer on the stage
  Layer layer2 = Layer::New();
  Stage::GetCurrent().Add( layer2 );
  layer.RaiseAbove( layer2 );
  DALI_TEST_GREATER( layer.GetDepth(), layer2.GetDepth(), TEST_LOCATION );
  layer2.RaiseAbove( layer );
  DALI_TEST_GREATER( layer2.GetDepth(), layer.GetDepth(), TEST_LOCATION );
  root.RaiseAbove( layer2 );
  DALI_TEST_GREATER( root.GetDepth(), layer2.GetDepth(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliLayerRaiseBelow(void)
{
  tet_infoline("Testing Dali::Layer::RaiseBelow()");
  TestApplication application;
  Layer layer = Layer::New();
  // try to lower below root layer
  Layer root = Stage::GetCurrent().GetLayer( 0 );
  layer.LowerBelow( root );
  // layer depth is zero as its not on stage
  DALI_TEST_EQUALS( layer.GetDepth(), 0u, TEST_LOCATION );
  // add to stage
  Stage::GetCurrent().Add( layer );
  DALI_TEST_EQUALS( layer.GetDepth(), 1u, TEST_LOCATION );
  layer.LowerBelow( root );
  DALI_TEST_EQUALS( layer.GetDepth(), 0u, TEST_LOCATION );
  root.LowerBelow( layer );
  DALI_TEST_EQUALS( layer.GetDepth(), 1u, TEST_LOCATION );
  layer.LowerBelow( layer );
  DALI_TEST_EQUALS( layer.GetDepth(), 1u, TEST_LOCATION );

  // make another layer on the stage
  Layer layer2 = Layer::New();
  Stage::GetCurrent().Add( layer2 );
  layer.LowerBelow( layer2 );
  DALI_TEST_GREATER( layer2.GetDepth(), layer.GetDepth(), TEST_LOCATION );
  layer2.LowerBelow( layer );
  DALI_TEST_GREATER( layer.GetDepth(), layer2.GetDepth(), TEST_LOCATION );
  root.LowerBelow( layer2 );
  DALI_TEST_GREATER( layer2.GetDepth(), root.GetDepth(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliLayerMoveAbove(void)
{
  tet_infoline("Testing Dali::Layer::MoveAbove()");
  TestApplication application;
  Layer layer = Layer::New();
  // try to raise above root layer
  Layer root = Stage::GetCurrent().GetLayer( 0 );
  layer.MoveAbove( root );
  // layer depth is zero as its not on stage
  DALI_TEST_EQUALS( layer.GetDepth(), 0u, TEST_LOCATION );
  root.MoveAbove( layer );
  // root depth is zero as layer is not on stage
  DALI_TEST_EQUALS( layer.GetDepth(), 0u, TEST_LOCATION );
  // add to stage
  Stage::GetCurrent().Add( layer );
  layer.MoveAbove( root );
  DALI_TEST_EQUALS( layer.GetDepth(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( root.GetDepth(), 0u, TEST_LOCATION );
  root.MoveAbove( layer );
  DALI_TEST_EQUALS( layer.GetDepth(), 0u, TEST_LOCATION );
  DALI_TEST_EQUALS( root.GetDepth(), 1u, TEST_LOCATION );

  // make another layer on the stage
  Layer layer2 = Layer::New();
  Stage::GetCurrent().Add( layer2 );
  layer.MoveAbove( layer2 );
  DALI_TEST_EQUALS( layer.GetDepth(), layer2.GetDepth() + 1u, TEST_LOCATION );
  layer2.MoveAbove( root );
  DALI_TEST_EQUALS( layer2.GetDepth(), root.GetDepth() + 1u, TEST_LOCATION );
  root.MoveAbove( layer );
  DALI_TEST_EQUALS( root.GetDepth(), layer.GetDepth() + 1u, TEST_LOCATION );

  Layer layer3 = Layer::New();
  Stage::GetCurrent().Add( layer3 );
  DALI_TEST_EQUALS( layer3.GetDepth(), 3u, TEST_LOCATION );
  root.MoveAbove( layer3 );
  DALI_TEST_EQUALS( root.GetDepth(), 3u, TEST_LOCATION );
  DALI_TEST_EQUALS( layer3.GetDepth(), 2u, TEST_LOCATION );
  DALI_TEST_EQUALS( Stage::GetCurrent().GetLayer( 0 ).GetDepth(), 0u, TEST_LOCATION );
  layer3.MoveAbove( Stage::GetCurrent().GetLayer( 0 ) );
  DALI_TEST_EQUALS( layer3.GetDepth(), 1u, TEST_LOCATION );
  END_TEST;
}

int UtcDaliLayerMoveBelow(void)
{
  tet_infoline("Testing Dali::Layer::MoveBelow()");
  TestApplication application;
  Layer layer = Layer::New();
  // try to raise above root layer
  Layer root = Stage::GetCurrent().GetLayer( 0 );
  layer.MoveBelow( root );
  // layer depth is zero as its not on stage
  DALI_TEST_EQUALS( layer.GetDepth(), 0u, TEST_LOCATION );
  root.MoveBelow( layer );
  // root depth is zero as layer is not on stage
  DALI_TEST_EQUALS( layer.GetDepth(), 0u, TEST_LOCATION );
  // add to stage
  Stage::GetCurrent().Add( layer );
  layer.MoveBelow( root );
  DALI_TEST_EQUALS( layer.GetDepth(), 0u, TEST_LOCATION );
  DALI_TEST_EQUALS( root.GetDepth(), 1u, TEST_LOCATION );
  root.MoveBelow( layer );
  DALI_TEST_EQUALS( layer.GetDepth(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( root.GetDepth(), 0u, TEST_LOCATION );

  // make another layer on the stage
  Layer layer2 = Layer::New();
  Stage::GetCurrent().Add( layer2 );
  layer.MoveBelow( layer2 );
  DALI_TEST_EQUALS( layer.GetDepth(), layer2.GetDepth() - 1u, TEST_LOCATION );
  layer2.MoveBelow( root );
  DALI_TEST_EQUALS( layer2.GetDepth(), root.GetDepth() - 1u, TEST_LOCATION );
  root.MoveBelow( layer );
  DALI_TEST_EQUALS( root.GetDepth(), layer.GetDepth() - 1u, TEST_LOCATION );

  Layer layer3 = Layer::New();
  Stage::GetCurrent().Add( layer3 );
  DALI_TEST_EQUALS( layer3.GetDepth(), 3u, TEST_LOCATION );
  root.MoveBelow( layer3 );
  DALI_TEST_EQUALS( root.GetDepth(), 2u, TEST_LOCATION );
  DALI_TEST_EQUALS( layer3.GetDepth(), 3u, TEST_LOCATION );
  END_TEST;
}

int UtcDaliLayerDefaultProperties(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Layer DefaultProperties");

  Layer actor = Layer::New();

  std::vector<Property::Index> indices ;
  indices.push_back(Layer::Property::ClippingEnable);
  indices.push_back(Layer::Property::ClippingBox);

  DALI_TEST_CHECK(actor.GetPropertyCount() == ( Actor::New().GetPropertyCount() + indices.size() ) );

  for(std::vector<Property::Index>::iterator iter = indices.begin(); iter != indices.end(); ++iter)
  {
    DALI_TEST_CHECK( *iter == actor.GetPropertyIndex(actor.GetPropertyName(*iter)) );
    DALI_TEST_CHECK( actor.IsPropertyWritable(*iter) );
    DALI_TEST_CHECK( !actor.IsPropertyAnimatable(*iter) );
    DALI_TEST_CHECK( actor.GetPropertyType(*iter) == actor.GetPropertyType(*iter) );  // just checking call succeeds
  }

  // set/get one of them
  actor.SetClippingBox(0,0,0,0);

  ClippingBox testBox(10,20,30,40);
  DALI_TEST_CHECK(actor.GetClippingBox() != testBox);

  actor.SetProperty(Layer::Property::ClippingBox, Property::Value(Rect<int>(testBox)));

  DALI_TEST_CHECK(Property::RECTANGLE == actor.GetPropertyType(Layer::Property::ClippingBox)) ;

  Property::Value v = actor.GetProperty(Layer::Property::ClippingBox);

  DALI_TEST_CHECK(v.Get<Rect<int> >() == testBox);

  // set the same boundaries, but through a clipping box object
  actor.SetClippingBox( testBox );

  DALI_TEST_CHECK( actor.GetClippingBox() == testBox );

  END_TEST;
}

int UtcDaliLayerSetDepthTestDisabled(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Layer::SetDepthTestDisabled() ");

  Layer actor = Layer::New();

  DALI_TEST_CHECK( !actor.IsDepthTestDisabled() );

  actor.SetDepthTestDisabled( true );
  DALI_TEST_CHECK( actor.IsDepthTestDisabled() );
  END_TEST;
}

int UtcDaliLayerCreateDestroy(void)
{
  tet_infoline("Testing Dali::Layer::CreateDestroy() ");
  Layer* layer = new Layer;
  DALI_TEST_CHECK( layer );
  delete layer;
  END_TEST;
}

int UtcDaliLayerPropertyIndices(void)
{
  TestApplication application;
  Actor basicActor = Actor::New();
  Layer layer = Layer::New();

  Property::IndexContainer indices;
  layer.GetPropertyIndices( indices );
  DALI_TEST_CHECK( indices.size() > basicActor.GetPropertyCount() );
  DALI_TEST_EQUALS( indices.size(), layer.GetPropertyCount(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliLayerTouchConsumed(void)
{
  TestApplication application;
  Layer layer = Layer::New();

  DALI_TEST_EQUALS( layer.IsTouchConsumed(), false, TEST_LOCATION );
  layer.SetTouchConsumed( true );
  DALI_TEST_EQUALS( layer.IsTouchConsumed(), true, TEST_LOCATION );
  END_TEST;
}

int UtcDaliLayerHoverConsumed(void)
{
  TestApplication application;
  Layer layer = Layer::New();

  DALI_TEST_EQUALS( layer.IsHoverConsumed(), false, TEST_LOCATION );
  layer.SetHoverConsumed( true );
  DALI_TEST_EQUALS( layer.IsHoverConsumed(), true, TEST_LOCATION );
  END_TEST;
}

int UtcDaliLayerClippingGLCalls(void)
{
  TestApplication application;
  const TestGlAbstraction::ScissorParams& glScissorParams( application.GetGlAbstraction().GetScissorParams() );
  Stage stage( Stage::GetCurrent() );

  ClippingBox testBox( 5, 6, 77, 83 );
  Layer layer = Stage::GetCurrent().GetRootLayer();
  layer.SetClipping( true );
  layer.SetClippingBox( testBox );

  // Add at least one renderable actor so the GL calls are actually made
  Actor actor = ImageActor::New();
  stage.Add( actor );

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( testBox.x, glScissorParams.x, TEST_LOCATION );
  DALI_TEST_EQUALS( testBox.y, stage.GetSize().height - glScissorParams.y - testBox.height, TEST_LOCATION ); // GL Coordinates are from bottom left
  DALI_TEST_EQUALS( testBox.width, glScissorParams.width, TEST_LOCATION );
  DALI_TEST_EQUALS( testBox.height, glScissorParams.height, TEST_LOCATION );
  END_TEST;
}
