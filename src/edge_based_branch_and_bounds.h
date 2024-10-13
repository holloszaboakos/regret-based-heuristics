//
// Created by LENOVO on 10/10/2024.
//

#ifndef EDGE_BASED_BRANCH_AND_BOUNDS_H
#define EDGE_BASED_BRANCH_AND_BOUNDS_H
#include <map>
#include <utility>
#include <vector>

#include "graph_edge.h"
#include "permutation.h"
#include "regret_data.h"
#include "utility/first.h"
#include "utility/first_or_default.h"
#include "utility/first_or_null.h"
//#include "windows.h"
// #include "psapi.h"

struct edge_builder_node {
    edge_builder_node(
        const int regretEdgeIndex,
        const graph_edge<double> regretEdge,
        const int level,
        const std::vector<edge_builder_node *> parents,
        const int lastVisitedChildrenIndex,
        std::vector<int> sequentialRepresentation,
        std::vector<int> sequencesFromSource,
        std::vector<int> sequencesToTarget,
        const double pathCost
    ): regretEdgeIndex(regretEdgeIndex),
       regretEdge(regretEdge),
       level(level),
       parents(parents),
       lastVisitedChildrenIndex(lastVisitedChildrenIndex),
       sequentialRepresentation(std::move(sequentialRepresentation)),
       sequencesFromSource(std::move(sequencesFromSource)),
       sequencesToTarget(std::move(sequencesToTarget)),
       pathCost(pathCost) {
    }

    edge_builder_node(const edge_builder_node &other) = default;

    edge_builder_node(edge_builder_node &&other) = default;

    edge_builder_node &operator=(const edge_builder_node &) = default;

    edge_builder_node &operator=(edge_builder_node &&) = default;

    [[nodiscard]] int getRegretEdgeIndex() const { return regretEdgeIndex; }
    [[nodiscard]] graph_edge<double> getRegretEdge() const { return regretEdge; }
    [[nodiscard]] int getLevel() const { return level; }
    [[nodiscard]] std::vector<edge_builder_node *> getParents() const { return parents; }
    [[nodiscard]] int getLastVisitedChildrenIndex() const { return lastVisitedChildrenIndex; }
    [[nodiscard]] std::vector<int> getSequentialRepresentation() const { return sequentialRepresentation; }
    [[nodiscard]] std::vector<int> getSequenceFromSource() const { return sequencesFromSource; }
    [[nodiscard]] std::vector<int> getSequenceToTarget() const { return sequencesToTarget; }
    [[nodiscard]] double getPathCost() const { return pathCost; }

    void setLastVisitedChildrenIndex(int last_visited_children_index) {
        this->lastVisitedChildrenIndex = last_visited_children_index;
    }

private:
    int regretEdgeIndex;
    graph_edge<double> regretEdge;
    int level;
    std::vector<edge_builder_node *> parents;
    int lastVisitedChildrenIndex;
    std::vector<int> sequentialRepresentation;
    std::vector<int> sequencesFromSource;
    std::vector<int> sequencesToTarget;
    double pathCost;
};

struct partial_edge_builder_node {
    partial_edge_builder_node(
        const int regretEdgeIndex,
        const graph_edge<double> regretEdge,
        std::vector<int> sequentialRepresentation,
        std::vector<int> sequencesFromSource,
        std::vector<int> sequencesToTarget
    ): regretEdgeIndex(regretEdgeIndex),
       regretEdge(regretEdge),
       sequentialRepresentation(std::move(sequentialRepresentation)),
       sequencesFromSource(std::move(sequencesFromSource)),
       sequencesToTarget(std::move(sequencesToTarget)) {
    }

