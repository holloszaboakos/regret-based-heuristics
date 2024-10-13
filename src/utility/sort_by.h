//
// Created by LENOVO on 10/11/2024.
//

#ifndef SORT_BY_H
#define SORT_BY_H
#include <utility>

struct sort_by_fn {
    template<typename R>
    auto operator()(R &&range) const {
        //TODO
    }

    template <typename R>
    friend auto operator|(R&& range, const sort_by_fn& fn) {
        return fn(std::forward<R>(range));
    }
};

inline constexpr sort_by_fn sort_by{};

#endif //SORT_BY_H
