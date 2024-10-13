//
// Created by LENOVO on 10/4/2024.
//
#pragma once
#include <vector>
#include <random>
#include <bits/ranges_algo.h>

struct permutation {
    explicit permutation(int size);

    explicit permutation(const std::vector<int> &values);

    [[nodiscard]] int getSize() const;

    static permutation random(int size);

    [[nodiscard]] int get(int index) const;

    void set(int index, int value);

    [[nodiscard]] int indexOf(int element) const;

    [[nodiscard]] bool contains(int element) const;

    int deletePosition(int position);

    int deleteValue(int value);

    void swapPositions(int position1, int position2);

    void clear();

    [[nodiscard]] int before(int value) const;

    [[nodiscard]] int after(int value) const;

    [[nodiscard]] permutation clone() const;

    [[nodiscard]] bool isComplete() const;

private:
    int size;
    std::vector<int> _permutation;
    std::vector<int> _inverse_permutation;
};


class PermutationException final : public std::exception {
    const char *message;

public:
    explicit PermutationException(const char *message) : message(message) {
    }

    [[nodiscard]] const char *what() const noexcept override {
        return message;
    }
};

inline int permutation::getSize() const {
    return size;
}

inline permutation::permutation(const int size) : size(size) {
    _permutation = std::vector(size, -1);
    _inverse_permutation = std::vector(size, -1);
}

inline permutation::permutation(const std::vector<int> &values) : size(values.size()) {
    _permutation = std::vector<int>(-1);
    _inverse_permutation = std::vector<int>(-1);

    for (int i = 0; i < size; i++) {
        if (values[i] != -1) {
            _permutation[i] = values[i];
            _inverse_permutation[values[i]] = i;
        }
    }
}

inline permutation permutation::random(const int size) {
    permutation p(size);
    for (int i = 0; i < size; i++)
        p.set(i, i);

    // Shuffle the vector
    std::random_device rd;
    std::mt19937 g(rd());
    std::ranges::shuffle(p._permutation, g);

    return p;
}

inline int permutation::get(const int index) const {
    return _permutation[index];
}

inline void permutation::set(const int index, const int value) {
    if (contains(value)) {
        throw PermutationException("Value already exists!");
    }
    if (_permutation[index] != -1) {
        throw PermutationException("Position already filled!");
    }
    _permutation[index] = value;
    _inverse_permutation[value] = index;
}

inline int permutation::indexOf(const int element) const {
    return _inverse_permutation[element];
}

inline bool permutation::contains(const int element) const {
    return _inverse_permutation[element] != -1;
}

inline int permutation::deletePosition(const int position) {
    const auto oldValue = _permutation[position];
    _permutation[position] = -1;

    if (oldValue == -1) return -1;

    _inverse_permutation[oldValue] = -1;

    return oldValue;
}

inline int permutation::deleteValue(const int value) {
    const auto oldPosition = _inverse_permutation[value];
    _inverse_permutation[value] = -1;

    if (oldPosition == -1) return -1;

    _permutation[oldPosition] = -1;

    return oldPosition;
}

inline void permutation::swapPositions(const int position1, const int position2) {
    if (position1 == position2) return;
    const auto firstValue = _permutation[position1];
    const auto secondValue = _permutation[position2];
    deletePosition(position1);
    deletePosition(position2);
    set(position1, secondValue);
    set(position2, firstValue);
}

inline void permutation::clear() {
    for (int i = 0; i < size; i++) {
        _permutation[i] = -1;
        _inverse_permutation[i] = -1;
    }
}

inline int permutation::before(const int value) const {
    if (value == size) return _permutation[_permutation.size() - 1];

    const auto index = _inverse_permutation[value];

    if (index == 0) return size;

    return _permutation[index - 1];
}

inline int permutation::after(const int value) const {
    if (value == size) return _permutation[0];

    const auto index = _inverse_permutation[value];
    if (index == _permutation.size() - 1) return size;

    return _permutation[index + 1];
}

inline permutation permutation::clone() const {
    permutation p(size);
    for (int i = 0; i < size; i++) {
        p.set(i, _permutation[i]);
    }
    return p;
}

inline bool permutation::isComplete() const {
    auto contains = std::vector(size, false);

    for (int i = 0; i < size; i++) {
        const auto value = _permutation[i];
        if (value < 0 || value > (size - 1) || contains[value]) {
            return false;
        }
        contains[value] = true;
    }

    return true;
}
