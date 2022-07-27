package dev.kdrag0n.safetynetfix

import android.os.Build

internal object BuildHooks {
    fun init() {
        // Append a space to the device model name
        Build::class.java.getDeclaredField("MODEL").let { field ->
            field.isAccessible = true
            field.set(null, Build.MODEL + " ")
        }
        Build::class.java.getDeclaredField("FINGERPRINT").let { field ->
            field.isAccessible = true
            field.set(null, "google/angler/angler:6.0/MDB08L/2343525:user/release-keys")
        }
    }
}
