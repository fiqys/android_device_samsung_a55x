#
# SPDX-FileCopyrightText: The LineageOS Project
# SPDX-License-Identifier: Apache-2.0
#

DEVICE_PATH := device/samsung/a55x

# Security
VENDOR_SECURITY_PATCH := 2026-07-05

# VINTF
DEVICE_FRAMEWORK_COMPATIBILITY_MATRIX_FILE += hardware/samsung/vintf/samsung_framework_compatibility_matrix.xml
DEVICE_MANIFEST_FILE := $(DEVICE_PATH)/configs/vintf/manifest.xml
