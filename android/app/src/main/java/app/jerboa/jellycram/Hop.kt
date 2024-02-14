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

    external fun initialise(state: String, resX: Int, resY: Int)

    external fun getState(): String

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

    external fun loop(first: Boolean = false)

    // Logging

    external fun printLog()
}