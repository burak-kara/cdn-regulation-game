#include <sstream>
#include <stdexcept>
#include <string>

#include "argument_parser.h"

ArgumentParser *ArgumentParser::pinstance_{nullptr};
std::mutex ArgumentParser::mutex_;

ArgumentParser *ArgumentParser::get_instance() {
    std::lock_guard lock(mutex_);
    if (pinstance_ == nullptr) {
        pinstance_ = new ArgumentParser();
    }
    return pinstance_;
}

void ArgumentParser::parse(const int argc, char *argv[]) {
    if (argc < 2) {
        std::ostringstream oss;
        oss << "Usage: " << argv[0] << " --configFile=<config_file>";
        throw std::runtime_error(oss.str());
    }

    for (int i = 1; i < argc; ++i) {
        if (std::string arg = argv[i]; arg.find('=') != std::string::npos) {
            std::string key = arg.substr(0, arg.find('='));
            const std::string value = arg.substr(arg.find('=') + 1);
            this->args[key] = value;
        }
    }
}

const std::string &ArgumentParser::find(const std::string &key) {
    if (this->args.contains(key)) {
        return this->args.at(key);
    }
    throw std::runtime_error("Argument not found");
}
