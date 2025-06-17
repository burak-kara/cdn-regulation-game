#include "manager.h"

#include <iostream>
#include <thread>

#include "../enums/game_mode.h"
#include "../models/sub_game.h"
#include "../tools/vector.h"
#include "argument_parser.h"
#include "config.h"
#include "file_operator.h"
#include "logger.h"

Manager *Manager::pinstance{nullptr};
std::mutex Manager::mutex;

Manager *Manager::get_instance() {
    std::lock_guard lock(mutex);
    if (pinstance == nullptr) {
        pinstance = new Manager();
    }
    return pinstance;
}

void Manager::init(const int argc, char *argv[]) {
    message("Initializing...");

    this->start_time = this->last_epoch = Time::time();
    this->argument_parser = ArgumentParser::get_instance();
    this->config = Config::get_instance();
    this->file_operator = FileOperator::get_instance(config);

    this->argument_parser->parse(argc, argv);
    const auto config_file = this->argument_parser->find("--configFile");

    this->config->load(config_file);
    const auto thread_count = std::thread::hardware_concurrency() * this->config->get_i("threadPerCore");
    this->config->set("threadCount", static_cast<float>(thread_count));
    this->file_operator->create_output_directory();
    this->config->export_to_file();

    tmessage("Tax Mode: " + this->config->get_s("taxMode"));
    tmessage("Processor count: " + std::to_string(thread_count));
    tmessage("Configuration file: " + config_file);
    tmessage("Output folder: " + this->config->get_s("outputFolder"));
    message("Initialization complete...\n");
}

void Manager::play_br(const std::string &cdn1_id, const std::string &cdn2_id) {
    message("Playing CDN best response...");
    tmessage("Game Mode:" + GameModeToHumanReadable.at(GameMode::BR) + " || " + "CDN 1: " + cdn1_id +
             " | CDN 2: " + cdn2_id);

    this->config->load_game(GameMode::BR, cdn1_id, cdn2_id);
    const std::vector<float> cdn1_prices = this->config->get_vf(cdn1_id + "Prices");
    const auto thread_count = static_cast<int>(this->config->get_f("threadCount"));
    const std::vector<std::vector<float>> parsed_cdn1_prices = Vector::divide(cdn1_prices, thread_count);

    tmessage("Spawning " + std::to_string(thread_count) + " sub games...");

    std::vector<std::thread> threads;
    threads.reserve(thread_count);

    for (int i = 0; i < thread_count; ++i) {
        threads.emplace_back(&Manager::start_br_sub_game, this, i, parsed_cdn1_prices[i]);
    }

    for (auto &th: threads) {
        th.join();
    }

    this->complete();
    this->file_operator->merge();
}

void Manager::start_br_sub_game(const int thread_index, const std::vector<float> &iteration_range) const {
    const std::string filename = this->config->get_s("outputFileName") + "_" + std::to_string(thread_index) + ".txt";
    SubGame game(this->config, Logger::get_instance(filename), iteration_range);

    game.construct();
    game.play_best_response();
}

void Manager::play_optimal_cdn2(const std::string &cdn1_id, const std::string &cdn2_id) {
    message("Proving CDN 2 has the best response...");
    tmessage("Game Mode:" + GameModeToHumanReadable.at(GameMode::OptimalCDN2));
    tmessage("CDN 1: " + cdn1_id + " | CDN 2: " + cdn2_id);

    this->config->load_game(GameMode::OptimalCDN2, cdn1_id, cdn2_id);
    const std::vector<float> cdn2_prices = this->config->get_vf(cdn2_id + "Prices");
    const auto thread_count = this->config->get_i("threadCount");
    const std::vector<std::vector<float>> parsed_cdn2_prices = Vector::divide(cdn2_prices, thread_count);

    tmessage("Spawning " + std::to_string(thread_count) + " sub games...");

    std::vector<std::thread> threads;
    threads.reserve(thread_count);

    for (int i = 0; i < thread_count; ++i) {
        threads.emplace_back(&Manager::start_optimal_cdn2_sub_game, this, i, parsed_cdn2_prices[i]);
    }

    for (auto &th: threads) {
        th.join();
    }

    this->complete();
    this->file_operator->merge();
}

void Manager::start_optimal_cdn2_sub_game(const int thread_index, const std::vector<float> &iteration_range) const {
    SubGame game(
            this->config,
            Logger::get_instance(this->config->get_s("outputFileName") + "_" + std::to_string(thread_index) + ".txt"),
            iteration_range);

    game.construct();
    game.play_optimal_cdn2();
}

void Manager::play_optimal_sp() {
    message("Proving SP has the optimal price...");
    tmessage("Game Mode:" + GameModeToHumanReadable.at(GameMode::OptimalSP));

    this->config->load_game(GameMode::OptimalSP);

    SubGame game(this->config, Logger::get_instance(this->config->get_s("outputFile")),
                 this->config->get_vf("spPrices"));

    game.construct();
    game.play_optimal_sp();

    this->complete();
}

void Manager::complete() {
    const std::chrono::system_clock::time_point now = Time::time();
    tmessage("Time taken: " + Time::human_time(this->last_epoch, now));
    message("All sub games completed...\n");
    this->last_epoch = now;
}

void Manager::finish() {
    this->end_time = Time::time();
    message("Completed...");
    tmessage("Human Friendly Execution Time: " + Time::human_time(this->start_time, this->end_time));
    tmessage("Excel Friendly Execution Time: " + Time::machine_time(this->start_time, this->end_time));

    send_notification();

    message("Exiting...");
}

void Manager::message(const std::string &message) { std::cout << message << std::endl; }

void Manager::tmessage(const std::string &message) { std::cout << "\t" << message << std::endl; }

void Manager::send_notification() const {
    try {
        const std::string command = "python3 notification.py " + this->config->get_s("taxMode");
        system(command.c_str());
    } catch (const std::exception &e) {
        std::cerr << "Failed to send notification: " << e.what() << std::endl;
    }
}
