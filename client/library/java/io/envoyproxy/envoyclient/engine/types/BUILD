load("@envoy//bazel:envoy_build_system.bzl", "envoy_client_package")
load("@rules_android//android:rules.bzl", "android_library")

licenses(["notice"])  # Apache 2

envoy_client_package()

android_library(
    name = "envoy_c_types_lib",
    srcs = [
        "EnvoyConnectionType.java",
        "EnvoyEventTracker.java",
        "EnvoyFinalStreamIntel.java",
        "EnvoyHTTPCallbacks.java",
        "EnvoyHTTPFilter.java",
        "EnvoyHTTPFilterCallbacks.java",
        "EnvoyHTTPFilterFactory.java",
        "EnvoyKeyValueStore.java",
        "EnvoyLogger.java",
        "EnvoyNetworkType.java",
        "EnvoyOnEngineRunning.java",
        "EnvoyStatus.java",
        "EnvoyStreamIntel.java",
        "EnvoyStringAccessor.java",
        "NetworkWithType.java",
    ],
    visibility = ["//visibility:public"],
)
