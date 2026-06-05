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

#include "ui_world_radio.hpp"
#include "audio.hpp"
#include "baseband_api.hpp"
#include "portapack.hpp"
#include "string_format.hpp"

#include <algorithm>
#include <cstdio>
#include <cstring>

using namespace portapack;

namespace ui::external_app::world_radio {

WorldRadioView::WorldRadioView(NavigationView& nav) : nav_{nav} {
    for (int i = 0; i < NUM_COUNTRIES; i++) {
        countries_[i] = COUNTRY_TABLE[i];
    }
    for (int i = 0; i < ROWS; i++) {
        row_btns[i].on_select = [this, i](Button&) {
            select_station(scroll_ + i);
        };
    }
    btn_up.on_select = [this](Button&) {
        if (scroll_ > 0) { scroll_--; rebuild_station_rows(); }
    };
    btn_dn.on_select = [this](Button&) {
        int max_s = (int)current_stations_.size() - ROWS;
        if (scroll_ < max_s) { scroll_++; rebuild_station_rows(); }
    };
    opt_region.on_change = [this](size_t, int32_t v) {
        on_region_changed((size_t)v);
    };
    opt_country.on_change = [this](size_t, int32_t v) {
        on_country_changed((size_t)v);
    };
    opt_file.on_change = [this](size_t, int32_t v) {
        on_file_changed((size_t)v);
    };
    btn_tune.on_select = [this](Button&) { tune(); };
    btn_info.on_select = [this](Button&) {
        if (sel_ >= 0 && sel_ < (int)current_stations_.size())
            set_status(current_stations_[sel_].label);
    };
    btn_exit.on_select = [this](Button&) { nav_.pop(); };

    add_children({
        &lbl_region,  &opt_region,
        &lbl_country, &opt_country,
        &lbl_file,    &opt_file,
        &lbl_hdr,
        &btn_up, &btn_dn, &txt_cnt,
        &big_freq,
        &txt_status,
        &btn_tune, &btn_info, &btn_exit,
    });
    for (auto& b : row_btns) add_child(&b);

    rebuild_region_opts();
    on_region_changed(0);
}

WorldRadioView::~WorldRadioView() {}

void WorldRadioView::focus() {
    opt_region.focus();
}

void WorldRadioView::rebuild_region_opts() {
    OptionsField::options_t opts;
    for (int i = 0; i < NUM_REGIONS; i++) {
        opts.push_back({REGIONS[i], i});
    }
    opt_region.set_options(opts);
    opt_region.set_selected_index(0);
}

void WorldRadioView::on_region_changed(size_t idx) {
    region_idx_   = (int)idx;
    country_idx_  = 0;
    file_idx_     = 0;
    scroll_       = 0;
    sel_          = -1;
    current_stations_.clear();
    big_freq.set(0);

    region_countries_.clear();
    for (int i = 0; i < NUM_COUNTRIES; i++) {
        if (countries_[i].region_idx == region_idx_)
            region_countries_.push_back(i);
    }

    rebuild_country_opts();
    rebuild_station_rows();
    set_status(std::string(REGIONS[region_idx_]) + " selected");
}

void WorldRadioView::rebuild_country_opts() {
    OptionsField::options_t opts;
    for (size_t i = 0; i < region_countries_.size(); i++) {
        const auto& c = countries_[region_countries_[i]];
        opts.push_back({c.name, (int32_t)i});
    }
    if (opts.empty()) opts.push_back({"(no data)", 0});
    opt_country.set_options(opts);
    opt_country.set_selected_index(0);
    on_country_changed(0);
}

void WorldRadioView::on_country_changed(size_t idx) {
    country_idx_ = (int)idx;
    file_idx_    = 0;
    scroll_      = 0;
    sel_         = -1;
    current_stations_.clear();
    big_freq.set(0);

    int arr_idx = current_country_arr_idx();
    if (arr_idx < 0) { rebuild_file_opts(); rebuild_station_rows(); return; }

    if (!countries_[arr_idx].scanned) {
        set_status("Scanning SD...");
        scan_country_files(arr_idx);
    }
    rebuild_file_opts();
}

void WorldRadioView::rebuild_file_opts() {
    int arr_idx = current_country_arr_idx();
    OptionsField::options_t opts;

    if (arr_idx >= 0 && !countries_[arr_idx].files.empty()) {
        for (size_t i = 0; i < countries_[arr_idx].files.size(); i++) {
            opts.push_back({countries_[arr_idx].files[i].display, (int32_t)i});
        }
    } else {
        opts.push_back({"(no file on SD)", 0});
    }

    opt_file.set_options(opts);
    opt_file.set_selected_index(0);
    on_file_changed(0);
}

void WorldRadioView::on_file_changed(size_t idx) {
    file_idx_ = (int)idx;
    scroll_   = 0;
    sel_      = -1;
    current_stations_.clear();
    big_freq.set(0);

    int arr_idx = current_country_arr_idx();
    if (arr_idx < 0 || countries_[arr_idx].files.empty()) {
        set_status("No file on SD card");
        rebuild_station_rows();
        return;
    }

    auto& f = countries_[arr_idx].files[file_idx_];
    if (!f.loaded) {
        set_status("Loading...");
        load_file(arr_idx, file_idx_);
    }

    current_stations_ = f.stations;
    rebuild_station_rows();

    char buf[32];
    snprintf(buf, sizeof(buf), "%d stations", (int)current_stations_.size());
    set_status(buf);
}

int WorldRadioView::current_country_arr_idx() const {
    if (country_idx_ < 0 || country_idx_ >= (int)region_countries_.size())
        return -1;
    return region_countries_[country_idx_];
}

void WorldRadioView::scan_country_files(int arr_idx) {
    auto& country = countries_[arr_idx];
    country.files.clear();
    country.scanned = true;

    const char* cc = country.code;

    {
        std::string path = "FREQMAN/";
        path += cc;
        path += ".txt";
        File f;
        if (f.open(path).is_ok()) {
            FreqFile ff;
            ff.filename = std::string(cc) + ".txt";
            ff.display  = country.name;
            ff.loaded   = false;
            country.files.push_back(std::move(ff));
        }
    }

    static const char* CA_SUFFIXES[] = {
        "AB","BC","MB","NB","NL","NS","NT","NU","ON","PE","QC","SK","YT",nullptr
    };
    static const char* US_SUFFIXES[] = {
        "W","E","MW","SE","NE","SW","OTHER",nullptr
    };
    static const char* AU_SUFFIXES[] = {
        "NSW","VIC","QLD","WA","SA","TAS","ACT","NT",nullptr
    };
    static const char* GENERIC_SUFFIXES[] = {
        "N","S","E","W","C","NE","NW","SE","SW","1","2","3","4","5",nullptr
    };

    static const char* SINGLE_FILE_COUNTRIES[] = {
        "GB","NZ","DE","FR","JP","IT","ES","PT","NL","BE","CH","AT",
        "SE","NO","DK","FI","PL","CZ","HU","RO","GR","IE","IS",
        "ZA","NG","KE","GH","EG","TN","MA",
        "AE","SA","IL","TR","IR","JO","KW","QA",
        "KR","SG","TW","NP","FJ",
        "MX","CU","BR","AR","CL","CO","PE","VE","EC",
        nullptr
    };

    bool single_file = false;
    for (int si = 0; SINGLE_FILE_COUNTRIES[si]; si++) {
        if (strcmp(cc, SINGLE_FILE_COUNTRIES[si]) == 0) {
            single_file = true; break;
        }
    }
    if (single_file) return;

    const char** suffix_list = GENERIC_SUFFIXES;
    if (strcmp(cc, "CA") == 0) suffix_list = CA_SUFFIXES;
    else if (strcmp(cc, "US") == 0) suffix_list = US_SUFFIXES;
    else if (strcmp(cc, "AU") == 0) suffix_list = AU_SUFFIXES;

    for (int si = 0; suffix_list[si] != nullptr; si++) {
        std::string path = "FREQMAN/";
        path += cc;
        path += "_";
        path += suffix_list[si];
        path += ".txt";

        File f;
        if (f.open(path).is_ok()) {
            FreqFile ff;
            ff.filename = std::string(cc) + "_" + suffix_list[si] + ".txt";
            ff.display  = file_display_name(ff.filename, cc);
            ff.loaded   = false;
            country.files.push_back(std::move(ff));
        }
    }
}

std::string WorldRadioView::file_display_name(const std::string& filename, const char* cc) {
    const size_t cc_len = strlen(cc);
    if (filename.size() <= cc_len + 5) return filename;
    std::string suffix = filename.substr(cc_len + 1, filename.size() - cc_len - 5);

    struct SuffixMap { const char* code; const char* name; };

    static const SuffixMap CA_NAMES[] = {
        {"AB","Alberta"},{"BC","British Columbia"},{"MB","Manitoba"},
        {"NB","New Brunswick"},{"NL","Newfoundland & Lab."},
        {"NS","Nova Scotia"},{"NT","NW Territories"},{"NU","Nunavut"},
        {"ON","Ontario"},{"PE","Prince Edward Is."},{"QC","Quebec"},
        {"SK","Saskatchewan"},{"YT","Yukon"},{nullptr,nullptr}
    };
    static const SuffixMap US_NAMES[] = {
        {"W","West"},{"E","East"},{"MW","Midwest"},
        {"SE","Southeast"},{"NE","Northeast"},{"SW","Southwest"},
        {"OTHER","Other (Territories)"},{nullptr,nullptr}
    };
    static const SuffixMap AU_NAMES[] = {
        {"NSW","New South Wales"},{"VIC","Victoria"},{"QLD","Queensland"},
        {"WA","Western Australia"},{"SA","South Australia"},
        {"TAS","Tasmania"},{"ACT","ACT"},{"NT","Northern Territory"},
        {nullptr,nullptr}
    };

    const SuffixMap* map = nullptr;
    if (strcmp(cc,"CA") == 0) map = CA_NAMES;
    else if (strcmp(cc,"US") == 0) map = US_NAMES;
    else if (strcmp(cc,"AU") == 0) map = AU_NAMES;

    if (map) {
        for (int i = 0; map[i].code; i++) {
            if (suffix == map[i].code) return map[i].name;
        }
    }
    return suffix;
}

bool WorldRadioView::load_file(int arr_idx, int file_idx) {
    auto& ff = countries_[arr_idx].files[file_idx];
    ff.stations.clear();

    std::string path = "FREQMAN/" + ff.filename;
    File f;
    if (f.open(path).is_error()) {
        ff.loaded = true;
        return false;
    }

    std::string line;
    char ch;
    while (f.read(&ch, 1).is_ok()) {
        if (ch == '\n' || ch == '\r') {
            if (!line.empty()) {
                parse_line(line, ff.stations);
                line.clear();
            }
        } else {
            line += ch;
        }
    }
    if (!line.empty()) parse_line(line, ff.stations);

    std::sort(ff.stations.begin(), ff.stations.end(),
              [](const Station& a, const Station& b){ return a.freq_hz < b.freq_hz; });

    ff.loaded = true;
    return true;
}

void WorldRadioView::parse_line(const std::string& line, std::vector<Station>& out) {
    if (line.empty() || line[0] == '#') return;

    rf::Frequency freq = 0;
    std::string mod_str;
    std::string desc;

    size_t pos = 0;
    while (pos < line.size()) {
        size_t eq = line.find('=', pos);
        if (eq == std::string::npos) break;

        std::string key = line.substr(pos, eq - pos);
        while (!key.empty() && key[0] == ' ') key = key.substr(1);

        size_t val_start = eq + 1;
        size_t val_end;

        if (key == "d") {
            val_end = line.size();
            pos = val_end;
        } else {
            val_end = line.find(',', val_start);
            if (val_end == std::string::npos) val_end = line.size();
            pos = val_end + 1;
        }

        std::string val = line.substr(val_start, val_end - val_start);

        if (key == "f") {
            try { freq = (rf::Frequency)std::stoll(val); } catch (...) {}
        } else if (key == "m") {
            mod_str = val;
        } else if (key == "d") {
            desc = val;
        }
    }

    if (freq == 0 || desc.empty()) return;
    if (freq < 1000000ULL || freq > 6000000000ULL) return;

    Station st;
    st.freq_hz  = freq;
    st.is_am    = (mod_str == "AM");
    st.label    = desc;

    auto dash = desc.find('-');
    if (dash != std::string::npos && dash < 10) {
        st.callsign = desc.substr(0, dash);
    } else {
        auto sp = desc.find(' ');
        st.callsign = (sp != std::string::npos) ? desc.substr(0, sp) : desc;
    }
    if (st.callsign.size() > 8) st.callsign = st.callsign.substr(0, 8);

    out.push_back(std::move(st));
}

void WorldRadioView::select_station(int idx) {
    if (idx < 0 || idx >= (int)current_stations_.size()) return;
    sel_ = idx;
    const auto& st = current_stations_[idx];
    big_freq.set(st.freq_hz);

    char buf[32];
    if (st.is_am)
        snprintf(buf, sizeof(buf), "%s  %.0f kHz AM",
                 st.callsign.c_str(), (double)st.freq_hz / 1000.0);
    else
        snprintf(buf, sizeof(buf), "%s  %.1f MHz FM",
                 st.callsign.c_str(), (double)st.freq_hz / 1000000.0);
    set_status(buf);
    rebuild_station_rows();
}

void WorldRadioView::rebuild_station_rows() {
    int max_scroll = std::max(0, (int)current_stations_.size() - ROWS);
    if (scroll_ > max_scroll) scroll_ = max_scroll;

    for (int i = 0; i < ROWS; i++) {
        int si = scroll_ + i;
        if (si < (int)current_stations_.size()) {
            const auto& st = current_stations_[si];
            char buf[31];
            if (st.is_am) {
                snprintf(buf, sizeof(buf), "%7.0fk %-9s AM",
                         (double)st.freq_hz / 1000.0,
                         st.callsign.c_str());
            } else {
                snprintf(buf, sizeof(buf), "%7.1fM %-9s FM",
                         (double)st.freq_hz / 1000000.0,
                         st.callsign.c_str());
            }
            std::string s(buf);
            while ((int)s.size() < 30) s += ' ';
            if (si == sel_) s[0] = '>';
            row_btns[i].set_text(s);
            row_btns[i].set_enabled(true);
        } else {
            row_btns[i].set_text(std::string(30, ' '));
            row_btns[i].set_enabled(false);
        }
    }

    char cnt[20];
    int total = (int)current_stations_.size();
    if (sel_ >= 0)
        snprintf(cnt, sizeof(cnt), " #%d / %d", sel_ + 1, total);
    else
        snprintf(cnt, sizeof(cnt), " %d stations", total);
    txt_cnt.set(cnt);

    btn_up.set_enabled(scroll_ > 0);
    btn_dn.set_enabled(scroll_ < max_scroll);
}

void WorldRadioView::tune() {
    if (sel_ < 0 || sel_ >= (int)current_stations_.size()) {
        set_status("Select a station first");
        return;
    }
    const auto& st = current_stations_[sel_];

    receiver_model.disable();
    receiver_model.set_tuning_frequency(st.freq_hz);
    receiver_model.set_sampling_rate(3072000);
    receiver_model.set_baseband_bandwidth(1750000);

    if (st.is_am) {
        receiver_model.set_modulation(ReceiverModel::Mode::AMAudio);
        receiver_model.set_am_configuration(0);
    } else {
        receiver_model.set_modulation(ReceiverModel::Mode::WidebandFMAudio);
        receiver_model.set_wfm_configuration(0);
    }

    receiver_model.enable();

    char buf[32];
    if (st.is_am)
        snprintf(buf, sizeof(buf), "Tuned: %s %.0f kHz",
                 st.callsign.c_str(), (double)st.freq_hz / 1000.0);
    else
        snprintf(buf, sizeof(buf), "Tuned: %s %.1f MHz",
                 st.callsign.c_str(), (double)st.freq_hz / 1000000.0);
    set_status(buf);
}

void WorldRadioView::set_status(const std::string& s) {
    std::string d = s;
    if (d.size() > 29) d = d.substr(0, 29);
    while ((int)d.size() < 29) d += ' ';
    txt_status.set(d);
}

}  // namespace ui::external_app::world_radio
