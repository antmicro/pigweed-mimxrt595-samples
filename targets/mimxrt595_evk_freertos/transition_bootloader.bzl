load("@pigweed//pw_build:merge_flags.bzl", "merge_flags_for_transition_impl", "merge_flags_for_transition_outputs")
load("//targets/mimxrt595_evk_freertos:transition.bzl", "MIMXRT595_SYSTEM_FLAGS")

_mimxrt595_bootloader_overrides = {
    "//command_line_option:platforms": str(Label("@pigweed//targets/mimxrt595_evk_freertos:platform")),
    "@pigweed//pw_system:extra_platform_libs": str(Label(":bootloader_extra_platform_libs")),
}

def _mimxrt595_bootloader_transition_impl(settings, attr):
    # buildifier: disable=unused-variable
    _ignore = settings, attr

    return merge_flags_for_transition_impl(
        base = MIMXRT595_SYSTEM_FLAGS,
        override = _mimxrt595_bootloader_overrides,
    )

_mimxrt595_bootloader_transition = transition(
    implementation = _mimxrt595_bootloader_transition_impl,
    inputs = [],
    outputs = merge_flags_for_transition_outputs(
        base = MIMXRT595_SYSTEM_FLAGS,
        override = _mimxrt595_bootloader_overrides,
    ),
)

def _mimxrt595_bootloader_binary_impl(ctx):
    out = ctx.actions.declare_file(ctx.label.name)
    ctx.actions.symlink(output = out, target_file = ctx.executable.binary)
    return [DefaultInfo(files = depset([out]), executable = out)]

mimxrt595_bootloader_binary = rule(
    _mimxrt595_bootloader_binary_impl,
    attrs = {
        "binary": attr.label(
            doc = "cc_binary to build a fastboot-compatible binary for the mimxrt595040",
            cfg = _mimxrt595_bootloader_transition,
            executable = True,
            mandatory = True,
        ),
        "_allowlist_function_transition": attr.label(
            default = "@bazel_tools//tools/allowlists/function_transition_allowlist",
        ),
    },
    doc = "Builds the specified binary for the mimxrt595 platform",
)
