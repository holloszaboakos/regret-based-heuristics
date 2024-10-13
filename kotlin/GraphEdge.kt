package hu.raven.puppet.model.utility.math

data class GraphEdge<E>(
    val sourceNodeIndex: Int,
    val targetNodeIndex: Int,
    val value: E
)