package dev.kdrag0n.safetynetfix.proxy

import dev.kdrag0n.safetynetfix.logDebug
import java.io.InputStream
import java.io.OutputStream
import java.security.Key
import java.security.KeyStoreSpi
import java.security.cert.Certificate
import java.util.*

class ProxyKeyStoreSpi private constructor(
    private val orig: KeyStoreSpi,
) : KeyStoreSpi() {
    @Suppress("unused")
    constructor() : this(androidImpl!!)

    init {
        logDebug("Init proxy KeyStore SPI")
    }

    // Avoid breaking other, legitimate uses of key attestation in Google Play Services, e.g.
    //   - com.google.android.gms.auth.cryptauth.register.ReEnrollmentChimeraService
    //   - tk_trace.129-RegisterForKeyPairOperation
    private fun isCallerSafetyNet() = Thread.currentThread().stackTrace.any {
        // a.a.engineGetCertificateChain(Unknown Source:15)
        // java.security.KeyStore.getCertificateChain(KeyStore.java:1087)
        // com.google.ccc.abuse.droidguard.DroidGuard.initNative(Native Method)
        // com.google.ccc.abuse.droidguard.DroidGuard.init(DroidGuard.java:447)
        // java.lang.reflect.Method.invoke(Native Method)
        // xvq.b(:com.google.android.gms@212621053@21.26.21 (190400-387928701):1)
        // xuc.a(:com.google.android.gms@212621053@21.26.21 (190400-387928701):5)
        // xuc.eX(:com.google.android.gms@212621053@21.26.21 (190400-387928701):1)
        // dzx.onTransact(:com.google.android.gms@212621053@21.26.21 (190400-387928701):8)
        // android.os.Binder.execTransactInternal(Binder.java:1179)
        // android.os.Binder.execTransact(Binder.java:1143)
        logDebug("Stack trace element: $it")
        it.className.contains("DroidGuard", ignoreCase = true)
    }

    override fun engineGetCertificateChain(alias: String?): Array<Certificate>? {
        logDebug("Proxy key store: get certificate chain")

        if (isCallerSafetyNet()) {
            logDebug("Blocking call")
            throw UnsupportedOperationException()
        } else {
            logDebug("Allowing call")
            return orig.engineGetCertificateChain(alias)
        }
    }

    // Direct delegation. We have to do this manually because the Kotlin compiler can only do it
    // for interfaces, not abstract classes.
    override fun engineGetKey(alias: String?, password: CharArray?): Key? = orig.engineGetKey(alias, password)
    override fun engineGetCertificate(alias: String?): Certificate? = orig.engineGetCertificate(alias)
    override fun engineGetCreationDate(alias: String?): Date? = orig.engineGetCreationDate(alias)
    override fun engineSetKeyEntry(alias: String?, key: Key?, password: CharArray?, chain: Array<out Certificate>?) = orig.engineSetKeyEntry(alias, key, password, chain)
    override fun engineSetKeyEntry(alias: String?, key: ByteArray?, chain: Array<out Certificate>?) = orig.engineSetKeyEntry(alias, key, chain)
    override fun engineSetCertificateEntry(alias: String?, cert: Certificate?) = orig.engineSetCertificateEntry(alias, cert)
    override fun engineDeleteEntry(alias: String?) = orig.engineDeleteEntry(alias)
    override fun engineAliases(): Enumeration<String>? = orig.engineAliases()
    override fun engineContainsAlias(alias: String?) = orig.engineContainsAlias(alias)
    override fun engineSize() = orig.engineSize()
    override fun engineIsKeyEntry(alias: String?) = orig.engineIsKeyEntry(alias)
    override fun engineIsCertificateEntry(alias: String?) = orig.engineIsCertificateEntry(alias)
    override fun engineGetCertificateAlias(cert: Certificate?): String? = orig.engineGetCertificateAlias(cert)
    override fun engineStore(stream: OutputStream?, password: CharArray?) = orig.engineStore(stream, password)
    override fun engineLoad(stream: InputStream?, password: CharArray?) = orig.engineLoad(stream, password)

    companion object {
        @Volatile internal var androidImpl: KeyStoreSpi? = null
    }
}
