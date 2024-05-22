# Copyright 2023 The Pigweed Authors
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.

"""Bazel transitions for sample project examples.

TODO:  b/301334234 - Use platform-based flags and retire these transitions.
"""

def _stm32_transition_impl(settings, attr):
    # buildifier: disable=unused-variable
    _ignore = settings, attr

    return {
        "//command_line_option:copt": ["-DSTM32CUBE_HEADER=\"stm32f4xx.h\"", "-DSTM32F429xx"],
        "//command_line_option:platforms": "//targets/stm32f429i_disc1_stm32cube:platform",
        "@hal_driver//:hal_config": "@pigweed//targets/stm32f429i_disc1_stm32cube:hal_config",
        "@pigweed//pw_log:backend": "@pigweed//pw_log_tokenized",
        "@pigweed//pw_log:backend_impl": "@pigweed//pw_log_tokenized:impl",
        "@pigweed//pw_log_tokenized:handler_backend": "@pigweed//pw_system:log_backend",
        "@pigweed//pw_system:extra_platform_libs": "//targets/stm32f429i_disc1_stm32cube:extra_platform_libs",
    }

_stm32_transition = transition(
    implementation = _stm32_transition_impl,
    inputs = [],
    outputs = [
        "//command_line_option:copt",
        "//command_line_option:platforms",
        "@hal_driver//:hal_config",
        "@pigweed//pw_log:backend_impl",
        "@pigweed//pw_log:backend",
        "@pigweed//pw_log_tokenized:handler_backend",
        "@pigweed//pw_system:extra_platform_libs",
    ],
)

def _stm32_binary_impl(ctx):
    out = ctx.actions.declare_file(ctx.label.name)
    ctx.actions.symlink(output = out, target_file = ctx.executable.binary)
    return [DefaultInfo(files = depset([out]), executable = out)]

stm32_binary = rule(
    _stm32_binary_impl,
    attrs = {
        "binary": attr.label(
            doc = "cc_binary to build for stm32f429i_disc1_stm32cube.",
            cfg = _stm32_transition,
            executable = True,
            mandatory = True,
        ),
        "_allowlist_function_transition": attr.label(
            default = "@bazel_tools//tools/allowlists/function_transition_allowlist",
        ),
    },
    doc = "Builds the specified binary for the stm32f429i_disc1_stm32cube platform.",
)

def _rp2040_transition_impl(settings, attr):
    # buildifier: disable=unused-variable
    _ignore = settings, attr

    return {
        "//command_line_option:platforms": "//targets/rp2040:platform",
        "@pigweed//pw_log:backend": "@pigweed//pw_log_tokenized",
        "@pigweed//pw_log:backend_impl": "@pigweed//pw_log_tokenized:impl",
        "@pigweed//pw_log_tokenized:handler_backend": "@pigweed//pw_system:log_backend",
        "@pigweed//pw_system:extra_platform_libs": "//targets/rp2040:extra_platform_libs",
    }

_rp2040_transition = transition(
    implementation = _rp2040_transition_impl,
    inputs = [],
    outputs = [
        "//command_line_option:platforms",
        "@pigweed//pw_log:backend_impl",
        "@pigweed//pw_log:backend",
        "@pigweed//pw_log_tokenized:handler_backend",
        "@pigweed//pw_system:extra_platform_libs",
    ],
)

def _rp2040_binary_impl(ctx):
    out = ctx.actions.declare_file(ctx.label.name)
    ctx.actions.symlink(output = out, target_file = ctx.executable.binary)
    return [DefaultInfo(files = depset([out]), executable = out)]

rp2040_binary = rule(
    _rp2040_binary_impl,
    attrs = {
        "binary": attr.label(
            doc = "cc_binary to build for rp2040 using pico-sdk.",
            cfg = _rp2040_transition,
            executable = True,
            mandatory = True,
        ),
        "_allowlist_function_transition": attr.label(
            default = "@bazel_tools//tools/allowlists/function_transition_allowlist",
        ),
    },
    doc = "Builds the specified binary for the pico-sdk rp2040 platform.",
)
