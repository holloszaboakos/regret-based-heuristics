package hu.akos.hollo.szabo.math

data class Permutation(override val size: Int) : List<Int> {

    val indices: IntRange get() = permutation.indices
    private val permutation: IntArray = IntArray(size) { -1 }
    private val inversePermutation: IntArray = IntArray(size) { -1 }

    constructor(initialPermutation: IntArray) : this(initialPermutation.size) {
        initialPermutation.forEachIndexed { index, value ->
            if (value != -1) {
                permutation[index] = value
                inversePermutation[value] = index
            }
        }
    }

    constructor(initialPermutation: List<Int>) : this(initialPermutation.size) {
        initialPermutation.forEachIndexed { index, value ->
            if (value != -1) {
                permutation[index] = value
                inversePermutation[value] = index
            }
        }
    }

    companion object {
        fun random(size: Int) = (0..<size).shuffled().toIntArray().asPermutation()
    }

    //Implement List
    override fun containsAll(elements: Collection<Int>): Boolean = elements.all(permutation::contains)
    override operator fun get(index: Int) = permutation[index]
    override fun isEmpty(): Boolean = size == 0
    override fun iterator(): Iterator<Int> = permutation.iterator()
    override fun listIterator() = permutation.asList().listIterator()
    override fun listIterator(index: Int): ListIterator<Int> = permutation.asList().listIterator(index)
    override fun subList(fromIndex: Int, toIndex: Int): List<Int> = permutation.slice(fromIndex..<toIndex)
    override fun lastIndexOf(element: Int): Int = indexOf(element)
    operator fun set(index: Int, value: Int) {
        if (contains(value)) {
            throw Exception("Value already exists!")
        }
        if (permutation[index] != -1) {
            throw Exception("Position already filled!")
        }

        permutation[index] = value
        inversePermutation[value] = index
    }

    override fun indexOf(element: Int): Int = inversePermutation[element]
    override fun contains(element: Int): Boolean = inversePermutation[element] != -1

    //Custom Implementations
    fun deletePosition(index: Int): Int {
        val oldValue = permutation[index]
        permutation[index] = -1

        if (oldValue == -1) {
            return -1
        }

        inversePermutation[oldValue] = -1

        return oldValue
    }

    fun deleteValue(value: Int): Int {
        val oldPosition = inversePermutation[value]
        inversePermutation[value] = -1

        if (oldPosition == -1) {
            return -1
        }

        permutation[oldPosition] = -1

        return oldPosition
    }

    fun swapValues(firstIndex: Int, secondIndex: Int) {
        if (firstIndex == secondIndex) return
        val firstValue = permutation[firstIndex]
        val secondValue = permutation[secondIndex]
        deletePosition(firstIndex)
        deletePosition(secondIndex)
        set(firstIndex, secondValue)
        set(secondIndex, firstValue)
    }

    fun clear() {
        indices.forEach { index ->
            permutation[index] = -1
            inversePermutation[index] = -1
        }
    }

    fun before(value: Int): Int {
        if (value == size) return permutation[permutation.lastIndex]

        val index = inversePermutation[value]
        if (index == 0) return size

        return permutation[index - 1]
    }

    fun after(value: Int): Int {
        if (value == size) return permutation[0]

        val index = inversePermutation[value]
        if (index == permutation.lastIndex) return size

        return permutation[index + 1]
    }


    fun clone() = permutation.clone().asPermutation()

    fun isFormatCorrect(): Boolean = permutation.run {
        if (permutation.any { it < 0 }) return false

        val contains = BooleanArray(size) { false }
        var result = true
        forEach {
            if (it !in indices || contains[it])
                result = false
            else
                contains[it] = true
        }
        return result
    }

    fun forEachEmptyIndex(function: (Int) -> Unit) = permutation.forEachIndexed { index, value ->
        if (value == -1) {
            function(index)
        }
    }

    fun sliced(slicer: (Int) -> Boolean): Array<IntArray> {
        val result = mutableListOf(mutableListOf<Int>())
        permutation.forEach { v ->
            if (slicer(v)) {
                result.add(mutableListOf())
            } else {
                result.last().add(v)
            }
        }
        return result
            .map { it.toIntArray() }
            .toTypedArray()
    }

    override fun toString(): String {
        return "[${permutation.toList()}, ${inversePermutation.toList()}]"
    }
}