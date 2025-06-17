#ifndef GAME_H
#define GAME_H

#include <memory>
#include <vector>

#include "../lib/logger.h"
#include "best_response.h"

class SubGame {
    Config *config;
    Logger &logger;

    BestResponse best_response;
    std::shared_ptr<ServiceProvider> service_provider;
    std::vector<std::shared_ptr<Cdn>> cdns;
    std::vector<std::shared_ptr<Region>> regions;

    std::vector<float> iteration_range;
    std::vector<float> cdn2_prices;
    std::vector<float> cdn1_green_servers;
    std::vector<float> cdn2_green_servers;
    std::vector<float> sp_prices;
    std::vector<float> tax_rates;

    void log(const BestResponse &best_response) const;

    void log_headers() const;

public:
    SubGame(Config *config, Logger &logger, const std::vector<float> &iteration_range);

    [[nodiscard]] const ServiceProvider *get_service_provider() const;

    [[nodiscard]] const std::vector<std::shared_ptr<Cdn>> &get_cdns() const;

    [[nodiscard]] const std::vector<std::shared_ptr<Region>> &get_regions() const;

    void set_iteration_range(const std::vector<float> &iteration_range);

    void set_config(Config *config);

    void add_service_provider(const std::shared_ptr<ServiceProvider> &sp);

    void add_cdns(const std::vector<std::shared_ptr<Cdn>> &cdns);

    void add_cdn(const std::shared_ptr<Cdn> &cdn);

    void add_regions(const std::vector<std::shared_ptr<Region>> &regions);

    void add_region(const std::shared_ptr<Region> &region);

    void construct();

    void play_best_response();

    void play_optimal_cdn2();

    void play_optimal_sp();

    void reset();
};

#endif // GAME_H
