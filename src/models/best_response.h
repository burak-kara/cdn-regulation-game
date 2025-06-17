#ifndef BEST_RESPONSE_H
#define BEST_RESPONSE_H

#include "cdn.h"
#include "region.h"
#include "service_provider.h"

class BestResponse {
    std::shared_ptr<ServiceProvider> service_provider;
    std::vector<std::shared_ptr<Cdn>> cdns;
    std::vector<std::shared_ptr<Region>> regions;

    [[nodiscard]] float cdn2_utility() const;

    [[nodiscard]] float cdn1_utility() const;

public:
    BestResponse() = default;

    [[nodiscard]] std::shared_ptr<ServiceProvider> get_service_provider() const;

    [[nodiscard]] const std::vector<std::shared_ptr<Cdn>> &get_cdns() const;

    [[nodiscard]] const std::vector<std::shared_ptr<Region>> &get_regions() const;

    void set_service_provider(const std::shared_ptr<ServiceProvider> &sp);

    void set_cdns(const std::vector<std::shared_ptr<Cdn>> &cdns);

    void set_regions(const std::vector<std::shared_ptr<Region>> &regions);

    [[nodiscard]] float sp_utility() const;

    void compare_and_replace(const BestResponse &best_response, const std::string &cdn_name = "cdn2");

    [[nodiscard]] std::stringstream str() const;
};

#endif // BEST_RESPONSE_H
