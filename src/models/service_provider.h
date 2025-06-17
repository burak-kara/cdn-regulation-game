#ifndef SERVICE_PROVIDER_H
#define SERVICE_PROVIDER_H

#include <memory>
#include <vector>

#include "../tools/types.h"

class ServiceProvider {
    Config *config;

    float max_demand = 0;
    float demand = 0;
    float demand_degradation = 0;
    float allocation = 0;
    float cost = 0;
    float price = 0;
    float revenue_margin = 0;
    float precision = 0;

    std::vector<std::shared_ptr<Cdn>> cdns;
    std::vector<std::shared_ptr<Region>> regions;

    void add_cdn(const std::shared_ptr<Cdn> &cdn);

    void add_region(const std::shared_ptr<Region> &region);

    void sort_cdns();

    [[nodiscard]] FloatTuple find_allocation_and_cost(const std::shared_ptr<Cdn> &cdn,
                                                      const std::shared_ptr<Region> &region);

    [[nodiscard]] float find_opt_allocation(float start, float end, float cdn_price);

    void fill_allocation(float allocation, float cost);

public:
    explicit ServiceProvider(Config *config);

    ServiceProvider(const ServiceProvider &sp);

    [[nodiscard]] float get_demand() const;

    [[nodiscard]] float get_allocation() const;

    [[nodiscard]] float get_cost() const;

    [[nodiscard]] float get_price() const;

    [[nodiscard]] float get_remaining_demand() const;

    [[nodiscard]] float get_revenue() const;

    [[nodiscard]] float get_utility() const;

    [[nodiscard]] const std::vector<std::shared_ptr<Cdn>> &get_cdns() const;

    [[nodiscard]] const std::vector<std::shared_ptr<Region>> &get_regions() const;

    [[nodiscard]] const Region *find_region(const std::string &name) const;

    [[nodiscard]] const Cdn *find_cdn(const std::string &name) const;

    void set_max_demand(float max_demand);

    void set_allocation(float allocation);

    void set_demand_degradation(float demand_degradation);

    void set_cost(float cost);

    void set_price(float price);

    void set_config(Config *config);

    void reset();

    void add_cdns(const std::vector<std::shared_ptr<Cdn>> &cdns);

    void add_regions(const std::vector<std::shared_ptr<Region>> &regions);

    void update_demand();

    void allocate();
};

#endif // SERVICE_PROVIDER_H
