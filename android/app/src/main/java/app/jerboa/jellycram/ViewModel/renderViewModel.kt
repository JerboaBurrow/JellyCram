package app.jerboa.jellycram.ViewModel

import android.util.Log
import androidx.lifecycle.*
import kotlinx.coroutines.*

class RenderViewModel : ViewModel() {

    private val _displayingAbout = MutableLiveData(false)
    val displayingAbout: MutableLiveData<Boolean> = _displayingAbout

    private val _darkMode = MutableLiveData(true)
    val darkMode: MutableLiveData<Boolean> = _darkMode

    private val _requestingSocial = MutableLiveData(SOCIAL.NOTHING)
    val requestingSocial: MutableLiveData<SOCIAL> = _requestingSocial

    private val _postingScore = MutableLiveData(false)
    private val _updateScoreQueue: MutableList<Pair<String, Long>> = mutableListOf()
    private val _score = MutableLiveData(Pair("", 0L))
    val score: MutableLiveData<Pair<String, Long>> = _score

    private val _requestingPlayServicesAchievementsUI = MutableLiveData(false)
    val requestingPlayServicesAchievementsUI: MutableLiveData<Boolean> = _requestingPlayServicesAchievementsUI

    private val _requestingPlayServicesLeaderBoardsUI = MutableLiveData(LeaderBoards.None)
    val requestingPlayServicesLeaderBoardsUI: MutableLiveData<LeaderBoards> = _requestingPlayServicesLeaderBoardsUI

    private val _postingAchievement = MutableLiveData(false)
    private val _updateAchievementQueue: MutableList<Pair<String, Int>> = mutableListOf()
    private val _updateAchievement = MutableLiveData(Pair("",0))
    val updateAchievement: MutableLiveData<Pair<String,Int>> = _updateAchievement

    private val _requestingLicenses = MutableLiveData(false)
    val requestingLicenses: MutableLiveData<Boolean> = _requestingLicenses

    private val _settings = MutableLiveData(Settings(invertTapControls = false, screenCentric = true))
    val settings: MutableLiveData<Settings> = _settings

    private val _tutorialDone = MutableLiveData(false)
    val tutorialDone: MutableLiveData<Boolean> = _tutorialDone

    private val _displayingNews = MutableLiveData(false)
    private val _seenNews = MutableLiveData(false)
    val displayingNews: MutableLiveData<Boolean> = _displayingNews

    private val _paused = MutableLiveData(false)
    val paused: MutableLiveData<Boolean> = _paused

    fun onEvent(e: Event)
    {
        Log.d("renderViewModel", "$e")
        when (e)
        {
            is DisplayingAboutChanged -> onDisplayingAboutChanged(e.newValue)
            is RequestingSocial -> onRequestingSocial(e.request)
            is Scored -> onScored(e.newScore)
            is RequestingAchievements -> onRequestPlayServicesAchievementsUI()
            is RequestingLeaderboard -> onRequestPlayServicesLeaderBoardUI(e.board)
            is UpdatingAchievement -> onAchievementStateChanged(e.name, e.increment)
            is RequestingLicenses -> onRequestingLicenses()
            is TutorialDone -> onTutorialDone()
            is SettingsChanged -> onSettingsChanged(e.newSettings)
            is RequestNews -> onRequestingNews()
            is NewsSeen -> onNewsSeen()
        }
    }

    private fun onDisplayingAboutChanged(newVal: Boolean){
        _displayingAbout.value = !_displayingAbout.value!!
        _paused.value = _displayingAbout.value!!
    }

    private fun onRequestingSocial(v: SOCIAL){
        _requestingSocial.value = v
    }

    private suspend fun postScores()
    {
        while (_updateScoreQueue.size > 0)
        {
            val s = _updateScoreQueue.removeAt(_updateScoreQueue.lastIndex)
            _score.postValue(s)
            Log.d("posting update Score","$s")
            delay(1000)
        }
        _postingAchievement.postValue(false)
    }

    @OptIn(DelicateCoroutinesApi::class)
    private fun onScored(score: Score){
        Log.d("on scored", "$score")
        _updateScoreQueue.add(Pair("leaderboard_high_scores", score.pieces))
        _updateScoreQueue.add(Pair("leaderboard_survival_time", score.timeMillis))
        _updateScoreQueue.add(Pair("leaderboard_most_clears", score.clears))
        if (!_postingScore.value!!)
        {
            _postingScore.postValue(true)
            GlobalScope.launch {
                postScores()
            }
        }
    }

    private fun onRequestPlayServicesAchievementsUI(){
        _requestingPlayServicesAchievementsUI.value = true
    }

    private fun onRequestPlayServicesLeaderBoardUI(l: LeaderBoards){
        _requestingPlayServicesLeaderBoardsUI.value = l
    }

    private suspend fun postAchievements()
    {
        while (_updateAchievementQueue.size > 0)
        {
            val ach = _updateAchievementQueue.removeAt(_updateAchievementQueue.lastIndex)
            _updateAchievement.postValue(ach)
            Log.d("posting update Achievement","$ach")
            delay(1000)
        }
        _postingAchievement.postValue(false)
    }
    @OptIn(DelicateCoroutinesApi::class)
    private fun onAchievementStateChanged(name: String, increment: Int){
        Log.d("queue update Achievement","${name}, $increment")
        _updateAchievementQueue.add(Pair(name, increment))
        if (!_postingAchievement.value!!)
        {
            _postingAchievement.postValue(true)
            GlobalScope.launch {
                postAchievements()
            }
        }
    }

    private fun onRequestingLicenses() {
        requestingLicenses.value = true
    }

    private fun onSettingsChanged(s: Settings)
    {
        Log.d("renderViewModel", "$s")
        _settings.value = s
    }

    private fun onTutorialDone()
    {
        _tutorialDone.postValue(true)
    }

    private fun onRequestingNews()
    {
        if (_seenNews.value!!) { return }
        _displayingNews.value = true
        _paused.value = true
    }

    private fun onNewsSeen()
    {
        _displayingNews.value = false
        _seenNews.value = true
        _paused.value = false
    }
}