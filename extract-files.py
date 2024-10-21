#!/usr/bin/env -S PYTHONPATH=../../../tools/extract-utils python3
#
# SPDX-FileCopyrightText: 2024 The LineageOS Project
# SPDX-License-Identifier: Apache-2.0
#

from extract_utils.fixups_blob import (
    blob_fixup,
    blob_fixups_user_type,
)

from extract_utils.main import (
    ExtractUtils,
    ExtractUtilsModule,
)

namespace_imports = [
    'device/samsung/a55x-common',
    'vendor/samsung/a55x-common',
]

blob_fixups: blob_fixups_user_type = {
    'vendor/lib64/libskeymint_cli.so': blob_fixup()
        .add_needed('libshim_crypto.so'),
    'vendor/lib64/hw/camera.s5e8845.so': blob_fixup()
        .add_needed('libui_shim.so'),
    'vendor/lib64/libvkservice.so': blob_fixup()
        .binary_regex_replace(rb'ro\.factory\.factory_binary', b'ro.vendor.factory_binary\x00'),
    'vendor/bin/vaultkeeperd': blob_fixup()
        .binary_regex_replace(rb'ro\.factory\.factory_binary', b'ro.vendor.factory_binary\x00'),
} # fmt: skip

module = ExtractUtilsModule(
    'a55x',
    'samsung',
    namespace_imports=namespace_imports,
    blob_fixups=blob_fixups,
)

if __name__ == '__main__':
    utils = ExtractUtils.device_with_common(
        module, 'a55x-common', module.vendor
    )
    utils.run()
