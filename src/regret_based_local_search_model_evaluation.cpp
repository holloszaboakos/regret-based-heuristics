#include <iostream>

#include "calc_cost_of_tsp.h"
#include "cost_record.h"
#include "edge_based_branch_and_bounds.h"
#include "permutation.h"
#include "regret_data.h"
#include "three_opt_cycle.h"

int main() {
    for (int model_index = 0; model_index <= 1; model_index++) {
        auto regretData = loadRegrets(
            "H:\\by-domain\\work_and_learning\\PhD\\research\\datasets\\2024_10_02_output_ATSP_trained_50_size_samples_100\\output_ATSP_trained_50_size_samples_100_size_250"
        );

        vector<cost_record> results =
                views::zip(views::iota(0, static_cast<int>(regretData.size())), regretData)
                | views::take(100)
                | views::transform([](std::pair<int, regret_data> item) {
                    auto [regretRecordIndex, regretRecord] = item;
                    cout << "Regret record! " << regretRecordIndex << endl;
                    auto [_,initial_cost] = edge_based_branch_and_bounds(
                        regretRecord.getDistanceMatrix(),
                        regretRecord.getPredictedRegretMatrix(),
                        true
                    );
                    auto [solution,cost] = edge_based_branch_and_bounds(
                        regretRecord.getDistanceMatrix(),
                        regretRecord.getPredictedRegretMatrix(),
                        false
                    );
                    cout << "COST: " << calc_cost_of_tsp(
                        solution, regretRecord.getDistanceMatrix()) << endl;
                    auto best_cost = cost;

                    for (auto repeat = 0; repeat < 1; repeat++) {
                        best_cost = three_opt_cycle(
                            solution,
                            regretRecord,
                            regretRecord.getPredictedRegretMatrix(),
                            cost
                        );
                    }

                    return cost_record(initial_cost, cost, best_cost, regretRecord.getOptCost());
                })
                | ranges::to<vector<cost_record> >();

        cout << "initial,pre_optimized,optimized,optimal" << endl;
        for (auto result: results) {
            cout << result.getInitialCost() <<
                    "," << result.getBuiltCost() <<
                    "," << result.getOptimizedCost() <<
                    "," << result.getOptimal() <<
                    endl;
        }
    }
}
