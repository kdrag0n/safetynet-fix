package dev.kdrag0n.safetynetriru

import dev.kdrag0n.safetynetriru.proxy.ProxyKeyStoreSpi
import dev.kdrag0n.safetynetriru.proxy.ProxyProvider
import java.security.KeyStore
import java.security.KeyStoreSpi
import java.security.Security

internal object SecurityBridge {
    const val PROVIDER_NAME = "AndroidKeyStore"

    fun init() {
        logDebug("Initializing SecurityBridge")

        val realProvider = Security.getProvider(PROVIDER_NAME)
        val realKeystore = KeyStore.getInstance(PROVIDER_NAME)
        val realSpi = realKeystore.get<KeyStoreSpi>("keyStoreSpi")
        logDebug("Real provider=$realProvider, keystore=$realKeystore, spi=$realSpi")

        val provider = ProxyProvider(realProvider)
        logDebug("Removing real provider")
        Security.removeProvider(PROVIDER_NAME)
        logDebug("Inserting provider $provider")
        Security.insertProviderAt(provider, 1)
        ProxyKeyStoreSpi.androidImpl = realSpi
        logDebug("Security hooks installed")
    }
}
