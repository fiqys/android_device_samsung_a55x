/*
 * Copyright (C) 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#define LOG_TAG "HallICSensor"

#include "HallICSensor.h"

#include <linux/input.h>
#include <unistd.h>
#include <log/log.h>
#include <utils/SystemClock.h>
#include <fstream>

namespace android {
namespace hardware {
namespace sensors {
namespace V2_0 {
namespace subhal {
namespace implementation {

HallICSensor::HallICSensor(int32_t sensorHandle, ISensorsEventCallback* callback)
    : Sensor(sensorHandle, callback) {
    mSensorInfo.name = "Samsung Hall IC";
    mSensorInfo.type = static_cast<SensorType>(33171030);
    mSensorInfo.typeAsString = "com.samsung.sensor.hall_ic";
    openInputDevice("hall");
}

HallICSensor::~HallICSensor() {
}

void HallICSensor::readEvents() {
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

    int coverStatus = getCertifyCoverStatus();

    Event out;
    memset(&out, 0, sizeof(Event));
    out.sensorHandle = mSensorInfo.sensorHandle;
    out.sensorType = mSensorInfo.type;
    out.timestamp = ::android::elapsedRealtimeNano();
    out.u.scalar = static_cast<float>(ev.value);

    ALOGI("%s: event %f cover %d", __func__, out.u.scalar, coverStatus);

    std::vector<Event> events;
    events.push_back(out);
    mCallback->postEvents(events, isWakeUpSensor());

    mPreviousEvent = out;
}

int HallICSensor::getKernelVersion() {
    return 0;
}

int HallICSensor::getCertifyCoverStatus() {
    std::ifstream file("/sys/class/sec/tsp/cmd");
    if (!file.is_open()) {
        return 0;
    }

    std::string status;
    std::getline(file, status);
    file.close();

    return 0;
}

}  // namespace implementation
}  // namespace subhal
}  // namespace V2_1
}  // namespace sensors
}  // namespace hardware
}  // namespace android
