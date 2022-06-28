#!/usr/bin/env bash

[[ -z "${SKIP_PROTO_FORMAT}" ]] && tools/proto_format/proto_format.sh fix

source_dirs=("//source/..." "//test/..." "//tools/...")
if [ "$1" == "--include-contrib" ]; then
  source_dirs+=("//contrib/...") 
fi

bazel_or_isk=bazelisk
command -v bazelisk &> /dev/null || bazel_or_isk=bazel

# Setting TEST_TMPDIR here so the compdb headers won't be overwritten by another bazel run
TEST_TMPDIR=${BUILD_DIR:-/tmp}/envoy-compdb tools/gen_compilation_database.py --vscode --bazel=$bazel_or_isk ${source_dirs[@]}

# Kill clangd to reload the compilation database
pkill clangd || :
