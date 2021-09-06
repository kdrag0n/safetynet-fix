package dev.kdrag0n.safetynetriru

@Suppress("unused")
object EntryPoint {
    @JvmStatic
    fun init() {
        runCatching {
            logDebug("Entry point: Initializing SafetyNet patches")
            SecurityBridge.init()
            BuildHooks.init()
        }.recoverCatching { e ->
            // Throwing an exception would require the JNI code to handle exceptions, so just catch
            // everything here.
            logDebug("Error in entry point", e)
        }
    }
}
