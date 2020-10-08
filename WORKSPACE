workspace(name="mmpl")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")


# TwoD
git_repository(
  name="twod",
  remote="https://github.com/briancairl/twod.git",
  commit="cdfc4a6b74dd7a06abac8217bf908dc1fe039c5a",
  shallow_since="1602129003 -0700"
)


# GTest/GMock
http_archive(
    name="googletest",
    url="https://github.com/google/googletest/archive/release-1.8.0.zip",
    sha256="f3ed3b58511efd272eb074a3a6d6fb79d7c2e6a0e374323d1e6bcbcc1ef141bf",
    build_file="@//:third_party/googletest.BUILD",
    strip_prefix="googletest-release-1.8.0",
)
