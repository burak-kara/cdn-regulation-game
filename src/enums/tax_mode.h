#ifndef TAX_MODE_H
#define TAX_MODE_H

#include <string>
#include <unordered_map>
#include <unordered_set>

enum class TaxMode { Allocation, Server, None };

inline const std::unordered_map<TaxMode, std::string> TaxModeToString{
        {TaxMode::Allocation, "Allocation"}, {TaxMode::Server, "Server"}, {TaxMode::None, "None"}};

inline const std::unordered_map<std::string, TaxMode> StringToTaxMode{
        {"Allocation", TaxMode::Allocation}, {"Server", TaxMode::Server}, {"None", TaxMode::None}};

inline bool is_valid(const TaxMode mode) {
    static const std::unordered_set valid_modes = {TaxMode::Allocation, TaxMode::Server};
    return valid_modes.contains(mode);
}

#endif // TAX_MODE_H
