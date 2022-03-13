package dev.kdrag0n.safetynetfix

import android.os.Build

internal object BuildHooks {
    fun init() {
        // Append a space to the device model name
        Build::class.java.getDeclaredField("MODEL").let { field ->
            field.isAccessible = true
            field.set(null, Build.MODEL + " ")
        }
    }
}
