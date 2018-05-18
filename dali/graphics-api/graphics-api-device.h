#ifndef DALI_GRAPHICS_API_DEVICE_H
#define DALI_GRAPHICS_API_DEVICE_H

namespace Dali
{
namespace Graphics
{
namespace API
{
class Device
{
public:
  // not copyable
  Device(const Device&) = delete;
  Device& operator=(const Device&) = delete;

  virtual ~Device() = default;

protected:
  // derived types should not be moved direcly to prevent slicing
  Device(Device&&) = default;
  Device& operator=(Device&&) = default;

  /**
   * Objects of this type should not directly.
   */
  Device() = default;
};
}
}
}

#endif // DALI_GRAPHICS_API_DEVICE_H
