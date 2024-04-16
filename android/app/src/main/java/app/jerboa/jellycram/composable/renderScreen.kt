package app.jerboa.jellycram.composable

import android.annotation.SuppressLint
import android.util.Log
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
import app.jerboa.jellycram.ui.theme.JellyCramTheme
import app.jerboa.jellycram.ui.view.GLView

@SuppressLint("UnusedMaterialScaffoldPaddingParameter")
@Composable
fun renderScreen(
    renderViewModel: RenderViewModel,
    resolution: Pair<Int,Int>,
    images: Map<String,Map<String, Int>>,
    info: AppInfo
){
    val displayingAbout: Boolean by renderViewModel.displayingAbout.observeAsState(initial = false)
    val paused: Boolean by renderViewModel.paused.observeAsState(initial = false)
    val settings: Settings by renderViewModel.settings.observeAsState(renderViewModel.settings.value!!)
    val displayingNews: Boolean by renderViewModel.displayingNews.observeAsState(initial = false)
    val playSuccess: Boolean by renderViewModel.playLogin.observeAsState(initial = false)

    val scaffoldState = rememberScaffoldState()

    val width75Percent = info.widthDp*0.75
    val height10Percent = info.heightDp*0.1
    val menuItemHeight = height10Percent*0.66

    Log.d("renderScreen", "$paused")

    val themeImages: Map<String, Int> = if (settings.darkMode)
    {
        images["dark"]!!
    }
    else
    {
        images["light"]!!
    }

    JellyCramTheme(
        darkTheme = settings.darkMode
    ) {

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
                            resolution
                        ) { v -> renderViewModel.onEvent(v) }
                    }
                ) { view ->
                    run {
                        view.onSetPauseGame(paused)
                        view.settings(settings)
                    }
                }

                about(
                    displayingAbout,
                    playSuccess,
                    settings,
                    width75Percent,
                    themeImages,
                    info
                ) { v -> renderViewModel.onEvent(v) }

                news(
                    displayingNews,
                    width75Percent,
                    themeImages
                ) { v -> renderViewModel.onEvent(v) }

                menuPrompt(
                    themeImages,
                    displayingAbout,
                    settings,
                    menuItemHeight
                ) { renderViewModel.onEvent(it) }
            }
        }
    }
}