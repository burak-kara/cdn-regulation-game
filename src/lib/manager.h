#ifndef MANAGER_H
#define MANAGER_H

#include <mutex>
#include <string>
#include <vector>

#include "../tools/time.h"
#include "argument_parser.h"
#include "config.h"
#include "file_operator.h"

class Manager {
    static Manager *pinstance;
    static std::mutex mutex;

    ArgumentParser *argument_parser{};
    Config *config{};
    FileOperator *file_operator{};

    std::chrono::system_clock::time_point start_time;
    std::chrono::system_clock::time_point last_epoch;
    std::chrono::system_clock::time_point end_time;

    Manager() = default;

    ~Manager() = default;

    Manager(Manager &other) = delete; // NOLINT(*-use-equals-delete)

    void operator=(const Manager &) = delete; // NOLINT(*-use-equals-delete)

    void start_br_sub_game(int thread_index, const std::vector<float> &iteration_range) const;

    void start_optimal_cdn2_sub_game(int thread_index, const std::vector<float> &iteration_range) const;

    static void message(const std::string &message);

    static void tmessage(const std::string &message);

    void complete();

    void send_notification() const;

public:
    static Manager *get_instance();

    void init(int argc, char *argv[]);

    void play_br(const std::string &cdn1_id, const std::string &cdn2_id);

    void play_optimal_cdn2(const std::string &cdn1_id, const std::string &cdn2_id);

    void play_optimal_sp();

    void finish();
};

#endif // MANAGER_H
