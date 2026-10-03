/*
 * Copyright (C) 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#define LOG_TAG "HoverProximitySensor"

#include "HoverProximitySensor.h"

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

HoverProximitySensor::HoverProximitySensor(int32_t sensorHandle, ISensorsEventCallback* callback)
    : Sensor(sensorHandle, callback) {
    mSensorInfo.name = "Hover Proximity";
    mSensorInfo.type = static_cast<SensorType>(33171007);
    mSensorInfo.typeAsString = "com.samsung.sensor.hover_proximity";
    openInputDevice("sec_touchproximity");
}

HoverProximitySensor::~HoverProximitySensor() {
}

void HoverProximitySensor::activate(bool enable) {
    ALOGI("%s: enable=%d", __func__, enable);

    if (enable) {
        writeSysfsStr(SYSFS_EAR_DETECT, "ear_detect_enable,1");
    } else {
        writeSysfsStr(SYSFS_EAR_DETECT, "ear_detect_enable,0");
    }

    Sensor::activate(enable);
}

void HoverProximitySensor::readEvents() {
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

bool HoverProximitySensor::isValidEvent(int value) {
    return (value == 0 || value == 1);
}

}  // namespace implementation
}  // namespace subhal
}  // namespace V2_0
}  // namespace sensors
}  // namespace hardware
}  // namespace android
