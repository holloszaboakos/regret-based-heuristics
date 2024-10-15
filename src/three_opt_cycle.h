//
// Created by LENOVO on 10/14/2024.
//

#ifndef THREE_OPT_CYCLE_H
#define THREE_OPT_CYCLE_H

#include "calc_cost_of_tsp.h"
#include "graph_edge.h"
#include "permutation.h"
#include "regret_data.h"
#include "three_opt_operator.h"

double three_opt_cycle(
    permutation &solution,
    const regret_data &regret_record,
    const vector<vector<double> > &regret_matrix_with_noise,
    const double &cost
) {
    double best_cost = cost;
    auto edges = std::vector<graph_edge<double> >(solution.getSize() + 1, graph_edge<double>(-1,-1,-1.0));
    for (int i = 1; i < solution.getSize(); ++i) {
        edges.emplace_back(
            solution.get(i - 1),
            solution.get(i),
            regret_matrix_with_noise[solution.get(i - 1)][solution.get(i)]
        );
    }
    edges.emplace_back(
        solution.get(solution.getSize() - 1),
        solution.getSize(),
        regret_matrix_with_noise[solution.get(solution.getSize() - 1)][solution.getSize()]
    );
    edges.emplace_back(
        solution.getSize(),
        solution.get(0),
        regret_matrix_with_noise[solution.getSize()][solution.get(0)]
    );


    vector<pair<int, graph_edge<double> > > edges_sorted_with_index =
            std::views::zip(std::views::iota(0, static_cast<int>(edges.size())), edges)
            | ranges::to<vector<pair<int, graph_edge<double> > > >();

    std::sort(
        edges_sorted_with_index.begin(),
        edges_sorted_with_index.end(),
        [](pair<int, graph_edge<double> > a, pair<int, graph_edge<double> > b) {
            return a.second.getValue() < b.second.getValue();
        }
    );

    for (auto i = 0; i < static_cast<int>(edges_sorted_with_index.size()) - 2; i++) {
        for (auto j = i + 1; j < static_cast<int>(edges_sorted_with_index.size()) - 1; j++) {
            for (auto k = j + 1; k < static_cast<int>(edges_sorted_with_index.size()); k++) {
                std::vector selected_edges = {edges[i], edges[j], edges[k]};
                std::vector selected_positions = {
                    edges_sorted_with_index[i].first,
                    edges_sorted_with_index[j].first,
                    edges_sorted_with_index[k].first
                };
                ranges::sort(selected_positions);
                apply_three_opt(solution, selected_positions);
                auto new_cost = calc_cost_of_tsp(solution, regret_record.getDistanceMatrix());
                if (new_cost < best_cost) {
                    best_cost = new_cost;
                } else {
                    revert_three_opt(solution, selected_positions);
                }
            }
        }
    }

    return best_cost;
}

#endif //THREE_OPT_CYCLE_H
