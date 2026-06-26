load("@envoy//bazel:envoy_build_system.bzl", "envoy_client_package")
load("@rules_kotlin//kotlin:android.bzl", "kt_android_library")

licenses(["notice"])  # Apache 2

envoy_client_package()

kt_android_library(
    name = "mocks_lib",
    srcs = [
        "MockEnvoyEngine.kt",
        "MockEnvoyHTTPStream.kt",
        "MockStream.kt",
        "MockStreamClient.kt",
        "MockStreamPrototype.kt",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//library/kotlin/io/envoyproxy/envoyclient:envoy_interfaces_lib",
    ],
)
