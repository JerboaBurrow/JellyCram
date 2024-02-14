package app.jerboa.jellycram.data

enum class Event {UP, DOWN, LEFT, RIGHT, ROT_LEFT, ROT_RIGHT, PAUSE}

class GameState {
    var gameOver: Boolean = false
    var incoming: Boolean = false
    var allowMove: Boolean = false
    var paused: Boolean = false
    var debug: Boolean = false
    var graceFrames: UByte = 0u
    var countDownSeconds: Double = 0.0
    var elapsed_countdown: Double = 0.0
    var countDownBegin: ULong = 0u
    var deletePulseBegin: ULong = 0u
    var currentImpulse: Double = 0.0
    var currentTorque: Double = 0.0
    var score: ULong = 0u
    var events: List<Pair<Event, Boolean>> = listOf()
    var deleteQueue: List<Pair<String, ULong>> = listOf()
    var deleteQueueIds: List<String> = listOf()
    var objects: List<String> = listOf()
    var current: String = ""
}