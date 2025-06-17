#ifndef TIME_H
#define TIME_H

#include <chrono>
#include <ctime>

class Time {
public:
    static std::chrono::system_clock::time_point time() { return std::chrono::high_resolution_clock::now(); }

    static std::string local_time(const std::chrono::system_clock::time_point &time) {
        const std::time_t now_time = std::chrono::system_clock::to_time_t(time);
        const std::tm local_time = *std::localtime(&now_time);

        std::ostringstream oss;
        oss << std::put_time(&local_time, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }

    static std::string human_time(const std::chrono::system_clock::time_point &start_time,
                                  const std::chrono::system_clock::time_point &end_time) {
        const auto duration = end_time - start_time;
        const auto days = std::chrono::duration_cast<std::chrono::days>(duration % std::chrono::hours(24));
        const auto hours = std::chrono::duration_cast<std::chrono::hours>(duration % std::chrono::days(1));
        const auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration % std::chrono::hours(1));
        const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration % std::chrono::minutes(1));
        const auto milliseconds =
                std::chrono::duration_cast<std::chrono::milliseconds>(duration % std::chrono::seconds(1));

        return std::to_string(days.count()) + " days " + std::to_string(hours.count()) + " hours " +
               std::to_string(minutes.count()) + " minutes " + std::to_string(seconds.count()) + " seconds " +
               std::to_string(milliseconds.count()) + " milliseconds";
    }

    static std::string machine_time(const std::chrono::system_clock::time_point &start_time,
                                    const std::chrono::system_clock::time_point &end_time) {
        const auto duration = end_time - start_time;
        const auto days = std::chrono::duration_cast<std::chrono::days>(duration % std::chrono::hours(24));
        const auto hours = std::chrono::duration_cast<std::chrono::hours>(duration % std::chrono::days(1));
        const auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration % std::chrono::hours(1));
        const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration % std::chrono::minutes(1));

        std::ostringstream oss;
        oss << std::setfill('0') << std::setw(2) << std::to_string(days.count()) << ":" << std::setfill('0')
            << std::setw(2) << std::to_string(hours.count()) << ":" << std::setfill('0') << std::setw(2)
            << std::to_string(minutes.count()) << ":" << std::setfill('0') << std::setw(2)
            << std::to_string(seconds.count());

        return oss.str();
    }
};

#endif // TIME_H
