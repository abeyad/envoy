load("@envoy//bazel:envoy_build_system.bzl", "envoy_client_package")
load("@rules_android//android:rules.bzl", "android_library")
load("@rules_jvm_external//:defs.bzl", "artifact")

licenses(["notice"])  # Apache 2

envoy_client_package()

# Android libraries for urlconnection

# A URL connection interface backed by Envoy-Client.

android_library(
    name = "urlconnection",
    srcs = [
        "CronvoyBufferedOutputStream.java",
        "CronvoyChunkedOutputStream.java",
        "CronvoyFixedModeOutputStream.java",
        "CronvoyHttpURLConnection.java",
        "CronvoyHttpURLStreamHandler.java",
        "CronvoyInputStream.java",
        "CronvoyMessageLoop.java",
        "CronvoyOutputStream.java",
        "CronvoyURLStreamHandlerFactory.java",
    ],
    manifest = "URLConnectionManifest.xml",
    visibility = ["//visibility:public"],
    deps = [
        "//library/java/io/envoyproxy/envoyclient/utilities",
        "//library/java/org/chromium/net",
        artifact("androidx.annotation:annotation"),
    ],
)
