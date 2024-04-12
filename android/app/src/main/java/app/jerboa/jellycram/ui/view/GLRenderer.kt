package app.jerboa.jellycram.ui.view

import android.opengl.GLES31.*
import android.opengl.GLSurfaceView
import android.util.Log
import app.jerboa.jellycram.Hop
import app.jerboa.jellycram.ViewModel.Event
import app.jerboa.jellycram.ViewModel.RenderViewModel
import app.jerboa.jellycram.ViewModel.Score
import app.jerboa.jellycram.ViewModel.Scored
import app.jerboa.jellycram.ViewModel.Settings
import app.jerboa.jellycram.ViewModel.TutorialDone
import app.jerboa.jellycram.ViewModel.UpdatingAchievement
import app.jerboa.jellycram.data.GameState
import app.jerboa.jellycram.utils.*
import java.nio.ByteBuffer
import java.nio.ByteOrder
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10
import android.opengl.GLES31 as gl3

class GLRenderer (
    private var tutorialDone: Boolean,
    private val resolution: Pair<Int,Int>,
    private val onEvent: (Event) -> Unit
    ) : GLSurfaceView.Renderer {

    // keep track of frame rate
    private var delta: Long = 0
    private var last: Long = System.nanoTime()
    // id for a frame (will be kep mod 60)
    private var frameNumber: Int = 0
    // smoothed framerate states
    private val deltas: FloatArray = FloatArray(60){0f}

    private var tapEvent: Pair<Float, Float>? = null
    private var swipeEvent: Pair<Pair<Float, Float>,Pair<Float, Float>>? = null

    private var postedScore: Boolean = false
    private var smasherHit: Boolean = false
    private var smasherMiss: Boolean = false
    private var hardLanding: Boolean = false
    private var softLanding: Boolean = false

    private var settings: Settings = Settings(invertTapControls = false, invertSwipeControls = false, screenCentric = true)
    private var updatedSettings: Boolean = false

    private lateinit var hop: Hop

    private val state: GameState = GameState()

    // propagate a tap event
    fun tap(x: Float,y: Float){
        tapEvent = Pair(x,resolution.second-y)
    }

    fun pause(v: Boolean)
    {
        if (this::hop.isInitialized) { hop.pause(v) }
    }

    fun restart()
    {
        if (this::hop.isInitialized)
        {
            hop.restart()
        }
    }

    fun swipe(a: Pair<Float, Float>, b: Pair<Float, Float>)
    {
        swipeEvent = Pair(Pair(a.first,resolution.second-a.second), Pair(b.first,resolution.second-b.second))
    }

    fun setSettings(s: Settings)
    {
        settings = s
        updatedSettings = true
    }

    fun initGPUData(){
        glError()
    }
    override fun onSurfaceCreated(p0: GL10?, p1: EGLConfig?) {
        val vers = IntArray(2)
        glGetIntegerv(GL_MAJOR_VERSION, vers, 0)
        glGetIntegerv(GL_MINOR_VERSION, vers, 1)
        Log.d("glVersion","${vers[0]}, ${vers[1]}")

        val maxSamples = ByteBuffer.allocateDirect(1 * Float.SIZE_BYTES).order(ByteOrder.nativeOrder()).asIntBuffer()
        glGetIntegerv(GL_MAX_SAMPLES, maxSamples)
        Log.d("GL_MAX_SAMPLES","${maxSamples[0]}")

        gl3.glClearColor(1f,1f,1f,1f)

        // instance textures
        initGPUData()

        hop = Hop()
        hop.initialise(resolution.first, resolution.second, tutorialDone)

        val stateJSON: String = hop.getState()
    }

    override fun onSurfaceChanged(p0: GL10?, p1: Int, p2: Int) {
        gl3.glViewport(0,0,p1,p2)
    }

    override fun onDrawFrame(p0: GL10?) {

        val t0 = System.nanoTime()
        if (updatedSettings)
        {
            hop.invertTapControls(settings.invertTapControls)
            hop.invertSwipeControls(settings.invertSwipeControls)
            hop.screenCentric(settings.screenCentric)
            hop.setDarkMode(settings.darkMode)
            updatedSettings = false
        }

        if (tapEvent != null)
        {
            hop.tap(tapEvent!!.first, tapEvent!!.second)
            tapEvent = null
        }

        if (swipeEvent != null)
        {
            val vx = swipeEvent!!.second.first - swipeEvent!!.first.first
            val vy = swipeEvent!!.second.second - swipeEvent!!.first.second

            hop.swipe(vx, vy)

            swipeEvent = null
        }

        hop.loop(frameNumber/*gameOver, score*/)

        val t1 = System.nanoTime()

        // measure time
        val t = System.nanoTime()
        delta = t-last
        last = t

        deltas[frameNumber] = 1.0f / (delta.toFloat()*1e-9f)

        frameNumber += 1
        if (frameNumber >= 60){
            frameNumber = 0
            val mu = deltas.sum()/deltas.size
            //Log.d("Runtime","FPS, $mu")
        }

        if (!postedScore && hop.isGameOver())
        {
            val score = hop.getScore()
            val time = hop.getGameTimeMillis()
            onEvent(Scored(Score(score, time)))

            if (score >= 20)
            {
                onEvent(UpdatingAchievement("achievement_20",1))
            }

            if (score >= 40)
            {
                onEvent(UpdatingAchievement("achievement_40",1))
            }

            if (score >= 60)
            {
                onEvent(UpdatingAchievement("achievement_60",1))
            }

            if (score >= 80)
            {
                onEvent(UpdatingAchievement("achievement_80",1))
            }

            if (score >= 100)
            {
                onEvent(UpdatingAchievement("achievement_100",1))
            }

            if (time >= 5*60*1000)
            {
                onEvent(UpdatingAchievement("achievement_lasted_5_minutes",1))
            }

            if (time >= 10*60*1000)
            {
                onEvent(UpdatingAchievement("achievement_lasted_10_minutes",1))
            }

            if (time >= 15*60*1000)
            {
                onEvent(UpdatingAchievement("achievement_lasted_15_minutes",1))
            }

            postedScore = true
        }

        if (!smasherHit)
        {
            if (hop.smasherHit())
            {
                onEvent(UpdatingAchievement("achievement_smashed",1))
                smasherHit = true
            }
        }

        if (!smasherMiss)
        {
            if (hop.smasherMissed())
            {
                onEvent(UpdatingAchievement("achievement_goodbye_cruel_world",1))
                smasherMiss = true
            }
        }

        if (hop.landed())
        {
            val landingSpeed = hop.landingSpeed()
            if (!hardLanding && landingSpeed > 0.0015)
            {
                hardLanding = true
                onEvent(UpdatingAchievement("achievement_i_came_in_like_a_wrecking_ball",1))
            }
            else if (!softLanding && landingSpeed < 1e-4)
            {
                softLanding = true
                onEvent(UpdatingAchievement("achievement_soft_landing",1))
            }
        }

        if (!tutorialDone && hop.tutorialDone())
        {
            tutorialDone = true;
            onEvent(TutorialDone())
        }

        if (frameNumber == 30){
            hop.printLog()
        }
    }
}