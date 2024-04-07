package app.jerboa.jellycram.composable

import android.annotation.SuppressLint
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.material.Scaffold
import androidx.compose.material.rememberScaffoldState
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.livedata.observeAsState
import androidx.compose.ui.Modifier
import androidx.compose.ui.viewinterop.AndroidView
import app.jerboa.jellycram.AppInfo
import app.jerboa.jellycram.ViewModel.RenderViewModel
import app.jerboa.jellycram.ViewModel.Settings
import app.jerboa.jellycram.ui.view.GLView

@SuppressLint("UnusedMaterialScaffoldPaddingParameter")
@Composable
fun renderScreen(
    renderViewModel: RenderViewModel,
    resolution: Pair<Int,Int>,
    images: Map<String,Int>,
    info: AppInfo
){
    val displayingAbout: Boolean by renderViewModel.displayingAbout.observeAsState(initial = false)
    val settings: Settings by renderViewModel.settings.observeAsState(renderViewModel.settings.value!!)

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
                        settings,
                        info.tutorialDone,
                        resolution,
                        {v -> renderViewModel.onDisplayingAboutChanged(v)},
                        {v -> renderViewModel.onAchievementStateChanged(v)},
                        {v -> renderViewModel.onScored(v)},
                        {renderViewModel.onTutorialDone()}
                    )
                },
                update = { view ->
                    run {
                        if (displayingAbout) {
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
                {renderViewModel.onRequestingLicenses()},
                {renderViewModel.onRequestingSocial(it)},
                {renderViewModel.onSettingsChanged(it)},
                {renderViewModel.onRequestPlayServicesAchievementsUI()},
                {renderViewModel.onRequestPlayServicesLeaderBoardUI(it)}
            )
            menuPrompt(images,displayingAbout,menuItemHeight) {
                renderViewModel.onDisplayingAboutChanged(
                    it
                )
            }
        }
    }
}