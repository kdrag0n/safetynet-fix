package dev.kdrag0n.safetynetfix

import android.os.Build
import android.app.Application
import android.util.Log

private const val DEBUG = true
private const val TAG = "SNFix/Java"

internal fun <T> Any.get(name: String) = this::class.java.getDeclaredField(name).let { field ->
    field.isAccessible = true
    @Suppress("unchecked_cast")
    field.get(this) as T
}

internal fun logDebug(msg: String) {
    if (DEBUG && Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
        Log.d(TAG, "[${Application.getProcessName()}] $msg")
    }
}

internal fun logDebug(msg: String, e: Throwable) {
    if (DEBUG && Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
        Log.d(TAG, "[${Application.getProcessName()}] $msg", e)
    }
}
