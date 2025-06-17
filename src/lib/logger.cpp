#include <filesystem>
#include <iostream>

#include "logger.h"

Logger::Logger(const std::string &filename) {
    this->config = Config::get_instance();
    this->filename = filename;

    const std::string output_folder = this->config->get_s("outputFolder");

    this->log_file.open(output_folder + "/" + filename, std::ios::out);
    if (!this->log_file.is_open()) {
        throw std::runtime_error("Failed to open log file");
    }
}

Logger::~Logger() {
    if (this->log_file.is_open()) {
        try {
            this->log_file.close();
        } catch (const std::ios_base::failure &e) {
            std::cerr << "Error closing log file: " << e.what() << std::endl;
        }
    }
}

Logger &Logger::get_instance(const std::string &filename) {
    thread_local Logger instance(filename);
    return instance;
}

void Logger::log(const std::string &message) {
    std::lock_guard lock(mutex);
    if (this->log_file.is_open()) {
        this->log_file << message << std::endl;
    }
}

void Logger::log(const std::stringstream &message) {
    std::lock_guard lock(mutex);
    if (this->log_file.is_open()) {
        std::string message_str = message.str();
        if (!message_str.empty() && message_str.back() == ' ') {
            message_str.pop_back();
        }
        this->log_file << message_str << std::endl;
    }
}
