load("@envoy//bazel:envoy_build_system.bzl", "envoy_client_package")
load("@envoy_client//bazel:envoy_client_android_test.bzl", "envoy_client_android_test")
load("@rules_android//android:rules.bzl", "android_library")

licenses(["notice"])  # Apache 2

envoy_client_package()

android_library(
    name = "testing",
    testonly = True,
    srcs = [
        "RequestScenario.java",
        "Response.java",
        "TestJni.java",
    ],
    visibility = ["//test:__subpackages__"],
    deps = [
        "//library/java/io/envoyproxy/envoyclient/engine:envoy_base_engine_lib",
        "//library/kotlin/io/envoyproxy/envoyclient:envoy_lib",
    ],
)

android_library(
    name = "http_test_server_factory_lib",
    testonly = True,
    srcs = [
        "HttpTestServerFactory.java",
    ],
    data = [
        "//test/jni:libenvoy_jni_http_test_server_factory.so",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//library/java/io/envoyproxy/envoyclient/engine:envoy_base_engine_lib",
        "//library/kotlin/io/envoyproxy/envoyclient:envoy_lib",
    ],
)

android_library(
    name = "http_proxy_test_server_factory_lib",
    testonly = True,
    srcs = [
        "HttpProxyTestServerFactory.java",
    ],
    data = [
        "//test/jni:libenvoy_jni_http_proxy_test_server_factory.so",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//library/java/io/envoyproxy/envoyclient/engine:envoy_base_engine_lib",
        "//library/kotlin/io/envoyproxy/envoyclient:envoy_lib",
    ],
)

envoy_client_android_test(
    name = "quic_test_server_test",
    srcs = [
        "QuicTestServerTest.java",
    ],
    native_deps = [
        "//test/jni:libenvoy_jni_with_test_extensions.so",
    ] + select({
        "@platforms//os:macos": [
            "//test/jni:libenvoy_jni_with_test_extensions_jnilib",
        ],
        "//conditions:default": [],
    }),
    native_lib_name = "envoy_jni_with_test_extensions",
    test_class = "io.envoyproxy.envoyclient.engine.testing.QuicTestServerTest",
    deps = [
        ":http_test_server_factory_lib",
        ":testing",
        "//library/kotlin/io/envoyproxy/envoyclient:envoy_lib",
    ],
)
