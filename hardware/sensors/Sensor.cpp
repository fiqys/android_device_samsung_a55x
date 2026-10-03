/*
 * Copyright (C) 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#define LOG_TAG "InputVirtualSensor"

#include "Sensor.h"

#include <dirent.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>
#include <fstream>
#include <log/log.h>

namespace android {
namespace hardware {
namespace sensors {
namespace V2_0 {
namespace subhal {
namespace implementation {

using ::android::hardware::sensors::V1_0::SensorFlagBits;

Sensor::Sensor(int32_t sensorHandle, ISensorsEventCallback* callback)
    : mCallback(callback), mFd(-1), mEnabled(false) {
    mSensorInfo.sensorHandle = sensorHandle;
    mSensorInfo.vendor = "Samsung Electronics";
    mSensorInfo.version = 1;
    mSensorInfo.maxRange = 1.0f;
    mSensorInfo.resolution = 1.0f;
    mSensorInfo.power = 0.001f;
    mSensorInfo.minDelay = 0;
    mSensorInfo.fifoReservedEventCount = 0;
    mSensorInfo.fifoMaxEventCount = 0;
    mSensorInfo.requiredPermission = "";
    mSensorInfo.maxDelay = 0;
    mSensorInfo.flags = static_cast<uint32_t>(SensorFlagBits::WAKE_UP |
                                               SensorFlagBits::ON_CHANGE_MODE);
    memset(&mPreviousEvent, 0, sizeof(Event));
}

Sensor::~Sensor() {
    if (mFd >= 0) {
        close(mFd);
        mFd = -1;
    }
}

const SensorInfo& Sensor::getSensorInfo() const {
    return mSensorInfo;
}

void Sensor::activate(bool enable) {
    if (mEnabled != enable) {
        mEnabled = enable;
    }
}

Result Sensor::flush() {
    if (!mEnabled) {
        return Result::BAD_VALUE;
    }
    return Result::OK;
}

bool Sensor::openInputDevice(const char* deviceName) {
    const char* dirname = "/dev/input";
    char devname[PATH_MAX];
    char* filename;
    DIR* dir;
    struct dirent* de;

    dir = opendir(dirname);
    if (dir == nullptr) {
        return false;
    }

    strcpy(devname, dirname);
    filename = devname + strlen(devname);
    *filename++ = '/';

    while ((de = readdir(dir))) {
        if (de->d_name[0] == '.' &&
            (de->d_name[1] == '\0' || (de->d_name[1] == '.' && de->d_name[2] == '\0'))) {
            continue;
        }

        strcpy(filename, de->d_name);
        int fd = open(devname, O_RDONLY | O_NONBLOCK);
        if (fd >= 0) {
            char name[80];
            if (ioctl(fd, EVIOCGNAME(sizeof(name) - 1), &name) >= 0) {
                if (strstr(name, deviceName)) {
                    mFd = fd;
                    ALOGI("Found input device: %s at %s", name, devname);
                    closedir(dir);
                    return true;
                }
            }
            close(fd);
        }
    }

    closedir(dir);
    ALOGE("Could not find input device: %s", deviceName);
    return false;
}

bool Sensor::writeSysfsInt(const char* path, int value) {
    FILE* file = fopen(path, "w");
    if (!file) {
        ALOGE("%s, ERROR open file %s to write with error %d", __func__, path, errno);
        return false;
    }
    fprintf(file, "%d", value);
    fclose(file);
    return true;
}

bool Sensor::writeSysfsStr(const char* path, const char* value) {
    FILE* file = fopen(path, "w");
    if (!file) {
        ALOGE("%s, ERROR open file %s to write with error %d", __func__, path, errno);
        return false;
    }
    fprintf(file, "%s", value);
    fclose(file);
    return true;
}

bool Sensor::isWakeUpSensor() {
    return mSensorInfo.flags & static_cast<uint32_t>(SensorFlagBits::WAKE_UP);
}

}  // namespace implementation
}  // namespace subhal
}  // namespace V2_0
}  // namespace sensors
}  // namespace hardware
}  // namespace android
