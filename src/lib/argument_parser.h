#ifndef ARGUMENTPARSER_H
#define ARGUMENTPARSER_H

#include <mutex>

#include "../tools/types.h"

class ArgumentParser {
    static ArgumentParser *pinstance_;
    static std::mutex mutex_;

    ArgumentType args = {};

protected:
    ArgumentParser() = default;

    ~ArgumentParser() = default;

public:
    ArgumentParser(ArgumentParser &other) = delete;

    void operator=(const ArgumentParser &) = delete;

    static ArgumentParser *get_instance();

    void parse(int argc, char *argv[]);

    const std::string &find(const std::string &key);
};

#endif // ARGUMENTPARSER_H
