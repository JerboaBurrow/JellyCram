package app.jerboa.jellycram.onlineServices

import android.app.Activity
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import app.jerboa.jellycram.R
import com.google.android.play.core.review.ReviewException
import com.google.android.play.core.review.ReviewManager
import com.google.android.play.core.review.ReviewManagerFactory
import com.google.android.play.core.review.model.ReviewErrorCode

class InAppReview {

    private var lastPlayTime: Long = 0L
    private var seenReview: Boolean = false

    private val reviewLimitMillis: Long = 1000 * 60 * 15

    private lateinit var reviewManager: ReviewManager

    private fun canReview(context: Activity): Boolean
    {
        if (seenReview) { return false }
        val prefs = context.getSharedPreferences(context.resources.getString(R.string.app_prefs), AppCompatActivity.MODE_PRIVATE)
        lastPlayTime = prefs.getLong("playTime", 0L)
        val coolOff = prefs.getFloat("coolOff", 0.0f)
        Log.d("canReview", "$lastPlayTime, $coolOff, ${reviewLimitMillis * (1.0f + coolOff)}, ${lastPlayTime > reviewLimitMillis * (1.0f + coolOff)}")
        return lastPlayTime > reviewLimitMillis * (1.0f + coolOff)
    }

    fun requestUserReviewPrompt(context: Activity) {

        if (!this::reviewManager.isInitialized)
        {
            reviewManager = ReviewManagerFactory.create(context)
        }

        if (canReview(context)) {

            val request = reviewManager.requestReviewFlow()
            request.addOnCompleteListener { task ->
                if (task.isSuccessful) {
                    // We got the ReviewInfo object
                    val reviewInfo = task.result
                    val flow = reviewManager.launchReviewFlow(context, reviewInfo)
                    flow.addOnCompleteListener {
                        Log.d("requestUserReviewPrompt", "complete")
                        seenReview = true
                    }
                } else {
                    // There was some problem, log or handle the error code.
                    @ReviewErrorCode val reviewErrorCode =
                        (task.exception as ReviewException).errorCode
                    Log.e("requestUserReviewPrompt", "$reviewErrorCode")
                }
            }

            val prefs = context.getSharedPreferences(context.resources.getString(R.string.app_prefs), AppCompatActivity.MODE_PRIVATE)
            val coolOff = prefs.getFloat("coolOff", 0.0f)
            val edit = prefs.edit()
            edit.putFloat("coolOff", 1.0f+coolOff)
            edit.apply()

        }
    }
}