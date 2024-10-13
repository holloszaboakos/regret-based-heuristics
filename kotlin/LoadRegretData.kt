package hu.raven.puppet.job

import hu.akos.hollo.szabo.collections.asImmutable
import hu.akos.hollo.szabo.math.Permutation
import hu.akos.hollo.szabo.math.asDoubleVector
import hu.akos.hollo.szabo.math.matrix.DoubleMatrix
import hu.akos.hollo.szabo.math.toDoubleVector
import hu.raven.puppet.model.utility.math.GraphEdge
import java.io.File

fun main() {
    val regretRecords = loadRegrets(File("D:\\Research\\Datasets\\tsp64x10_000-regret-2024-04-03"))
    regretRecords.asSequence()
        .map { record ->
            val optimalEdges = record.expectedRegretMatrix
                .mapEachEntryIndexed { columnIndex, rowIndex, value ->
                    GraphEdge(columnIndex, rowIndex, value)
                }
                .flatten()
                .filter { it.sourceNodeIndex != it.targetNodeIndex }
                .filter { it.value == 0.0 }
                .toList()
                .groupBy { it.sourceNodeIndex }
                .mapValues { (_, value) -> value.map { it.targetNodeIndex } }
                .entries
                .sortedBy { it.key }
                .map { it.value }
                .toTypedArray()

            var permutations = mutableListOf<Permutation>()

            optimalEdges.last().forEach { targetNode ->
                permutations.add(
                    Permutation(size = 127).apply { set(0, targetNode) }
                )
            }

            for (nextIndex in 1 until 127) {
                val newPermutations = mutableListOf<Permutation>()
                permutations.forEach { permutation ->
                    val lastValue = permutation[nextIndex - 1]
                    val nextValueOptions = optimalEdges[lastValue]
                        .filter { it != 127 }
                        .filter { !permutation.contains(it) }

                    nextValueOptions.forEach { nextValue ->
                        newPermutations.add(
                            permutation.clone().apply { set(nextIndex, nextValue) }
                        )
                    }
                }
                permutations = newPermutations
            }

            println(permutations)
        }
        .toList()
}

data class RegretData(
    val file: String,
    val distanceMatrix: DoubleMatrix,
    val expectedRegretMatrix: DoubleMatrix,
    val predictedRegretMatrix: DoubleMatrix,
    val optCost: Double,
    val numberOfIterations: Int,
    val initialCost: Long,
    val bestCost: Long
)

fun loadRegrets(sourceFolder: File): List<RegretData> {
    return sourceFolder.listFiles().asSequence()
        .map { file ->
            val distanceMatrix = file.useLines { lines ->
                lines
                    .drop(1)
                    .takeWhile { it.isNotBlank() }
                    .map { line ->
                        line
                            .split(" ")
                            .map { it.toDouble() }
                            .toDoubleArray()
                            .asDoubleVector()
                    }
                    .toList()
                    .toTypedArray()
                    .asImmutable()
                    .let { rows -> DoubleMatrix(rows) }
            }

            val expectedRegretMatrix = file.useLines { lines ->
                var wasStartLabel = false
                lines
                    .dropWhile {
                        wasStartLabel = wasStartLabel || it == "regret:"
                        it == "regret:" || !wasStartLabel
                    }
                    .takeWhile { it.isNotBlank() }
                    .map { line ->
                        line
                            .split(" ")
                            .map { it.toDouble() }
                            .toDoubleArray()
                            .asDoubleVector()
                    }
                    .toList()
                    .toTypedArray()
                    .asImmutable()
                    .let { rows -> DoubleMatrix(rows) }
            }

            val predictedRegretMatrix = file.useLines { lines ->
                var wasStartLabel = false
                lines
                    .dropWhile {
                        wasStartLabel = wasStartLabel || it == "regret_pred:"
                        it == "regret_pred:" || !wasStartLabel
                    }
                    .takeWhile { it.isNotBlank() }
                    .map { line ->
                        line
                            .split(" ")
                            .map { it.toDouble() }
                            .toDoubleArray()
                            .toDoubleVector()
                    }
                    .toList()
                    .toTypedArray()
                    .asImmutable()
                    .let { rows -> DoubleMatrix(rows) }
            }

            val optCost = file.useLines { lines ->
                lines.first { it.startsWith("opt_cost") }.split(" ")[1].toDouble()
            }

            val numberOfIterations = 0
//                file.useLines { lines ->
//                lines.first { it.startsWith("num_iterations") }.split(" ")[1].toInt()
//            }

            val initialCost = 0L
//                file.useLines { lines ->
//                    lines.first { it.startsWith("init_cost") }.split(" ")[1].toDouble().toLong()
//                }

            val bestCost = 0L
//                file.useLines { lines ->
//                    lines.first { it.startsWith("best_cost") }.split(" ")[1].toDouble().toLong()
//                }

            RegretData(
                file = file.absolutePath,
                distanceMatrix = distanceMatrix,
                expectedRegretMatrix = expectedRegretMatrix,
                predictedRegretMatrix = predictedRegretMatrix,
                optCost = optCost,
                numberOfIterations = numberOfIterations,
                initialCost = initialCost,
                bestCost = bestCost,
            )
        }
        .toList()
}

fun loadDoubleMatrices(sourceFolder: File): List<DoubleMatrix> {
    return sourceFolder.listFiles().asSequence()
        .map { file ->
            file.useLines { lines ->
                lines
                    .takeWhile { it.isNotBlank() }
                    .map { line ->
                        line
                            .split(" ")
                            .map { it.toDouble() }
                            .toDoubleArray()
                            .asDoubleVector()
                    }
                    .toList()
                    .toTypedArray()
                    .asImmutable()
                    .let { rows -> DoubleMatrix(rows) }
            }
        }
        .toList()
}