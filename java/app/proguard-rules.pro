# Add project specific ProGuard rules here.
# You can control the set of applied configuration files using the
# proguardFiles setting in build.gradle.
#
# For more details, see
#   http://developer.android.com/guide/developing/tools/proguard.html

# If your project uses WebView with JS, uncomment the following
# and specify the fully qualified class name to the JavaScript interface
# class:
#-keepclassmembers class fqcn.of.javascript.interface.for.webview {
#   public *;
#}

# Uncomment this to preserve the line number information for
# debugging stack traces.
#-keepattributes SourceFile,LineNumberTable

# If you keep the line number information, uncomment this to
# hide the original source file name.
#-renamesourcefileattribute SourceFile

-keep class dev.kdrag0n.safetynetfix.EntryPoint {
    public static void init();
}

-keepclassmembers class dev.kdrag0n.safetynetfix.proxy.ProxyKeyStoreSpi {
    public <init>(...);
}

# Remove @DebugMetadata annotations to avoid leaking info
# Source: https://proandroiddev.com/kotlin-cleaning-java-bytecode-before-release-9567d4c63911
-checkdiscard @interface kotlin.coroutines.jvm.internal.DebugMetadata
-assumenosideeffects public class kotlin.coroutines.jvm.internal.BaseContinuationImpl {
  private kotlin.coroutines.jvm.internal.DebugMetadata getDebugMetadataAnnotation() return null;
  public java.lang.StackTraceElement getStackTraceElement() return null;
  public java.lang.String[] getSpilledVariableFieldMapping() return null;
}

-assumenosideeffects class kotlin.jvm.internal.Intrinsics {
    # Remove verbose NPE intrinsics to reduce code size and avoid leaking info
    # Source: https://issuetracker.google.com/issues/190489514
    static void checkParameterIsNotNull(java.lang.Object, java.lang.String);
    static void checkNotNullParameter(java.lang.Object, java.lang.String);
    static void checkFieldIsNotNull(java.lang.Object, java.lang.String);
    static void checkFieldIsNotNull(java.lang.Object, java.lang.String, java.lang.String);
    static void checkReturnedValueIsNotNull(java.lang.Object, java.lang.String);
    static void checkReturnedValueIsNotNull(java.lang.Object, java.lang.String, java.lang.String);
    static void checkNotNullExpressionValue(java.lang.Object, java.lang.String);
    static void checkExpressionValueIsNotNull(java.lang.Object, java.lang.String);
    static void checkNotNull(java.lang.Object);
    static void checkNotNull(java.lang.Object, java.lang.String);

    # Remove remaining stray calls to stringPlus
    static java.lang.String stringPlus(java.lang.String, java.lang.Object);
}
