package app.jerboa.jellycram

class Hop
{
    companion object
    {
        init
        {
            System.loadLibrary("HopAndroid")
        }
    }

    external fun initialise(resX: Int, resY: Int, skipTutorial: Boolean = false)

    external fun setDarkMode(isDarkMode: Boolean)
    external fun getState(): String

    external fun getGameTimeMillis(): Long

    external fun getScore(): Long

    external fun getClears(): Long

    external fun isGameOver(): Boolean

    external fun smasherMissed(): Boolean

    external fun smasherHit(): Boolean

    external fun landingSpeed(): Double

    external fun landed(): Boolean

    external fun pause(v: Boolean)

    external fun restart()

    external fun screenCentric(v: Boolean)
    external fun invertTapControls(v: Boolean)

    external fun invertSwipeControls(v: Boolean)

    external fun tutorialDone(): Boolean

    // World

    external fun screenToWorld(x: Float, y: Float, rx: Float, ry: Float)

    // Rendering

    external fun beginFrame()

    external fun endFrame()

    external fun render(refreshObjectShaders: Boolean)

    external fun renderText(
        text: String,
        x: Float,
        y: Float,
        scale: Float,
        r: Float,
        g: Float,
        b: Float,
        a: Float = 1.0f,
        centredx: Boolean = false,
        centredy: Boolean = false
    )

    // physics

    external fun loop(frameId: Int, first: Boolean = false)

    external fun tap(sx: Float, sy: Float)
    external fun swipe(vx: Float, vy: Float)

    // Logging

    external fun printLog()
}