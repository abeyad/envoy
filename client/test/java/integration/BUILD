load("@envoy//bazel:envoy_build_system.bzl", "envoy_client_package")
load("@envoy_client//bazel:envoy_client_android_test.bzl", "envoy_client_android_test")

licenses(["notice"])  # Apache 2

envoy_client_package()

envoy_client_android_test(
    name = "android_engine_start_test",
    srcs = [
        "AndroidEngineStartUpTest.java",
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
    test_class = "test.java.integration.AndroidEngineStartUpTest",
    deps = [
        "//library/kotlin/io/envoyproxy/envoyclient:envoy_lib",
    ],
)

envoy_client_android_test(
    name = "android_engine_flow_test",
    srcs = [
        "AndroidEngineFlowTest.java",
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
    test_class = "test.java.integration.AndroidEngineFlowTest",
    deps = [
        "//library/kotlin/io/envoyproxy/envoyclient:envoy_lib",
        "//test/java/io/envoyproxy/envoyclient/engine/testing",
    ],
)

envoy_client_android_test(
    name = "android_engine_explicit_flow_test",
    srcs = [
        "AndroidEngineExplicitFlowTest.java",
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
    test_class = "test.java.integration.AndroidEngineExplicitFlowTest",
    deps = [
        "//library/kotlin/io/envoyproxy/envoyclient:envoy_lib",
        "//test/java/io/envoyproxy/envoyclient/engine/testing",
    ],
)

envoy_client_android_test(
    name = "android_engine_socket_tag_test",
    srcs = [
        "AndroidEngineSocketTagTest.java",
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
    test_class = "test.java.integration.AndroidEngineSocketTagTest",
    deps = [
        "//library/kotlin/io/envoyproxy/envoyclient:envoy_lib",
        "//test/java/io/envoyproxy/envoyclient/engine/testing",
    ],
)
