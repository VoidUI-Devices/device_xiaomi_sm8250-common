/*
 * Copyright (C) 2021 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "AntiFlickerService"

#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/strings.h>

#include "AntiFlicker.h"

namespace vendor {
namespace lineage {
namespace livedisplay {
namespace V2_1 {
namespace implementation {

static constexpr const char* kDcDimmingPath =
    "/sys/class/drm/card0/card0-DSI-1/disp_param";

static constexpr const char* kBrightnessPath =
    "/sys/class/backlight/panel0-backlight/brightness";

Return<bool> AntiFlicker::isEnabled() {
    std::string buf;
    if (!android::base::ReadFileToString(kDcDimmingPath, &buf)) {
        LOG(ERROR) << "Failed to read " << kDcDimmingPath;
        return false;
    }
    return std::stoi(android::base::Trim(buf)) == 1;
}

Return<bool> AntiFlicker::setEnabled(bool enabled) {
    if (enabled) {
        // Write DISPPARAM_DC_ON to the kDcDimmingPath file
        if (!android::base::WriteStringToFile("0x50000", kDcDimmingPath)) {
            LOG(ERROR) << "Failed to write DISPPARAM_DC_ON to " << kDcDimmingPath;
            return false;
        }
        // Write DISPPARAM_DIMMING_OFF to the kDcDimmingPath file
        if (!android::base::WriteStringToFile("0xE00", kDcDimmingPath)) {
            LOG(ERROR) << "Failed to write DISPPARAM_DIMMING_OFF to " << kDcDimmingPath;
            return false;
        }
        // Write DISPPARAM_DIMMING_ON to the kDcDimmingPath file
        if (!android::base::WriteStringToFile("0xF00", kDcDimmingPath)) {
            LOG(ERROR) << "Failed to write DISPPARAM_DIMMING_ON to " << kDcDimmingPath;
            return false;
        }
		
        // Read the current value of the kBrightnessPath file
        std::string brightness;
        if (!android::base::ReadFileToString(kBrightnessPath, &brightness)) {
            LOG(ERROR) << "Failed to read " << kBrightnessPath;
            return false;
        }

        // Write the current value of the kBrightnessPath file back
        if (!android::base::WriteStringToFile(brightness, kBrightnessPath)) {
            LOG(ERROR) << "Failed to update the brightness node";
            return false;
        }
    } else {
        // Write DISPPARAM_DIMMING_OFF to the kDcDimmingPath file	
        if (!android::base::WriteStringToFile("0xE00", kDcDimmingPath)) {
            LOG(ERROR) << "Failed to write DISPPARAM_DIMMING_OFF to " << kDcDimmingPath;
            return false;
        }
        // Write DISPPARAM_CRC_OFF to the kDcDimmingPath file
        if (!android::base::WriteStringToFile("0xF00000", kDcDimmingPath)) {
            LOG(ERROR) << "Failed to write DISPPARAM_CRC_OFF to " << kDcDimmingPath;
            return false;
        }
        // Write DISPPARAM_DC_OFF to the kDcDimmingPath file
        if (!android::base::WriteStringToFile("0x50000", kDcDimmingPath)) {
            LOG(ERROR) << "Failed to write DISPPARAM_DC_OFF to " << kDcDimmingPath;
            return false;
        }
        // Write DISPPARAM_DIMMING_ON to the kDcDimmingPath file
        if (!android::base::WriteStringToFile("0xF00", kDcDimmingPath)) {
            LOG(ERROR) << "Failed to write DISPPARAM_DIMMING_ON to " << kDcDimmingPath;
            return false;
        }
        // Read the current value of the kBrightnessPath file
        std::string brightness;
        if (!android::base::ReadFileToString(kBrightnessPath, &brightness)) {
            LOG(ERROR) << "Failed to read " << kBrightnessPath;
            return false;
        }

        // Write the current value of the kBrightnessPath file back
        if (!android::base::WriteStringToFile(brightness, kBrightnessPath)) {
            LOG(ERROR) << "Failed to update the brightness node";
            return false;
        }
    }
    return true;
}

}  // namespace implementation
}  // namespace V2_1
}  // namespace livedisplay
}  // namespace lineage
}  // namespace vendor
