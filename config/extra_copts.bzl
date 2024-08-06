def _extra_copts_impl(ctx):
    context = cc_common.create_compilation_context(
        direct_textual_headers = depset(ctx.build_setting_value)
    )
    return [CcInfo(compilation_context = context)]

extra_copts = rule(
    implementation = _extra_copts_impl,
    build_setting = config.string_list(flag = False),
)

def _use_extra_copts_implementation(ctx):
    return [ctx.attr._copts[CcInfo]]

use_extra_copts = rule(
    implementation = _use_extra_copts_implementation,
    attrs = {
        "_copts": attr.label(default = "//config:extra_copts")
    },
)

def _add_copts_impl(settings, attr):
    return {"//config:extra_copts": ["MY_DEFINE"]}

_add_copts = transition(
    implementation = _add_copts_impl,
    inputs = [],
    outputs = ["//config:extra_copts"],
)

def _with_extra_copts_implementation(ctx):
    infos = [d[CcInfo] for d in ctx.attr.deps]
    return [cc_common.merge_cc_infos(cc_infos = infos)]

with_extra_copts = rule(
    implementation = _with_extra_copts_implementation,
    attrs = {
        "deps": attr.label_list(cfg = _add_copts),
        "_allowlist_function_transition": attr.label(
            default = "@bazel_tools//tools/allowlists/function_transition_allowlist"
        )
    },
)
