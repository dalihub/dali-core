#ifndef DALI_INTERNAL_UPDATE_PROXY_PROPERTY_MODIFIER_H
#define DALI_INTERNAL_UPDATE_PROXY_PROPERTY_MODIFIER_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/internal/common/owner-pointer.h>
#include <dali/internal/update/common/property-resetter.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/update/manager/update-proxy-impl.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{
class Node;
class PropertyBase;
}

/**
 * Keeps track of any non-transform manager properties that are modified by the UpdateProxy.
 *
 * This is required so the Update Manager can then reset the value to the base at the start of every frame.
 */
class UpdateProxy::PropertyModifier final
{
public:

  using Resetter = SceneGraph::Resetter< PropertyModifier >;

  /**
   * Observer to determine when the animator is no longer present
   */
  class LifecycleObserver
  {
  public:

    /**
     * Called shortly before the animator is destroyed.
     */
    virtual void ObjectDestroyed() = 0;

  protected:

    /**
     * Virtual destructor, no deletion through this interface
     */
    virtual ~LifecycleObserver() = default;
  };

  /**
   * Default Constructor.
   * @param[in]  updateManager  A reference to the update-manager
   */
  PropertyModifier( SceneGraph::UpdateManager& updateManager )
  : mProperties(),
    mLifecycleObservers(),
    mUpdateManager( &updateManager )
  {
  }

  /**
   * Non-virtual destructor.
   */
  ~PropertyModifier()
  {
    for( auto& observer : mLifecycleObservers )
    {
      observer->ObjectDestroyed();
    }
  }

  // Movable but not copyable

  PropertyModifier( const PropertyModifier& )            = delete;  ///< Deleted copy constructor.
  PropertyModifier& operator=( const PropertyModifier& ) = delete;  ///< Deleted assignment operator.

  /**
   * Move constructor.
   */
  PropertyModifier( PropertyModifier&& other )
  : mProperties( std::move( other.mProperties ) ),
    mLifecycleObservers( std::move( other.mLifecycleObservers ) ),
    mUpdateManager( std::move( other.mUpdateManager ) )
  {
    // Clear other so that it does not remove any resetters unintentionally
    other.mLifecycleObservers.clear();
  }

  /**
   * Move assignment operator.
   */
  PropertyModifier& operator=( PropertyModifier&& other )
  {
    if( this != &other )
    {
      mProperties = std::move( other.mProperties );
      mLifecycleObservers = std::move( other.mLifecycleObservers );
      mUpdateManager = std::move( other.mUpdateManager );

      // Clear other so that it does not remove any resetters unintentionally
      other.mLifecycleObservers.clear();
    }
    return *this;
  }

  /**
   * Allows Resetter to track the life-cycle of this object.
   * @param[in]  observer  The observer to add.
   */
  void AddLifecycleObserver( LifecycleObserver& observer )
  {
    mLifecycleObservers.push_back( &observer );
  }

  /**
   * The Resetter no longer needs to track the life-cycle of this object.
   * @param[in]  observer  The observer that to remove.
   */
  void RemoveLifecycleObserver( LifecycleObserver& observer )
  {
    std::remove( mLifecycleObservers.begin(), mLifecycleObservers.end(), &observer );
  }

  /**
   * Adds a resetter to the given node and property if it hasn't already been added previously.
   * @param[in]  node          The associated Node
   * @param[in]  propertyBase  The associated PropertyBase
   */
  void AddResetter( SceneGraph::Node& node, SceneGraph::PropertyBase& propertyBase )
  {
    // Check if we've already added a resetter for this node and property to the update-manager
    NodePropertyPair pair{ &node, &propertyBase };
    if( mUpdateManager &&
        ( mProperties.end() == std::find( mProperties.begin(), mProperties.end(), pair ) ) )
    {
      // We haven't, add the pair to our container to ensure we don't add it again
      // Then create a Resetter which will observe the life of this object
      // Finally, add the resetter to the Update-Manager
      // When this object is destroyed, the resetter will be informed and will automatically be removed

      mProperties.emplace_back( std::move( pair ) );
      OwnerPointer< SceneGraph::PropertyResetterBase > resetter( Resetter::New( node, propertyBase, *this ) );
      mUpdateManager->AddPropertyResetter( resetter );
    }
  }

public:

  /**
   * Structure to store the Node & property-base pair
   */
  struct NodePropertyPair
  {
    SceneGraph::Node* node;
    SceneGraph::PropertyBase* propertyBase;

    NodePropertyPair( const NodePropertyPair& )            = delete;  ///< Deleted copy constructor.
    NodePropertyPair( NodePropertyPair&& )                 = default; ///< Default move constructor.
    NodePropertyPair& operator=( const NodePropertyPair& ) = delete;  ///< Deleted assignment operator.
    NodePropertyPair& operator=( NodePropertyPair&& )      = default; ///< Default move assignment operator.

    /**
     * Comparison operator
     */
    bool operator==( const NodePropertyPair& other )
    {
      return ( other.node == node ) &&
             ( other.propertyBase == propertyBase );
    }
  };

  std::vector< NodePropertyPair > mProperties;
  std::vector< LifecycleObserver* > mLifecycleObservers;
  SceneGraph::UpdateManager* mUpdateManager;
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_UPDATE_PROXY_PROPERTY_MODIFIER_H
