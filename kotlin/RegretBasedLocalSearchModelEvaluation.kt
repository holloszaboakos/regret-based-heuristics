package hu.raven.puppet.job.experiments

import hu.raven.puppet.job.loadRegrets
import hu.raven.puppet.logic.step.bruteforce_solver.edgeBasedBranchAndBounds
import java.io.File
import kotlin.time.measureTimedValue


fun main() {
    (0..0).forEach { modelId ->

        val regretData = loadRegrets(
            File(
                "H:\\by-domain\\work_and_learning\\PhD\\research\\datasets",
                "2024_10_02_output_ATSP_trained_50_size_samples_100\\output_ATSP_trained_50_size_samples_100_size_250"
            )
        )

        val costDataPerNoiseLevelAndInstance =
            regretData
                .take(100)
                .mapIndexed { regretRecordIndex, regretRecord ->
                    measureTimedValue {
                        println("Regret record! $regretRecordIndex")
                        val (_, initialCost) = measureTimedValue {
                            edgeBasedBranchAndBounds(
                                regretRecord.distanceMatrix,
                                regretRecord.predictedRegretMatrix,//.transpose(),
                                true,
                            )
                        }.let {
                            println("Finding initial solution took: ${it.duration}")
                            it.value
                        }
                        val (permutation, cost) = measureTimedValue {
                            edgeBasedBranchAndBounds(
                                regretRecord.distanceMatrix,
                                regretRecord.predictedRegretMatrix,//.transpose(),
                                false,
                            )
                        }.let {
                            println("Refining with branch and bounds took: ${it.duration}")
                            it.value
                        }

                        println(
                            "COST ${
                                calcCostOfTspSolution(
                                    permutation,
                                    regretRecord.distanceMatrix
                                )
                            }"
                        )

                        var bestCost = cost

                        repeat(1) {
                            //println(it)
                            bestCost = threeOptCycle(
                                permutation,
                                regretRecord,
                                regretRecord.predictedRegretMatrix.transpose(),
                                cost
                            )
                        }

                        CostRecord(
                            initialCost = initialCost,
                            builtCost = cost,
                            optimizedCost = bestCost,
                            optimal = regretRecord.optCost
                        )
                    }.also {
                        println("processing instance took: ${it.duration}")
                    }
                }
                .toList()
        println(
            costDataPerNoiseLevelAndInstance.map { it.value.optimizedCost / it.value.optimal - 1.0 }.toDoubleArray().statistics()
        )
        println(
            costDataPerNoiseLevelAndInstance.map { it.duration.inWholeMilliseconds.toDouble() }.toDoubleArray().statistics()
        )
    }
}