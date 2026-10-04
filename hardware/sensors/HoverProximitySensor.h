/*
 * Copyright (C) 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#pragma once

#include "Sensor.h"
#include <chrono>
#include <linux/input.h>
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
    void updateScreenState(bool screenOn);

  private:
    bool isValidEvent(const input_event& event);
    bool handleScreenStateEvent(const input_event& event);
    void setEarDetectMode(int mode);
    void transitionToMode0();
    void transitionToMode1();
    void transitionToMode3();
    bool shouldSkipEvent();
    bool isDuplicateEvent(float distance);

    static constexpr const char* SYSFS_EAR_DETECT = "/sys/class/sec/tsp/cmd";
    static constexpr int ABS_MT_CUSTOM = 0x3e;
    static constexpr int SCREEN_ON_EVENT = 0xFD;
    static constexpr int SCREEN_OFF_EVENT = 0xFE;

    std::mutex mLock;
    bool mEnabledEarHover;
    bool mScreenOn;
    bool mFirstEvent;
    bool mHoverEventSkip;
    int mCurrentMode;
    float mLastReportedDistance;
    int mLastProximityState;
};

}  // namespace implementation
}  // namespace subhal
}  // namespace V2_0
}  // namespace sensors
}  // namespace hardware
}  // namespace android
