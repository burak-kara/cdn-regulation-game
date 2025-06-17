#include <algorithm>
#include <sstream>

#include "../lib/config.h"
#include "../tools/string.h"
#include "best_response.h"
#include "cdn.h"
#include "region.h"
#include "service_provider.h"
#include "sub_game.h"

SubGame::SubGame(Config *config, Logger &logger, const std::vector<float> &iteration_range) :
    config(config), logger(logger), iteration_range(iteration_range) {}

[[nodiscard]] const ServiceProvider *SubGame::get_service_provider() const { return this->service_provider.get(); }

[[nodiscard]] const std::vector<std::shared_ptr<Cdn>> &SubGame::get_cdns() const { return this->cdns; }

[[nodiscard]] const std::vector<std::shared_ptr<Region>> &SubGame::get_regions() const { return this->regions; }

void SubGame::set_iteration_range(const std::vector<float> &iteration_range) {
    this->iteration_range = iteration_range;
}

void SubGame::set_config(Config *config) { this->config = config; }

void SubGame::add_service_provider(const std::shared_ptr<ServiceProvider> &sp) { this->service_provider = sp; }

void SubGame::add_cdns(const std::vector<std::shared_ptr<Cdn>> &cdns) {
    for (const auto &cdn: cdns) {
        this->add_cdn(cdn);
    }
}

void SubGame::add_cdn(const std::shared_ptr<Cdn> &cdn) { this->cdns.push_back(cdn); }

void SubGame::add_regions(const std::vector<std::shared_ptr<Region>> &regions) {
    for (const auto &region: regions) {
        this->add_region(region);
    }
}

void SubGame::add_region(const std::shared_ptr<Region> &region) { this->regions.push_back(region); }

void SubGame::construct() {
    const std::string &cdn1_id = this->config->get_s("cdn1");
    const std::string &cdn2_id = this->config->get_s("cdn2");

    const auto service_provider = std::make_shared<ServiceProvider>(this->config);
    const auto cdns = std::vector{std::make_shared<Cdn>(this->config, String::to_cdn_name(cdn1_id),
                                                        this->config->get_f(cdn1_id + "Capacity")),
                                  std::make_shared<Cdn>(this->config, String::to_cdn_name(cdn2_id),
                                                        this->config->get_f(cdn2_id + "Capacity"))};
    const auto regions = std::vector{std::make_shared<Region>(this->config, "Region 1"),
                                     std::make_shared<Region>(this->config, "Region 2"),
                                     std::make_shared<Region>(this->config, "Region 1,2")};

    this->add_service_provider(service_provider);
    this->add_cdns(cdns);
    this->add_regions(regions);
    this->log_headers();

    service_provider->add_cdns(cdns);
    service_provider->add_regions(regions);

    cdns[0]->add_region(regions[0]);
    cdns[0]->add_region(regions[2]);
    cdns[1]->add_region(regions[1]);
    cdns[1]->add_region(regions[2]);

    service_provider->set_max_demand(this->config->get_f("maxDemand"));
    service_provider->set_demand_degradation(this->config->get_f("demandDegradation"));

    cdns[0]->set_dirty_energy_unit_cost(this->config->get_f(cdn1_id + "DirtyEnergyPrice"));
    cdns[1]->set_dirty_energy_unit_cost(this->config->get_f(cdn2_id + "DirtyEnergyPrice"));

    cdns[0]->set_green_energy_unit_cost(this->config->get_f(cdn1_id + "GreenEnergyPrice"));
    cdns[1]->set_green_energy_unit_cost(this->config->get_f(cdn2_id + "GreenEnergyPrice"));

    cdns[0]->set_dirty_server_threshold(this->config->get_f(cdn1_id + "DirtyServerThreshold"));
    cdns[1]->set_dirty_server_threshold(this->config->get_f(cdn2_id + "DirtyServerThreshold"));

    cdns[0]->set_green_server_threshold(this->config->get_f(cdn1_id + "GreenServerThreshold"));
    cdns[1]->set_green_server_threshold(this->config->get_f(cdn2_id + "GreenServerThreshold"));

    cdns[0]->set_dirty_server_turnon_cost(this->config->get_f(cdn1_id + "DirtyServerTurnOnCost"));
    cdns[1]->set_dirty_server_turnon_cost(this->config->get_f(cdn1_id + "DirtyServerTurnOnCost"));

    cdns[0]->set_green_server_turnon_cost(this->config->get_f(cdn1_id + "GreenServerTurnOnCost"));
    cdns[1]->set_green_server_turnon_cost(this->config->get_f(cdn2_id + "GreenServerTurnOnCost"));

    cdns[0]->set_tax_mode(StringToTaxMode.at(this->config->get_s("taxMode")));
    cdns[1]->set_tax_mode(StringToTaxMode.at(this->config->get_s("taxMode")));

    // ************** Some of the rest will be overwritten in some games **************

    // will be overwritten in the best response game
    cdns[0]->set_tax_rate(this->config->get_f("fixedTaxRate"));
    cdns[1]->set_tax_rate(this->config->get_f("fixedTaxRate"));

    cdns[0]->set_price(this->config->get_f("fixedCdn1Price"));
    cdns[0]->set_green_servers(this->config->get_i("fixedGreenServers"));
    // will be overwritten in the optimal CDN2 and best response games
    cdns[1]->set_price(this->config->get_f("fixedCdn1Price"));
    cdns[1]->set_green_servers(this->config->get_i("fixedGreenServers"));

    // ************** To avoid map lookups in the loops **************

    // will be used in the best response game
    this->cdn2_prices = this->config->get_vf(cdn2_id + "Prices");
    // will be used in the best response games
    this->cdn1_green_servers = this->config->get_vf(cdn1_id + "GreenServers");
    // will be used in the optimal CDN2 and best response games
    this->cdn2_green_servers = this->config->get_vf(cdn2_id + "GreenServers");
    // will be used in the best response game
    this->tax_rates = this->config->get_vf("taxRates");
    // will be used in the optimal CDN2 and best response games
    this->sp_prices = this->config->get_vf("spPrices");
}

