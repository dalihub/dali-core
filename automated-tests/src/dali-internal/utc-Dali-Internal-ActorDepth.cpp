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
 */
#include <iostream>

#include <stdlib.h>
#include <dali/public-api/dali-core.h>

#include <dali-test-suite-utils.h>
#include <dali/devel-api/actors/actor-devel.h>

// Internal headers are allowed here
#define protected public
#include <dali/internal/event/actors/actor-impl.h>

using namespace Dali;

void utc_dali_internal_actor_depth_startup()
{
  test_return_value = TET_UNDEF;
}

void utc_dali_internal_actor_depth_cleanup()
{
  test_return_value = TET_PASS;
}

Actor CreateActor( Actor parent, int siblingOrder, const char* name )
{
  Actor actor = Actor::New();
  actor.SetProperty( DevelActor::Property::SIBLING_ORDER, siblingOrder);
  actor.SetName( name );
  parent.Add(actor);
  return actor;
}

void PrintActor(Dali::Actor a, int depth)
{
  int siblingOrder;
  Dali::Property::Value v = a.GetProperty( Dali::DevelActor::Property::SIBLING_ORDER );
  v.Get(siblingOrder);

  Dali::Internal::Actor& actorImpl = GetImplementation(a);
  for( int i=0; i<depth; ++i)
    std::cout << "  ";
  std::cout << "Actor: " << a.GetName() << "(" << a.GetId() << ") siblingOrder: " <<
    siblingOrder << " depthOrder: " << actorImpl.GetSortingDepth() << std::endl;
}

void PrintActorTree( Dali::Actor a, int depth )
{
  PrintActor( a, depth );
  for( unsigned int i=0; i<a.GetChildCount(); ++i )
  {
    PrintActorTree( a.GetChildAt(i), depth+1 );
  }
}

void PrintNode( Dali::Internal::ActorDepthTreeNode& node, int depth )
{
  for( int i=0; i<depth; ++i)
    std::cout << "  ";
  std::cout << "Node: " << &node << "  siblingOrder:" << node.mSiblingOrder << " Actors:";
  for( std::vector<Internal::Actor*>::iterator iter = node.mActors.begin() ;
       iter != node.mActors.end(); ++iter )
  {
    std::cout << (*iter)->GetName() << ", ";
  }
  std::cout << std::endl;

  if( node.mFirstChildNode )
    PrintNode( *node.mFirstChildNode, depth+1);

  if( node.mNextSiblingNode )
  {
    PrintNode( *node.mNextSiblingNode, depth );
  }
}

void CheckNodeForActor( Dali::Internal::ActorDepthTreeNode*node, Actor actor, const char* loc )
{
  bool found = false;
  Dali::Internal::Actor& actorImpl = Dali::GetImplementation(actor);

  for( std::vector<Internal::Actor*>::iterator iter = node->mActors.begin(); iter != node->mActors.end(); ++iter )
  {
    if( *iter == &actorImpl )
    {
      found = true;
      break;
    }
  }
  DALI_TEST_EQUALS( found, true, loc );
}

unsigned int GetActorCount( Dali::Internal::ActorDepthTreeNode*node )
{
  unsigned int size = node->mActors.size();

  for( Dali::Internal::ActorDepthTreeNode* childNode = node->mFirstChildNode;
       childNode != NULL;
       childNode = childNode->mNextSiblingNode )
  {
    size += GetActorCount( childNode );
  }

  return size;
}

