#ifndef __DALI_INTERNAL_ANIMATOR_CONNECTOR_BASE_H__
#define __DALI_INTERNAL_ANIMATOR_CONNECTOR_BASE_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// EXTERNAL INCLUDES
#include <boost/function.hpp>

// INTERNAL INCLUDES
#include <dali/internal/common/owner-pointer.h>
#include <dali/internal/common/owner-container.h>
#include <dali/public-api/animation/alpha-functions.h>
#include <dali/public-api/animation/time-period.h>
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

namespace Internal
{

class Animation;
class AnimatorConnectorBase;

typedef OwnerPointer<AnimatorConnectorBase> AnimatorConnectorPtr;

typedef OwnerContainer< AnimatorConnectorBase* > AnimatorConnectorContainer;

typedef AnimatorConnectorContainer::Iterator AnimatorConnectorIter;
typedef AnimatorConnectorContainer::ConstIterator AnimatorConnectorConstIter;

/**
 * An abstract base class for animator connectors.
 */
class AnimatorConnectorBase
{
public:

  /**
   * Constructor.
   */
  AnimatorConnectorBase(AlphaFunction alpha, const TimePeriod& period)
  : mParent(NULL),
    mAlphaFunction(alpha),
    mTimePeriod(period)
  {
  }

  /**
   * Virtual destructor.
   */
  virtual ~AnimatorConnectorBase()
  {
  }

  /**
   * Set the parent of the AnimatorConnector.
   * @pre The connector does not already have a parent.
   * @param [in] parent The parent object.
   */
  virtual void SetParent(Animation& parent) = 0;

  /**
   * Retrieve the parent of the AnimatorConnector.
   * @return The parent object, or NULL.
   */
  Animation* GetParent()
  {
    return mParent;
  }

protected:

  Animation* mParent; ///< The parent owns the connector.

  AlphaFunction mAlphaFunction;
  TimePeriod mTimePeriod;
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_ANIMATOR_CONNECTOR_BASE_H__
