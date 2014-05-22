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

#include <dali/dali.h>
#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

int UtcDaliAnimatableMeshDownCast02(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::AnimatableMesh::DownCast()");

  MeshData meshData;
  CreateMeshData(meshData);
  Mesh mesh = Mesh::New(meshData);
  BaseHandle* bh = &mesh;

  AnimatableMesh mesh2 = AnimatableMesh::DownCast(*bh);
  DALI_TEST_CHECK( ! mesh2 );
  END_TEST;
}
