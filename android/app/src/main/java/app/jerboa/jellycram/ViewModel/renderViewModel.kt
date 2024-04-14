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

    private val _score = MutableLiveData(Score(0L,0L, 0L))
    val score: MutableLiveData<Score> = _score

    private val _requestingPlayServicesAchievementsUI = MutableLiveData(false)
    val requestingPlayServicesAchievementsUI: MutableLiveData<Boolean> = _requestingPlayServicesAchievementsUI

    private val _requestingPlayServicesLeaderBoardsUI = MutableLiveData(LeaderBoards.None)
    val requestingPlayServicesLeaderBoardsUI: MutableLiveData<LeaderBoards> = _requestingPlayServicesLeaderBoardsUI

    private val _posting = MutableLiveData(false)
    private val _updateQueue: MutableList<Pair<String, Int>> = mutableListOf()
    private val _updateAchievement = MutableLiveData(Pair("",0))
    val updateAchievement: MutableLiveData<Pair<String,Int>> = _updateAchievement

    private val _requestingLicenses = MutableLiveData(false)
    val requestingLicenses: MutableLiveData<Boolean> = _requestingLicenses

    private val _settings = MutableLiveData(Settings(invertTapControls = false, screenCentric = true))
    val settings: MutableLiveData<Settings> = _settings

    private val _tutorialDone = MutableLiveData(false)
    val tutorialDone: MutableLiveData<Boolean> = _tutorialDone

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
        }
    }

    private fun onDisplayingAboutChanged(newVal: Boolean){
        _displayingAbout.value = !_displayingAbout.value!!
    }

    private fun onRequestingSocial(v: SOCIAL){
        _requestingSocial.value = v
    }

    private fun onScored(score: Score){
        Log.d("on scored", "$score")
        _score.postValue(score)
    }

    private fun onRequestPlayServicesAchievementsUI(){
        _requestingPlayServicesAchievementsUI.value = true
    }

    private fun onRequestPlayServicesLeaderBoardUI(l: LeaderBoards){
        _requestingPlayServicesLeaderBoardsUI.value = l
    }

    private suspend fun postAchievements()
    {
        while (_updateQueue.size > 0)
        {
            val ach = _updateQueue.removeAt(_updateQueue.lastIndex)
            _updateAchievement.postValue(ach)
            Log.d("posting update Achievement","$ach")
            delay(1000)
        }
        _posting.postValue(false)
    }
    @OptIn(DelicateCoroutinesApi::class)
    private fun onAchievementStateChanged(name: String, increment: Int){
        Log.d("queue update Achievement","${name}, $increment")
        _updateQueue.add(Pair(name, increment))
        if (!_posting.value!!)
        {
            _posting.postValue(true)
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
}