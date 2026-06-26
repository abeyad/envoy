load("@envoy//bazel:envoy_build_system.bzl", "envoy_client_package")

licenses(["notice"])  # Apache 2

envoy_client_package()

filegroup(
    name = "proguard_rules",
    srcs = [
        "java_proto_proguard.txt",
        "proguard.txt",
    ],
    visibility = ["//visibility:public"],
)