    [[nodiscard]] int getRegretEdgeIndex() const { return regretEdgeIndex; }
    [[nodiscard]] graph_edge<double> getRegretEdge() const { return regretEdge; }
    [[nodiscard]] std::vector<int> getSequentialRepresentation() const { return sequentialRepresentation; }
    [[nodiscard]] std::vector<int> getSequenceFromSource() const { return sequencesFromSource; }
    [[nodiscard]] std::vector<int> getSequenceToTarget() const { return sequencesToTarget; }

private:
    int regretEdgeIndex;
    graph_edge<double> regretEdge;
    std::vector<int> sequentialRepresentation;
    std::vector<int> sequencesFromSource;
    std::vector<int> sequencesToTarget;
};


long nearestNeighbourUnderEstimateCost(
    int regret_edge_index,
    graph_edge<double> regret_edge,
    const vector<vector<double> > &graph,
    const vector<edge_builder_node *> &parents,
    const vector<int> &sequence_from_source,
    const vector<int> &sequence_to_target,
    const vector<int> &sequential_representation,
    const std::vector<std::vector<std::pair<int, graph_edge<double> > > > &regretEdgesSortedByDistanceGroupedBySource
) {
    int count = std::ranges::count_if(
        sequential_representation.begin(),
        sequential_representation.end(),
        [](const int &targetNodeIndex) { return targetNodeIndex < 0; }
    );

    int first_not_null_target =
            sequence_to_target
            | std::views::filter([](int it) { return it >= 0; })
            | first;
    int first_not_null_source =
            sequence_from_source
            | views::filter([](int it) { return it >= 0; })
            | first;

    if (count == 1) {
        return floor(graph[first_not_null_target][first_not_null_source]);
    }

    vector<long> values = std::views::iota(0, static_cast<int>(sequential_representation.size()))
                          | std::views::transform([sequential_representation](int index) {
                              return pair(index, sequential_representation[index]);
                          })
                          | std::views::transform([&](pair<int, int> indexedValue)-> long {
                              auto [sourceNodeIndex, targetNodeIndex] = indexedValue;
                              if (targetNodeIndex < 0) { return std::numeric_limits<int>::min(); }

                              auto selected =
                                      regretEdgesSortedByDistanceGroupedBySource[sourceNodeIndex]
                                      | std::views::filter([parents](const std::pair<int, graph_edge<double> > &it) {
                                          for (const auto &parent: parents) {
                                              if (
                                                  parent->getRegretEdge().getSourceNodeIndex() == it.second.
                                                  getSourceNodeIndex() ||
                                                  parent->getRegretEdge().getTargetNodeIndex() == it.second.
                                                  getTargetNodeIndex() ||
                                                  (parent->getRegretEdge().getTargetNodeIndex() == it.second.
                                                   getSourceNodeIndex() &&
                                                   parent->getRegretEdge().getSourceNodeIndex() == it.second.
                                                   getTargetNodeIndex())
                                              ) {
                                                  return false;
                                              }
                                          }
                                          return true;
                                      })
                                      | views::filter([regret_edge](const std::pair<int, graph_edge<double> > &it) {
                                          return regret_edge.getSourceNodeIndex() != it.second.getSourceNodeIndex();
                                      })
                                      | views::filter([regret_edge](const std::pair<int, graph_edge<double> > &it) {
                                          return regret_edge.getTargetNodeIndex() != it.second.getTargetNodeIndex();
                                      })
                                      | views::filter([regret_edge](const std::pair<int, graph_edge<double> > &it) {
                                          return regret_edge.getTargetNodeIndex() != it.second.getSourceNodeIndex() ||
                                                 regret_edge.getSourceNodeIndex() != it.second.getTargetNodeIndex();
                                      })
                                      | views::filter(
                                          [sequence_from_source,sequence_to_target ](
                                      const std::pair<int, graph_edge<double> > &it) {
                                              return sequence_from_source[it.second.getTargetNodeIndex()] != it.second.
                                                     getSourceNodeIndex() ||
                                                     sequence_to_target[it.second.getSourceNodeIndex()] != it.second.
                                                     getTargetNodeIndex();
                                          }
                                      )
                                      | first_or_default(
                                          pair(std::numeric_limits<int>::min(), graph_edge(
                                                   std::numeric_limits<int>::min(),
                                                   std::numeric_limits<int>::min(),
                                                   std::numeric_limits<double>::min()
                                               ))
                                      );

                              if (selected.first == std::numeric_limits<int>::min())
                                  return std::numeric_limits<long>::min();

                              return static_cast<long>(floor(
                                  graph[selected.second.getSourceNodeIndex()][selected.second.getTargetNodeIndex()]));
                          })
                          | std::views::filter([](const long it) { return it > 0; })
                          | ranges::to<vector<long> >();

    const auto missing_count = (
        sequential_representation
        | views::filter([](long it) { return it < 0; })
        | ranges::to<vector<long> >()
    ).size();

    if (missing_count != values.size()) {
        return std::numeric_limits<long>::max() / 2;
    }

    return std::accumulate(values.begin(), values.end(), 0L);
}

