//
// Created by LENOVO on 10/14/2024.
//

#ifndef CALC_COST_OF_TSP_H
#define CALC_COST_OF_TSP_H

#include "permutation.h"

double calc_cost_of_tsp(const permutation &solution, const std::vector<std::vector<double> > &distance_matrix) {
    double cost = 0;

    for (int index = 0; index < solution.getSize(); index++) {
        cost += distance_matrix[solution.get(index - 1)][solution.get(index)];
    }
    cost += distance_matrix[solution.get(solution.getSize() - 1)][solution.getSize()];
    cost += distance_matrix[solution.getSize()][solution.get(0)];

    return cost;
}

#endif //CALC_COST_OF_TSP_H
