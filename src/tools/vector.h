#ifndef VECTOR_H
#define VECTOR_H

#include <vector>

class Vector {
public:
    template<typename T>
    static std::vector<T> generate_range(T start, T end, T step) {
        std::vector<T> v;
        for (T i = start; i < end; i += step) {
            v.push_back(i);
        }
        return v;
    }

    template<typename T>
    static std::vector<std::vector<T>> divide(const std::vector<T> &full_vector, const int divisor) {
        std::vector<std::vector<float>> result(divisor);

        if (divisor <= 0 || full_vector.empty()) {
            return result;
        }
        const size_t total_size = full_vector.size();
        const size_t base_size = total_size / divisor;
        const size_t extra = total_size % divisor;
        auto it = full_vector.begin();
        for (int i = 0; i < divisor; ++i) {
            size_t current_size = base_size + (i < extra ? 1 : 0);
            result[i].assign(it, it + current_size);
            it += current_size;
        }
        return result;
    }
};

#endif // VECTOR_H