inline permutation toPermutation(const std::vector<int> &best_sequential_representation) {
    permutation result(static_cast<int>(best_sequential_representation.size()) - 1);
    result.set(0, best_sequential_representation[best_sequential_representation.size() - 1]);
    for (int it = 1; it < result.getSize(); it++) {
        result.set(it, best_sequential_representation[result.get(it - 1)]);
    }
    if (!result.isComplete()) {
        cout << "WRONG PERMUTATION FORMAT!" << endl;
    }

    return result;
}

inline edge_builder_node *visit_node(
    const optional<partial_edge_builder_node>::value_type &selectedPartialNode,
    edge_builder_node &currentNode,
    const std::vector<std::vector<double> > &graph
) {
    std::vector<edge_builder_node *> parents = std::vector(currentNode.getParents());
    parents.push_back(&currentNode);
    auto pathCost = currentNode.getPathCost() +
                    graph[selectedPartialNode.getRegretEdge().getSourceNodeIndex()][selectedPartialNode.getRegretEdge().
                        getTargetNodeIndex()];

    return new edge_builder_node{
        selectedPartialNode.getRegretEdgeIndex(),
        selectedPartialNode.getRegretEdge(),
        currentNode.getLevel() + 1,
        parents,
        selectedPartialNode.getRegretEdgeIndex(),
        selectedPartialNode.getSequentialRepresentation(),
        selectedPartialNode.getSequenceFromSource(),
        selectedPartialNode.getSequenceToTarget(),
        pathCost
    };
}

