/*
 * World Radio — Global Broadcast Station Browser
 * Copyright (C) 2026 Len Cochrane
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

#ifndef __UI_WORLD_RADIO_H__
#define __UI_WORLD_RADIO_H__

#include "ui_widget.hpp"
#include "ui_navigation.hpp"
#include "receiver_model.hpp"
#include "file.hpp"

#include <string>
#include <vector>
#include <array>

namespace ui::external_app::world_radio {

struct Station {
    rf::Frequency freq_hz{0};
    std::string   callsign{};
    std::string   label{};
    bool          is_am{false};
};

struct FreqFile {
    std::string filename{};
    std::string display{};
    std::vector<Station> stations{};
    bool loaded{false};
};

// Fixed static declaration structure without nested runtime vectors
struct Country {
    const char* code;
    const char* name;
    int         region_idx;
};

// Runtime dynamic context mapping
struct RuntimeCountryState {
    const char* code;
    const char* name;
    int         region_idx;
    std::vector<FreqFile> files{};
    bool        scanned{false};
};

static const char* const REGIONS[] = {
    "North America",
    "South America",
    "Europe",
    "Asia-Pacific",
    "Middle East",
    "Africa",
};
static constexpr int NUM_REGIONS = 6;

static const Country COUNTRY_TABLE[] = {
    {"CA", "Canada",               0},
    {"US", "United States",        0},
    {"MX", "Mexico",               0},
    {"CU", "Cuba",                 0},
    {"GT", "Guatemala",            0},
    {"HN", "Honduras",             0},
    {"CR", "Costa Rica",           0},
    {"PA", "Panama",               0},
    {"JM", "Jamaica",              0},
    {"TT", "Trinidad & Tobago",    0},
    {"DO", "Dominican Republic",   0},
    {"PR", "Puerto Rico",          0},
    {"BR", "Brazil",               1},
    {"AR", "Argentina",            1},
    {"CL", "Chile",                1},
    {"CO", "Colombia",             1},
    {"PE", "Peru",                 1},
    {"VE", "Venezuela",            1},
    {"EC", "Ecuador",              1},
    {"BO", "Bolivia",              1},
    {"PY", "Paraguay",             1},
    {"UY", "Uruguay",              1},
    {"GB", "United Kingdom",       2},
    {"DE", "Germany",              2},
    {"FR", "France",               2},
    {"IT", "Italy",                2},
    {"ES", "Spain",                2},
    {"PT", "Portugal",             2},
    {"NL", "Netherlands",          2},
    {"BE", "Belgium",              2},
    {"CH", "Switzerland",          2},
    {"AT", "Austria",              2},
    {"SE", "Sweden",               2},
    {"NO", "Norway",               2},
    {"DK", "Denmark",              2},
    {"FI", "Finland",              2},
    {"PL", "Poland",               2},
    {"CZ", "Czech Republic",       2},
    {"HU", "Hungary",              2},
    {"RO", "Romania",              2},
    {"GR", "Greece",               2},
    {"IE", "Ireland",              2},
    {"IS", "Iceland",              2},
    {"JP", "Japan",                3},
    {"AU", "Australia",            3},
    {"NZ", "New Zealand",          3},
    {"KR", "South Korea",          3},
    {"CN", "China",                3},
    {"IN", "India",                3},
    {"ID", "Indonesia",            3},
    {"TH", "Thailand",             3},
    {"PH", "Philippines",          3},
    {"MY", "Malaysia",             3},
    {"SG", "Singapore",            3},
    {"TW", "Taiwan",               3},
    {"VN", "Vietnam",              3},
    {"PK", "Pakistan",             3},
    {"BD", "Bangladesh",           3},
    {"LK", "Sri Lanka",            3},
    {"NP", "Nepal",                3},
    {"FJ", "Fiji",                 3},
    {"AE",
