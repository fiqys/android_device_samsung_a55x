/*
 * Copyright (C) 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#define LOG_TAG "TouchPocketSensor"

#include "TouchPocketSensor.h"

#include <linux/input.h>
#include <unistd.h>
#include <log/log.h>
#include <utils/SystemClock.h>

namespace android {
namespace hardware {
namespace sensors {
namespace V2_0 {
namespace subhal {
namespace implementation {

TouchPocketSensor::TouchPocketSensor(int32_t sensorHandle, ISensorsEventCallback* callback)
    : Sensor(sensorHandle, callback) {
    mSensorInfo.name = "Touch Pocket";
    mSensorInfo.type = static_cast<SensorType>(33171025);
    mSensorInfo.typeAsString = "com.samsung.sensor.touch_pocket";
    openInputDevice("sec_touchproximity");
}

TouchPocketSensor::~TouchPocketSensor() {
}

void TouchPocketSensor::readEvents() {
    if (mFd < 0 || !mEnabled) {
        return;
    }

    struct input_event ev;
    ssize_t n = read(mFd, &ev, sizeof(ev));

    if (n != sizeof(ev)) {
        return;
    }

    if (ev.type == EV_SYN) {
        return;
    }

    if (!isValidEvent(ev.value)) {
        return;
    }

    if (mPreviousEvent.u.scalar == static_cast<float>(ev.value)) {
        return;
    }

    Event out;
    memset(&out, 0, sizeof(Event));
    out.sensorHandle = mSensorInfo.sensorHandle;
    out.sensorType = mSensorInfo.type;
    out.timestamp = ::android::elapsedRealtimeNano();
    out.u.scalar = static_cast<float>(ev.value);

    ALOGI("%s: report event %f", __func__, out.u.scalar);

    std::vector<Event> events;
    events.push_back(out);
    mCallback->postEvents(events, isWakeUpSensor());

    mPreviousEvent = out;
}

bool TouchPocketSensor::isValidEvent(int value) {
    return (value == 0 || value == 1);
}

}  // namespace implementation
}  // namespace subhal
}  // namespace V2_1
}  // namespace sensors
}  // namespace hardware
}  // namespace android
