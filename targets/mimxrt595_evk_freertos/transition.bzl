load("@pigweed//pw_build:merge_flags.bzl", "merge_flags_for_transition_impl", "merge_flags_for_transition_outputs")

# Configuration for the MIMXRT595-EVK FreeRTOS target
MIMXRT595_SYSTEM_FLAGS = {
    "@pigweed//pw_system:extra_platform_libs": str(Label("//targets/mimxrt595_evk_freertos:extra_platform_libs")),
    "@pigweed//pw_thread_freertos:config_override": str(Label("//examples/01_bluetooth:config_overrides")),
    "@pigweed//pw_function:config_override": str(Label("//config:pw_function_config_override")),
}

_mimxrt595_overrides = {
    "//command_line_option:platforms": "//targets/mimxrt595_evk_freertos:platform",
    "@mcuxpresso//:user_config": "//config:mcuxpresso_sdk_lib_config_bt_usb",
    "@mcuxpresso//:user_defines": "//config:mcuxpresso_sdk_lib_config_define",
    "@mcuxpresso//:app_include": "//config:app_bluetooth_config.h",
    "@pigweed//targets:mcuxpresso_sdk": "//config:mcuxpresso_sdk_bt_usb",
}

def _mimxrt595_transition_impl(settings, attr):
    # buildifier: disable=unused-variable
    _ignore = settings, attr

    return merge_flags_for_transition_impl(
        base = MIMXRT595_SYSTEM_FLAGS,
        override = _mimxrt595_overrides,
    )

_mimxrt595_transition = transition(
    implementation = _mimxrt595_transition_impl,
    inputs = [],
    outputs = merge_flags_for_transition_outputs(
        base = MIMXRT595_SYSTEM_FLAGS,
        override = _mimxrt595_overrides,
    ),
)

def _mimxrt595_binary_impl(ctx):
    out = ctx.actions.declare_file(ctx.label.name)
    ctx.actions.symlink(output = out, target_file = ctx.executable.binary)
    return [DefaultInfo(files = depset([out]), executable = out)]

mimxrt595_binary = rule(
    _mimxrt595_binary_impl,
    attrs = {
        "binary": attr.label(
            doc = "cc_binary to build for the mimxrt595040",
            cfg = _mimxrt595_transition,
            executable = True,
            mandatory = True,
        ),
        "_allowlist_function_transition": attr.label(
            default = "@bazel_tools//tools/allowlists/function_transition_allowlist",
        ),
    },
    doc = "Builds the specified binary for the mimxrt595 platform",
    # This target is for mimxrt595 and can't be run on host.
    executable = False,
)
