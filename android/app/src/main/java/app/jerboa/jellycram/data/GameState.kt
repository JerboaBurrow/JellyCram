package app.jerboa.jellycram.data

class GameState {
    var gameOver: Boolean = false
    var incoming: Boolean = false
    var allowMove: Boolean = false
    var paused: Boolean = false
    var score: ULong = 0U
    var tap: Pair<Double, Double> = Pair(Double.NaN, Double.NaN)
    var drag: Pair<Pair<Double, Double>, Pair<Double, Double>> = Pair(Pair(Double.NaN, Double.NaN),Pair(Double.NaN, Double.NaN))
}