load("@envoy//bazel:envoy_build_system.bzl", "envoy_client_package")
load("@envoy_client//bazel:envoy_client_android_test.bzl", "envoy_client_android_test")

licenses(["notice"])  # Apache 2

envoy_client_package()

envoy_client_android_test(
    name = "certificate_verification_tests",
    srcs = [
        "CertificateVerificationTest.java",
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
    test_class = "io.envoyproxy.envoyclient.utilities.CertificateVerificationTest",
    deps = [
        "//library/java/io/envoyproxy/envoyclient/engine:envoy_base_engine_lib",
        "//library/java/io/envoyproxy/envoyclient/engine:envoy_engine_lib",
        "//library/java/io/envoyproxy/envoyclient/utilities",
        "//library/java/io/envoyproxy/envoyclient/utilities:network_utilities",
        "//library/java/org/chromium/net",
    ],
)

envoy_client_android_test(
    name = "android_network_test",
    srcs = [
        "AndroidNetworkTest.java",
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
    test_class = "io.envoyproxy.envoyclient.utilities.AndroidNetworkTest",
    deps = [
        "//library/java/io/envoyproxy/envoyclient/engine:envoy_base_engine_lib",
        "//library/java/io/envoyproxy/envoyclient/engine:envoy_engine_lib",
        "//library/java/io/envoyproxy/envoyclient/engine/types:envoy_c_types_lib",
        "//library/java/io/envoyproxy/envoyclient/utilities",
        "//library/java/io/envoyproxy/envoyclient/utilities:network_utilities",
        "@maven//:org_robolectric_annotations",
    ],
)
