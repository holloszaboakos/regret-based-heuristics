//
// Created by LENOVO on 10/14/2024.
//

#ifndef COST_RECORD_H
#define COST_RECORD_H

struct cost_record {
    cost_record(
        const double &initialCost,
        const double &builtCost,
        const double &optimizedCost,
        const double &optimal
    ) : initialCost(initialCost),
        builtCost(builtCost),
        optimizedCost(optimizedCost),
        optimal(optimal) {
    }

    [[nodiscard]] double getInitialCost() const {return initialCost;}
    [[nodiscard]] double getBuiltCost() const {return builtCost;}
    [[nodiscard]] double getOptimizedCost() const {return optimizedCost;}
    [[nodiscard]] double getOptimal() const {return optimal;}

private:
    const double &initialCost;
    const double &builtCost;
    const double &optimizedCost;
    const double &optimal;
};

#endif //COST_RECORD_H
