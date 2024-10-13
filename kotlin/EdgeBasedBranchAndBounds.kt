package hu.raven.puppet.logic.step.bruteforce_solver

import hu.akos.hollo.szabo.collections.slice
import hu.akos.hollo.szabo.math.Permutation
import hu.akos.hollo.szabo.math.matrix.DoubleMatrix
import hu.akos.hollo.szabo.math.matrix.IntMatrix
import hu.akos.hollo.szabo.math.vector.IntVector
import hu.akos.hollo.szabo.math.vector.IntVector.Companion.set
import hu.akos.hollo.szabo.math.vector.IntVector2D
import hu.raven.puppet.model.utility.math.GraphEdge
import java.time.Duration
import java.time.LocalDateTime
import kotlin.math.max
import kotlin.math.min


data class EdgeBuilderNode(
    val regretEdgeIndex: Int,
    val regretEdge: GraphEdge<Double>,
    val level: Int,
    val parents: List<EdgeBuilderNode>,
    var lastVisitedChildrenIndex: Int,

    val sequentialRepresentation: IntArray,
    val sequencesFromSource: IntArray,
    val sequencesToTarget: IntArray,

    val pathCost: Double,
)

data class PartialEdgeBuilderNode(
    val regretEdgeIndex: Int,
    val regretEdge: GraphEdge<Double>,
    val sequentialRepresentation: IntArray,
    val sequencesFromSource: IntArray,
    val sequencesToTarget: IntArray,
)

