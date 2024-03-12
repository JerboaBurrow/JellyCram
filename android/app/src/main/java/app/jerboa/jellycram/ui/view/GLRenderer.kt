package app.jerboa.jellycram.ui.view

import android.opengl.GLES31.*
import android.opengl.GLSurfaceView
import android.util.Log
import app.jerboa.jellycram.Hop
import app.jerboa.jellycram.ViewModel.RenderViewModel
import app.jerboa.jellycram.data.GameState
import app.jerboa.jellycram.utils.*
import com.google.gson.Gson
import java.nio.ByteBuffer
import java.nio.ByteOrder
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10
import android.opengl.GLES31 as gl3

class GLRenderer (
    private val resolution: Pair<Int,Int>,
    private val onAchievementStateChanged: (RenderViewModel.AchievementUpdateData) -> Unit,
    private val onScored: (Long) -> Unit
    ) : GLSurfaceView.Renderer {

    // keep track of frame rate
    private var delta: Long = 0
    private var last: Long = System.nanoTime()
    // id for a frame (will be kep mod 60)
    private var frameNumber: Int = 0
    // rate-limit touch events
    private val touchRateLimit = 10 // frames
    private var lastTapped: Int = 0
    // smoothed framerate states
    private val deltas: FloatArray = FloatArray(60){0f}

    private var tapEvent: Pair<Float, Float>? = null
    private var swipeEvent: Pair<Pair<Float, Float>,Pair<Float, Float>>? = null

    private var scoreClock = 10
    private var scoreLastTapped = 0
    private var score: Long = 0L

    private lateinit var hop: Hop

    private val state: GameState = GameState()

    init {}

    private fun achievements(){
    }

    // propagate a tap event
    fun tap(x: Float,y: Float){
        tapEvent = Pair(x,resolution.second-y)
    }

    fun swipe(a: Pair<Float, Float>, b: Pair<Float, Float>)
    {
        swipeEvent = Pair(Pair(a.first,resolution.second-a.second), Pair(b.first,resolution.second-b.second))
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

        Log.d("resolution","$resolution")

        hop = Hop()
        hop.initialise(resolution.first, resolution.second)

        val stateJSON: String = hop.getState()
        Log.d("gameState", stateJSON)
    }

    override fun onSurfaceChanged(p0: GL10?, p1: Int, p2: Int) {
        gl3.glViewport(0,0,p1,p2)
    }

    override fun onDrawFrame(p0: GL10?) {
        lastTapped++
        scoreLastTapped++

        val t0 = System.nanoTime()

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
            Log.d("Runtime","FPS, $mu")
        }

        //if (frameNumber == 30){
        hop.printLog()
        //}

        if (scoreLastTapped > scoreClock){
            if (score > 0){onScored(score)}
            score = 0
            scoreLastTapped = 0
        }

        achievements()
    }
}