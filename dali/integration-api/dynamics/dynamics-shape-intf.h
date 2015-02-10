#ifndef __DALI_INTEGRATION_DYNAMICS_SHAPE_INTF_H__
#define __DALI_INTEGRATION_DYNAMICS_SHAPE_INTF_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

struct Vector3;

namespace Integration
{

/**
 *
 */
class DALI_IMPORT_API DynamicsShape
{
public:
  /**
   * Destructor
   */
  virtual ~DynamicsShape() {}

  /**
   * Initialize the shape
   */
  virtual void Initialize( int type, const Vector3& dimensions ) = 0;


}; // class DynamicsShape

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_DYNAMICS_SHAPE_INTF_H__