inline std::optional<edge_builder_node *> findNewNode(
    edge_builder_node *current_node,
    const std::vector<std::pair<int, graph_edge<double> > > &regret_edges,
    const std::vector<std::vector<std::pair<int, graph_edge<double> > > > &regretEdgesSortedByDistanceGroupedBySource,
    double best_cost,
    const std::vector<std::vector<double> > &graph
) {
    auto now = chrono::high_resolution_clock::now();

    auto parents = std::vector(current_node->getParents());
    parents.push_back(current_node);
    for (auto current = parents.rbegin(); current != parents.rend(); ++current) {
        auto parent = *current;
        if (parent->getLevel() == graph.size() - 2) continue;

        auto selectedPartialNode =
                regret_edges
                | views::drop(parent->getLastVisitedChildrenIndex() + 1)
                | views::filter([parent](const std::pair<int, graph_edge<double> > &regret_edge) {
                    for (const auto &it: parent->getParents()) {
                        if (it->getRegretEdge().getSourceNodeIndex() == regret_edge.second.getSourceNodeIndex() ||
                            it->getRegretEdge().getTargetNodeIndex() == regret_edge.second.getTargetNodeIndex() ||
                            (it->getRegretEdge().getSourceNodeIndex() == regret_edge.second.getTargetNodeIndex() &&
                             it->getRegretEdge().getTargetNodeIndex() == regret_edge.second.getSourceNodeIndex())) {
                            return false;
                        }
                    }
                    return true;
                })
                | views::filter([parent](const std::pair<int, graph_edge<double> > &regret_edge) {
                    return parent->getRegretEdge().getSourceNodeIndex() != regret_edge.second.getSourceNodeIndex();
                })
                | views::filter([parent](const std::pair<int, graph_edge<double> > &regret_edge) {
                    return parent->getRegretEdge().getTargetNodeIndex() != regret_edge.second.getTargetNodeIndex();
                })
                | views::filter([parent](const std::pair<int, graph_edge<double> > &regret_edge) {
                    return parent->getRegretEdge().getTargetNodeIndex() != regret_edge.second.getSourceNodeIndex() ||
                           parent->getRegretEdge().getSourceNodeIndex() != regret_edge.second.getTargetNodeIndex();
                })
        // sequencesFromSource[edge.targetNodeIndex] != edge.sourceNodeIndex || sequencesToTarget[edge.sourceNodeIndex] != edge.targetNodeIndex
                | views::filter([parent](const std::pair<int, graph_edge<double> > &regret_edge) {
                    return parent->getSequenceFromSource()[regret_edge.second.getTargetNodeIndex()] != regret_edge.second.getSourceNodeIndex() ||
                           parent->getSequenceToTarget()[regret_edge.second.getSourceNodeIndex()] != regret_edge.second.getTargetNodeIndex();
                })
                | views::transform([parent](const std::pair<int, graph_edge<double> > &it) {
                    auto [regretEdgeIndex, regretEdge] = it;
                    auto matchingOnSource = parent->getSequenceToTarget()[regretEdge.getSourceNodeIndex()];
                    auto matchingOnTarget = parent->getSequenceFromSource()[regretEdge.getTargetNodeIndex()];
                    pair<int, int> new_sequence = pair(
                        std::numeric_limits<int>::min(),
                        std::numeric_limits<int>::min()
                    );

                    if (matchingOnSource < 0 && matchingOnTarget < 0) {
                        new_sequence = pair(regretEdge.getSourceNodeIndex(), regretEdge.getTargetNodeIndex());
                    } else if (matchingOnSource >= 0 && matchingOnTarget < 0) {
                        new_sequence = pair(matchingOnSource, regretEdge.getTargetNodeIndex());
                    } else if (matchingOnSource < 0 && matchingOnTarget >= 0) {
                        new_sequence = pair(regretEdge.getSourceNodeIndex(), matchingOnTarget);
                    } else {
                        new_sequence = pair(matchingOnSource, matchingOnTarget);
                    }

                    std::vector<int> sequencesToTarget = std::vector(parent->getSequenceToTarget());
                    sequencesToTarget[regretEdge.getSourceNodeIndex()] = -1;
                    sequencesToTarget[new_sequence.second] = new_sequence.first;

                    std::vector<int> sequencesFromSource = std::vector(parent->getSequenceFromSource());
                    sequencesFromSource[regretEdge.getTargetNodeIndex()] = -1;
                    sequencesFromSource[new_sequence.first] = new_sequence.second;

                    std::vector<int> sequentialRepresentation = std::vector(parent->getSequentialRepresentation());
                    sequentialRepresentation[regretEdge.getSourceNodeIndex()] = regretEdge.getTargetNodeIndex();

                    return partial_edge_builder_node(
                        regretEdgeIndex,
                        regretEdge,
                        sequentialRepresentation,
                        sequencesFromSource,
                        sequencesToTarget
                    );
                })
                | views::filter(
                    [parent,graph,regretEdgesSortedByDistanceGroupedBySource,best_cost
                    ](const partial_edge_builder_node &node) {
                        const auto parent_path = parent->getPathCost();
                        const auto regret_edge_cost = graph[node.getRegretEdge().getSourceNodeIndex()][node.
                            getRegretEdge().
                            getTargetNodeIndex()];

                        auto local_parents = vector(parent->getParents());
                        local_parents.push_back(parent);
                        const auto underEstimateCost = nearestNeighbourUnderEstimateCost(
                            node.getRegretEdgeIndex(),
                            node.getRegretEdge(),
                            graph,
                            local_parents,
                            node.getSequenceFromSource(),
                            node.getSequenceToTarget(),
                            node.getSequentialRepresentation(),
                            regretEdgesSortedByDistanceGroupedBySource
                        );
                        return parent_path + regret_edge_cost + underEstimateCost <= best_cost;
                    })
                | first_or_null;

        if (!selectedPartialNode.has_value()) {
            continue;
        }

        edge_builder_node *new_node = visit_node(
            selectedPartialNode.value(),
            *parent,
            graph
        );

        parent->setLastVisitedChildrenIndex(selectedPartialNode->getRegretEdgeIndex());

        if (parent->getRegretEdgeIndex() != parents[parents.size() - 1]->getRegretEdgeIndex()) {
            for (auto parentIndex = parent->getParents().size() + 1; parentIndex < parents.size(); parentIndex++) {
                delete parents[parentIndex];
            }
        }

        return new_node;
    }

    return std::nullopt;
}

