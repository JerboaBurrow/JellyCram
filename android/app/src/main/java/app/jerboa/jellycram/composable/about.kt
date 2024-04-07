package app.jerboa.jellycram.composable

import androidx.compose.animation.AnimatedVisibility
import androidx.compose.animation.ExperimentalAnimationApi
import androidx.compose.animation.fadeIn
import androidx.compose.animation.fadeOut
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.Checkbox
import androidx.compose.material.CheckboxColors
import androidx.compose.material.CheckboxDefaults
import androidx.compose.material.IconButton
import androidx.compose.material.MaterialTheme
import androidx.compose.material.Text
import androidx.compose.material.TextButton
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.alpha
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import app.jerboa.jellycram.AppInfo
import app.jerboa.jellycram.ViewModel.SOCIAL
import app.jerboa.jellycram.R
import app.jerboa.jellycram.ViewModel.Event
import app.jerboa.jellycram.ViewModel.LeaderBoards
import app.jerboa.jellycram.ViewModel.RenderViewModel
import app.jerboa.jellycram.ViewModel.RequestingAchievements
import app.jerboa.jellycram.ViewModel.RequestingLeaderboard
import app.jerboa.jellycram.ViewModel.RequestingLicenses
import app.jerboa.jellycram.ViewModel.Settings
import app.jerboa.jellycram.ViewModel.SettingsChanged

@Composable
fun myCheckBoxColors(): CheckboxColors {
    return CheckboxDefaults.colors(
        checkedColor = Color.Black,
        uncheckedColor = Color.Black
    )
}

@OptIn(ExperimentalAnimationApi::class)
@Composable
fun about(
    displayingAbout: Boolean,
    menuItemHeight: Double,
    settings: Settings,
    width75Percent: Double,
    images: Map<String,Int>,
    info: AppInfo,
    onEvent: (e: Event) -> Unit
){
    AnimatedVisibility(
        visible = displayingAbout,
        enter = fadeIn(),
        exit = fadeOut()
    ) {
        Column(
            horizontalAlignment = Alignment.CenterHorizontally,
            verticalArrangement = Arrangement.Center,
            modifier = Modifier.fillMaxWidth()
        ) {
            Spacer(modifier = Modifier.size(2.dp))
            Box(
                Modifier
                    .width(width75Percent.dp)
                    .height((width75Percent * 1.1).dp)
                    .background(
                        color = MaterialTheme.colors.secondary,
                        shape = RoundedCornerShape(5)
                    )
            ) {
                Column(
                    verticalArrangement = Arrangement.SpaceBetween,
                    modifier = Modifier.fillMaxWidth()
                ) {
                    Column(
                        horizontalAlignment = Alignment.CenterHorizontally,
                        verticalArrangement = Arrangement.SpaceBetween,
                        modifier = Modifier.fillMaxWidth()
                    ) {
                        Text(
                            stringResource(id = R.string.tagline),
                            textAlign = TextAlign.Center,
                            modifier = Modifier.weight(0.5f),
                            fontSize = MaterialTheme.typography.body1.fontSize * info.density
                        )
                        Text(
                            stringResource(id = R.string.description),
                            textAlign = TextAlign.Center,
                            modifier = Modifier.weight(1f),
                            fontSize = MaterialTheme.typography.body1.fontSize * info.density
                        )
                        TextButton(onClick = { onEvent(RequestingLicenses()) }) {
                            Text(
                                stringResource(id = R.string.OSSprompt),
                                textAlign = TextAlign.Center,
                                modifier = Modifier.weight(0.5f),
                                fontSize = MaterialTheme.typography.caption.fontSize * info.density
                            )
                        }
                        Text(
                            stringResource(R.string.attrib) + " version: " + info.versionString,
                            modifier = Modifier.weight(0.5f),
                            fontSize = MaterialTheme.typography.overline.fontSize * info.density,
                            textAlign = TextAlign.Center
                        )
                        Row(
                            modifier = Modifier.weight(0.5f),
                            verticalAlignment = Alignment.CenterVertically,
                            horizontalArrangement = Arrangement.SpaceBetween
                        )
                        {
                            IconButton(onClick = { onEvent(RequestingLeaderboard(LeaderBoards.Score)) }) {
                                Image(
                                    modifier = Modifier
                                        .weight(1f)
                                        .alpha(
                                            if (!info.playGamesServices) {
                                                0.33f
                                            } else {
                                                1f
                                            }
                                        ),
                                    painter = painterResource(id = images["score_lead"]!!),
                                    contentDescription = "button for high score leaderboards"
                                )
                            }
                            IconButton(onClick = { onEvent(RequestingLeaderboard(LeaderBoards.Survival)) }) {
                                Image(
                                    modifier = Modifier
                                        .weight(1f)
                                        .alpha(
                                            if (!info.playGamesServices) {
                                                0.33f
                                            } else {
                                                1f
                                            }
                                        ),
                                    painter = painterResource(id = images["time_lead"]!!),
                                    contentDescription = "button for game time leaderboards"
                                )
                            }
                            IconButton(onClick = { onEvent(RequestingAchievements()) }) {
                                Image(
                                    modifier = Modifier
                                        .weight(1f)
                                        .alpha(
                                            if (!info.playGamesServices) {
                                                0.33f
                                            } else {
                                                1f
                                            }
                                        ),
                                    painter = painterResource(id = images["play-ach"]!!),
                                    contentDescription = "button for achievements"
                                )
                            }
                        }
                        Column(
                            verticalArrangement = Arrangement.SpaceBetween,
                            modifier = Modifier.fillMaxWidth()
                        ) {
                            Row(
                                horizontalArrangement = Arrangement.Start,
                                verticalAlignment = Alignment.CenterVertically
                            )
                            {
                                Checkbox(
                                    checked = settings.invertTapControls,
                                    onCheckedChange =
                                    {
                                        val s = settings.copy()
                                        s.invertTapControls = !s.invertTapControls
                                        onEvent(SettingsChanged(s))
                                    },
                                    colors = myCheckBoxColors()
                                )
                                Text(
                                    "Invert tap controls",
                                    fontSize = MaterialTheme.typography.body1.fontSize * info.density
                                )
                            }
                            Row(
                                horizontalArrangement = Arrangement.Start,
                                verticalAlignment = Alignment.CenterVertically
                            )
                            {
                                Checkbox(
                                    checked = settings.invertSwipeControls,
                                    onCheckedChange =
                                    {
                                        val s = settings.copy()
                                        s.invertSwipeControls = !s.invertSwipeControls
                                        onEvent(SettingsChanged(s))
                                    },
                                    colors = myCheckBoxColors()
                                )
                                Text(
                                    "Invert swipe controls",
                                    fontSize = MaterialTheme.typography.body1.fontSize * info.density
                                )
                            }
                            Row(
                                horizontalArrangement = Arrangement.Start,
                                verticalAlignment = Alignment.CenterVertically
                            )
                            {
                                Checkbox(
                                    checked = settings.screenCentric,
                                    onCheckedChange =
                                    {
                                        val s = settings.copy()
                                        s.screenCentric = !s.screenCentric
                                        onEvent(SettingsChanged(s))
                                    },
                                    colors = myCheckBoxColors()
                                )
                                Text(
                                    "Screen centric controls",
                                    fontSize = MaterialTheme.typography.body1.fontSize * info.density
                                )
                            }
                        }
                    }
                }
            }
            Spacer(modifier = Modifier.size(2.dp))
            socials(images, info) { onEvent(it) }
        }
    }
}