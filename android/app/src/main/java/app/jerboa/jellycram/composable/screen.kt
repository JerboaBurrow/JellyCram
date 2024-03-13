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
import app.jerboa.jellycram.ui.view.GLView

@OptIn(ExperimentalAnimationApi::class)
@SuppressLint("CoroutineCreationDuringComposition")
@Composable
fun screen(
    displayingMenu: Boolean,
    displayingAbout: Boolean,
    resolution: Pair<Int,Int>,
    images: Map<String,Int>,
    info: AppInfo,
    onDisplayingMenuChanged: (Boolean) -> Unit,
    onDisplayingAboutChanged: (Boolean) -> Unit,
    onRequestAchievements: () -> Unit,
    onRequestLeaderboards: () -> Unit,
    onAchievementStateChanged: (RenderViewModel.AchievementUpdateData) -> Unit,
    onScored: (Long) -> Unit,
    onRequestingSocial: (SOCIAL) -> Unit,
    onRequestingLicenses: () -> Unit,
){

    val scaffoldState = rememberScaffoldState()
    val coroutineScope = rememberCoroutineScope()

    val seenHelp = remember { mutableStateOf(!info.firstLaunch) }

    val width75Percent = info.widthDp*0.75
    val height25Percent = info.heightDp*0.25
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
                        onDisplayingMenuChanged,
                        onAchievementStateChanged,
                        onScored
                    )
                },
                update = { view ->
                    run {
                        if (displayingAbout || displayingMenu) {
                            view.onSetPauseGame(true)
                        }
                        else
                        {
                            view.onSetPauseGame(false)
                        }
                    }
                }
            )
            about(
                displayingAbout,
                width75Percent,
                images,
                info,
                onRequestingLicenses,
                onRequestingSocial
            )
            menuPrompt(images,displayingMenu,menuItemHeight,onDisplayingAboutChanged)
        }
    }
}