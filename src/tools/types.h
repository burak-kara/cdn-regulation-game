#ifndef TYPES_H
#define TYPES_H

#include <map>
#include <memory>
#include <variant>
#include <vector>

using FloatTuple = std::tuple<float, float>;

using VariantT = std::variant<float, std::string, std::vector<float>>;

using ConfigMap = std::map<std::string, VariantT>;

using ArgumentType = std::map<std::string, std::string>;

#endif // TYPES_H
