// USB ドライバクラス用のドライバのベースk流明日

#pragma once

#include "error.hpp"
#include "usb/endpoint.hpp"
#include "usb/setupdata.hpp"

namespace usb {
class Device;

class ClassDriver {
 public:
  ClassDriver(Device* dev);
  virtual ~ClassDriver();

  virtual Error Initialize() = 0;
  virtual Error SetEndpoint(const EndpointConfig& config) = 0;
  virtual Error OnEndpointsConfigured() = 0;
  virtual Error OnControlCompleted(EndpointID ep_id, SetupData setup_data,
                                   const void* buf, int len) = 0;
  virtual Error OnInterruptCompleted(EndpointID ep_id, const void* buf,
                                     int len) = 0;

  // クラスドライバを保持する USB デバイスを返却する
  Device* ParentDevice() const { return dev_; }

 private:
  Device* dev_;
};
}  // namespace usb