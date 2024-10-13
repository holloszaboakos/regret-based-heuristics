//
// Created by LENOVO on 10/11/2024.
//

#ifndef FIRST_H
#define FIRST_H

struct first_fn {
    template<typename R>
    auto operator()(R &&range) const {
        auto it = std::ranges::begin(range);
        return *it;
    }

    template <typename R>
    friend auto operator|(R&& range, const first_fn& fn) {
        return fn(std::forward<R>(range));
    }
};

inline constexpr first_fn first{};
#endif //FIRST_H
