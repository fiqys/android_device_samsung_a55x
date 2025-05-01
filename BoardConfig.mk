#
# Copyright (C) 2020-2024 The LineageOS Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

DEVICE_PATH := device/samsung/a55x
TARGET_KERNEL_DIR := $(DEVICE_PATH)-kernel

## Inherit from the common tree
include device/samsung/a55x-common/BoardConfigCommon.mk

## Inherit from the proprietary configuration
include vendor/samsung/a55x/BoardConfigVendor.mk

# Prebuilts
BOARD_PREBUILT_DTBOIMAGE := $(TARGET_KERNEL_DIR)/dtbo.img
BOARD_PREBUILT_DTBIMAGE_DIR := $(TARGET_KERNEL_DIR)/dtb

TARGET_NO_KERNEL_OVERRIDE := true
TARGET_KERNEL_SOURCE := $(TARGET_KERNEL_DIR)/kernel-headers
PRODUCT_COPY_FILES += \
	$(TARGET_KERNEL_DIR)/kernel:kernel

# Kernel modules
BOARD_VENDOR_RAMDISK_KERNEL_MODULES_LOAD := $(strip $(shell cat $(TARGET_KERNEL_DIR)/vendor_ramdisk/modules.load))
BOARD_VENDOR_KERNEL_MODULES_LOAD := $(strip $(shell cat $(TARGET_KERNEL_DIR)/vendor_dlkm/modules.load))
BOARD_SYSTEM_KERNEL_MODULES_LOAD := $(strip $(shell cat $(TARGET_KERNEL_DIR)/system_dlkm/modules.load))
SYSTEM_KERNEL_MODULES := $(BOARD_SYSTEM_KERNEL_MODULES_LOAD)

PRODUCT_COPY_FILES += \
    $(call find-copy-subdir-files,*,$(TARGET_KERNEL_DIR)/vendor_dlkm/,$(TARGET_COPY_OUT_VENDOR_DLKM)/lib/modules) \
    $(call find-copy-subdir-files,*,$(TARGET_KERNEL_DIR)/vendor_ramdisk/,$(TARGET_COPY_OUT_VENDOR_RAMDISK)/lib/modules) \
    $(call find-copy-subdir-files,*,$(TARGET_KERNEL_DIR)/system_dlkm/,$(TARGET_COPY_OUT_SYSTEM_DLKM)/lib/modules)

## Properties
TARGET_VENDOR_PROP += $(DEVICE_PATH)/vendor.prop
