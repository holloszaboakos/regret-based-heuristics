//
// Created by LENOVO on 10/11/2024.
//

#ifndef FIRST_OR_DEFAULT_H
#define FIRST_OR_DEFAULT_H
template<typename T>
struct first_or_default_fn {
    const T& default_value;

    explicit first_or_default_fn(const T& default_value) : default_value(default_value) {}

    template<typename R>
    auto operator()(R &&range) const {
        auto it = std::ranges::begin(range);
        if (it == std::ranges::end(range)) {
            return default_value;
        }
        return *it;
    }

    template<typename R>
    friend auto operator|(R &&range,const first_or_default_fn &fn) {
        return fn(std::forward<R>(range));
    }
};

template<typename T>
first_or_default_fn<T> first_or_default(const T& default_value) {
    return first_or_default_fn<T>(default_value);
};


#endif //FIRST_OR_DEFAULT_H
