load("@envoy//bazel:envoy_build_system.bzl", "envoy_client_package")
load("@envoy_client//bazel:apple.bzl", "envoy_client_swift_test")

licenses(["notice"])  # Apache 2

envoy_client_package()

envoy_client_swift_test(
    name = "test",
    srcs = [
        "CounterImplTests.swift",
        "ElementTests.swift",
        "TagsBuilderTests.swift",
    ],
    exec_properties = {
        "sandboxNetwork": "standard",
    },
    flaky = True,  # TODO(jpsim): Fix timeouts when running these tests on CI
    visibility = ["//visibility:public"],
    deps = [
        "//library/objective-c:envoy_engine_objc_lib",
    ],
)
