#include <iostream>
#include <stdexcept>
#include <utility>

#include "cdn.h"
#include "region.h"

Region::Region(Config *config, std::string name) : config(config), name(std::move(name)) {}

Region::Region(const Region &region) {
    this->config = nullptr;
    this->name = region.name;
    this->cdn_count = region.cdn_count;
    this->demand = region.demand;
    this->allocation = region.allocation;
    this->cdns = {};
}

[[nodiscard]] const std::string &Region::get_name() const { return this->name; }

[[nodiscard]] int Region::get_cdn_count() const { return this->cdn_count; }

[[nodiscard]] float Region::get_demand() const { return this->demand; }

[[nodiscard]] float Region::get_allocation() const { return this->allocation; }

[[nodiscard]] float Region::get_remaining_demand() const { return this->demand - this->allocation; }

[[nodiscard]] const std::vector<std::shared_ptr<Cdn>> &Region::get_cdns() const { return this->cdns; }

[[nodiscard]] const Cdn *Region::find_cdn(const std::string &name) const {
    for (const auto &cdn: this->cdns) {
        if (cdn->get_name() == name) {
            return cdn.get();
        }
    }
    throw std::runtime_error("CDN not found");
}

void Region::set_demand(const float demand) { this->demand = demand; }

void Region::set_name(const std::string &name) { this->name = name; }

void Region::set_config(Config *config) { this->config = config; }

void Region::allocate(const float allocation) { this->allocation += allocation; }

void Region::reset() {
    this->demand = 0;
    this->allocation = 0;
}

bool Region::is_allocatable() const { return get_remaining_demand() > 0; }

bool Region::is_monopoly() const { return this->cdn_count == 1; }

void Region::add_cdn(const std::shared_ptr<Cdn> &cdn) {
    this->cdns.push_back(cdn);
    ++this->cdn_count;
}
