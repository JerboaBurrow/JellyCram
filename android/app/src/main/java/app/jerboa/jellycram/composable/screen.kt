package app.jerboa.jellycram.composable

import android.annotation.SuppressLint
import androidx.compose.animation.*
import androidx.compose.foundation.layout.*
import androidx.compose.material.*
import androidx.compose.runtime.Composable
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.ui.Modifier
import androidx.compose.ui.viewinterop.AndroidView
import app.jerboa.jellycram.AppInfo
import app.jerboa.jellycram.ViewModel.RenderViewModel
import app.jerboa.jellycram.ViewModel.SOCIAL
import app.jerboa.jellycram.ViewModel.Settings
import app.jerboa.jellycram.ui.view.GLView

@SuppressLint("CoroutineCreationDuringComposition")
@Composable
fun screen(
    settings: Settings,
    pausing: Boolean,
    displayingAbout: Boolean,
    resolution: Pair<Int,Int>,
    images: Map<String,Int>,
    info: AppInfo,
    onDisplayingAboutChanged: (Boolean) -> Unit,
    onRequestAchievements: () -> Unit,
    onRequestLeaderboards: () -> Unit,
    onAchievementStateChanged: (RenderViewModel.AchievementUpdateData) -> Unit,
    onScored: (Long) -> Unit,
    onRequestingSocial: (SOCIAL) -> Unit,
    onRequestingLicenses: () -> Unit,
    onSettingChanged: (Settings) -> Unit
){

    val scaffoldState = rememberScaffoldState()

    val width75Percent = info.widthDp*0.75
    val height10Percent = info.heightDp*0.1
    val menuItemHeight = height10Percent*0.66

    Column(
        modifier = Modifier.fillMaxWidth()
    ) {

        Scaffold(
            scaffoldState = scaffoldState,
            topBar = {
            },
            bottomBar = {
            }
        ) {
            AndroidView(
                factory = {
                    GLView(
                        it, null,
                        resolution,
                        onDisplayingAboutChanged,
                        onAchievementStateChanged,
                        onScored
                    )
                },
                update = { view ->
                    run {
                        if (displayingAbout || pausing) {
                            view.onSetPauseGame(true)
                        }
                        else
                        {
                            view.onSetPauseGame(false)
                        }
                        view.settings(settings)
                    }
                }
            )
            about(
                displayingAbout,
                menuItemHeight,
                settings,
                width75Percent,
                images,
                info,
                onRequestingLicenses,
                onRequestingSocial,
                onSettingChanged,
                onRequestAchievements,
                onRequestLeaderboards
            )
            menuPrompt(images,displayingAbout,menuItemHeight,onDisplayingAboutChanged)
        }
    }
}