inline pair<permutation, double> edge_based_branch_and_bounds(
    const vector<vector<double> > &graph,
    const vector<vector<double> > &regret,
    bool stopOnFirstResult
) {
    auto start = std::chrono::high_resolution_clock::now();
    auto bestSequentialRepresentation = vector(graph.size(), std::numeric_limits<int>::min());
    auto bestCost = numeric_limits<double>::max();

    vector<graph_edge<double> > flattened_edges =
            std::views::iota(0, static_cast<int>(regret.size()))
            | std::views::transform([regret](const int columnIndex) {
                return std::views::iota(0, static_cast<int>(regret[columnIndex].size()))
                       | std::views::transform([columnIndex, regret](const int rowIndex) {
                           return graph_edge(
                               columnIndex,
                               rowIndex,
                               regret[columnIndex][rowIndex]
                           );
                       })
                       | ranges::to<vector<graph_edge<double> > >();
            })
            | std::views::transform([](vector<graph_edge<double> > column) {
                std::ranges::sort(column, [](const graph_edge<double> &a, const graph_edge<double> &b) {
                    return a.getValue() < b.getValue();
                });
                return column;
            })
            | std::views::join
            | std::views::filter([](const graph_edge<double> &value) {
                return value.getSourceNodeIndex() != value.getTargetNodeIndex();
            })
            | ranges::to<vector<graph_edge<double> > >();

    std::ranges::sort(
        flattened_edges.begin(),
        flattened_edges.end(),
        [](const graph_edge<double> &a, const graph_edge<double> &b) {
            return a.getValue() < b.getValue();
        }
    );

    vector<pair<int, graph_edge<double> > > regret_edges =
            views::zip(views::iota(0, static_cast<int>(flattened_edges.size())), flattened_edges)
            | ranges::to<vector<pair<int, graph_edge<double> > > >();

    vector<vector<pair<int, graph_edge<double> > > > regretEdgesSortedByDistanceGroupedBySource =
            vector<vector<pair<int, graph_edge<double> > > >(graph.size());
    //goup by
    //array<T> -> map<K,array<T>>
    vector<pair<int, pair<int, graph_edge<double> > > > tmp =
            regret_edges
            | views::transform([](pair<int, graph_edge<double> > item) {
                return pair(item.second.getSourceNodeIndex(), item);
            })
            | std::ranges::to<vector<pair<int, pair<int, graph_edge<double> > > > >();

    for (auto [key, value]: tmp) {
        if (regretEdgesSortedByDistanceGroupedBySource[key].size() != 0) {
            regretEdgesSortedByDistanceGroupedBySource[key].push_back(value);
        } else {
            regretEdgesSortedByDistanceGroupedBySource[key] = vector(1, value);
        }
    }

    tmp = vector<pair<int, pair<int, graph_edge<double> > > >();

    vector<edge_builder_node> routeNodes =
            regret_edges
            | views::transform([graph](pair<int, graph_edge<double> > item) {
                auto [regretEdgeIndex, regretEdge] = item;

                auto sequentialRepresentation = vector(graph.size(), -1);
                sequentialRepresentation[regretEdge.getSourceNodeIndex()] = regretEdge.getTargetNodeIndex();
                auto sequencesFromSource = vector(graph.size(), -1);
                sequencesFromSource[regretEdge.getSourceNodeIndex()] = regretEdge.getTargetNodeIndex();
                auto sequencesToTarget = vector(graph.size(), -1);
                sequencesToTarget[regretEdge.getTargetNodeIndex()] = regretEdge.getSourceNodeIndex();

                return edge_builder_node(
                    regretEdgeIndex,
                    regretEdge,
                    0,
                    std::vector<edge_builder_node *>{},
                    regretEdgeIndex,
                    sequentialRepresentation,
                    sequencesFromSource,
                    sequencesToTarget,
                    graph[regretEdge.getSourceNodeIndex()][regretEdge.getTargetNodeIndex()]
                );
            })
            | std::ranges::to<vector<edge_builder_node> >();

    for (auto routeNode: routeNodes) {
        auto potentialCost = graph[routeNode.getRegretEdge().getSourceNodeIndex()][routeNode.getRegretEdge().
                                 getTargetNodeIndex()]
                             + nearestNeighbourUnderEstimateCost(
                                 routeNode.getRegretEdgeIndex(),
                                 routeNode.getRegretEdge(),
                                 graph,
                                 std::vector<edge_builder_node *>{},
                                 routeNode.getSequenceFromSource(),
                                 routeNode.getSequenceToTarget(),
                                 routeNode.getSequentialRepresentation(),
                                 regretEdgesSortedByDistanceGroupedBySource
                             );
        if (potentialCost > bestCost) continue;
        auto currentNode = std::optional{&routeNode};

        while (true) {

            //LEAF
            if (currentNode.value()->getParents().size() == graph.size() - 2) {
                const vector<pair<int, int> > sequences =
                        views::zip(
                            std::views::iota(0, static_cast<int>(currentNode.value()->getSequenceFromSource().size())),
                            currentNode.value()->getSequenceFromSource()
                        )
                        | std::views::filter([](const pair<int, int> &indexedValue) {
                            return indexedValue.second >= 0;
                        })
                        | ranges::to<vector<pair<int, int> > >();

                if (sequences.size() != 1) {
                    cout << "Hamiltonian path should be built!" << endl;
                }

                if (currentNode.value()->getPathCost() + graph[sequences[0].second][sequences[0].first] < bestCost) {
                    bestSequentialRepresentation = currentNode.value()->getSequentialRepresentation();
                    bestSequentialRepresentation[sequences[0].second] = sequences[0].first;
                    bestCost = currentNode.value()->getPathCost() + graph[sequences[0].second][sequences[0].first];
                }

                if (stopOnFirstResult) {
                    cout << "DONE-" << duration_cast<chrono::seconds>(std::chrono::high_resolution_clock::now() - start).count() << endl;
                    return pair(toPermutation(bestSequentialRepresentation), bestCost);
                }
            }

            //TODO: extremely sus
            //TODO: this takes too much time!!!
            currentNode = findNewNode(
                currentNode.value(),
                regret_edges,
                regretEdgesSortedByDistanceGroupedBySource,
                bestCost,
                graph
            );

            if (!currentNode.has_value()) break;
        }
    }

    return {toPermutation(bestSequentialRepresentation), bestCost};
}

#endif //EDGE_BASED_BRANCH_AND_BOUNDS_H
