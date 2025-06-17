#include <cmath>
#include <ranges>
#include <sstream>
#include <stdexcept>
#include <utility>

#include "cdn.h"
#include "region.h"

Cdn::Cdn(Config *config, std::string name, const float capacity) :
    config(config), name(std::move(name)), capacity(capacity) {}

Cdn::Cdn(const Cdn &cdn) : enable_shared_from_this(cdn) {
    this->config = nullptr;
    this->name = cdn.name;
    this->price = cdn.price;
    this->capacity = cdn.capacity;
    this->allocation = cdn.allocation;
    this->revenue = cdn.revenue;
    this->cost = cdn.cost;

    this->dirty_energy_unit_cost = cdn.dirty_energy_unit_cost;
    this->green_energy_unit_cost = cdn.green_energy_unit_cost;
    this->dirty_server_threshold = cdn.dirty_server_threshold;
    this->green_server_threshold = cdn.green_server_threshold;
    this->dirty_server_turnon_cost = cdn.dirty_server_turnon_cost;
    this->green_server_turnon_cost = cdn.green_server_turnon_cost;

    this->dirty_allocation = cdn.dirty_allocation;
    this->green_allocation = cdn.green_allocation;
    this->green_servers = cdn.green_servers;
    this->dirty_servers_in_use = cdn.dirty_servers_in_use;
    this->green_servers_in_use = cdn.green_servers_in_use;
    this->servers_in_use = cdn.servers_in_use;

    this->tax_rate = cdn.tax_rate;
    this->tax_mode = cdn.tax_mode;
    this->regions = {};
    this->region_allocations = cdn.region_allocations;
}

[[nodiscard]] const std::string &Cdn::get_name() const { return this->name; }

[[nodiscard]] float Cdn::get_price() const { return this->price; }

[[nodiscard]] float Cdn::get_capacity() const { return this->capacity; }

[[nodiscard]] float Cdn::get_allocation() const { return this->allocation; }

[[nodiscard]] float Cdn::get_dirty_energy_unit_cost() const { return this->dirty_energy_unit_cost; }

[[nodiscard]] float Cdn::get_green_energy_unit_cost() const { return this->green_energy_unit_cost; }

[[nodiscard]] float Cdn::get_dirty_server_threshold() const { return this->dirty_server_threshold; }

[[nodiscard]] float Cdn::get_green_server_threshold() const { return this->green_server_threshold; }

[[nodiscard]] float Cdn::get_dirty_server_turnon_cost() const { return this->dirty_server_turnon_cost; }

[[nodiscard]] float Cdn::get_green_server_turnon_cost() const { return this->green_server_turnon_cost; }

[[nodiscard]] float Cdn::get_dirty_allocation() const { return this->dirty_allocation; }

[[nodiscard]] float Cdn::get_green_allocation() const { return this->green_allocation; }

[[nodiscard]] std::string Cdn::get_region_allocations() const {
    std::stringstream log_stream;

    for (const auto &allocation: std::views::values(this->region_allocations)) {
        log_stream << allocation << " ";
    }
    std::string allocations = log_stream.str();

    if (!allocations.empty() && allocations.back() == ' ') {
        allocations.pop_back();
    }

    return allocations;
}

[[nodiscard]] int Cdn::get_green_servers() const { return this->green_servers; }

[[nodiscard]] int Cdn::get_dirty_servers_in_use() const { return this->dirty_servers_in_use; }

[[nodiscard]] int Cdn::get_green_servers_in_use() const { return this->green_servers_in_use; }

[[nodiscard]] int Cdn::get_servers_in_use() const { return this->dirty_servers_in_use + this->green_servers_in_use; }

[[nodiscard]] float Cdn::get_tax_rate() const { return this->tax_rate; }

[[nodiscard]] TaxMode Cdn::get_tax_mode() const { return this->tax_mode; }

[[nodiscard]] std::string Cdn::get_str_tax_mode() const { return TaxModeToString.at(this->tax_mode); }

[[nodiscard]] float Cdn::get_remaining_capacity() const { return this->capacity - this->allocation; }

[[nodiscard]] const std::vector<std::shared_ptr<Region>> &Cdn::get_regions() const { return this->regions; }

