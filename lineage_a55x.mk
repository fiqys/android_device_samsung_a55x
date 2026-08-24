#
# SPDX-FileCopyrightText: The LineageOS Project
# SPDX-License-Identifier: Apache-2.0
#

# Inherit from a55x device
$(call inherit-product, device/samsung/a55x/device.mk)

# Device identifier, this must come after all inclusions
PRODUCT_DEVICE := a55x
PRODUCT_NAME := lineage_a55x
PRODUCT_MODEL := SM-A556B

PRODUCT_BUILD_PROP_OVERRIDES += \
    BuildDesc="a55xnaxx-user 14 UP1A.231005.007 A556BXXSGDZG2 release-keys" \
    BuildFingerprint=samsung/a55xnaxx/a55x:14/UP1A.231005.007/A556BXXSGDZG2:user/release-keys \
    DeviceProduct=a55xnaxx \
    SystemName=a55xnaxx
