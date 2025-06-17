#ifndef CONFIG_H
#define CONFIG_H

#include <map>
#include <mutex>
#include <string>

#include "../enums/game_mode.h"
#include "../tools/types.h"

class Config {
    static Config *pinstance;
    static std::mutex mutex;

    std::string filename;
    ConfigMap map;

    Config() = default;

    ~Config() = default;

    Config(Config &other) = delete; // NOLINT(*-use-equals-delete)

    void operator=(const Config &) = delete; // NOLINT(*-use-equals-delete)

    void configure_cdn_names(const std::string &cdn1_id, const std::string &cdn2_id);

    void configure_outputs(const GameMode &game_mode);

    void configure_prices();

    void configure_servers();

    void configure_taxes();

public:
    static Config *get_instance();

    void load(const std::string &filename);

    void load_game(const GameMode &game_mode, const std::string &cdn1_id = "cdn1", const std::string &cdn2_id = "cdn2");

    [[nodiscard]] const ConfigMap &get() const;

    [[nodiscard]] const VariantT &get(const std::string &key);

    [[nodiscard]] const float &get_f(const std::string &key);

    [[nodiscard]] int get_i(const std::string &key);

    [[nodiscard]] const std::string &get_s(const std::string &key);

    [[nodiscard]] const std::vector<float> &get_vf(const std::string &key);

    void set(const std::string &key, const VariantT &value);

    void export_to_file();
};

#endif // CONFIG_H
