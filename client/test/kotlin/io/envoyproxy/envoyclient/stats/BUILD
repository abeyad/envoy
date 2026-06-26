load("@envoy//bazel:envoy_build_system.bzl", "envoy_client_package")
load("@envoy_client//bazel:envoy_client_android_test.bzl", "envoy_client_android_test")

licenses(["notice"])  # Apache 2

envoy_client_package()

envoy_client_android_test(
    name = "element_test",
    srcs = [
        "ElementTest.kt",
    ],
    test_class = "io.envoyproxy.envoyclient.ElementTest",
    deps = [
        "//library/kotlin/io/envoyproxy/envoyclient:envoy_interfaces_lib",
    ],
)

envoy_client_android_test(
    name = "tags_builder_test",
    srcs = [
        "TagsBuilderTest.kt",
    ],
    test_class = "io.envoyproxy.envoyclient.TagsBuilderTest",
    deps = [
        "//library/kotlin/io/envoyproxy/envoyclient:envoy_interfaces_lib",
    ],
)