int UtcDaliActorDepthTreeTest01(void)
{
  TestApplication application;
  tet_infoline("Testing Actor tree depth");

  Stage stage = Stage::GetCurrent();

  Actor Root = CreateActor(stage.GetRootLayer(), 0, "ROOT" );
  Actor A = CreateActor( Root, 0, "A");
  Actor B = CreateActor( Root, 2, "B");
  Actor C = CreateActor( Root, 0, "C");
  Actor D = CreateActor( Root, 1, "D");

  Actor E = CreateActor(A, 0, "E");
  Actor F = CreateActor(A, 2, "F");
  Actor G = CreateActor(A, 1, "G");

  Actor H = CreateActor(B, 2, "H");
  Actor I = CreateActor(B, 1, "I");
  Actor J = CreateActor(B, 0, "J");

  Actor K = CreateActor(C, 1, "K");
  Actor L = CreateActor(C, 2, "L");
  Actor M = CreateActor(C, 0, "M");

  Actor N = CreateActor(D, 2, "N");
  Actor O = CreateActor(D, 2, "O");
  Actor P = CreateActor(D, 1, "P");

  PrintActorTree( Root, 0 );

  Internal::Actor& rootLayerImpl = GetImplementation(Root);

  Internal::DepthNodeMemoryPool nodeMemoryPool;
  Internal::ActorDepthTreeNode* rootNode = new (nodeMemoryPool.AllocateRaw()) Internal::ActorDepthTreeNode( &rootLayerImpl, 0 );
  rootLayerImpl.BuildDepthTree( nodeMemoryPool, rootNode ) ;

  int depth=0;
  PrintNode( *rootNode, depth );

  // Check that first child node contains actors A and C
  // check that first grand child node contains actors E, M
  // check that it's sibling node contains actors G, K
  // check that it's sibling node contains actors F, L
  // Check that tree only contains 16 actors.
  CheckNodeForActor( rootNode->mFirstChildNode, A, TEST_LOCATION );
  CheckNodeForActor( rootNode->mFirstChildNode, C, TEST_LOCATION );
  CheckNodeForActor( rootNode->mFirstChildNode->mFirstChildNode, E, TEST_LOCATION );
  CheckNodeForActor( rootNode->mFirstChildNode->mFirstChildNode, M, TEST_LOCATION );
  CheckNodeForActor( rootNode->mFirstChildNode->mFirstChildNode->mNextSiblingNode, G, TEST_LOCATION );
  CheckNodeForActor( rootNode->mFirstChildNode->mFirstChildNode->mNextSiblingNode, K, TEST_LOCATION );
  CheckNodeForActor( rootNode->mFirstChildNode->mFirstChildNode->mNextSiblingNode->mNextSiblingNode, F, TEST_LOCATION );
  CheckNodeForActor( rootNode->mFirstChildNode->mFirstChildNode->mNextSiblingNode->mNextSiblingNode, L, TEST_LOCATION );
  CheckNodeForActor( rootNode->mFirstChildNode->mNextSiblingNode->mNextSiblingNode, B, TEST_LOCATION );

  unsigned int actorCount = GetActorCount( rootNode );
  DALI_TEST_EQUALS( actorCount, 17, TEST_LOCATION );

  END_TEST;
}



int UtcDaliActorDepthTreeTest02(void)
{
  TestApplication application;
  tet_infoline("Testing Actor tree depth");

  Stage stage = Stage::GetCurrent();

  Actor Root = CreateActor(stage.GetRootLayer(), 0, "ROOT" );
  Actor A = CreateActor( Root, 0, "A");
  Actor B = CreateActor( Root, 0, "B");
  Actor C = CreateActor( Root, 0, "C");
  Actor D = CreateActor( Root, 0, "D");

  Actor E = CreateActor(A, 0, "E");
  Actor F = CreateActor(A, 0, "F");
  Actor G = CreateActor(A, 0, "G");

  Actor H = CreateActor(B, 0, "H");
  Actor I = CreateActor(B, 0, "I");
  Actor J = CreateActor(B, 0, "J");

  Actor K = CreateActor(C, 0, "K");
  Actor L = CreateActor(C, 0, "L");
  Actor M = CreateActor(C, 0, "M");

  Actor N = CreateActor(D, 0, "N");
  Actor O = CreateActor(D, 0, "O");
  Actor P = CreateActor(D, 0, "P");

  PrintActorTree( Root, 0 );

  Internal::Actor& rootLayerImpl = GetImplementation(Root);

  Internal::DepthNodeMemoryPool nodeMemoryPool;
  Internal::ActorDepthTreeNode* rootNode = new (nodeMemoryPool.AllocateRaw()) Internal::ActorDepthTreeNode( &rootLayerImpl, 0 );
  rootLayerImpl.BuildDepthTree( nodeMemoryPool, rootNode ) ;

  int depth=0;
  PrintNode( *rootNode, depth );

  CheckNodeForActor( rootNode->mFirstChildNode, A, TEST_LOCATION );
  CheckNodeForActor( rootNode->mFirstChildNode, C, TEST_LOCATION );
  CheckNodeForActor( rootNode->mFirstChildNode->mFirstChildNode, E, TEST_LOCATION );
  CheckNodeForActor( rootNode->mFirstChildNode->mFirstChildNode, M, TEST_LOCATION );
  CheckNodeForActor( rootNode->mFirstChildNode->mFirstChildNode, G, TEST_LOCATION );
  CheckNodeForActor( rootNode->mFirstChildNode->mFirstChildNode, K, TEST_LOCATION );
  CheckNodeForActor( rootNode->mFirstChildNode->mFirstChildNode, F, TEST_LOCATION );
  CheckNodeForActor( rootNode->mFirstChildNode->mFirstChildNode, L, TEST_LOCATION );
  CheckNodeForActor( rootNode->mFirstChildNode, B, TEST_LOCATION );

  unsigned int actorCount = GetActorCount( rootNode );
  DALI_TEST_EQUALS( actorCount, 17, TEST_LOCATION );

  END_TEST;
}
