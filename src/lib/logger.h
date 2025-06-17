#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <mutex>
#include <string>

#include "config.h"

class Logger {
    std::ofstream log_file;
    std::mutex mutex;

    Config *config;
    std::string filename;

    explicit Logger(const std::string &filename);

    ~Logger();

    Logger(const Logger &) = delete; // NOLINT(*-use-equals-delete)

    Logger &operator=(const Logger &) = delete; // NOLINT(*-use-equals-delete)
public:
    static Logger &get_instance(const std::string &filename = "log.txt");

    void log(const std::string &message);

    void log(const std::stringstream &message);
};

#endif // LOGGER_H
