#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

#include "../enums/game_mode.h"
#include "../enums/tax_mode.h"
#include "../tools/string.h"
#include "../tools/types.h"
#include "../tools/vector.h"
#include "config.h"

Config *Config::pinstance{nullptr};
std::mutex Config::mutex;

Config *Config::get_instance() {
    std::lock_guard lock(mutex);
    if (pinstance == nullptr) {
        pinstance = new Config();
    }
    return pinstance;
}

void Config::load(const std::string &filename) {
    this->filename = filename;

    std::ifstream file(this->filename);
    std::string line;

    if (!file.is_open()) {
        throw std::runtime_error("Unable to open configuration file: " + filename);
    }

    while (std::getline(file, line)) {
        std::istringstream line_stream(line);
        std::string key, value;

        if (std::getline(line_stream, key, '=') && std::getline(line_stream, value)) {
            String::clean(key);
            String::clean(value);

            if (value.front() == '"' && value.back() == '"') {
                this->map[key] = value.substr(1, value.size() - 2);
            } else if (value.find('.') != std::string::npos || std::ranges::all_of(value, isdigit)) {
                this->map[key] = std::stof(value);
            } else {
                this->map[key] = value;
            }
        }
    }
}

void Config::load_game(const GameMode &game_mode, const std::string &cdn1_id, const std::string &cdn2_id) {
    this->configure_cdn_names(cdn1_id, cdn2_id);
    this->configure_outputs(game_mode);
    this->configure_prices();
    this->configure_servers();
    this->configure_taxes();
}

void Config::configure_cdn_names(const std::string &cdn1_id, const std::string &cdn2_id) {
    this->set("cdn1", cdn1_id);
    this->set("cdn2", cdn2_id);
}

void Config::configure_outputs(const GameMode &game_mode) {
    const std::string cdn2_id = this->get_s("cdn2");
    std::string file_name;

    if (game_mode == GameMode::BR) {
        file_name = this->get_s("out" + String::capitalize(cdn2_id) + "File");
    } else if (game_mode == GameMode::OptimalCDN2) {
        file_name = this->get_s("outOptimal" + String::capitalize(cdn2_id) + "File");
    } else if (game_mode == GameMode::OptimalSP) {
        file_name = this->get_s("outOptimalSPFile");
    }

    this->set("outputFileName", file_name);
    this->set("outputFile", file_name + ".txt");
    this->set("fileSearchPattern", file_name + "_(\\d+)\\.txt");
}

void Config::configure_prices() {
    this->set("spPrices", Vector::generate_range<float>(this->get_f("minSPUnitPrice"), this->get_f("maxSPUnitPrice"),
                                                        this->get_f("spStep")));
    this->set("cdn1Prices", Vector::generate_range<float>(this->get_f("minCdn1UnitPrice"),
                                                          this->get_f("maxCdn1UnitPrice"), this->get_f("cdn1Step")));
    this->set("cdn2Prices", Vector::generate_range<float>(this->get_f("minCdn2UnitPrice"),
                                                          this->get_f("maxCdn2UnitPrice"), this->get_f("cdn2Step")));
}

void Config::configure_servers() {
    if (const TaxMode tax_mode = StringToTaxMode.at(this->get_s("taxMode")); tax_mode == TaxMode::None) {
        this->set("cdn1GreenServers", std::vector<float>{0});
        this->set("cdn2GreenServers", std::vector<float>{0});
    } else {
        this->set("cdn1GreenServers", Vector::generate_range<float>(this->get_f("minCdn1GreenServers"),
                                                                    this->get_f("maxCdn1GreenServers"), 1));
        this->set("cdn2GreenServers", Vector::generate_range<float>(this->get_f("minCdn2GreenServers"),
                                                                    this->get_f("maxCdn2GreenServers"), 1));
    }
}

void Config::configure_taxes() {
    if (const TaxMode tax_mode = StringToTaxMode.at(this->get_s("taxMode")); tax_mode == TaxMode::Allocation) {
        this->set("taxRates",
                  Vector::generate_range<float>(this->get_f("minAllocationTaxRate"),
                                                this->get_f("maxAllocationTaxRate"), this->get_f("allocationTaxStep")));
    } else if (tax_mode == TaxMode::Server) {
        this->set("taxRates", Vector::generate_range<float>(this->get_f("minDirtyServerTaxRate"),
                                                            this->get_f("maxDirtyServerTaxRate"),
                                                            this->get_f("dirtyServerTaxStep")));
    } else if (tax_mode == TaxMode::None) {
        this->set("taxRates", std::vector<float>{0});
    }
}

[[nodiscard]] const ConfigMap &Config::get() const { return this->map; }

[[nodiscard]] const VariantT &Config::get(const std::string &key) {
    try {
        return this->map[key];
    } catch ([[maybe_unused]] const std::out_of_range &e) {
        throw std::runtime_error("Failed to get value from key: " + key);
    }
}

[[nodiscard]] const float &Config::get_f(const std::string &key) {
    try {
        return std::get<float>(this->map[key]);
    } catch ([[maybe_unused]] const std::bad_variant_access &e) {
        throw std::runtime_error("Failed to get float from key: " + key);
    }
}

[[nodiscard]] int Config::get_i(const std::string &key) {
    try {
        return static_cast<int>(std::get<float>(this->map[key]));
    } catch ([[maybe_unused]] const std::bad_variant_access &e) {
        throw std::runtime_error("Failed to get integer from key: " + key);
    }
}

[[nodiscard]] const std::string &Config::get_s(const std::string &key) {
    try {
        return std::get<std::string>(this->map[key]);
    } catch ([[maybe_unused]] const std::bad_variant_access &e) {
        throw std::runtime_error("Failed to get string from key: " + key);
    }
}

[[nodiscard]] const std::vector<float> &Config::get_vf(const std::string &key) {
    try {
        return std::get<std::vector<float>>(this->map[key]);
    } catch ([[maybe_unused]] const std::bad_variant_access &e) {
        throw std::runtime_error("Failed to get vector<float> from key: " + key);
    }
}

void Config::set(const std::string &key, const VariantT &value) { this->map[key] = value; }

void Config::export_to_file() {
    std::ofstream file(this->get_s("outputFolder") + "/parameters.tex");
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open parameters.tex file");
    }

    for (const auto &[key, value]: this->map) {
        std::string new_key = String::replace_substring(key, "1", "One");
        new_key = String::replace_substring(new_key, "2", "Two");
        if (std::holds_alternative<float>(value)) {
            file << "\\newcommand{\\" << new_key << "}{" << std::get<float>(value) << "}\n";
        } else if (std::holds_alternative<std::string>(value)) {
            file << "\\newcommand{\\" << new_key << "}{" << std::get<std::string>(value) << "}\n";
        }
    }
    file.close();
}