fun edgeBasedBranchAndBounds(
    graph: DoubleMatrix,
    regret: DoubleMatrix,
    stopOnFirstResult: Boolean
): Pair<Permutation, Double> {
    val startTime = LocalDateTime.now()
    //[24, 88, 18, 82, 3, 67, 0, 64, 4, 68, 21, 85, 56, 120, 33, 97, 40, 104, 57, 121, 39, 103, 61, 125, 41, 105, 5, 69, 12, 76, 54, 118, 38, 102, 28, 92, 11, 75, 1, 65, 23, 87, 49, 113, 60, 124, 32, 96, 14, 78, 58, 122, 22, 86, 25, 89, 16, 80, 27, 91, 43, 107, 47, 111, 34, 98, 36, 100, 15, 79, 53, 117, 8, 72, 35, 99, 45, 109, 44, 108, 6, 70, 31, 95, 42, 106, 46, 110, 26, 90, 37, 101, 50, 114, 17, 81, 9, 73, 7, 71, 20, 84, 59, 123, 10, 74, 48, 112, 52, 116, 55, 119, 62, 126, 2, 66, 13, 77, 19, 83, 30, 94, 51, 115, 29, 93, 63]
    var bestSequentialRepresentation = intArrayOf(0)
    var bestCost = Double.MAX_VALUE

    val regretEdges = regret
        .mapEachEntryIndexed { columnIndex, rowIndex, value ->
            GraphEdge(
                sourceNodeIndex = columnIndex,
                targetNodeIndex = rowIndex,
                value = value
            )
        }
        .map {
            it
                .sortedBy { it.value }
                .slice(0 until regret.dimensions.x)
        }
        .flatten()
        .filter { it.sourceNodeIndex != it.targetNodeIndex }
        .sortedBy { it.value }
        .withIndex()
        .toList()

    val regretEdgesSortedByDistance =
        regretEdges //.sortedBy { graph[it.value.sourceNodeIndex][it.value.targetNodeIndex] }
    val regretEdgesSortedByDistanceGroupedBySource = regretEdgesSortedByDistance
        .groupBy { it.value.sourceNodeIndex }
        .entries
        .sortedBy { it.key }
        .map { it.value }
        .toTypedArray()

    val routNodes = regretEdges.map { (regretEdgeIndex, regretEdge) ->
        EdgeBuilderNode(
            regretEdgeIndex = regretEdgeIndex,
            regretEdge = regretEdge,
            level = 0,
            parents = listOf(),
            lastVisitedChildrenIndex = regretEdgeIndex,
            sequentialRepresentation = IntArray(graph.dimensions.x) { -1 }
                .apply {
                    set(
                        regretEdge.sourceNodeIndex,
                        regretEdge.targetNodeIndex,
                    )
                },
            sequencesFromSource = IntArray(graph.dimensions.x) { -1 }
                .apply {
                    set(
                        regretEdge.sourceNodeIndex,
                        regretEdge.targetNodeIndex,
                    )
                },
            sequencesToTarget = IntArray(graph.dimensions.x) { -1 }
                .apply {
                    set(
                        regretEdge.targetNodeIndex,
                        regretEdge.sourceNodeIndex
                    )
                },
            pathCost = graph[regretEdge.sourceNodeIndex, regretEdge.targetNodeIndex],
        )
    }

    routNodes.forEach { routNode ->
        //println(routNode.regretEdge)
        val potentialCost =
            graph[routNode.regretEdge.sourceNodeIndex, routNode.regretEdge.targetNodeIndex].toLong() +
                    nearestNeighbourUnderEstimateCost(
                        regretEdgeIndex = routNode.regretEdgeIndex,
                        regretEdge = routNode.regretEdge,
                        graph = graph,
                        parents = listOf(),
                        sequencesFromSource = routNode.sequencesFromSource,
                        sequencesToTarget = routNode.sequencesToTarget,
                        sequentialRepresentation = routNode.sequentialRepresentation,
                        regretEdgesSortedByDistanceGroupedBySource = regretEdgesSortedByDistanceGroupedBySource
                    )
        if (potentialCost > bestCost) return@forEach

        var currentNode = routNode
        while (true) {
            if (Duration.between(startTime, LocalDateTime.now()).seconds >= 5) {
                return Pair(toPermutation(bestSequentialRepresentation), bestCost)
            }

            //LEAF
            if (currentNode.parents.size == graph.dimensions.x - 2) {
                //println()
                //println("LEAF: ${currentNode.regretEdge}")
                val sequences = currentNode.sequencesFromSource.withIndex().filter { it.value != -1 }
                if (sequences.size != 1) {
                    throw Exception("Hamiltonian path should be built!")
                }
                if (currentNode.pathCost + graph[sequences[0].value][sequences[0].index] < bestCost) {
                    bestSequentialRepresentation = currentNode.sequentialRepresentation
                    bestSequentialRepresentation[sequences[0].value] = sequences[0].index
                    bestCost = currentNode.pathCost + graph[sequences[0].value][sequences[0].index]
                    //println("$bestCost ${toPermutation(bestSequentialRepresentation)}")
                }
                if (stopOnFirstResult)
                    return Pair(toPermutation(bestSequentialRepresentation), bestCost)
            }

            currentNode = findNewNode(
                currentNode = currentNode,
                regretEdges = regretEdges,
                regretEdgesSortedByDistanceGroupedBySource = regretEdgesSortedByDistanceGroupedBySource,
                bestCost = bestCost,
                graph = graph
            ) ?: break

            //println(currentNode.level)
        }
        //println(bestCost)
    }

    return Pair(toPermutation(bestSequentialRepresentation), bestCost)
}

