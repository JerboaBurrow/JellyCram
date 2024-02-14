package app.jerboa.jellycram.composable

import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.livedata.observeAsState
import app.jerboa.jellycram.AppInfo
import app.jerboa.jellycram.ViewModel.RenderViewModel

@Composable
fun renderScreen(
    renderViewModel: RenderViewModel,
    resolution: Pair<Int,Int>,
    images: Map<String,Int>,
    info: AppInfo
){
    val displayingMenu: Boolean by renderViewModel.displayingMenu.observeAsState(initial = false)
    val displayingAbout: Boolean by renderViewModel.displayingAbout.observeAsState(initial = false)

    screen(
        displayingMenu,
        displayingAbout,
        resolution,
        images,
        info,
        onDisplayingMenuChanged = {renderViewModel.onDisplayingMenuChanged(it)},
        onDisplayingAboutChanged = {renderViewModel.onDisplayingAboutChanged(it)},
        onRequestAchievements = {renderViewModel.onRequestPlayServicesAchievementsUI()},
        onRequestLeaderboards = {renderViewModel.onRequestPlayServicesLeaderBoardUI()},
        onAchievementStateChanged = {renderViewModel.onAchievementStateChanged(it)},
        onScored = {renderViewModel.onScored(it)},
        onRequestingSocial = {renderViewModel.onRequestingSocial(it)},
        onRequestingLicenses ={renderViewModel.onRequestingLicenses()},
    )
}