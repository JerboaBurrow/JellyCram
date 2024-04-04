package app.jerboa.jellycram.ui.view

import android.content.Context
import android.opengl.GLSurfaceView
import android.util.AttributeSet
import android.util.Log
import android.view.GestureDetector
import android.view.MotionEvent
import androidx.core.view.GestureDetectorCompat
import app.jerboa.jellycram.ViewModel.RenderViewModel
import app.jerboa.jellycram.ViewModel.Settings

class GLView (
    context: Context,
    attr: AttributeSet? = null,
    settings: Settings,
    private val tutorialDone: Boolean,
    private val resolution: Pair<Int,Int>,
    private val onDisplayingMenuChanged: (Boolean) -> Unit,
    private val onAchievementStateChanged: (RenderViewModel.AchievementUpdateData) -> Unit,
    private val onScored: (RenderViewModel.Score) -> Unit,
    private val onTutorialDone: () -> Unit,
    ) : GLSurfaceView(context,attr), GestureDetector.OnGestureListener {

    private val renderer =
        GLRenderer(tutorialDone, resolution,onAchievementStateChanged, onScored, onTutorialDone)
    private val gestures: GestureDetectorCompat = GestureDetectorCompat(context,this)
    var isDisplayingMenuChanged: Boolean = false

    init {
        setEGLContextClientVersion(3)
        preserveEGLContextOnPause = true
        setRenderer(renderer)
        settings(settings)
    }

    fun onSetPauseGame(v: Boolean)
    {
        renderer.pause(v)
    }
    fun settings(s: Settings)
    {
        renderer.setSettings(s)
    }
    override fun onTouchEvent(event: MotionEvent): Boolean {
        return if (gestures.onTouchEvent(event)) {
            true
        } else {
            super.onTouchEvent(event)
        }
    }

    override fun onDown(p0: MotionEvent): Boolean {
        return true
    }

    override fun onShowPress(p0: MotionEvent) {
        return
    }

    override fun onSingleTapUp(p0: MotionEvent): Boolean {
        val x: Float = p0!!.x
        val y: Float = p0!!.y

        renderer.tap(x,y)

        return true
    }

    override fun onScroll(e1: MotionEvent?, p0: MotionEvent, p2: Float, p3: Float): Boolean {
        return true
    }

    override fun onLongPress(p0: MotionEvent) {
        return
    }

    override fun onFling(e1: MotionEvent?, p0: MotionEvent, p2: Float, p3: Float): Boolean {
        if (e1 != null)
        {
            renderer.swipe(Pair(e1.x, e1.y), Pair(p0.x, p0.y))
        }
        return true
    }

}