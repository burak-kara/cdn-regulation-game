#include <limits>
#include <sstream>

#include "best_response.h"

[[nodiscard]] std::shared_ptr<ServiceProvider> BestResponse::get_service_provider() const {
    return this->service_provider;
}

[[nodiscard]] const std::vector<std::shared_ptr<Cdn>> &BestResponse::get_cdns() const { return this->cdns; }

[[nodiscard]] const std::vector<std::shared_ptr<Region>> &BestResponse::get_regions() const { return this->regions; }

void BestResponse::set_service_provider(const std::shared_ptr<ServiceProvider> &sp) {
    this->service_provider = std::make_shared<ServiceProvider>(*sp);
}

void BestResponse::set_cdns(const std::vector<std::shared_ptr<Cdn>> &cdns) {
    this->cdns.clear();
    for (const auto &cdn: cdns) {
        this->cdns.push_back(std::make_shared<Cdn>(*cdn));
    }
}

void BestResponse::set_regions(const std::vector<std::shared_ptr<Region>> &regions) {
    this->regions.clear();
    for (const auto &region: regions) {
        this->regions.push_back(std::make_shared<Region>(*region));
    }
}

[[nodiscard]] float BestResponse::sp_utility() const {
    if (!this->service_provider) {
        return -std::numeric_limits<float>::infinity();
    }
    return this->service_provider->get_utility();
}

void BestResponse::compare_and_replace(const BestResponse &best_response, const std::string &cdn_name) {


    if ((cdn_name == "cdn2" && best_response.cdn2_utility() > this->cdn2_utility()) ||
        (cdn_name == "cdn1" && best_response.cdn1_utility() > this->cdn1_utility())) {
        this->set_service_provider(best_response.get_service_provider());
        this->set_cdns(best_response.get_cdns());
        this->set_regions(best_response.get_regions());
    }
}

[[nodiscard]] float BestResponse::cdn2_utility() const {
    if (this->cdns.size() < 2) {
        return -std::numeric_limits<float>::infinity();
    }
    return this->cdns[1]->get_utility();
}

[[nodiscard]] float BestResponse::cdn1_utility() const {
    if (this->cdns.size() < 2) {
        return -std::numeric_limits<float>::infinity();
    }
    return this->cdns[0]->get_utility();
}

[[nodiscard]] std::stringstream BestResponse::str() const {
    std::stringstream log_stream;

    for (const auto &cdn: this->cdns) {
        log_stream << cdn->get_price() << " " << cdn->get_green_servers() << " " << cdn->get_utility() << " "
                   << cdn->get_revenue() << " " << cdn->get_cost() << " " << cdn->get_allocation() << " "
                   << cdn->get_dirty_allocation() << " " << cdn->get_green_allocation() << " "
                   << cdn->get_servers_in_use() << " " << cdn->get_dirty_servers_in_use() << " "
                   << cdn->get_green_servers_in_use() << " " << cdn->get_tax_rate() << " " << cdn->get_str_tax_mode()
                   << " ";
    }

    log_stream << this->service_provider->get_price() << " " << this->service_provider->get_utility() << " "
               << this->service_provider->get_revenue() << " " << this->service_provider->get_cost() << " "
               << this->service_provider->get_demand() << " " << this->service_provider->get_allocation() << " ";

    for (const auto &region: this->regions) {
        log_stream << region->get_demand() << " " << region->get_allocation() << " ";
    }
    return log_stream;
}
