//
// Created by LENOVO on 10/5/2024.
//

#ifndef GRAPH_EDGE_H
#define GRAPH_EDGE_H


template<typename T>
struct graph_edge {
    graph_edge(
        const int &sourceNodeIndex,
        const int &targetNodeIndex,
        const T &value
    ): sourceNodeIndex(sourceNodeIndex), targetNodeIndex(targetNodeIndex), value(value) {
    }

    graph_edge(const graph_edge<T> & other) = default;

    graph_edge(graph_edge<T> && other) = default;

    [[nodiscard]] int getSourceNodeIndex() const { return sourceNodeIndex; }
    [[nodiscard]] int getTargetNodeIndex() const { return targetNodeIndex; }
    T getValue() const { return value; }

private:
    int sourceNodeIndex;
    int targetNodeIndex;
    T value;
};


#endif //GRAPH_EDGE_H