fun findNewNode(
    currentNode: EdgeBuilderNode,
    regretEdges: List<IndexedValue<GraphEdge<Double>>>,
    regretEdgesSortedByDistanceGroupedBySource: Array<List<IndexedValue<GraphEdge<Double>>>>,
    bestCost: Double,
    graph: DoubleMatrix,
): EdgeBuilderNode? {

    (currentNode.parents + currentNode).reversed()
        .forEach { parent ->
            if (parent.level == graph.dimensions.x - 2) return@forEach

            //println("parent level: ${parent.level}")

            val selectedPartialNode = regretEdges
                .asSequence()
                .slice(parent.lastVisitedChildrenIndex + 1 until regretEdges.size)
                .filter { (_, edge) ->
                    parent.parents.none {
                        it.regretEdge.sourceNodeIndex == edge.sourceNodeIndex ||
                                it.regretEdge.targetNodeIndex == edge.targetNodeIndex ||
                                it.regretEdge.targetNodeIndex == edge.sourceNodeIndex && it.regretEdge.sourceNodeIndex == edge.targetNodeIndex
                    }
                }
                .filter { (_, edge) -> parent.regretEdge.sourceNodeIndex != edge.sourceNodeIndex }
                .filter { (_, edge) -> parent.regretEdge.targetNodeIndex != edge.targetNodeIndex }
                .filter { (_, edge) ->
                    parent.regretEdge.targetNodeIndex != edge.sourceNodeIndex ||
                            parent.regretEdge.sourceNodeIndex != edge.targetNodeIndex
                }
                .filter { (_, edge) ->
                    parent.sequencesFromSource[edge.targetNodeIndex] != edge.sourceNodeIndex ||
                            parent.sequencesToTarget[edge.sourceNodeIndex] != edge.targetNodeIndex
                }
                .map {
                    val (regretEdgeIndex, regretEdge) = it
                    val matchingOnSource = parent.sequencesToTarget[regretEdge.sourceNodeIndex]
                    val matchingOnTarget = parent.sequencesFromSource[regretEdge.targetNodeIndex]
                    val newSequence = when {
                        matchingOnSource == -1 && matchingOnTarget == -1 ->
                            GraphEdge(regretEdge.sourceNodeIndex, regretEdge.targetNodeIndex, Unit)

                        matchingOnSource != -1 && matchingOnTarget == -1 ->
                            GraphEdge(matchingOnSource, regretEdge.targetNodeIndex, Unit)

                        matchingOnSource == -1 && matchingOnTarget != -1 ->
                            GraphEdge(regretEdge.sourceNodeIndex, matchingOnTarget, Unit)

                        else ->
                            GraphEdge(matchingOnSource, matchingOnTarget, Unit)
                    }
                    val sequencesToTarget: IntArray = parent.sequencesToTarget.clone().apply {
                        set(regretEdge.sourceNodeIndex, -1)
                        set(newSequence.targetNodeIndex, newSequence.sourceNodeIndex)
                    }
                    val sequencesFromSource: IntArray = parent.sequencesFromSource.clone().apply {
                        set(regretEdge.targetNodeIndex, -1)
                        set(newSequence.sourceNodeIndex, newSequence.targetNodeIndex)
                    }

                    val sequentialRepresentation: IntArray = parent.sequentialRepresentation.clone()
                        .apply { set(regretEdge.sourceNodeIndex, regretEdge.targetNodeIndex) }

                    PartialEdgeBuilderNode(
                        regretEdge = regretEdge,
                        regretEdgeIndex = regretEdgeIndex,
                        sequencesFromSource = sequencesFromSource,
                        sequencesToTarget = sequencesToTarget,
                        sequentialRepresentation = sequentialRepresentation
                    )
                }
                .filter {
                    (parent.pathCost +
                            graph[it.regretEdge.sourceNodeIndex][it.regretEdge.targetNodeIndex] +
                            nearestNeighbourUnderEstimateCost(
                                regretEdgeIndex = it.regretEdgeIndex,
                                regretEdge = it.regretEdge,
                                sequentialRepresentation = it.sequentialRepresentation,
                                regretEdgesSortedByDistanceGroupedBySource = regretEdgesSortedByDistanceGroupedBySource,
                                graph = graph,
                                sequencesFromSource = it.sequencesFromSource,
                                sequencesToTarget = it.sequencesToTarget,
                                parents = parent.parents + parent,
                            )
                            ) <= bestCost
                }
                .firstOrNull()

            if (selectedPartialNode == null) return@forEach

            val newNode = visitNode(
                selectedPartialNode = selectedPartialNode,
                currentNode = parent,
                graph = graph
            )

            parent.lastVisitedChildrenIndex = selectedPartialNode.regretEdgeIndex

            return newNode
        }

    return null
}


