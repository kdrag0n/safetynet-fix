package dev.kdrag0n.safetynetfix

@Suppress("unused")
object EntryPoint {
    @JvmStatic
    fun init() {
        try {
            logDebug("Entry point: Initializing SafetyNet patches")
            SecurityHooks.init()
            BuildHooks.init()
        } catch (e: Throwable) {
            // Throwing an exception would require the JNI code to handle exceptions, so just catch
            // everything here.
            logDebug("Error in entry point", e)
        }
    }
}
