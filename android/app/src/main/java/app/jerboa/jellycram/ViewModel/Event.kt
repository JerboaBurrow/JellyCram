package app.jerboa.jellycram.ViewModel

abstract class Event {}

class DisplayingAboutChanged (val newValue: Boolean) : Event()

enum class SOCIAL {NOTHING, WEB, PLAY, YOUTUBE, GITHUB}
class RequestingSocial(val request: SOCIAL) : Event()

data class Score(val pieces: Long, val timeMillis: Long)
class Scored(val newScore: Score) : Event()

class RequestingAchievements() : Event()

class UpdatingAchievement(val name: String, val increment: Int) : Event()

enum class LeaderBoards {Survival, Score, None}
class RequestingLeaderboard(val board: LeaderBoards) : Event()

class RequestingLicenses() : Event()

class TutorialDone() : Event()

data class Settings(
    var invertTapControls: Boolean = false,
    var invertSwipeControls: Boolean = false,
    var screenCentric: Boolean = false
)

class SettingsChanged(val newSettings: Settings) : Event()