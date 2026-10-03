/*
 * Copyright (C) 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#pragma once

#include <android/hardware/sensors/1.0/types.h>
#include <vector>

using ::android::hardware::sensors::V1_0::Event;
using ::android::hardware::sensors::V1_0::Result;
using ::android::hardware::sensors::V1_0::SensorInfo;
using ::android::hardware::sensors::V1_0::SensorType;

namespace android {
namespace hardware {
namespace sensors {
namespace V2_0 {
namespace subhal {
namespace implementation {

class ISensorsEventCallback {
  public:
    virtual ~ISensorsEventCallback() = default;
    virtual void postEvents(const std::vector<Event>& events, bool wakeup) = 0;
};

class Sensor {
  public:
    Sensor(int32_t sensorHandle, ISensorsEventCallback* callback);
    virtual ~Sensor();

    const SensorInfo& getSensorInfo() const;
    virtual void activate(bool enable);
    virtual Result flush();
    virtual void readEvents() = 0;

    int getFd() const { return mFd; }
    bool isEnabledSensor() const { return mEnabled; }
    bool checkInit() const { return mFd >= 0; }

  protected:
    bool openInputDevice(const char* deviceName);
    bool writeSysfsInt(const char* path, int value);
    bool writeSysfsStr(const char* path, const char* value);
    bool isWakeUpSensor();

    SensorInfo mSensorInfo;
    ISensorsEventCallback* mCallback;
    int mFd;
    bool mEnabled;
    Event mPreviousEvent;
};

}  // namespace implementation
}  // namespace subhal
}  // namespace V2_0
}  // namespace sensors
}  // namespace hardware
}  // namespace android
