package app.jerboa.jellycram

import android.content.ActivityNotFoundException
import android.content.Intent
import android.net.Uri
import android.os.Bundle
import android.util.DisplayMetrics
import android.util.Log
import android.view.View
import androidx.activity.compose.setContent
import androidx.activity.viewModels
import androidx.appcompat.app.AppCompatActivity
import app.jerboa.jellycram.ViewModel.LeaderBoards
import app.jerboa.jellycram.ViewModel.RenderViewModel
import app.jerboa.jellycram.ViewModel.RequestNews
import app.jerboa.jellycram.ViewModel.SOCIAL
import app.jerboa.jellycram.ViewModel.Settings
import app.jerboa.jellycram.ViewModel.SettingsChanged
import app.jerboa.jellycram.composable.renderScreen
import app.jerboa.jellycram.onlineServices.Client
import app.jerboa.jellycram.onlineServices.InAppReview
import com.google.android.gms.games.PlayGamesSdk
import com.google.android.gms.oss.licenses.OssLicensesMenuActivity
import com.google.gson.Gson
import java.util.*


data class AppInfo(
    val versionString: String,
    val firstLaunch: Boolean,
    val tutorialDone: Boolean,
    val playGamesServices: Boolean,
    val density: Float,
    val heightDp: Float,
    val widthDp: Float
)

class MainActivity : AppCompatActivity() {

    private val renderViewModel by viewModels<RenderViewModel>()

    private var playSuccess = false

    private lateinit var client: Client

    private val startTimeMillis: Long = System.currentTimeMillis()

    private val imageResources: Map<String,Map<String, Int>> = mapOf(
        "dark" to mapOf(
            "logo" to R.drawable.logo_dark,
            "play-ach" to R.drawable.games_achievements_dark,
            "play-lead" to R.drawable.games_leaderboards_dark,
            "play-logo" to R.drawable.play_,
            "yt" to R.drawable.ic_yt,
            "github" to R.drawable.github_mark_dark,
            "burger" to R.drawable.menu_dark,
            "dismiss" to R.drawable.menu_dimiss_dark,
            "score_lead" to R.drawable.score_leaderboard_dark,
            "time_lead" to R.drawable.time_leaderboard_dark,
            "clears_lead" to R.drawable.clears_leaderboard_dark,
            "darklight" to R.drawable.swap_theme_dark,
            "news" to R.drawable.news
        ),
        "light" to mapOf(
            "logo" to R.drawable.logo,
            "play-ach" to R.drawable.games_achievements,
            "play-lead" to R.drawable.games_leaderboards,
            "play-logo" to R.drawable.play_,
            "yt" to R.drawable.ic_yt,
            "github" to R.drawable.github_mark,
            "burger" to R.drawable.menu,
            "dismiss" to R.drawable.menu_dimiss,
            "score_lead" to R.drawable.score_leaderboard,
            "time_lead" to R.drawable.time_leaderboard,
            "clears_lead" to R.drawable.clears_leaderboard,
            "darklight" to R.drawable.swap_theme,
            "news" to R.drawable.news
        )
    )

    private fun playRate(){
        try {
            val intent = Intent(Intent.ACTION_VIEW).apply {
                data = Uri.parse(
                    "https://play.google.com/store/apps/details?id=app.jerboa.jellycram"
                )
                setPackage("com.android.vending")
            }
            startActivity(intent)
        } catch (e: ActivityNotFoundException) {
            Log.e("playRate", "ActivityNotFoundException, is Google Play Installed?")
        }
    }

    private fun showLicenses(){
        val intent = Intent(this.applicationContext, OssLicensesMenuActivity::class.java)
        intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
        //on openning OSS sometimes there is a crash..
        //https://github.com/google/play-services-plugins/issues/100
        //com.google.android.gms.internal.oss_licenses.zzf.dummy_placeholder = getResources().getIdentifier("third_party_license_metadata", "raw", getPackageName());
        startActivity(intent)
    }
    private fun youtube(){
        val uri = Uri.parse("https://www.youtube.com/channel/UCP3KhLhmG3Z1CMWyLkn7pbQ")
        val intent = Intent(Intent.ACTION_VIEW, uri)
        startActivity(intent)
    }

    private fun web(){
        val uri = Uri.parse("https://jerboa.app")
        val intent = Intent(Intent.ACTION_VIEW, uri)
        startActivity(intent)
    }

    private fun github(){
        val uri = Uri.parse("https://github.com/JerboaBurrow/JellyCram")
        val intent = Intent(Intent.ACTION_VIEW, uri)
        startActivity(intent)
    }

    private fun bugs(){
        val uri = Uri.parse("https://github.com/JerboaBurrow/JellyCram/issues")
        val intent = Intent(Intent.ACTION_VIEW, uri)
        startActivity(intent)
    }

    private fun saveSettings()
    {
        val gson = Gson()
        val settings = gson.toJson(renderViewModel.settings.value)
        val prefs = getSharedPreferences(resources.getString(R.string.app_prefs), MODE_PRIVATE)
        val prefsEdit = prefs.edit()
        prefsEdit.putString("settings", settings)
        Log.d("saveSettings", settings)
        val playTime = System.currentTimeMillis() - startTimeMillis
        val lastPlayTime = prefs.getLong("playTime", 0L)
        prefsEdit.putLong("playTime", playTime+lastPlayTime)
        prefsEdit.apply()
    }
    override fun onDestroy() {
        saveSettings()
        super.onDestroy()
    }

