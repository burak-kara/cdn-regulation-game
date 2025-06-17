#ifndef REGION_H
#define REGION_H

#include <string>

#include "../headers/config.fwd.h"
#include "../tools/types.h"

class Region {
    Config *config;
    std::string name;

    int cdn_count = 0;
    float demand = 0;
    float allocation = 0;

    std::vector<std::shared_ptr<Cdn>> cdns;

public:
    Region(Config *config, std::string name);

    Region(const Region &region);

    [[nodiscard]] const std::string &get_name() const;

    [[nodiscard]] int get_cdn_count() const;

    [[nodiscard]] float get_demand() const;

    [[nodiscard]] float get_allocation() const;

    [[nodiscard]] float get_remaining_demand() const;

    [[nodiscard]] const std::vector<std::shared_ptr<Cdn>> &get_cdns() const;

    [[nodiscard]] const Cdn *find_cdn(const std::string &name) const;

    void set_demand(float demand);

    void set_name(const std::string &name);

    void set_config(Config *config);

    void allocate(float allocation);

    void reset();

    [[nodiscard]] bool is_allocatable() const;

    [[nodiscard]] bool is_monopoly() const;

    void add_cdn(const std::shared_ptr<Cdn> &cdn);
};

#endif // REGION_H
