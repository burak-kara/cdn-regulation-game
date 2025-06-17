#ifndef FILEOPERATOR_H
#define FILEOPERATOR_H

#include <mutex>

#include "config.h"

class FileOperator {
    static FileOperator *pinstance;
    static std::mutex mutex;

    Config *config;

    explicit FileOperator(Config *config);

    ~FileOperator() = default;

    FileOperator(FileOperator &other) = delete; // NOLINT(*-use-equals-delete)

    void operator=(const FileOperator &) = delete; // NOLINT(*-use-equals-delete)

    [[nodiscard]] std::vector<std::string> find_files() const;

    static void delete_file(const std::string &file);

    void sort_files(std::vector<std::string> &files) const;

    [[nodiscard]] int extract_number(const std::string &filename) const;

    void merge_files(const std::vector<std::string> &input_files) const;

    void delete_log_folder_if_exists() const;

    static void create_log_folder_if_not_exists(const std::string &output_folder);

public:
    static FileOperator *get_instance();

    static FileOperator *get_instance(Config *config);

    void merge() const;

    void create_output_directory() const;
};

#endif // FILEOPERATOR_H
