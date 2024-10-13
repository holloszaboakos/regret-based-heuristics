//
// Created by LENOVO on 10/11/2024.
//

#ifndef FIRST_OR_NULL_H
#define FIRST_OR_NULL_H


struct first_or_null_fn {
    template<typename R>
    auto operator()(R &&range) const {
        auto it = std::ranges::begin(range);
        if (it != std::ranges::end(range)) {
            return std::optional{*it};
        }
        return std::optional<std::ranges::range_value_t<R> >{};
    }

    template <typename R>
    friend auto operator|(R&& range, const first_or_null_fn& fn) {
        return fn(std::forward<R>(range));
    }
};

inline constexpr first_or_null_fn first_or_null{};

#endif //FIRST_OR_NULL_H
