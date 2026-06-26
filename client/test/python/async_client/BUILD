load("@envoy//bazel:envoy_build_system.bzl", "envoy_client_package")
load("@rules_python//python:defs.bzl", "py_test")

licenses(["notice"])  # Apache 2

envoy_client_package()

py_test(
    name = "async_client_fetch_test",
    srcs = ["async_client_fetch_test.py"],
    main = "async_client_fetch_test.py",
    deps = [
        "//library/python:envoy_client_lib",
        "//test/python:echo_test_server",
    ],
)
