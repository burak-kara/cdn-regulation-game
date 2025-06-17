#ifndef GAME_MODE_H
#define GAME_MODE_H

#include <string>
#include <unordered_map>
#include <unordered_set>

const std::string br = "br";
const std::string optimal_cdn2 = "optimal_cdn2";
const std::string optimal_sp = "optimal_sp";

enum class GameMode { BR, OptimalCDN2, OptimalSP };

inline const std::unordered_map<GameMode, std::string> GameModeToString{
        {GameMode::BR, br}, {GameMode::OptimalCDN2, optimal_cdn2}, {GameMode::OptimalSP, optimal_sp}};

inline const std::unordered_map<std::string, GameMode> StringToGameMode{
        {br, GameMode::BR}, {optimal_cdn2, GameMode::OptimalCDN2}, {optimal_sp, GameMode::OptimalSP}};

inline const std::unordered_map<GameMode, std::string> GameModeToHumanReadable{
        {GameMode::BR, "Best Response"}, {GameMode::OptimalCDN2, "Optimal CDN 2"}, {GameMode::OptimalSP, "Optimal SP"}};

inline bool is_valid(const GameMode mode) {
    static const std::unordered_set valid_modes = {GameMode::BR, GameMode::OptimalCDN2, GameMode::OptimalSP};
    return valid_modes.contains(mode);
}

#endif // GAME_MODE_H
