package dev.kdrag0n.safetynetfix.proxy

import android.os.Build
import dev.kdrag0n.safetynetfix.SecurityHooks
import dev.kdrag0n.safetynetfix.logDebug
import java.security.Provider
import kotlin.concurrent.thread

private const val PATCH_DURATION = 2000L

// This is mostly just a pass-through provider that exists to change the provider's ClassLoader.
// This works because Service looks up the class by name from the *provider* ClassLoader, not
// necessarily the bootstrap one.
class ProxyProvider(
    orig: Provider,
) : Provider(orig.name, orig.version, orig.info) {
    init {
        logDebug("Init proxy provider - wrapping $orig")

        putAll(orig)
        this["KeyStore.${SecurityHooks.PROVIDER_NAME}"] = ProxyKeyStoreSpi::class.java.name
    }

    override fun getService(type: String?, algorithm: String?): Service? {
        logDebug("Provider: get service - type=$type algorithm=$algorithm")
        if (algorithm == "AndroidCAStore") {
            val orig = Build.FINGERPRINT
            val patched = "google/angler/angler:6.0/MDB08L/2343525:user/release-keys"
            logDebug("patch build for castore $orig -> $patched")
            // Append a space to the device model name
            Build::class.java.getDeclaredField("FINGERPRINT").let { field ->
                field.isAccessible = true
                field.set(null, patched)
            }

            thread(isDaemon = true) {
                Thread.sleep(PATCH_DURATION)
                logDebug("unpatch")
                Build::class.java.getDeclaredField("FINGERPRINT").let { field ->
                    field.isAccessible = true
                    field.set(null, orig)
                }
            }
        }
        return super.getService(type, algorithm)
    }

    override fun getServices(): MutableSet<Service>? {
        logDebug("Get services")
        return super.getServices()
    }
}
