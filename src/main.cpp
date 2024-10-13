#include <iostream>

#include "edge_based_branch_and_bounds.h"
#include "permutation.h"
#include "regret_data.h"

int main() {
    std::cout << "Hello, World!" << std::endl;
    auto p = permutation(5);
    p.set(4, 4);
    auto regrets = loadRegrets(
        "H:\\by-domain\\work_and_learning\\PhD\\research\\datasets\\2024_10_02_output_ATSP_trained_50_size_samples_100\\output_ATSP_trained_50_size_samples_100_size_250");
    auto result = edge_based_branch_and_bounds(
        regrets[0].getDistanceMatrix(),
        regrets[0].getPredictedRegretMatrix(),
        true
    );
    return 0;
}
