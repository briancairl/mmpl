cc_test(
    name="state-space-unit-tests",
    srcs=["state_space.cpp"],
    copts=["-Iexternal/googletest/googletest/include"],
    deps=[
        "//:mmpl",
        "@googletest//:gtest",
    ],
    timeout="short",
)


cc_test(
    name="value-unit-tests",
    srcs=["value.cpp"],
    copts=["-Iexternal/googletest/googletest/include"],
    deps=[
        "//:mmpl",
        "@googletest//:gtest",
    ],
    timeout="short",
)
