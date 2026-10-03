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

class HallICSensor : public Sensor {
  public:
    HallICSensor(int32_t sensorHandle, ISensorsEventCallback* callback);
    virtual ~HallICSensor();

    void readEvents() override;

  private:
    int getKernelVersion();
    int getCertifyCoverStatus();
};

}  // namespace implementation
}  // namespace subhal
}  // namespace V2_1
}  // namespace sensors
}  // namespace hardware
}  // namespace android
