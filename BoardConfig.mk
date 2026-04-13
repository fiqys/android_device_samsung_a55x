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
TARGET_KERNEL_DIR := kernel/samsung/a55x

## Inherit from the common tree
include device/samsung/a55x-common/BoardConfigCommon.mk

## Inherit from the proprietary configuration
include vendor/samsung/a55x/BoardConfigVendor.mk

# Kernel
BOARD_KERNEL_IMAGE_NAME := Image
TARGET_KERNEL_ADDITIONAL_FLAGS := TARGET_SOC=s5e8845 BRANCH=android14-6.1 KMI_GENERATION=11
TARGET_KERNEL_NO_GCC := true
TARGET_KERNEL_SOURCE := kernel/samsung/a55x
TARGET_KERNEL_CONFIG := essi_defconfig

# Kernel Modules
BOARD_VENDOR_RAMDISK_KERNEL_MODULES_LOAD := $(strip $(shell cat $(DEVICE_PATH)/configs/kernel/modules.load))
BOARD_SYSTEM_KERNEL_MODULES_LOAD := $(strip $(shell cat $(DEVICE_PATH)/configs/kernel/modules.load.system_dlkm))
BOARD_VENDOR_KERNEL_MODULES_LOAD := $(strip $(shell cat $(DEVICE_PATH)/configs/kernel/modules.load.vendor_dlkm))
BOOT_KERNEL_MODULES := $(BOARD_VENDOR_RAMDISK_KERNEL_MODULES_LOAD)
SYSTEM_KERNEL_MODULES := $(strip $(shell cat $(DEVICE_PATH)/configs/kernel/modules.include.system_dlkm))

## Properties
TARGET_VENDOR_PROP += $(DEVICE_PATH)/vendor.prop
