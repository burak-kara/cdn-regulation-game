#ifndef CDN_H
#define CDN_H

#include <memory>
#include <string>

#include "../enums/tax_mode.h"
#include "../headers/config.fwd.h"
#include "../headers/region.fwd.h"
#include "../tools/types.h"

class Cdn : public std::enable_shared_from_this<Cdn> {
    Config *config;
    std::string name;

    float price = 0;
    float capacity = 0;
    float allocation = 0;
    float revenue = 0;
    float cost = 0;

    float dirty_energy_unit_cost = 0;
    float green_energy_unit_cost = 0;
    float dirty_server_threshold = 0;
    float green_server_threshold = 0;
    float dirty_server_turnon_cost = 0;
    float green_server_turnon_cost = 0;

    float dirty_allocation = 0;
    float green_allocation = 0;
    int green_servers = 0;
    int dirty_servers_in_use = 0;
    int green_servers_in_use = 0;
    int servers_in_use = 0;

    float tax_rate = 0;
    TaxMode tax_mode = TaxMode::Server;

    std::vector<std::shared_ptr<Region>> regions;
    std::map<std::string, float> region_allocations;

    void calculate_revenue();

    void calculate_cost();

    void calculate_server_in_use();

    [[nodiscard]] float calculate_dirty_cost() const;

    [[nodiscard]] float calculate_green_cost() const;

    [[nodiscard]] float calculate_tax_cost() const;

public:
    Cdn(Config *config, std::string name, float capacity);

    Cdn(const Cdn &cdn);

    [[nodiscard]] const std::string &get_name() const;

    [[nodiscard]] float get_price() const;

    [[nodiscard]] float get_capacity() const;

    [[nodiscard]] float get_allocation() const;

    [[nodiscard]] float get_dirty_energy_unit_cost() const;

    [[nodiscard]] float get_green_energy_unit_cost() const;

    [[nodiscard]] float get_dirty_server_threshold() const;

    [[nodiscard]] float get_green_server_threshold() const;

    [[nodiscard]] float get_dirty_server_turnon_cost() const;

    [[nodiscard]] float get_green_server_turnon_cost() const;

    [[nodiscard]] float get_dirty_allocation() const;

    [[nodiscard]] float get_green_allocation() const;

    [[nodiscard]] std::string get_region_allocations() const;

    [[nodiscard]] int get_green_servers() const;

    [[nodiscard]] int get_dirty_servers_in_use() const;

    [[nodiscard]] int get_green_servers_in_use() const;

    [[nodiscard]] int get_servers_in_use() const;

    [[nodiscard]] float get_tax_rate() const;

    [[nodiscard]] TaxMode get_tax_mode() const;

    [[nodiscard]] std::string get_str_tax_mode() const;

    [[nodiscard]] float get_remaining_capacity() const;

    [[nodiscard]] const std::vector<std::shared_ptr<Region>> &get_regions() const;

    [[nodiscard]] const Region *find_region(const std::string &name) const;

    [[nodiscard]] float get_utility() const;

    [[nodiscard]] float get_revenue() const;

    [[nodiscard]] float get_cost() const;

    void set_price(float price);

    void set_capacity(float capacity);

    void set_dirty_energy_unit_cost(float dirty_energy_unit_cost);

    void set_green_energy_unit_cost(float green_energy_unit_cost);

    void set_dirty_server_threshold(float dirty_server_threshold);

    void set_green_server_threshold(float green_server_threshold);

    void set_dirty_server_turnon_cost(float dirty_server_turnon_cost);

    void set_green_server_turnon_cost(float green_server_turnon_cost);

    void set_green_servers(int green_servers);

    void set_tax_rate(float tax_rate);

    void set_tax_mode(TaxMode tax_mode);

    void set_config(Config *config);

    void add_region(const std::shared_ptr<Region> &region);

    void allocate(const std::shared_ptr<Region> &region, float allocation);

    void reset();
};

#endif // CDN_H
