#ifndef __DALI_INTEGRATION_PLATFORM_ABSTRACTION_H__
#define __DALI_INTEGRATION_PLATFORM_ABSTRACTION_H__

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
#include <dali/integration-api/bitmap.h>
#include <dali/integration-api/resource-cache.h>

namespace Dali
{

namespace Integration
{

class Bitmap;
class DynamicsFactory;


/**
 * PlatformAbstraction is an abstract interface, used by Dali to access platform specific services.
 * A concrete implementation must be created for each platform, and provided when creating the
 * Dali::Integration::Core object.
 */
class PlatformAbstraction
{
public:

  virtual ~PlatformAbstraction() {}

  // Dali Lifecycle

  /**
   * Get the monotonic time since an unspecified reference point.
   * This is used by Dali to calculate time intervals during animations; the interval is
   * recalculated when Dali is resumed with Dali::Integration::Core::Resume().
   * Multi-threading note: this method may be called from either the main or rendering thread.
   * @param[out] seconds The time in seconds since the reference point.
   * @param[out] microSeconds The remainder in microseconds.
   */
  virtual void GetTimeMicroseconds(unsigned int& seconds, unsigned int& microSeconds) = 0;

  /**
   * Tell the platform abstraction that Dali is ready to pause, such as when the
   * application enters a background state.
   * Allows background threads to pause their work until Resume() is called.
   * This is a good time to release recreatable data such as memory caches
   * to cooperate with other apps and reduce the chance of this one being
   * force-killed in a low memory situation.
   */
  virtual void Suspend() {} ///!ToDo: Make pure virtual once dali-adaptor patch is in place = 0;

  /**
   * Tell the platform abstraction that Dali is resuming from a pause, such as
   * when it has transitioned from a background state to a foreground one.
   * It is time to wake up sleeping background threads and recreate memory
   * caches and other temporary data.
   */
  virtual void Resume() {} ///!ToDo: Make pure virtual once dali-adaptor patch is in place = 0;

  // Resource Loading

  /**
   * Determine the size of an image the resource loaders will provide when given the same
   * image attributes.
   * This is a synchronous request.
   * This function is used to determine the size of an image before it has loaded.
   * @param[in] filename name of the image.
   * @param[in] attributes The attributes used to load the image
   * @param[out] closestSize Size of the image that will be loaded.
   */
  virtual void GetClosestImageSize( const std::string& filename,
                                    const ImageAttributes& attributes,
                                    Vector2& closestSize ) = 0;

  /**
   * Determine the size of an image the resource loaders will provide when given the same
   * image attributes.
   * This is a synchronous request.
   * This function is used to determine the size of an image before it has loaded.
   * @param[in] resourceBuffer A pointer to an encoded image buffer
   * @param[in] attributes The attributes used to load the image
   * @param[out] closestSize Size of the image that will be loaded.
   */
  virtual void GetClosestImageSize( ResourcePointer resourceBuffer,
                                    const ImageAttributes& attributes,
                                    Vector2& closestSize ) = 0;

  /**
   * Request a resource from the native filesystem. This is an asynchronous request.
   * After this method returns, FillResourceCache() will be called to retrieve the result(s) of the
   * resource loading operation. Loading resources in separate worker thread is recommended.
   * Multi-threading note: this method will be called from the main thread only i.e. not
   * from within the Core::Render() method.
   * @param[in] request A unique resource request. This is not guaranteed to survive after LoadResource
   * returns; the loading process should take a copy.
   */
  virtual void LoadResource(const ResourceRequest& request) = 0;

  /**
   * Request a resource from the native filesystem. This is a synchronous request, i.e.
   * it will block the main loop whilst executing. It should therefore be used sparingly.
   *
   * Multi-threading note: this method will be called from the main thread only i.e. not
   * from within the Core::Render() method.
   * @param[in] resourceType The type of resource to load
   * @param[in] resourcePath The path to the resource
   * @return A pointer to a ref-counted resource
   */
  virtual ResourcePointer LoadResourceSynchronously( const ResourceType& resourceType, const std::string& resourcePath ) = 0;

  /**
   * Request that a resource be saved to the native filesystem.
   * This is an asynchronous request.
   */
  virtual void SaveResource(const ResourceRequest& request) = 0;
  /**
   * Cancel an ongoing LoadResource() request.
   * Multi-threading note: this method will be called from the main thread only i.e. not
   * from within the Core::Render() method.
   * @param[in] id The ID of the resource to cancel.
   * @param[in] typeId The ID type of the resource to cancel.
   */
  virtual void CancelLoad(ResourceId id, ResourceTypeId typeId) = 0;

  /**
   * Query whether any asynchronous LoadResource() requests are ongoing.
   * Multi-threading note: this method may be called from either the main or rendering thread.
   * @return True if resources are being loaded.
   */
  virtual bool IsLoading() = 0;

  /**
   * Retrieve newly loaded resources.
   * If no resources have finished loading, then this method returns immediately.
   * Multi-threading note: this method will be called from the update thread, from within
   * the UpdateManager::Update() method.
   * @param[in] cache The resource cache to fill.
   */
  virtual void GetResources(ResourceCache& cache) = 0;

  /**
   * Waits for the asynchronous loader threads (if any) to finish.
   * This will be only be called before Core destruction; no resource loading requests will be
   * made following this method.
   */
  virtual void JoinLoaderThreads() = 0;

  /**
   * Sets horizontal and vertical pixels per inch value that is used by the display
   * @param[in] dpiHorizontal horizontal dpi value
   * @param[in] dpiVertical   vertical dpi value
   */
  virtual void SetDpi (unsigned int dpiHorizontal, unsigned int dpiVertical) = 0;

  /**
   * Load a file into a buffer
   * @param[in] filename The filename to load
   * @param[out] buffer  A buffer to receive the file.
   * @result             true if the file is loaded.
   */
  virtual bool LoadFile( const std::string& filename, std::vector< unsigned char >& buffer ) const = 0;

  /**
   * Load a file into a buffer
   * @param[in] filename The filename to save
   * @param[out] buffer  A buffer containing some data
   *                     The buffer is implemeneted with a std::vector. The size() member specifies the buffer length.
   * @result             true if the file is saved.
   */
  virtual bool SaveFile(const std::string& filename, std::vector< unsigned char >& buffer) const = 0;

  /**
   * Get a pointer to the DynamicsFactory.
   */
  virtual DynamicsFactory* GetDynamicsFactory() = 0;

}; // class PlatformAbstraction

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_PLATFORM_ABSTRACTION_H__