void SubGame::play_best_response() {
    for (const auto cdn1_price: this->iteration_range) {
        this->cdns[0]->set_price(cdn1_price);

        for (const auto tax_rate: this->tax_rates) {
            this->cdns[0]->set_tax_rate(tax_rate);
            this->cdns[1]->set_tax_rate(tax_rate);
            BestResponse cdn1_green_response;

            for (const auto cdn1_green_server: this->cdn1_green_servers) {
                this->cdns[0]->set_green_servers(static_cast<int>(cdn1_green_server));
                BestResponse cdn2_response;

                for (const auto cdn2_price: this->cdn2_prices) {
                    this->cdns[1]->set_price(cdn2_price);
                    BestResponse cdn2_green_response;

                    for (const auto cdn2_green_server: this->cdn2_green_servers) {
                        this->cdns[1]->set_green_servers(static_cast<int>(cdn2_green_server));
                        BestResponse sp_response;

                        for (const auto sp_price: this->sp_prices) {
                            this->service_provider->set_price(sp_price);
                            this->service_provider->update_demand();
                            this->service_provider->allocate();

                            if (this->service_provider->get_utility() > sp_response.sp_utility()) {
                                sp_response.set_service_provider(this->service_provider);
                                sp_response.set_cdns(this->cdns);
                                sp_response.set_regions(this->regions);
                            }

                            this->reset();
                        }
                        cdn2_green_response.compare_and_replace(sp_response);
                    }
                    cdn2_response.compare_and_replace(cdn2_green_response);
                }
                cdn1_green_response.compare_and_replace(cdn2_response, "cdn1");
            }
            this->log(cdn1_green_response);
        }
    }
}

void SubGame::play_optimal_cdn2() {
    for (const auto cdn2_price: this->iteration_range) {
        this->cdns[1]->set_price(cdn2_price);
        BestResponse cdn2_response;

        for (const auto cdn2_green_server: this->cdn2_green_servers) {
            this->cdns[1]->set_green_servers(static_cast<int>(cdn2_green_server));
            BestResponse sp_response;

            for (const auto sp_price: this->sp_prices) {
                this->service_provider->set_price(sp_price);
                this->service_provider->update_demand();
                this->service_provider->allocate();

                if (this->service_provider->get_utility() > sp_response.sp_utility()) {
                    sp_response.set_service_provider(this->service_provider);
                    sp_response.set_cdns(this->cdns);
                    sp_response.set_regions(this->regions);
                }

                this->reset();
            }
            cdn2_response.compare_and_replace(sp_response);
        }
        this->log(cdn2_response);
    }
}

void SubGame::play_optimal_sp() {
    BestResponse sp_response;

    for (const auto sp_price: this->iteration_range) {
        this->service_provider->set_price(sp_price);
        this->service_provider->update_demand();
        this->service_provider->allocate();

        sp_response.set_service_provider(this->service_provider);
        sp_response.set_cdns(this->cdns);
        sp_response.set_regions(this->regions);
        this->log(sp_response);
        this->reset();
    }
}

void SubGame::reset() {
    this->service_provider->reset();
    for (const auto &cdn: this->cdns) {
        cdn->reset();
    }
    for (const auto &region: this->regions) {
        region->reset();
    }
}

void SubGame::log(const BestResponse &best_response) const { this->logger.log(best_response.str()); }

void SubGame::log_headers() const {
    std::stringstream log_stream;

    for (const auto &cdn: this->cdns) {
        std::string name = cdn->get_name();
        std::ranges::replace(name, ' ', '_');
        log_stream << name << "_Price " << name << "_Green_Servers " << name << "_Utility " << name << "_Revenue "
                   << name << "_Cost " << name << "_Allocation " << name << "_Dirty_Allocation " << name
                   << "_Green_Allocation " << name << "_Servers_In_Use " << name << "_Dirty_Servers_In_Use " << name
                   << "_Green_Servers_In_Use " << name << "_Tax_Rate " << name << "_Tax_Mode ";
    }

    log_stream << "SP_Price SP_Utility SP_Revenue SP_Cost SP_Demand SP_Allocation ";

    for (const auto &region: this->regions) {
        std::string name = region->get_name();
        std::ranges::replace(name, ' ', '_');
        log_stream << name << "_Demand " << name << "_Allocation ";
    }

    this->logger.log(log_stream);
}
