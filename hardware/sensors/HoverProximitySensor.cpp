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
    : Sensor(sensorHandle, callback),
      mEnabledEarHover(false),
      mScreenOn(true),
      mFirstEvent(true),
      mHoverEventSkip(false),
      mCurrentMode(0),
      mLastReportedDistance(-1.0f),
      mLastProximityState(-1) {
    mSensorInfo.name = "Hover Proximity";
    mSensorInfo.type = static_cast<SensorType>(33171007);
    mSensorInfo.typeAsString = "com.samsung.sensor.hover_proximity";
    openInputDevice("sec_touchproximity");
    ALOGI("[EARPROXIMITY] Samsung Ear Proximity initialized");
}

HoverProximitySensor::~HoverProximitySensor() {
    transitionToMode0();
}

void HoverProximitySensor::setEarDetectMode(int mode) {
    if (mCurrentMode == mode) {
        return;
    }

    char cmd[64];
    snprintf(cmd, sizeof(cmd), "ear_detect_enable,%d", mode);
    writeSysfsStr(SYSFS_EAR_DETECT, cmd);
    mCurrentMode = mode;
    ALOGI("path : %s, : %d, disable : false", SYSFS_EAR_DETECT, mode);
}

void HoverProximitySensor::transitionToMode0() {
    ALOGI("[EARPROXIMITY] ear_detect_enable set 0");
    setEarDetectMode(0);
}

void HoverProximitySensor::transitionToMode1() {
    ALOGI("[EARPROXIMITY] ear_detect_enable set 1");
    setEarDetectMode(1);
}

void HoverProximitySensor::transitionToMode3() {
    ALOGI("[EARPROXIMITY] ear_detect_enable set 3");
    setEarDetectMode(3);
}

void HoverProximitySensor::activate(bool enable) {
    std::lock_guard<std::mutex> lock(mLock);

    ALOGI("[EARPROXIMITY] activateEarProximity enable %d , proximity on %d, callgesture on 0",
          enable, enable);

    if (mEnabledEarHover == enable) {
        return;
    }

    mEnabledEarHover = enable;
    mLastReportedDistance = -1.0f;
    mLastProximityState = -1;

    if (enable) {
        if (mScreenOn) {
            transitionToMode3();
        } else {
            transitionToMode1();
        }

        mFirstEvent = true;
        mHoverEventSkip = false;

    } else {
        transitionToMode0();

        mFirstEvent = false;
        mHoverEventSkip = false;
    }

    Sensor::activate(enable);
}

void HoverProximitySensor::updateScreenState(bool screenOn) {
    std::lock_guard<std::mutex> lock(mLock);

    bool stateChanged = (mScreenOn != screenOn);

    if (screenOn) {
        ALOGI("[EARHOVER_PROXIMITY] screen on distance %d , proxdistance %d powerKey 0 doubleTap 0, mHallICFar 1",
              (int)mLastReportedDistance, (int)mLastReportedDistance);

        if (stateChanged) {
            if (mEnabledEarHover) {
                transitionToMode3();
            }
            mHoverEventSkip = false;
            mFirstEvent = true;
            mLastReportedDistance = -1.0f;
            mLastProximityState = -1;
        }

    } else {
        ALOGI("[EARPROXIMITY] screen off");
        ALOGI("[EARHOVER_PROXIMITY] screen off distance %d , proxdistance %d powerKey 0 doubleTap 0",
              (int)mLastReportedDistance, (int)mLastReportedDistance);

        if (stateChanged && mEnabledEarHover) {
            transitionToMode1();

            mFirstEvent = false;
            mHoverEventSkip = false;
            mLastReportedDistance = -1.0f;
            mLastProximityState = -1;
        }
    }

    mScreenOn = screenOn;
}

bool HoverProximitySensor::shouldSkipEvent() {
    if (!mEnabledEarHover || (mScreenOn && mCurrentMode != 3) || mHoverEventSkip) {
        ALOGV("[EARHOVER_PROXIMITY] Skip Hover %d mScreenOn %d, mEnabledEarHover %d, first %u mHoverEventSkip %d",
              mCurrentMode, mScreenOn, mEnabledEarHover, mFirstEvent, mHoverEventSkip);
        mFirstEvent = false;
        mHoverEventSkip = false;
        return true;
    }

    return false;
}

bool HoverProximitySensor::isDuplicateEvent(float distance) {
    if (distance == 1.0f) {
        if (mLastProximityState == 0) {
            ALOGV("[EARHOVER_PROXIMITY] Skip duplicated FAR event");
            return true;
        }
        return false;
    }

    if (mLastProximityState == 5) {
        ALOGV("[EARHOVER_PROXIMITY] Skip duplicated CLOSE event");
        return true;
    }

    return false;
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

    if (handleScreenStateEvent(ev)) {
        return;
    }

    if (!isValidEvent(ev)) {
        return;
    }

    std::lock_guard<std::mutex> lock(mLock);

    float distance = static_cast<float>(ev.value);
    int proximityState = (distance == 1.0f) ? 0 : 5;

    if (shouldSkipEvent()) {
        return;
    }

    if (isDuplicateEvent(distance)) {
        return;
    }

    mLastReportedDistance = distance;
    mLastProximityState = proximityState;
    mFirstEvent = false;

    Event out;
    memset(&out, 0, sizeof(Event));
    out.sensorHandle = mSensorInfo.sensorHandle;
    out.sensorType = mSensorInfo.type;
    out.timestamp = ::android::elapsedRealtimeNano();
    out.u.scalar = distance;

    ALOGI("%s: report event %f", __func__, out.u.scalar);

    std::vector<Event> events;
    events.push_back(out);
    mCallback->postEvents(events, isWakeUpSensor());

    mPreviousEvent = out;
}

bool HoverProximitySensor::handleScreenStateEvent(const input_event& event) {
    if (event.type != EV_ABS || event.code != ABS_MT_CUSTOM) {
        return false;
    }

    if (event.value == SCREEN_ON_EVENT) {
        updateScreenState(true);
        return true;
    }

    if (event.value == SCREEN_OFF_EVENT) {
        updateScreenState(false);
        return true;
    }

    return false;
}

bool HoverProximitySensor::isValidEvent(const input_event& event) {
    return event.type == EV_ABS && event.code == ABS_MT_CUSTOM &&
           (event.value == 0 || event.value == 1 || event.value == 2 || event.value == 3 ||
            event.value == 4 || event.value == 5);
}

}  // namespace implementation
}  // namespace subhal
}  // namespace V2_0
}  // namespace sensors
}  // namespace hardware
}  // namespace android
