/*
 * Copyright (C) 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#pragma once

#include "Sensor.h"

namespace android {
namespace hardware {
namespace sensors {
namespace V2_0 {
namespace subhal {
namespace implementation {

class TouchPocketSensor : public Sensor {
  public:
    TouchPocketSensor(int32_t sensorHandle, ISensorsEventCallback* callback);
    virtual ~TouchPocketSensor();

    void readEvents() override;

  private:
    bool isValidEvent(int value);
};

}  // namespace implementation
}  // namespace subhal
}  // namespace V2_1
}  // namespace sensors
}  // namespace hardware
}  // namespace android
