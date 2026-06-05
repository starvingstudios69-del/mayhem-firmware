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

struct Country {
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
    {"CA", "Canada",               0, {}, false},
    {"US", "United States",        0, {}, false},
    {"MX", "Mexico",               0, {}, false},
    {"CU", "Cuba",                 0, {}, false},
    {"GT", "Guatemala",            0, {}, false},
    {"HN", "Honduras",             0, {}, false},
    {"CR", "Costa Rica",           0, {}, false},
    {"PA", "Panama",               0, {}, false},
    {"JM", "Jamaica",              0, {}, false},
    {"TT", "Trinidad & Tobago",    0, {}, false},
    {"DO", "Dominican Republic",   0, {}, false},
    {"PR", "Puerto Rico",          0, {}, false},
    {"BR", "Brazil",               1, {}, false},
    {"AR", "Argentina",            1, {}, false},
    {"CL", "Chile",                1, {}, false},
    {"CO", "Colombia",             1, {}, false},
    {"PE", "Peru",                 1, {}, false},
    {"VE", "Venezuela",            1, {}, false},
    {"EC", "Ecuador",              1, {}, false},
    {"BO", "Bolivia",              1, {}, false},
    {"PY", "Paraguay",             1, {}, false},
    {"UY", "Uruguay",              1, {}, false},
    {"GB", "United Kingdom",       2, {}, false},
    {"DE", "Germany",              2, {}, false},
    {"FR", "France",               2, {}, false},
    {"IT", "Italy",                2, {}, false},
    {"ES", "Spain",                2, {}, false},
    {"PT", "Portugal",             2, {}, false},
    {"NL", "Netherlands",          2, {}, false},
    {"BE", "Belgium",              2, {}, false},
    {"CH", "Switzerland",          2, {}, false},
    {"AT", "Austria",              2, {}, false},
    {"SE", "Sweden",               2, {}, false},
    {"NO", "Norway",               2, {}, false},
    {"DK", "Denmark",              2, {}, false},
    {"FI", "Finland",              2, {}, false},
    {"PL", "Poland",               2, {}, false},
    {"CZ", "Czech Republic",       2, {}, false},
    {"HU", "Hungary",              2, {}, false},
    {"RO", "Romania",              2, {}, false},
    {"GR", "Greece",               2, {}, false},
    {"IE", "Ireland",              2, {}, false},
    {"IS", "Iceland",              2, {}, false},
    {"JP", "Japan",                3, {}, false},
    {"AU", "Australia",            3, {}, false},
    {"NZ", "New Zealand",          3, {}, false},
    {"KR", "South Korea",          3, {}, false},
    {"CN", "China",                3, {}, false},
    {"IN", "India",                3, {}, false},
    {"ID", "Indonesia",            3, {}, false},
    {"TH", "Thailand",             3, {}, false},
    {"PH", "Philippines",          3, {}, false},
    {"MY", "Malaysia",             3, {}, false},
    {"SG", "Singapore",            3, {}, false},
    {"TW", "Taiwan",               3, {}, false},
    {"VN", "Vietnam",              3, {}, false},
    {"PK", "Pakistan",             3, {}, false},
    {"BD", "Bangladesh",           3, {}, false},
    {"LK", "Sri Lanka",            3, {}, false},
    {"NP", "Nepal",                3, {}, false},
    {"FJ", "Fiji",                 3, {}, false},
    {"AE", "UAE",                  4, {}, false},
    {"SA", "Saudi Arabia",         4, {}, false},
    {"IL", "Israel",               4, {}, false},
    {"TR", "Turkey",               4, {}, false},
    {"IR", "Iran",                 4, {}, false},
    {"IQ", "Iraq",                 4, {}, false},
    {"JO", "Jordan",               4, {}, false},
    {"LB", "Lebanon",              4, {}, false},
    {"KW", "Kuwait",               4, {}, false},
    {"QA", "Qatar",                4, {}, false},
    {"ZA", "South Africa",         5, {}, false},
    {"NG", "Nigeria",              5, {}, false},
    {"KE", "Kenya",                5, {}, false},
    {"GH", "Ghana",                5, {}, false},
    {"ET", "Ethiopia",             5, {}, false},
    {"EG", "Egypt",                5, {}, false},
    {"TZ", "Tanzania",             5, {}, false},
    {"UG", "Uganda",               5, {}, false},
    {"ZW", "Zimbabwe",             5, {}, false},
    {"ZM", "Zambia",               5, {}, false},
    {"TN", "Tunisia",              5, {}, false},
    {"MA", "Morocco",              5, {}, false},
};
static constexpr int NUM_COUNTRIES = sizeof(COUNTRY_TABLE) / sizeof(COUNTRY_TABLE[0]);

