package app.jerboa.jellycram.composable

import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.livedata.observeAsState
import app.jerboa.jellycram.AppInfo
import app.jerboa.jellycram.ViewModel.RenderViewModel
import app.jerboa.jellycram.ViewModel.Settings

@Composable
fun renderScreen(
    renderViewModel: RenderViewModel,
    resolution: Pair<Int,Int>,
    images: Map<String,Int>,
    info: AppInfo
){
    val displayingAbout: Boolean by renderViewModel.displayingAbout.observeAsState(initial = false)
    val pausing: Boolean by renderViewModel.pausingGame.observeAsState(initial = false)
    val settings: Settings by renderViewModel.settings.observeAsState(renderViewModel.settings.value!!)

    screen(
        settings,
        pausing,
        displayingAbout,
        resolution,
        images,
        info,
        onDisplayingAboutChanged = {renderViewModel.onDisplayingAboutChanged(it)},
        onRequestAchievements = {renderViewModel.onRequestPlayServicesAchievementsUI()},
        onRequestLeaderboards = {renderViewModel.onRequestPlayServicesLeaderBoardUI(it)},
        onAchievementStateChanged = {renderViewModel.onAchievementStateChanged(it)},
        onScored = {renderViewModel.onScored(it)},
        onRequestingSocial = {renderViewModel.onRequestingSocial(it)},
        onRequestingLicenses = {renderViewModel.onRequestingLicenses()},
        onSettingChanged = {renderViewModel.onSettingsChanged(it)},
        onTutorialDone = {renderViewModel.onTutorialDone()}
    )
}