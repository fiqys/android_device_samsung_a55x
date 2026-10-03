/*
 * Copyright (C) 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#pragma once

#include <vector>
#include <thread>

#include "Sensor.h"
#include "V2_0/SubHal.h"

namespace android {
namespace hardware {
namespace sensors {
namespace V2_0 {
namespace subhal {
namespace implementation {

using ::android::hardware::sensors::V1_0::Event;
using ::android::hardware::sensors::V1_0::OperationMode;
using ::android::hardware::sensors::V1_0::RateLevel;
using ::android::hardware::sensors::V1_0::Result;
using ::android::hardware::sensors::V1_0::SensorInfo;
using ::android::hardware::sensors::V1_0::SharedMemInfo;
using ::android::hardware::sensors::V2_0::implementation::IHalProxyCallback;
using ::android::hardware::sensors::V2_0::implementation::ISensorsSubHal;

class InputVirtualSensorSubHal : public ISensorsSubHal, public ISensorsEventCallback {
  public:
    InputVirtualSensorSubHal();
    virtual ~InputVirtualSensorSubHal();

    Return<void> getSensorsList(ISensors::getSensorsList_cb _hidl_cb) override;
    Return<Result> injectSensorData(const Event& event) override;
    Return<Result> initialize(const sp<IHalProxyCallback>& halProxyCallback) override;
    Return<Result> setOperationMode(OperationMode mode) override;
    Return<Result> activate(int32_t sensorHandle, bool enabled) override;
    Return<Result> batch(int32_t sensorHandle, int64_t samplingPeriodNs,
                         int64_t maxReportLatencyNs) override;
    Return<Result> flush(int32_t sensorHandle) override;
    Return<void> registerDirectChannel(const SharedMemInfo& mem,
                                       ISensors::registerDirectChannel_cb _hidl_cb) override;
    Return<Result> unregisterDirectChannel(int32_t channelHandle) override;
    Return<void> configDirectReport(int32_t sensorHandle, int32_t channelHandle, RateLevel rate,
                                    ISensors::configDirectReport_cb _hidl_cb) override;
    Return<void> debug(const hidl_handle& fd, const hidl_vec<hidl_string>& args) override;
    const std::string getName() override { return "InputVirtualSensorSubHal"; }

    void postEvents(const std::vector<Event>& events, bool wakeup) override;

  private:
    void makeSensorList();
    void deleteSensorList();
    void run();
    static void startThread(InputVirtualSensorSubHal* hal);

    sp<IHalProxyCallback> mCallback;
    std::vector<Sensor*> mSensors;
    std::thread mPollThread;
    bool mStopThread;
    OperationMode mOperationMode;
};

}  // namespace implementation
}  // namespace subhal
}  // namespace V2_0
}  // namespace sensors
}  // namespace hardware
}  // namespace android