[[nodiscard]] const Region *Cdn::find_region(const std::string &name) const {
    for (const auto &region: this->regions) {
        if (region->get_name() == name) {
            return region.get();
        }
    }
    throw std::runtime_error("Region not found");
}

[[nodiscard]] float Cdn::get_utility() const { return this->get_revenue() - this->get_cost(); }

[[nodiscard]] float Cdn::get_revenue() const { return this->revenue; }

[[nodiscard]] float Cdn::get_cost() const { return this->cost; }

void Cdn::set_price(const float price) { this->price = price; }

void Cdn::set_capacity(const float capacity) { this->capacity = capacity; }

void Cdn::set_dirty_energy_unit_cost(const float dirty_energy_unit_cost) {
    this->dirty_energy_unit_cost = dirty_energy_unit_cost;
}

void Cdn::set_green_energy_unit_cost(const float green_energy_unit_cost) {
    this->green_energy_unit_cost = green_energy_unit_cost;
}

void Cdn::set_dirty_server_threshold(const float dirty_server_threshold) {
    this->dirty_server_threshold = dirty_server_threshold;
}

void Cdn::set_green_server_threshold(const float green_server_threshold) {
    this->green_server_threshold = green_server_threshold;
}

void Cdn::set_dirty_server_turnon_cost(const float dirty_server_turnon_cost) {
    this->dirty_server_turnon_cost = dirty_server_turnon_cost;
}

void Cdn::set_green_server_turnon_cost(const float green_server_turnon_cost) {
    this->green_server_turnon_cost = green_server_turnon_cost;
}

void Cdn::set_green_servers(const int green_servers) { this->green_servers = green_servers; }

void Cdn::set_tax_rate(const float tax_rate) {
    if (tax_rate < 0) {
        throw std::runtime_error("Tax rate must be greater than or equal to 0");
    }

    if (is_valid(this->tax_mode)) {
        this->tax_rate = tax_rate;
    } else {
        this->tax_rate = 0;
    }
}

void Cdn::set_tax_mode(const TaxMode tax_mode) { this->tax_mode = tax_mode; }

void Cdn::set_config(Config *config) { this->config = config; }

void Cdn::add_region(const std::shared_ptr<Region> &region) {
    this->regions.push_back(region);
    region->add_cdn(shared_from_this());
}

void Cdn::allocate(const std::shared_ptr<Region> &region, const float allocation) {
    this->allocation += allocation;
    region->allocate(allocation);

    this->calculate_revenue();
    this->calculate_cost();
}

void Cdn::calculate_revenue() { this->revenue = this->allocation * this->price; }

void Cdn::calculate_cost() {
    this->calculate_server_in_use();

    this->cost = this->calculate_dirty_cost() + this->calculate_green_cost();
    this->cost += this->calculate_tax_cost();
}

void Cdn::calculate_server_in_use() {
    this->green_servers_in_use =
            std::min(this->green_servers, static_cast<int>(std::ceil(this->allocation / this->green_server_threshold)));

    this->green_allocation =
            std::min(this->allocation, static_cast<float>(this->green_servers_in_use) * this->green_server_threshold);
    this->dirty_allocation = std::max(this->allocation - this->green_allocation, 0.0f);

    if (this->dirty_allocation <= 0) {
        this->dirty_servers_in_use = 0;
    } else {
        this->dirty_servers_in_use = std::ceil(this->dirty_allocation / this->dirty_server_threshold);
    }
}

[[nodiscard]] float Cdn::calculate_dirty_cost() const {
    return this->dirty_energy_unit_cost * this->dirty_allocation +
           static_cast<float>(this->dirty_servers_in_use) * this->dirty_server_turnon_cost;
}

[[nodiscard]] float Cdn::calculate_green_cost() const {
    return this->green_energy_unit_cost * this->green_allocation +
           static_cast<float>(this->green_servers_in_use) * this->green_server_turnon_cost;
}

[[nodiscard]] float Cdn::calculate_tax_cost() const {
    if (this->tax_mode == TaxMode::Server) {
        return static_cast<float>(this->dirty_servers_in_use) * this->tax_rate;
    }
    if (this->tax_mode == TaxMode::Allocation) {
        return this->dirty_allocation * this->tax_rate;
    }
    return 0;
}

void Cdn::reset() {
    this->allocation = 0;
    this->revenue = 0;
    this->cost = 0;
}
