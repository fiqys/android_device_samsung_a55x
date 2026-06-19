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
#

# Inherit from the common tree
$(call inherit-product, device/samsung/a55x-common/common.mk)

# Inherit proprietary files
$(call inherit-product, vendor/samsung/a55x/a55x-vendor.mk)

# Overlays
DEVICE_PACKAGE_OVERLAYS += $(LOCAL_PATH)/overlay

# Permissions
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.telephony.gsm.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.telephony.gsm.xml \
    frameworks/native/data/etc/android.hardware.telephony.satellite.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.telephony.satellite.xml

# Ramdisk firmware
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/firmware/sgpu/vangogh_lite_unified_evt0_e.bin:$(TARGET_COPY_OUT_VENDOR_RAMDISK)/lib/firmware/sgpu/vangogh_lite_unified_evt0_e.bin \
    $(LOCAL_PATH)/firmware/sgpu/vangogh_lite_unified_evt0_b.bin:$(TARGET_COPY_OUT_VENDOR_RAMDISK)/lib/firmware/sgpu/vangogh_lite_unified_evt0_b.bin \
    $(LOCAL_PATH)/firmware/sgpu/vangogh_lite_unified_evt0_e.bin:$(TARGET_COPY_OUT_VENDOR_RAMDISK)/first_stage_ramdisk/lib/firmware/sgpu/vangogh_lite_unified_evt0_e.bin \
    $(LOCAL_PATH)/firmware/sgpu/vangogh_lite_unified_evt0_b.bin:$(TARGET_COPY_OUT_VENDOR_RAMDISK)/first_stage_ramdisk/lib/firmware/sgpu/vangogh_lite_unified_evt0_b.bin

# Bootctl blobs
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/recovery/root/system/bin/hw/android.hardware.boot-service.exynos_recovery:$(TARGET_COPY_OUT_RECOVERY)/system/bin/hw/android.hardware.boot-service.exynos_recovery \
    $(LOCAL_PATH)/recovery/root/system/lib64/android.hardware.boot-V1-ndk.so:$(TARGET_COPY_OUT_RECOVERY)/system/lib64/android.hardware.boot-V1-ndk.so \
    $(LOCAL_PATH)/recovery/root/system/etc/init/android.hardware.boot-service.exynos_recovery.rc:$(TARGET_COPY_OUT_RECOVERY)/system/etc/init/android.hardware.boot-service.exynos_recovery.rc \
    $(LOCAL_PATH)/recovery/root/system/etc/vintf/manifest/android.hardware.boot-service.exynos.xml:$(TARGET_COPY_OUT_RECOVERY)/system/etc/vintf/manifest/android.hardware.boot-service.exynos.xml

# Placeholders
EMPTY_PLACEHOLDER := $(LOCAL_PATH)/configs/placeholder

PRODUCT_COPY_FILES += \
    $(EMPTY_PLACEHOLDER):$(TARGET_COPY_OUT_VENDOR)/firmware/AIE.bin \
    $(EMPTY_PLACEHOLDER):$(TARGET_COPY_OUT_VENDOR)/firmware/calliope_sram.bin \
    $(EMPTY_PLACEHOLDER):$(TARGET_COPY_OUT_VENDOR)/firmware/mfc_fw.bin \
    $(EMPTY_PLACEHOLDER):$(TARGET_COPY_OUT_VENDOR)/firmware/os.checked.bin \
    $(EMPTY_PLACEHOLDER):$(TARGET_COPY_OUT_VENDOR)/firmware/pablo_icpufw.bin \
    $(EMPTY_PLACEHOLDER):$(TARGET_COPY_OUT_VENDOR)/firmware/vts.bin \
    $(EMPTY_PLACEHOLDER):$(TARGET_COPY_OUT_VENDOR)/firmware/wifi/mx140.bin \
    $(EMPTY_PLACEHOLDER):$(TARGET_COPY_OUT_VENDOR)/firmware/wifi/slsi_reg_database.bin

# RIL
PRODUCT_PACKAGES += \
    secril_config_svc \
    sehradiomanager

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/ril/sehradiomanager.conf:$(TARGET_COPY_OUT_VENDOR)/etc/sehradiomanager.conf