fun visitNode(
    selectedPartialNode: PartialEdgeBuilderNode,
    currentNode: EdgeBuilderNode,
    graph: DoubleMatrix,
): EdgeBuilderNode {

    return EdgeBuilderNode(
        regretEdgeIndex = selectedPartialNode.regretEdgeIndex,
        regretEdge = selectedPartialNode.regretEdge,
        parents = currentNode.parents + currentNode,
        pathCost = currentNode.pathCost + graph[selectedPartialNode.regretEdge.sourceNodeIndex][selectedPartialNode.regretEdge.targetNodeIndex],
        level = currentNode.level + 1,
        sequencesToTarget = selectedPartialNode.sequencesToTarget,
        sequencesFromSource = selectedPartialNode.sequencesFromSource,
        sequentialRepresentation = selectedPartialNode.sequentialRepresentation,
        lastVisitedChildrenIndex = selectedPartialNode.regretEdgeIndex
    )
}

fun minimalSpanningTreeCost(
    regretEdgeIndex: Int,
    regretEdge: GraphEdge<Double>,
    graph: IntMatrix,
    regretEdges: List<IndexedValue<GraphEdge<Double>>>,
    parents: List<EdgeBuilderNode>,
    sequencesFromSource: IntArray,
    sequencesToTarget: IntArray,
    sequentialRepresentation: IntArray,
): Long {
    val count = sequentialRepresentation.count { targetNodeIndex -> targetNodeIndex == -1 }
    if (count == 1) {
        return graph[sequencesToTarget.first { it != -1 }, sequencesFromSource.first { it != -1 }].toLong()
    }

    val nodeGrouping = IntVector(graph.dimensions.x) { it }

    sequentialRepresentation
        .withIndex()
        .filter { (_, targetNodeIndex) -> targetNodeIndex != -1 }
        .also { if (it.size == 128) return 0 }
        .forEach { (sourceNodeIndex, targetNodeIndex) ->
            if (targetNodeIndex != -1)
                mergeGroups(nodeGrouping, IntVector2D(nodeGrouping[sourceNodeIndex], nodeGrouping[targetNodeIndex]))
        }

    var addedNonTreeEdge = false

    val spanningTreeCost = regretEdges
        .asSequence()
        .slice(regretEdgeIndex + 1 until regretEdges.size)
        .filter { (_, edge) ->
            parents.none {
                it.regretEdge.sourceNodeIndex == edge.sourceNodeIndex ||
                        it.regretEdge.targetNodeIndex == edge.targetNodeIndex ||
                        it.regretEdge.targetNodeIndex == edge.sourceNodeIndex && it.regretEdge.sourceNodeIndex == edge.targetNodeIndex
            }
        }
        .filter { (_, edge) -> regretEdge.sourceNodeIndex != edge.sourceNodeIndex }
        .filter { (_, edge) -> regretEdge.targetNodeIndex != edge.targetNodeIndex }
        .filter { (_, edge) -> regretEdge.targetNodeIndex != edge.sourceNodeIndex || regretEdge.sourceNodeIndex != edge.targetNodeIndex }
        .filter { (_, edge) ->
            sequencesFromSource[edge.targetNodeIndex] != edge.sourceNodeIndex || sequencesToTarget[edge.sourceNodeIndex] != edge.targetNodeIndex
        }
        .sortedBy { (_, edge) ->
            edge.value
            //graph[edge.sourceNodeIndex, edge.targetNodeIndex]
        }
        .filterIndexed { index, (_, edge) ->
            if (nodeGrouping[edge.sourceNodeIndex] != nodeGrouping[edge.targetNodeIndex]) {
                mergeGroups(
                    nodeGrouping,
                    IntVector2D(nodeGrouping[edge.sourceNodeIndex], nodeGrouping[edge.targetNodeIndex])
                )
                true
            } else if (!addedNonTreeEdge) {
                addedNonTreeEdge = true
                true
            } else {
                false
            }
        }
        .toList()
        .also {
            if (it.count() != count) {
                return Long.MAX_VALUE / 2
            }
        }
        .sumOf { (_, edge) -> graph[edge.sourceNodeIndex, edge.targetNodeIndex].toLong() }

    return spanningTreeCost + 0
}

