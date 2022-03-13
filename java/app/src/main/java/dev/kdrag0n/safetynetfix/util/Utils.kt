package dev.kdrag0n.safetynetfix

import android.util.Log

private const val DEBUG = false
private const val TAG = "safetynetfix/Java"

internal fun <T> Any.get(name: String) = this::class.java.getDeclaredField(name).let { field ->
    field.isAccessible = true
    @Suppress("unchecked_cast")
    field.get(this) as T
}

internal fun logDebug(msg: String) {
    if (DEBUG) {
        Log.d(TAG, msg)
    }
}

internal fun logDebug(msg: String, e: Throwable) {
    if (DEBUG) {
        Log.d(TAG, msg, e)
    }
}
