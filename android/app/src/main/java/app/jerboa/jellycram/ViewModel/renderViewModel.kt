package app.jerboa.jellycram.ViewModel

import android.util.Log
import androidx.lifecycle.*

enum class SOCIAL {NOTHING, WEB, PLAY, YOUTUBE, GITHUB}

data class Settings(
    var invertControls: Boolean = false,
    var screenCentric: Boolean = false
)

class RenderViewModel : ViewModel() {

    // Menu

    private val _displayingAbout = MutableLiveData(false)
    val displayingAbout: MutableLiveData<Boolean> = _displayingAbout
    fun onDisplayingAboutChanged(newVal: Boolean){
        _displayingAbout.value = !_displayingAbout.value!!
    }

    // SOCIAL
    private val _requestingSocial = MutableLiveData(SOCIAL.NOTHING)
    val requestingSocial: MutableLiveData<SOCIAL> = _requestingSocial

    fun onRequestingSocial(v: SOCIAL){
        _requestingSocial.value = v
    }

    // Game flow
    private val _pausingGame = MutableLiveData(false)
    val pausingGame: MutableLiveData<Boolean> = _pausingGame

    // scoring

    data class Score(val pieces: Long, val timeMillis: Long)

    private val _score = MutableLiveData(Score(0L,0L))
    val score: MutableLiveData<Score> = _score
    fun onScored(score: Score){
        Log.d("on scored", "$score")
        _score.postValue(score)
    }

    // privacy (if ads)
    private val _displayingPrivacy = MutableLiveData(false)
    val displayingPrivacy: MutableLiveData<Boolean> = _displayingPrivacy

    private val _agreedToPrivacyPolicy = MutableLiveData(false)
    val agreedToPrivacyPolicy: MutableLiveData<Boolean> = _agreedToPrivacyPolicy
    fun onDismissPrivacy(agreed: Boolean){
        _displayingPrivacy.value = false
        _agreedToPrivacyPolicy.value = agreed
    }

    fun onPrivacyPopUp(){
        _displayingAbout.value = false
        if (_agreedToPrivacyPolicy.value == false){_displayingPrivacy.value=true}
    }

    // Play services

    private val _requestingPlayServicesAchievementsUI = MutableLiveData(false)
    val requestingPlayServicesAchievementsUI: MutableLiveData<Boolean> = _requestingPlayServicesAchievementsUI
    fun onRequestPlayServicesAchievementsUI(){
        _requestingPlayServicesAchievementsUI.value = true
    }

    enum class LeaderBoards {Surivial, Score, None}

    private val _requestingPlayServicesLeaderBoardsUI = MutableLiveData(LeaderBoards.None)
    val requestingPlayServicesLeaderBoardsUI: MutableLiveData<LeaderBoards> = _requestingPlayServicesLeaderBoardsUI
    fun onRequestPlayServicesLeaderBoardUI(l: LeaderBoards){
        _requestingPlayServicesLeaderBoardsUI.value = l
    }

    // Achievements

    data class AchievementUpdateData(
        val name: String,
        val increment: Int
    )

    private val _updateAchievement = MutableLiveData(Pair("",0))
    val updateAchievement: MutableLiveData<Pair<String,Int>> = _updateAchievement

    fun onAchievementStateChanged(data: AchievementUpdateData){
        Log.d("view model update Achievement","${data.name}, ${data.increment}")
        _updateAchievement.postValue(Pair(data.name,data.increment))
    }

    private val _requestingLicenses = MutableLiveData(false)
    val requestingLicenses: MutableLiveData<Boolean> = _requestingLicenses
    fun onRequestingLicenses() {
        requestingLicenses.value = true
    }

    // settings

    private val _settings = MutableLiveData(Settings(invertControls = false, screenCentric = true))
    val settings: MutableLiveData<Settings> = _settings

    fun onSettingsChanged(s: Settings)
    {
        Log.d("renderViewModel", "$s")
        _settings.value = s
    }
}