    override fun onPause() {
        saveSettings()
        super.onPause()
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        // play game services
        PlayGamesSdk.initialize(this)

        val prefs = getSharedPreferences(resources.getString(R.string.app_prefs), MODE_PRIVATE)

        client = Client(resources, getSharedPreferences(resources.getString(R.string.app_prefs), MODE_PRIVATE))
        client.playGamesServicesLogin(this)
        client.sync(this)

        //client.loadRewardedAd(this)

        renderViewModel.requestingPlayServicesAchievementsUI.observe(
            this, androidx.lifecycle.Observer {
                    request -> if(request){client.showPlayAchievementsUI(this)}
            }
        )

        renderViewModel.requestingPlayServicesLeaderBoardsUI.observe(
            this, androidx.lifecycle.Observer {
                request ->
                if(request == LeaderBoards.Score)
                {client.showPlayLeaderBoardUI("leaderboard_high_scores", this)}
                else if(request == LeaderBoards.Survival)
                {client.showPlayLeaderBoardUI("leaderboard_survival_time", this)}
                else if(request == LeaderBoards.Clears)
                {client.showPlayLeaderBoardUI("leaderboard_most_clears", this)}
            }
        )

        renderViewModel.updateAchievement.observe(
            this, androidx.lifecycle.Observer {
                    s -> if(client.updateAchievement(s.first,s.second)){
                client.updatePlayServicesAchievement(this,s.first)
                client.updateLocalAchievement(s.first)
            }
            }
        )

        renderViewModel.score.observe(
            this, androidx.lifecycle.Observer {
                s -> if (s != null)
                {
                    client.postScore("",s.second,s.first, this)
                }
            }
        )

        renderViewModel.requestingLicenses.observe(
            this, androidx.lifecycle.Observer { request -> if(request){ showLicenses() }}
        )

        renderViewModel.requestingSocial.observe(
            this, androidx.lifecycle.Observer { request ->
                when (request){
                    SOCIAL.WEB -> web()
                    SOCIAL.PLAY -> playRate()
                    SOCIAL.YOUTUBE -> youtube()
                    SOCIAL.GITHUB -> github()
                    SOCIAL.BUGS -> bugs()
                }
            }
        )

        renderViewModel.tutorialDone.observe(
            this, androidx.lifecycle.Observer {
                v -> if (v) {
                    val prefs = getSharedPreferences(resources.getString(R.string.app_prefs), MODE_PRIVATE)
                    val prefsEdit = prefs.edit()
                    prefsEdit.putBoolean("tutorialDone",true)
                    prefsEdit.apply()
                }
            }
        )

//        if (BuildConfig.DEBUG){
//            prefs.edit().clear().apply()
//        }

        if (!prefs.contains("firstLaunch")){
            val prefsEdit = prefs.edit()
            prefsEdit.putBoolean("firstLaunch",true)
            prefsEdit.apply()
        }

        if (!prefs.contains("settings"))
        {
            renderViewModel.onEvent(SettingsChanged(Settings(invertTapControls = false, invertSwipeControls = false, screenCentric = true, darkMode = true)))
        }
        else
        {
            val gson = Gson()
            try {
                renderViewModel.onEvent(SettingsChanged(gson.fromJson(prefs.getString("settings", ""), Settings::class.java)))
            }
            catch (e: Error)
            {
                Log.d("load", "$e")
                val prefsEdit = prefs.edit()
                prefsEdit.remove("settings")
                prefsEdit.apply()
                renderViewModel.onEvent(SettingsChanged(Settings(invertTapControls = false, invertSwipeControls = false, screenCentric = true, darkMode = true)))
            }
        }

        if (!prefs.contains("news-15-04-2024"))
        {
            val prefsEdit = prefs.edit()
            prefsEdit.putString("news-15-04-2024","seen")
            prefsEdit.apply()
            renderViewModel.onEvent(RequestNews())
        }

        InAppReview().requestUserReviewPrompt(this)

        if (!prefs.contains("tutorialDone"))
        {
            val prefsEdit = prefs.edit()
            prefsEdit.putBoolean("tutorialDone",false)
            prefsEdit.apply()
        }

        val tutorialDone: Boolean = prefs.getBoolean("tutorialDone", false)
        Log.d("tutorial done", "$tutorialDone")

        val firstLaunch: Boolean = prefs.getBoolean("firstLaunch",true)

        Log.d("launch", firstLaunch.toString())

        val versionString = BuildConfig.VERSION_NAME + ": " + Date(BuildConfig.TIMESTAMP)

        val displayInfo = resources.displayMetrics
        val dpHeight = displayInfo.heightPixels / displayInfo.density
        val dpWidth = displayInfo.widthPixels / displayInfo.density
        val appInfo = AppInfo(
            versionString,
            firstLaunch,
            tutorialDone,
            playSuccess,
            if (resources.getBoolean(R.bool.isTablet)){displayInfo.density}else{1f},
            dpHeight,
            dpWidth
        )

        Log.d("density",""+ resources.displayMetrics.density)

        window.decorView.systemUiVisibility = View.SYSTEM_UI_FLAG_FULLSCREEN
        actionBar?.hide()

        val displayMetrics = DisplayMetrics()
        windowManager.defaultDisplay.getMetrics(displayMetrics)
        val height = displayMetrics.heightPixels
        val width = displayMetrics.widthPixels
        setContent {
            // A surface container using the 'background' color from the theme
            renderScreen(
                renderViewModel,
                Pair(width,height),
                imageResources,
                appInfo
            )
        }
    }
}