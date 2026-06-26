load("@envoy//bazel:envoy_build_system.bzl", "envoy_client_package")
load("@envoy_client//bazel:envoy_client_android_test.bzl", "envoy_client_android_test")

licenses(["notice"])  # Apache 2

envoy_client_package()

envoy_client_android_test(
    name = "engine_builder_test",
    srcs = [
        "EngineBuilderTest.kt",
    ],
    exec_properties = {
        # TODO(lfpino): Remove this once the JVM paths are allow-listed in the sandbox.
        "sandboxAllowed": "False",
    },
    native_deps = [
        "//test/jni:libenvoy_jni_with_test_extensions.so",
    ] + select({
        "@platforms//os:macos": [
            "//test/jni:libenvoy_jni_with_test_extensions_jnilib",
        ],
        "//conditions:default": [],
    }),
    native_lib_name = "envoy_jni_with_test_extensions",
    test_class = "io.envoyproxy.envoyclient.EngineBuilderTest",
    deps = [
        "//library/java/io/envoyproxy/envoyclient/engine:envoy_engine_lib",
        "//library/kotlin/io/envoyproxy/envoyclient:envoy_interfaces_lib",
    ],
)

envoy_client_android_test(
    name = "grpc_request_headers_builder_test",
    srcs = [
        "GRPCRequestHeadersBuilderTest.kt",
    ],
    test_class = "io.envoyproxy.envoyclient.GRPCRequestHeadersBuilderTest",
    deps = [
        "//library/kotlin/io/envoyproxy/envoyclient:envoy_interfaces_lib",
    ],
)

envoy_client_android_test(
    name = "grpc_stream_test",
    srcs = [
        "GRPCStreamTest.kt",
    ],
    test_class = "io.envoyproxy.envoyclient.GRPCStreamTest",
    deps = [
        "//library/kotlin/io/envoyproxy/envoyclient:envoy_interfaces_lib",
        "//test/kotlin/io/envoyproxy/envoyclient/mocks:mocks_lib",
    ],
)

envoy_client_android_test(
    name = "headers_builder_test",
    srcs = [
        "HeadersBuilderTest.kt",
    ],
    test_class = "io.envoyproxy.envoyclient.HeadersBuilderTest",
    deps = [
        "//library/kotlin/io/envoyproxy/envoyclient:envoy_interfaces_lib",
    ],
)

envoy_client_android_test(
    name = "pulse_client_impl_test",
    srcs = [
        "PulseClientImplTest.kt",
    ],
    exec_properties = {
        # TODO(lfpino): Remove this once the JVM paths are allow-listed in the sandbox.
        "sandboxAllowed": "False",
    },
    test_class = "io.envoyproxy.envoyclient.PulseClientImplTest",
    deps = [
        "//library/kotlin/io/envoyproxy/envoyclient:envoy_interfaces_lib",
    ],
)

envoy_client_android_test(
    name = "request_headers_builder_test",
    srcs = [
        "RequestHeadersBuilderTest.kt",
    ],
    test_class = "io.envoyproxy.envoyclient.RequestHeadersBuilderTest",
    deps = [
        "//library/kotlin/io/envoyproxy/envoyclient:envoy_interfaces_lib",
    ],
)

envoy_client_android_test(
    name = "response_headers_test",
    srcs = [
        "ResponseHeadersTest.kt",
    ],
    test_class = "io.envoyproxy.envoyclient.ResponseHeadersTest",
    deps = [
        "//library/kotlin/io/envoyproxy/envoyclient:envoy_interfaces_lib",
    ],
)

envoy_client_android_test(
    name = "retry_policy_mapper_test",
    srcs = [
        "RetryPolicyMapperTest.kt",
    ],
    test_class = "io.envoyproxy.envoyclient.RetryPolicyMapperTest",
    deps = [
        "//library/kotlin/io/envoyproxy/envoyclient:envoy_interfaces_lib",
    ],
)

envoy_client_android_test(
    name = "headers_container_test",
    srcs = [
        "HeadersContainerTest.kt",
    ],
    test_class = "io.envoyproxy.envoyclient.HeadersContainerTest",
    deps = [
        "//library/kotlin/io/envoyproxy/envoyclient:envoy_interfaces_lib",
    ],
)
