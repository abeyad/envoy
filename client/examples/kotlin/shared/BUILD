load("@envoy//bazel:envoy_build_system.bzl", "envoy_client_package")
load("@rules_jvm_external//:defs.bzl", "artifact")
load("@rules_kotlin//kotlin:android.bzl", "kt_android_library")

licenses(["notice"])  # Apache 2

envoy_client_package()

kt_android_library(
    name = "hello_envoy_shared_lib",
    srcs = [
        "Response.kt",
        "ResponseRecyclerViewAdapter.kt",
        "ResponseViewHolder.kt",
    ],
    custom_package = "io.envoyproxy.envoyclient.shared",
    manifest = "AndroidManifest.xml",
    resource_files = glob([
        "res/layout/item.xml",
        "res/values/colors.xml",
        "res/values/strings.xml",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        artifact("androidx.recyclerview:recyclerview"),
    ],
)
