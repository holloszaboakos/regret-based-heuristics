//
// Created by LENOVO on 10/11/2024.
//

#ifndef GROUP_BY_H
#define GROUP_BY_H
#include <utility>
#include <bits/ranges_base.h>


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

inline constexpr sort_by_fn group_by{};

#endif //GROUP_BY_H
