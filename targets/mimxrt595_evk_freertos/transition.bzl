load("@rules_platform//platform_data:defs.bzl", "platform_data")

def mimxrt595_binary(name = "", binary = "", testonly = False):
    return platform_data(name = name, target = binary, testonly = testonly, platform = "//targets/mimxrt595_evk_freertos:platform")

def mimxrt595_bootloader_binary(name = "", binary = "", testonly = False):
    return platform_data(name = name, target = binary, testonly = testonly, platform = "//targets/mimxrt595_evk_freertos:bootloader_platform")