fun nearestNeighbourUnderEstimateCost(
    regretEdgeIndex: Int,
    regretEdge: GraphEdge<Double>,
    sequentialRepresentation: IntArray,
    sequencesFromSource: IntArray,
    sequencesToTarget: IntArray,
    regretEdgesSortedByDistanceGroupedBySource: Array<List<IndexedValue<GraphEdge<Double>>>>,
    graph: DoubleMatrix,
    parents: List<EdgeBuilderNode>,
): Long {
    val count = sequentialRepresentation.count { targetNodeIndex -> targetNodeIndex == -1 }
    if (count == 1) {
        return graph[sequencesToTarget.first { it != -1 }, sequencesFromSource.first { it != -1 }].toLong()
    }

    return sequentialRepresentation
        .mapIndexed { sourceNodeIndex, targetNodeIndex ->
            if (targetNodeIndex != -1) return@mapIndexed null

            val selected =
                regretEdgesSortedByDistanceGroupedBySource[sourceNodeIndex].asSequence()
                    .filter { (_, edge) ->
                        parents.none {
                            it.regretEdge.sourceNodeIndex == edge.sourceNodeIndex ||
                                    it.regretEdge.targetNodeIndex == edge.targetNodeIndex ||
                                    it.regretEdge.targetNodeIndex == edge.sourceNodeIndex && it.regretEdge.sourceNodeIndex == edge.targetNodeIndex
                        }
                    }
                    .filter { (_, edge) -> regretEdge.sourceNodeIndex != edge.sourceNodeIndex }
                    .filter { (_, edge) -> regretEdge.targetNodeIndex != edge.targetNodeIndex }
                    .filter { (_, edge) -> regretEdge.targetNodeIndex != edge.sourceNodeIndex || regretEdge.sourceNodeIndex != edge.targetNodeIndex }
                    .filter { (_, edge) ->
                        sequencesFromSource[edge.targetNodeIndex] != edge.sourceNodeIndex || sequencesToTarget[edge.sourceNodeIndex] != edge.targetNodeIndex
                    }
                    .firstOrNull { it.index > regretEdgeIndex }

            if (selected == null)
                null
            else
                graph[selected.value.sourceNodeIndex, selected.value.targetNodeIndex].toLong()
        }
        .filterNotNull()
        .apply {
            if (sequentialRepresentation.count { it == -1 } != size) {
                return Long.MAX_VALUE / 2
            }
        }
        .sum()
}

private fun mergeGroups(nodeGrouping: IntVector, groups: IntVector2D) {
    val smallerGroup = min(groups.x, groups.y)
    val biggerGroup = max(groups.x, groups.y)
    (smallerGroup..<nodeGrouping.size)
        .asSequence()
        .filter { nodeGrouping[it] == biggerGroup }
        .forEach { nodeGrouping[it] = smallerGroup }
}

fun toPermutation(bestSequentialRepresentation: IntArray): Permutation {
    val result = Permutation(bestSequentialRepresentation.size - 1)
    result[0] = bestSequentialRepresentation.last()
    (1..<result.size).forEach {
        result[it] = bestSequentialRepresentation[result[it - 1]]
    }
    if (!result.isFormatCorrect()) {
        throw Exception("permutation is malformed")
    }
    return result
}
