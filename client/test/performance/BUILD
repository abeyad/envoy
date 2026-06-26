load("@envoy//bazel:envoy_build_system.bzl", "envoy_cc_binary", "envoy_client_package")

licenses(["notice"])  # Apache 2

envoy_client_package()

envoy_cc_binary(
    name = "test_binary_size",
    srcs = ["test_binary_size.cc"],
    repository = "@envoy",
    stamped = True,
    deps = ["//library/common:internal_engine_lib"],
)
