#include <algorithm>
#include <iostream>
#include <stdexcept>

#include "../lib/config.h"
#include "cdn.h"
#include "region.h"
#include "service_provider.h"

ServiceProvider::ServiceProvider(Config *config) : config(config) {
    if (config)
        this->precision = config->get_f("precision");
}

ServiceProvider::ServiceProvider(const ServiceProvider &sp) {
    this->config = nullptr;
    this->max_demand = sp.max_demand;
    this->demand = sp.demand;
    this->demand_degradation = sp.demand_degradation;
    this->allocation = sp.allocation;
    this->cost = sp.cost;
    this->price = sp.price;
    this->revenue_margin = sp.revenue_margin;
    this->cdns = {};
    this->regions = {};
}

[[nodiscard]] float ServiceProvider::get_demand() const { return this->demand; }

[[nodiscard]] float ServiceProvider::get_allocation() const { return this->allocation; }

[[nodiscard]] float ServiceProvider::get_cost() const { return this->cost; }

[[nodiscard]] float ServiceProvider::get_price() const { return this->price; }

[[nodiscard]] float ServiceProvider::get_remaining_demand() const { return this->demand - this->allocation; }

[[nodiscard]] float ServiceProvider::get_revenue() const { return this->allocation * this->price; }

[[nodiscard]] float ServiceProvider::get_utility() const { return this->get_revenue() - this->get_cost(); }

[[nodiscard]] const std::vector<std::shared_ptr<Cdn>> &ServiceProvider::get_cdns() const { return this->cdns; }

[[nodiscard]] const std::vector<std::shared_ptr<Region>> &ServiceProvider::get_regions() const { return this->regions; }

[[nodiscard]] const Region *ServiceProvider::find_region(const std::string &name) const {
    for (const auto &region: this->regions) {
        if (region->get_name() == name) {
            return region.get();
        }
    }
    throw std::runtime_error("Region not found");
}

[[nodiscard]] const Cdn *ServiceProvider::find_cdn(const std::string &name) const {
    for (const auto &cdn: this->cdns) {
        if (cdn->get_name() == name) {
            return cdn.get();
        }
    }
    throw std::runtime_error("CDN not found");
}

void ServiceProvider::set_max_demand(const float max_demand) { this->max_demand = max_demand; }

void ServiceProvider::set_allocation(const float allocation) { this->allocation = allocation; }

void ServiceProvider::set_demand_degradation(const float demand_degradation) {
    this->demand_degradation = demand_degradation;
}

void ServiceProvider::set_cost(const float cost) { this->cost = cost; }

void ServiceProvider::set_price(const float price) { this->price = price; }

void ServiceProvider::set_config(Config *config) { this->config = config; }

void ServiceProvider::reset() {
    this->demand = 0;
    this->allocation = 0;
    this->cost = 0;
}

void ServiceProvider::add_cdns(const std::vector<std::shared_ptr<Cdn>> &cdns) {
    for (const auto &cdn: cdns) {
        this->add_cdn(cdn);
    }
}

void ServiceProvider::add_cdn(const std::shared_ptr<Cdn> &cdn) { this->cdns.push_back(cdn); }

void ServiceProvider::add_regions(const std::vector<std::shared_ptr<Region>> &regions) {
    for (const auto &region: regions) {
        this->add_region(region);
    }
}

void ServiceProvider::add_region(const std::shared_ptr<Region> &region) { this->regions.push_back(region); }

void ServiceProvider::update_demand() {
    this->demand = this->max_demand - this->price * this->demand_degradation;
    if (this->demand < 0)
        this->demand = 0;

    for (const auto &region: this->regions) {
        region->set_demand(this->demand / static_cast<float>(this->regions.size()));
    }
}

void ServiceProvider::allocate() {
    if (this->demand <= 0)
        return;
    if (this->regions.empty())
        return;
    if (this->cdns.empty())
        return;

    this->sort_cdns();

    for (const auto &cdn: this->cdns) {
        for (const auto &region: cdn->get_regions()) {
            if (region->is_allocatable()) {
                const auto [allocation, cost] = this->find_allocation_and_cost(cdn, region);
                this->fill_allocation(allocation, cost);
                cdn->allocate(region, allocation);
            }
        }
    }
}

void ServiceProvider::sort_cdns() {
    std::ranges::sort(this->cdns,
                      [](const auto &cdn1, const auto &cdn2) { return cdn1->get_price() < cdn2->get_price(); });
}

[[nodiscard]] FloatTuple ServiceProvider::find_allocation_and_cost(const std::shared_ptr<Cdn> &cdn,
                                                                   const std::shared_ptr<Region> &region) {
    float allocation = std::min(cdn->get_remaining_capacity(), region->get_remaining_demand());
    float pay_to_cdn = allocation * cdn->get_price();

    const float possible_cost = this->cost + this->revenue_margin + pay_to_cdn;
    const float possible_revenue = (allocation + this->allocation) * this->price;

    if (possible_cost > possible_revenue) {
        allocation = this->find_opt_allocation(0, allocation, cdn->get_price());
        pay_to_cdn = allocation * cdn->get_price();
    }
    return std::make_tuple(allocation, pay_to_cdn);
}

[[nodiscard]] float ServiceProvider::find_opt_allocation(const float start, // NOLINT(*-no-recursion)
                                                         const float end, const float cdn_price) {
    const float mid_allocation = (start + end) / 2;

    if (mid_allocation == 0)
        return 0;
    if (mid_allocation == start || mid_allocation == end || end <= start || end - start <= this->precision)
        return mid_allocation;

    const float temp_pay_to_cdn = mid_allocation * cdn_price;
    const float possible_cost = this->cost + this->revenue_margin + temp_pay_to_cdn;
    const float possible_revenue = (mid_allocation + this->allocation) * this->price;
    // ReSharper disable once CppTooWideScopeInitStatement
    const float possible_utility = possible_revenue - possible_cost;

    if (possible_utility > 0) {
        return find_opt_allocation(mid_allocation, end, cdn_price);
    }
    return find_opt_allocation(start, mid_allocation, cdn_price);
}

void ServiceProvider::fill_allocation(const float allocation, const float cost) {
    this->allocation += allocation;
    this->cost += cost;
}
