/*
 * Copyright (C) 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#define LOG_TAG "SensorSubHal"

#include "SensorSubHal.h"
#include "HoverProximitySensor.h"
#include "TouchPocketSensor.h"
#include "HallICSensor.h"

#include <poll.h>
#include <log/log.h>

using ::android::hardware::sensors::V2_0::implementation::ISensorsSubHal;
using ::android::hardware::sensors::V2_0::subhal::implementation::InputVirtualSensorSubHal;

namespace android {
namespace hardware {
namespace sensors {
namespace V2_0 {
namespace subhal {
namespace implementation {

using ::android::hardware::Void;
using ::android::hardware::sensors::V2_0::implementation::ScopedWakelock;

InputVirtualSensorSubHal::InputVirtualSensorSubHal()
    : mCallback(nullptr), mStopThread(false), mOperationMode(OperationMode::NORMAL) {
    makeSensorList();
}

InputVirtualSensorSubHal::~InputVirtualSensorSubHal() {
    mStopThread = true;
    if (mPollThread.joinable()) {
        mPollThread.join();
    }
    deleteSensorList();
}

Return<void> InputVirtualSensorSubHal::getSensorsList(ISensors::getSensorsList_cb _hidl_cb) {
    std::vector<SensorInfo> sensors;
    for (auto sensor : mSensors) {
        if (sensor->checkInit()) {
            sensors.push_back(sensor->getSensorInfo());
        }
    }
    _hidl_cb(sensors);
    return Void();
}

Return<Result> InputVirtualSensorSubHal::setOperationMode(OperationMode mode) {
    mOperationMode = mode;
    return Result::OK;
}

Return<Result> InputVirtualSensorSubHal::activate(int32_t sensorHandle, bool enabled) {
    for (auto sensor : mSensors) {
        if (sensor->getSensorInfo().sensorHandle == sensorHandle) {
            sensor->activate(enabled);
            return Result::OK;
        }
    }

    return Result::BAD_VALUE;
}

Return<Result> InputVirtualSensorSubHal::batch(int32_t sensorHandle, int64_t samplingPeriodNs,
                                               int64_t maxReportLatencyNs) {
    ALOGV("batch: handle=%d", sensorHandle);
    return Result::OK;
}

Return<Result> InputVirtualSensorSubHal::flush(int32_t sensorHandle) {
    ALOGV("flush: handle=%d", sensorHandle);

    for (auto sensor : mSensors) {
        if (sensor->getSensorInfo().sensorHandle == sensorHandle) {
            return sensor->flush();
        }
    }

    return Result::BAD_VALUE;
}

Return<Result> InputVirtualSensorSubHal::injectSensorData(const Event& event) {
    return Result::INVALID_OPERATION;
}

Return<void> InputVirtualSensorSubHal::registerDirectChannel(const SharedMemInfo& mem,
                                                             ISensors::registerDirectChannel_cb _hidl_cb) {
    _hidl_cb(Result::INVALID_OPERATION, -1);
    return Void();
}

Return<Result> InputVirtualSensorSubHal::unregisterDirectChannel(int32_t channelHandle) {
    return Result::INVALID_OPERATION;
}

Return<void> InputVirtualSensorSubHal::configDirectReport(int32_t sensorHandle,
                                                         int32_t channelHandle,
                                                         RateLevel rate,
                                                         ISensors::configDirectReport_cb _hidl_cb) {
    _hidl_cb(Result::INVALID_OPERATION, 0);
    return Void();
}

Return<void> InputVirtualSensorSubHal::debug(const hidl_handle& fd,
                                            const hidl_vec<hidl_string>& args) {
    return Void();
}

Return<Result> InputVirtualSensorSubHal::initialize(const sp<IHalProxyCallback>& halProxyCallback) {
    mCallback = halProxyCallback;
    mPollThread = std::thread(startThread, this);
    return Result::OK;
}

void InputVirtualSensorSubHal::postEvents(const std::vector<Event>& events, bool wakeup) {
    ScopedWakelock wakelock = mCallback->createScopedWakelock(wakeup);
    mCallback->postEvents(events, std::move(wakelock));
}

void InputVirtualSensorSubHal::makeSensorList() {
    mSensors.push_back(new HoverProximitySensor(1, this));
    mSensors.push_back(new TouchPocketSensor(2, this));
    mSensors.push_back(new HallICSensor(3, this));
}

void InputVirtualSensorSubHal::deleteSensorList() {
    for (auto sensor : mSensors) {
        delete sensor;
    }
    mSensors.clear();
}

void InputVirtualSensorSubHal::run() {
    while (!mStopThread) {
        std::vector<struct pollfd> pollfds;

        for (auto sensor : mSensors) {
            if (sensor->isEnabledSensor() && sensor->getFd() >= 0) {
                struct pollfd pfd;
                pfd.fd = sensor->getFd();
                pfd.events = POLLIN;
                pfd.revents = 0;
                pollfds.push_back(pfd);
            }
        }

        if (pollfds.empty()) {
            usleep(100000);
            continue;
        }

        int ret = poll(pollfds.data(), pollfds.size(), 100);
        if (ret < 0) {
            ALOGE("poll() failed: %s", strerror(errno));
            continue;
        }

        if (ret == 0) {
            continue;
        }

        size_t pollfdIndex = 0;
        for (auto sensor : mSensors) {
            if (sensor->isEnabledSensor() && sensor->getFd() >= 0) {
                if (pollfdIndex < pollfds.size() && pollfds[pollfdIndex].revents & POLLIN) {
                    sensor->readEvents();
                }
                pollfdIndex++;
            }
        }
    }
}

void InputVirtualSensorSubHal::startThread(InputVirtualSensorSubHal* hal) {
    hal->run();
}

}  // namespace implementation
}  // namespace subhal
}  // namespace V2_0
}  // namespace sensors
}  // namespace hardware
}  // namespace android

extern "C" ISensorsSubHal* sensorsHalGetSubHal(uint32_t* version) {
    static InputVirtualSensorSubHal subHal;
    *version = SUB_HAL_2_0_VERSION;
    return &subHal;
}