class WorldRadioView : public View {
   public:
    explicit WorldRadioView(NavigationView& nav);
    ~WorldRadioView();

    void focus() override;
    std::string title() const override { return "World Radio"; }

   private:
    NavigationView& nav_;

    std::array<Country, NUM_COUNTRIES> countries_{};

    int  region_idx_  {0};
    int  country_idx_ {0};
    int  file_idx_    {0};
    int  scroll_      {0};
    int  sel_         {-1};

    std::vector<int> region_countries_{};
    std::vector<Station> current_stations_{};

    void     scan_country_files(int country_arr_idx);
    bool     load_file(int country_arr_idx, int file_idx);
    void     parse_line(const std::string& line, std::vector<Station>& out);
    std::string file_display_name(const std::string& filename, const char* cc);

    void on_region_changed(size_t idx);
    void on_country_changed(size_t idx);
    void on_file_changed(size_t idx);
    void select_station(int idx);
    void rebuild_region_opts();
    void rebuild_country_opts();
    void rebuild_file_opts();
    void rebuild_station_rows();
    void tune();
    void set_status(const std::string& s);
    int  current_country_arr_idx() const;

    Labels lbl_region{{{ 0, 0*16}, "REGION: ", Color::light_grey()}};
    OptionsField opt_region{{8*8, 0*16}, 21, {{"Loading...", 0}}};

    Labels lbl_country{{{ 0, 1*16}, "COUNTRY:", Color::light_grey()}};
    OptionsField opt_country{{8*8+8, 1*16}, 20, {{"--", 0}}};

    Labels lbl_file{{{ 0, 2*16}, "FILE:   ", Color::light_grey()}};
    OptionsField opt_file{{8*8, 2*16}, 21, {{"--", 0}}};

    Labels lbl_hdr{{
        {0, 3*16}, "  FREQ       CALL      MOD", Color::dark_grey()
    }};

    static constexpr int ROWS = 12;
    std::array<Button, ROWS> row_btns{{
        Button{{ 0,  4*16, 30*8, 16}, ""},
        Button{{ 0,  5*16, 30*8, 16}, ""},
        Button{{ 0,  6*16, 30*8, 16}, ""},
        Button{{ 0,  7*16, 30*8, 16}, ""},
        Button{{ 0,  8*16, 30*8, 16}, ""},
        Button{{ 0,  9*16, 30*8, 16}, ""},
        Button{{ 0, 10*16, 30*8, 16}, ""},
        Button{{ 0, 11*16, 30*8, 16}, ""},
        Button{{ 0, 12*16, 30*8, 16}, ""},
        Button{{ 0, 13*16, 30*8, 16}, ""},
        Button{{ 0, 14*16, 30*8, 16}, ""},
        Button{{ 0, 15*16, 30*8, 16}, ""},
    }};

    Button btn_up {{ 0*8, 16*16,  4*8, 16}, " /\\ "};
    Button btn_dn {{ 4*8, 16*16,  4*8, 16}, " \\/ "};
    Text   txt_cnt{{ 9*8, 16*16, 21*8, 16}, ""};

    BigFrequency big_freq{{0, 17*16, 28*8, 32}, 0};

    Text txt_status{{0, 18*16+16, 30*8, 16}, "Select a region"};

    Button btn_tune{{  0, 19*16+16, 12*8, 16}, "  TUNE  "};
    Button btn_info{{12*8+4, 19*16+16,  8*8, 16}, " INFO "};
    Button btn_exit{{22*8, 19*16+16,  8*8, 16}, " EXIT "};
};

}  // namespace ui::external_app::world_radio

#endif  /* __UI_WORLD_RADIO_H__ */
