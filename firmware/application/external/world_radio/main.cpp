/*
 * World Radio — Global Broadcast Station Browser
 * Copyright (C) 2024 Len Cochrane
 *
 * This file is part of PortaPack.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include "ui.hpp"
#include "ui_world_radio.hpp"
#include "ui_navigation.hpp"
#include "external_app.hpp"

namespace ui::external_app::world_radio {
void initialize_app(NavigationView& nav) {
    nav.push<WorldRadioView>();
}
}  // namespace ui::external_app::world_radio

extern "C" {
__attribute__((section(".external_app.app_world_radio.application_information"), used))
application_information_t _application_information_world_radio = {
    .memory_location   = (uint8_t*)0x00000000,
    .externalAppEntry  = ui::external_app::world_radio::initialize_app,
    .header_version    = CURRENT_HEADER_VERSION,
    .app_version       = VERSION_MD5,
    .app_name          = "World Radio",
    .bitmap_data       = {
        0xF0, 0x0F, 0xFE, 0x7F, 0xFF, 0xFF, 0x1F, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0xF0, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0x1F, 0xF8, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },
    // Explicitly defining trailing fields to resolve the Python post-processor IndexError
    .keep_baseband     = false,
    .baseband_bin      = BASEBAND_TYPE_NONE 
};
}
