/*
 * Copyright (C) 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#pragma once

#include "Sensor.h"
#include <chrono>
#include <mutex>

namespace android {
namespace hardware {
namespace sensors {
namespace V2_0 {
namespace subhal {
namespace implementation {

using std::chrono::milliseconds;
using std::chrono::system_clock;
using std::chrono::duration_cast;

class HoverProximitySensor : public Sensor {
  public:
    HoverProximitySensor(int32_t sensorHandle, ISensorsEventCallback* callback);
    virtual ~HoverProximitySensor();

    void activate(bool enable) override;
    void readEvents() override;

  private:
    bool isValidEvent(int value);

    static constexpr const char* SYSFS_EAR_DETECT = "/sys/class/sec/tsp/cmd";
};

}  // namespace implementation
}  // namespace subhal
}  // namespace V2_0
}  // namespace sensors
}  // namespace hardware
}  // namespace android
