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
import app.jerboa.jellycram.ViewModel.RenderViewModel
import app.jerboa.jellycram.ViewModel.Settings

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
    onRequestingLicenses: () -> Unit,
    onRequestingSocial: (SOCIAL) -> Unit,
    onSettingChanged: (Settings) -> Unit,
    onRequestAchievements: () -> Unit,
    onRequestLeaderboards: (RenderViewModel.LeaderBoards) -> Unit
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
                        TextButton(onClick = { onRequestingLicenses() }) {
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
                            IconButton(onClick = { onRequestLeaderboards(RenderViewModel.LeaderBoards.Score) }) {
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
                                    painter = painterResource(id = images["play-lead"]!!),
                                    contentDescription = "leaderboards"
                                )
                            }
                            IconButton(onClick = { onRequestLeaderboards(RenderViewModel.LeaderBoards.Surivial) }) {
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
                                    painter = painterResource(id = images["play-lead"]!!),
                                    contentDescription = "leaderboards"
                                )
                            }
                            IconButton(onClick = { onRequestAchievements() }) {
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
                                    contentDescription = "achievements"
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
                                    checked = settings.invertControls,
                                    onCheckedChange =
                                    {
                                        val s = settings.copy()
                                        s.invertControls = !s.invertControls
                                        onSettingChanged(s)
                                    },
                                    colors = myCheckBoxColors()
                                )
                                Text(
                                    "Invert touch controls",
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
                                        onSettingChanged(s)
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
            socials(images, info, onRequestingSocial)
        }
    }
}