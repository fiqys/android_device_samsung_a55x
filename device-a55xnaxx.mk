#
# SPDX-FileCopyrightText: The LineageOS Project
# SPDX-License-Identifier: Apache-2.0
#

DEVICE_PATH := device/samsung/a55x

# Inherit the shared a55x device configuration.
$(call inherit-product, device/samsung/a55x/device-common.mk)

# Get non-open-source specific aspects for SM-A556B
$(call inherit-product, vendor/samsung/a55xnaxx/a55xnaxx-vendor.mk)
