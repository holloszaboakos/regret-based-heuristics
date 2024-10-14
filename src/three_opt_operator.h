//
// Created by LENOVO on 10/14/2024.
//

#ifndef THREE_OPT_OPERATOR_H
#define THREE_OPT_OPERATOR_H
#include <ranges>

#include "permutation.h"

inline void apply_three_opt(
    permutation &solution,
    const std::vector<int> &positions
) {
    std::vector<std::vector<int> > sections = {
        std::views::iota(0, positions[0] + 1)
        | std::views::transform([&solution](auto index) { return solution.get(index); })
        | std::ranges::to<std::vector<int> >(),
        std::views::iota(positions[1] + 1, positions[2] - positions[1])
        | std::views::transform([&solution](auto index) { return solution.get(index); })
        | std::ranges::to<std::vector<int> >(),
        std::views::iota(positions[0] + 1, positions[1] - positions[0])
        | std::views::transform([&solution](auto index) { return solution.get(index); })
        | std::ranges::to<std::vector<int> >(),
        std::views::iota(positions[3], solution.getSize() - positions[3] + 1)
        | std::views::transform([&solution](auto index) { return solution.get(index); })
        | std::ranges::to<std::vector<int> >()
    };

    solution.clear();

    int count = 0;
    for (int i = 0; i < 4; i++) {
        for (const int value : sections[i]) {
            solution.set(count, value);
            count++;
        }
    }
}


inline void revert_three_opt(
    permutation &solution,
    const std::vector<int> &positions
) {
    const auto middlePosition = positions[0] + positions[2] - positions[1];
    auto reverting_positions = vector(positions);
    reverting_positions[1]=middlePosition;
    apply_three_opt(solution, reverting_positions);
}

#endif //THREE_OPT_OPERATOR_H
