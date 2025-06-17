#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

#include "file_operator.h"


namespace fs = std::filesystem;


FileOperator *FileOperator::pinstance{nullptr};
std::mutex FileOperator::mutex;

FileOperator::FileOperator(Config *config) : config(config) {}

FileOperator *FileOperator::get_instance() {
    std::lock_guard lock(mutex);
    if (pinstance == nullptr) {
        throw std::runtime_error("Config instance is not set!");
    }
    return pinstance;
}

FileOperator *FileOperator::get_instance(Config *config) {
    std::lock_guard lock(mutex);
    if (pinstance == nullptr) {
        pinstance = new FileOperator(config);
    }
    return pinstance;
}

[[nodiscard]] std::vector<std::string> FileOperator::find_files() const {
    const std::string directory = this->config->get_s("outputFolder");
    const std::regex regex_pattern(this->config->get_s("fileSearchPattern"));
    std::vector<std::string> matched_files;

    for (const auto &entry: fs::directory_iterator(directory)) {
        if (is_regular_file(entry) && std::regex_match(entry.path().filename().string(), regex_pattern)) {
            matched_files.push_back(entry.path().string());
        }
    }

    return matched_files;
}

void FileOperator::sort_files(std::vector<std::string> &files) const {
    std::sort(files.begin(), files.end(),
              [&](const std::string &a, const std::string &b) { return extract_number(a) < extract_number(b); });
}

[[nodiscard]] int FileOperator::extract_number(const std::string &filename) const {
    const std::regex pattern(this->config->get_s("fileSearchPattern"));

    if (std::smatch match; std::regex_match(filename, match, pattern) && match.size() > 1) {
        return std::stoi(match[1].str());
    }

    return -1;
}

void FileOperator::merge_files(const std::vector<std::string> &input_files) const {
    const std::string directory = this->config->get_s("outputFolder");
    const std::string output_file = directory + "/" + this->config->get_s("outputFile");

    std::ofstream out(output_file, std::ios::out | std::ios::trunc);
    if (!out.is_open()) {
        std::cerr << "Failed to open output file: " << output_file << std::endl;
        return;
    }

    bool first_file = true;
    for (const auto &file: input_files) {
        std::ifstream in(file, std::ios::in);
        if (!in.is_open()) {
            std::cerr << "Failed to open input file: " << file << std::endl;
            continue;
        }
        std::string line;
        bool skip_first_line = !first_file;

        while (std::getline(in, line)) {
            if (skip_first_line) {
                skip_first_line = false;
                continue;
            }
            out << line << "\n";
        }

        first_file = false;
        in.close();

        delete_file(file);
    }

    out.close();
}

void FileOperator::delete_file(const std::string &file) {
    if (!fs::remove(file)) {
        std::cerr << "Failed to delete file: " << file << std::endl;
    }
}

void FileOperator::merge() const {
    std::cout << "Merging files..." << std::endl;

    auto files = find_files();
    sort_files(files);
    merge_files(files);

    std::cout << "Files merged...\n" << std::endl;
}

void FileOperator::create_output_directory() const {
    delete_log_folder_if_exists();
    create_log_folder_if_not_exists(this->config->get_s("outputFolder"));
}

void FileOperator::delete_log_folder_if_exists() const {
    if (const std::string directory = this->config->get_s("outputFolder"); fs::exists(directory)) {
        try {
            std::cout << "\tDeleting existing log folder: " << directory << std::endl;
            fs::remove_all(directory);
        } catch (const fs::filesystem_error &e) {
            std::cerr << "\tFailed to delete log folder: " << e.what() << std::endl;
        }
    }
}

void FileOperator::create_log_folder_if_not_exists(const std::string &output_folder) {
    if (const std::filesystem::path path = output_folder; !exists(path)) {
        std::cout << "\tCreating log folder: " << output_folder << std::endl;
        create_directory(path);
    }
}
