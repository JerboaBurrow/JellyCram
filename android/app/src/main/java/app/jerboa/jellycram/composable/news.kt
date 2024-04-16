package app.jerboa.jellycram.composable

import androidx.compose.animation.AnimatedVisibility
import androidx.compose.animation.fadeIn
import androidx.compose.animation.fadeOut
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxHeight
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.MaterialTheme
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.unit.dp
import app.jerboa.jellycram.AppInfo
import app.jerboa.jellycram.R
import app.jerboa.jellycram.ViewModel.Event
import app.jerboa.jellycram.ViewModel.NewsSeen
import app.jerboa.jellycram.ViewModel.Settings

@Composable
fun news(
    displayingNews: Boolean,
    width75Percent: Double,
    images: Map<String,Int>,
    onEvent: (e: Event) -> Unit
) {
    AnimatedVisibility(
        visible = displayingNews,
        enter = fadeIn(),
        exit = fadeOut()
    ) {
        Box(modifier = Modifier
            .fillMaxWidth()
            .fillMaxHeight()
            .background(
                color = MaterialTheme.colors.background,
            )
            .clickable { onEvent(NewsSeen()) })
        {
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
                            color = MaterialTheme.colors.background,
                            shape = RoundedCornerShape(5)
                        )
                ) {
                    Column(
                        horizontalAlignment = Alignment.CenterHorizontally,
                        verticalArrangement = Arrangement.Center,
                        modifier = Modifier.fillMaxWidth()
                    ) {
                        Image(
                            painter = painterResource(id = images["news"]!!),
                            contentDescription = stringResource(id = R.string.news),
                            modifier = Modifier.width((width75Percent*0.9f).dp)
                        )
                    }
                }
            }
        }
    }
}